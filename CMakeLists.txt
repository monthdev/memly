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

file(GLOB APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/App/*.cpp")
file(GLOB APP_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/App/*.[Hh]*")

set(APP_MAIN_CPP "${CMAKE_SOURCE_DIR}/src/App/main.cpp")
list(REMOVE_ITEM APP_CPP "${APP_MAIN_CPP}")

file(GLOB BRIDGE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Bridge/*.cpp")
file(GLOB BRIDGE_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Bridge/*.[Hh]*")

file(GLOB LANGUAGE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Language/*.cpp")
file(GLOB LANGUAGE_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Language/*.[Hh]*")

file(GLOB MODEL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Model/*.cpp")
file(GLOB MODEL_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Model/*.[Hh]*")

file(GLOB SCHEDULER_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Scheduler/*.cpp")
file(GLOB SCHEDULER_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Scheduler/*.[Hh]*")

file(GLOB SQL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Sql/*.cpp")
file(GLOB SQL_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Sql/*.[Hh]*")

file(GLOB SUPPORT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/*.cpp")
file(GLOB SUPPORT_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/*.[Hh]*")

set(QML_DIR "${CMAKE_SOURCE_DIR}/src/Qml")
file(GLOB_RECURSE QT_QML CONFIGURE_DEPENDS "${QML_DIR}/*.qml")
foreach(qml_file IN LISTS QT_QML)
  file(RELATIVE_PATH rel_qml "${QML_DIR}" "${qml_file}")
  set_source_files_properties("${qml_file}" PROPERTIES QT_RESOURCE_ALIAS "${rel_qml}")
endforeach()

file(GLOB SQL_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Sql/Migration/*.sql"
     "${CMAKE_SOURCE_DIR}/src/Sql/Seed/*.sql" "${CMAKE_SOURCE_DIR}/src/Sql/Statement/*.sql"
)

file(GLOB TEST_APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/App/*.cpp")
file(GLOB TEST_BRIDGE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Bridge/*.cpp")
file(GLOB TEST_LANGUAGE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Language/*.cpp")
file(GLOB TEST_MODEL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Model/*.cpp")
file(GLOB TEST_SCHEDULER_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Scheduler/*.cpp")
file(GLOB TEST_QML_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Qml/*.cpp")
file(GLOB TEST_SQL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Sql/*.cpp")
file(GLOB TEST_SUPPORT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/Support/*.cpp")

set(TEST_MAIN_CPP "${CMAKE_SOURCE_DIR}/test/App/TestMain.cpp")
list(REMOVE_ITEM TEST_APP_CPP "${TEST_MAIN_CPP}")

set(TEST_PLATFORM_CPP)

set(TEST_SUPPORT_DATA_MACOS_CPP "${CMAKE_SOURCE_DIR}/test/Bridge/TestSupportDataMacOS.cpp")

list(REMOVE_ITEM TEST_BRIDGE_CPP "${TEST_SUPPORT_DATA_MACOS_CPP}")

if(APPLE AND EXISTS "${TEST_SUPPORT_DATA_MACOS_CPP}")
  list(APPEND TEST_PLATFORM_CPP "${TEST_SUPPORT_DATA_MACOS_CPP}")
endif()

set(DUCKDB_VERSION "1.4.4")
set(DUCKDB_TAG "v${DUCKDB_VERSION}")

if(APPLE)
  set(DUCKDB_URL "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-osx-universal.zip")
  set(DUCKDB_SHA "5e6d79f5fb86bbd4f24d59cee3bc38f113d1433761df35337e3c01c62eeafe26")
elseif(WIN32)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    set(DUCKDB_URL "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-windows-amd64.zip")
    set(DUCKDB_SHA "18184f2d73bf962cb78eeefb17196d112fcd18b9411b16a3b47cd718693e004c")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|arm64|aarch64|AARCH64)$")
    set(DUCKDB_URL "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-windows-arm64.zip")
    set(DUCKDB_SHA "f5d3507e611fdab7e972b13edde62205ffc39333f68ac9010e93bf1aa2983ab7")
  else()
    message(FATAL_ERROR "Unsupported Windows architecture for DuckDB: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
elseif(UNIX)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    set(DUCKDB_URL "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-linux-amd64.zip")
    set(DUCKDB_SHA "09cc288295964d897b47665d1898e16e8ef176cae9ea615797fc136eae15bd5d")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|arm64|aarch64|AARCH64)$")
    set(DUCKDB_URL "https://install.duckdb.org/${DUCKDB_TAG}/libduckdb-linux-arm64.zip")
    set(DUCKDB_SHA "768f66e0b40838d567a5524ae3901daa4823f4829fd1e6f3615d75ae55562b52")
  else()
    message(FATAL_ERROR "Unsupported Linux architecture for DuckDB: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()
else()
  message(FATAL_ERROR "Unsupported platform for DuckDB")
endif()

FetchContent_Declare(libduckdb URL "${DUCKDB_URL}" URL_HASH "SHA256=${DUCKDB_SHA}")
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

set(_duckdb_inc "${libduckdb_SOURCE_DIR}/include")
if(NOT EXISTS "${_duckdb_inc}/duckdb.hpp")
  set(_duckdb_inc "${libduckdb_SOURCE_DIR}")
endif()

if(NOT EXISTS "${_duckdb_lib}")
  message(FATAL_ERROR "DuckDB library not found in ${libduckdb_SOURCE_DIR}")
endif()

if(NOT EXISTS "${_duckdb_inc}/duckdb.hpp")
  message(FATAL_ERROR "duckdb.hpp not found in ${libduckdb_SOURCE_DIR}")
endif()

set(_duckdb_wrapper_dir "${CMAKE_BINARY_DIR}/duckdb_wrapper")
file(MAKE_DIRECTORY "${_duckdb_wrapper_dir}")
file(WRITE "${_duckdb_wrapper_dir}/duckdb.hpp" "#pragma GCC system_header\n#include \"${_duckdb_inc}/duckdb.hpp\"\n")

add_library(duckdb SHARED IMPORTED GLOBAL)
set_target_properties(
  duckdb
  PROPERTIES IMPORTED_LOCATION "${_duckdb_lib}" INTERFACE_INCLUDE_DIRECTORIES "${_duckdb_inc}"
)

get_filename_component(_duckdb_libdir "${_duckdb_lib}" DIRECTORY)

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

add_library(
  memly_core STATIC
  ${APP_CPP}
  ${APP_HDR}
  ${BRIDGE_CPP}
  ${BRIDGE_HDR}
  ${LANGUAGE_CPP}
  ${LANGUAGE_HDR}
  ${MODEL_CPP}
  ${MODEL_HDR}
  ${SCHEDULER_CPP}
  ${SCHEDULER_HDR}
  ${SQL_CPP}
  ${SQL_HDR}
  ${SUPPORT_CPP}
  ${SUPPORT_HDR}
  $<TARGET_OBJECTS:memly_sql_resources>
)

target_include_directories(memly_core BEFORE PUBLIC "${CMAKE_SOURCE_DIR}/src" "${_duckdb_wrapper_dir}")

target_include_directories(memly_core SYSTEM PUBLIC "${_duckdb_inc}")

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

if(BUILD_TESTING)
  qt_add_executable(
    memly-test
    MANUAL_FINALIZATION
    ${TEST_MAIN_CPP}
    ${TEST_APP_CPP}
    ${TEST_BRIDGE_CPP}
    ${TEST_LANGUAGE_CPP}
    ${TEST_MODEL_CPP}
    ${TEST_SCHEDULER_CPP}
    ${TEST_QML_CPP}
    ${TEST_SQL_CPP}
    ${TEST_SUPPORT_CPP}
    ${TEST_PLATFORM_CPP}
  )

  target_include_directories(memly-test BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/src" "${_duckdb_wrapper_dir}")

  target_include_directories(memly-test SYSTEM PRIVATE "${_duckdb_inc}")

  target_compile_definitions(memly-test PRIVATE CMAKE_GENERATED_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

  target_link_libraries(memly-test PRIVATE memly_qml Qt6::Test memly_warnings memly_asan)

  set_target_properties(memly-test PROPERTIES BUILD_RPATH "${_duckdb_libdir}")

  qt_finalize_executable(memly-test)

  add_test(NAME memly-test COMMAND memly-test)
endif()
