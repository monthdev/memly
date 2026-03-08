cmake_minimum_required(VERSION 3.25)
project(memly LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(CTest)
include(FetchContent)

find_package(Qt6 REQUIRED COMPONENTS Core Network Qml Quick Test)
qt_standard_project_setup(REQUIRES 6.9)

if(COMMAND qt_policy)
  qt_policy(SET QTP0004 NEW)
endif()

# ============================== Source discovery ==============================

file(GLOB APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/App/*.cpp")
file(GLOB APP_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/App/*.[Hh]*")

set(APP_MAIN_CPP "${CMAKE_SOURCE_DIR}/src/App/main.cpp")
list(REMOVE_ITEM APP_CPP "${APP_MAIN_CPP}")

file(GLOB BRIDGE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Bridge/*.cpp")
file(GLOB BRIDGE_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Bridge/*.[Hh]*")

set(QML_DIR "${CMAKE_SOURCE_DIR}/src/Qml")
file(GLOB_RECURSE QT_QML CONFIGURE_DEPENDS "${QML_DIR}/*.qml")
foreach(qml_file IN LISTS QT_QML)
  file(RELATIVE_PATH rel_qml "${QML_DIR}" "${qml_file}")
  set_source_files_properties(
    "${qml_file}" PROPERTIES QT_RESOURCE_ALIAS "${rel_qml}"
  )
endforeach()

file(GLOB SQL_FILES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Sql/Migrations/*.sql"
     "${CMAKE_SOURCE_DIR}/src/Sql/Statements/*.sql"
)

file(GLOB TEST_APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/App/*.cpp")
file(GLOB TEST_BRIDGE_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/test/Bridge/*.cpp"
)
file(GLOB TEST_QML_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Qml/*.cpp")
file(GLOB TEST_SQL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Sql/*.cpp")

set(TEST_MAIN_CPP "${CMAKE_SOURCE_DIR}/test/App/TestMain.cpp")
list(REMOVE_ITEM TEST_APP_CPP "${TEST_MAIN_CPP}")

set(TEST_PLATFORM_CPP)

set(TEST_SUPPORT_DATA_MACOS_CPP
    "${CMAKE_SOURCE_DIR}/test/Bridge/TestSupportDataMacOS.cpp"
)
set(TEST_SUPPORT_DATA_WINDOWS_CPP
    "${CMAKE_SOURCE_DIR}/test/Bridge/TestSupportDataWindows.cpp"
)
set(TEST_SUPPORT_DATA_LINUX_CPP
    "${CMAKE_SOURCE_DIR}/test/Bridge/TestSupportDataLinux.cpp"
)

list(REMOVE_ITEM TEST_BRIDGE_CPP "${TEST_SUPPORT_DATA_MACOS_CPP}"
     "${TEST_SUPPORT_DATA_WINDOWS_CPP}" "${TEST_SUPPORT_DATA_LINUX_CPP}"
)

if(APPLE AND EXISTS "${TEST_SUPPORT_DATA_MACOS_CPP}")
  list(APPEND TEST_PLATFORM_CPP "${TEST_SUPPORT_DATA_MACOS_CPP}")
elseif(WIN32 AND EXISTS "${TEST_SUPPORT_DATA_WINDOWS_CPP}")
  list(APPEND TEST_PLATFORM_CPP "${TEST_SUPPORT_DATA_WINDOWS_CPP}")
elseif(UNIX AND EXISTS "${TEST_SUPPORT_DATA_LINUX_CPP}")
  list(APPEND TEST_PLATFORM_CPP "${TEST_SUPPORT_DATA_LINUX_CPP}")
else()
  message(FATAL_ERROR "Unsupported target platform: ${CMAKE_SYSTEM_NAME}")
endif()

# ====================== DuckDB: platform-specific binaries ====================

set(DUCKDB_VERSION "1.4.4")
set(DUCKDB_TAG "v${DUCKDB_VERSION}")

if(APPLE)
  set(DUCKDB_URL
      "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-osx-universal.zip"
  )
  set(DUCKDB_SHA
      "5e6d79f5fb86bbd4f24d59cee3bc38f113d1433761df35337e3c01c62eeafe26"
  )
elseif(WIN32)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    set(DUCKDB_URL
        "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-windows-amd64.zip"
    )
    set(DUCKDB_SHA
        "18184f2d73bf962cb78eeefb17196d112fcd18b9411b16a3b47cd718693e004c"
    )
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|arm64|aarch64|AARCH64)$")
    set(DUCKDB_URL
        "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-windows-arm64.zip"
    )
    set(DUCKDB_SHA
        "f5d3507e611fdab7e972b13edde62205ffc39333f68ac9010e93bf1aa2983ab7"
    )
  else()
    message(
      FATAL_ERROR
        "Unsupported Windows architecture for DuckDB: ${CMAKE_SYSTEM_PROCESSOR}"
    )
  endif()
elseif(UNIX)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    set(DUCKDB_URL
        "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-linux-amd64.zip"
    )
    set(DUCKDB_SHA
        "09cc288295964d897b47665d1898e16e8ef176cae9ea615797fc136eae15bd5d"
    )
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|arm64|aarch64|AARCH64)$")
    set(DUCKDB_URL
        "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-linux-arm64.zip"
    )
    set(DUCKDB_SHA
        "768f66e0b40838d567a5524ae3901daa4823f4829fd1e6f3615d75ae55562b52"
    )
  else()
    message(
      FATAL_ERROR
        "Unsupported Linux architecture for DuckDB: ${CMAKE_SYSTEM_PROCESSOR}"
    )
  endif()
else()
  message(FATAL_ERROR "Unsupported platform for DuckDB")
endif()

FetchContent_Declare(
  libduckdb URL "${DUCKDB_URL}" URL_HASH "SHA256=${DUCKDB_SHA}"
)
FetchContent_MakeAvailable(libduckdb)

if(APPLE)
  set(_duckdb_lib_name "libduckdb.dylib")
elseif(WIN32)
  set(_duckdb_lib_name "duckdb.dll")
else()
  set(_duckdb_lib_name "libduckdb.so")
endif()

set(_duckdb_lib "${libduckdb_SOURCE_DIR}/${_duckdb_lib_name}")
if(NOT EXISTS "${_duckdb_lib}")
  set(_duckdb_lib "${libduckdb_SOURCE_DIR}/lib/${_duckdb_lib_name}")
endif()

set(_duckdb_inc "${libduckdb_SOURCE_DIR}")
if(NOT EXISTS "${_duckdb_inc}/duckdb.hpp")
  set(_duckdb_inc "${libduckdb_SOURCE_DIR}/include")
endif()

if(NOT EXISTS "${_duckdb_lib}")
  message(FATAL_ERROR "DuckDB library not found in ${libduckdb_SOURCE_DIR}")
endif()

if(NOT EXISTS "${_duckdb_inc}/duckdb.hpp")
  message(FATAL_ERROR "duckdb.hpp not found in ${libduckdb_SOURCE_DIR}")
endif()

add_library(duckdb SHARED IMPORTED GLOBAL)
set_target_properties(
  duckdb
  PROPERTIES IMPORTED_LOCATION "${_duckdb_lib}" INTERFACE_INCLUDE_DIRECTORIES
                                                "${_duckdb_inc}"
)

get_filename_component(_duckdb_libdir "${_duckdb_lib}" DIRECTORY)

# ============================== Shared options ================================

add_library(memly_warnings INTERFACE)
target_compile_options(
  memly_warnings
  INTERFACE -Wall -Wextra -Wunused -pedantic -Werror
)

add_library(memly_asan INTERFACE)
target_compile_options(
  memly_asan
  INTERFACE $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>
)
target_link_options(
  memly_asan INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>
)

# ============================ Shared SQL resources ============================

add_library(memly_sql_resources OBJECT)

qt_add_resources(
  memly_sql_resources
  "Sql"
  PREFIX
  "/Sql"
  BASE
  "${CMAKE_SOURCE_DIR}/src/Sql"
  FILES
  ${SQL_FILES}
)

# ============================= Shared core library ============================

add_library(
  memly_core STATIC ${APP_CPP} ${APP_HDR} ${BRIDGE_CPP} ${BRIDGE_HDR}
                    $<TARGET_OBJECTS:memly_sql_resources>
)

target_include_directories(memly_core PUBLIC "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(
  memly_core
  PUBLIC Qt6::Core
         Qt6::Network
         Qt6::Qml
         Qt6::Quick
         duckdb
         memly_warnings
         memly_asan
)

set_target_properties(
  memly_core
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)

# ============================== Shared QML module =============================

add_library(memly_qml STATIC)

qt_add_qml_module(
  memly_qml
  URI
  Memly
  VERSION
  1.0
  NO_PLUGIN
  OUTPUT_DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/Memly"
  QML_FILES
  ${QT_QML}
)

target_link_libraries(
  memly_qml
  PUBLIC memly_core
)

# ============================== App executable ================================

qt_add_executable(memly MANUAL_FINALIZATION "${APP_MAIN_CPP}")

target_link_libraries(
  memly
  PRIVATE memly_qml memly_warnings memly_asan
)

set_target_properties(
  memly
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)

qt_finalize_executable(memly)

# ============================= Test executable ================================

if(BUILD_TESTING)
  qt_add_executable(
    memly-test
    MANUAL_FINALIZATION
    ${TEST_MAIN_CPP}
    ${TEST_APP_CPP}
    ${TEST_BRIDGE_CPP}
    ${TEST_QML_CPP}
    ${TEST_SQL_CPP}
    ${TEST_PLATFORM_CPP}
  )

  target_include_directories(memly-test PRIVATE "${CMAKE_SOURCE_DIR}/src")

  target_compile_definitions(
    memly-test PRIVATE CMAKE_GENERATED_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
  )

  target_link_libraries(
    memly-test PRIVATE memly_qml Qt6::Test memly_warnings memly_asan
  )

  set_target_properties(memly-test PROPERTIES BUILD_RPATH "${_duckdb_libdir}")

  qt_finalize_executable(memly-test)

  add_test(NAME memly-test COMMAND memly-test)
endif()
