cmake_minimum_required(VERSION 3.25)
project(memly LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(Qt6 REQUIRED COMPONENTS Quick Qml Network Core)

qt_standard_project_setup(REQUIRES 6.9)

# App
file(GLOB_RECURSE APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Qt/App/*.cpp")
file(GLOB_RECURSE APP_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Qt/App/*.[Hh]*")
list(REMOVE_ITEM APP_CPP "${CMAKE_SOURCE_DIR}/src/Qt/App/main.cpp")

# Utility
file(GLOB_RECURSE UTIL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Utility/*.cpp")
file(GLOB_RECURSE UTIL_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Utility/*.[Hh]*")

# QML files
set(QML_DIR "${CMAKE_SOURCE_DIR}/src/Qt/Qml")
file(GLOB_RECURSE QT_QML CONFIGURE_DEPENDS "${QML_DIR}/*.qml")
foreach(qml IN LISTS QT_QML)
  file(RELATIVE_PATH rel "${QML_DIR}" "${qml}")
  set_source_files_properties("${qml}" PROPERTIES QT_RESOURCE_ALIAS "${rel}")
endforeach()

# Sql
file(GLOB SQL_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Sql/Migrations/*.sql" "${CMAKE_SOURCE_DIR}/src/Sql/Statements/*.sql")

# Core
file(GLOB_RECURSE CORE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Core/*.cpp")
file(GLOB_RECURSE CORE_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Core/*.[Hh]*")

# TransactionScripts
file(GLOB_RECURSE TS_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/TransactionScripts/*.cpp")
file(GLOB_RECURSE TS_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/TransactionScripts/*.[Hh]*")

set(ALL_SOURCES
    ${APP_CPP}
    ${APP_HDR}
    ${UTIL_CPP}
    ${UTIL_HDR}
    ${TS_CPP}
    ${TS_HDR}
    ${CORE_CPP}
    ${CORE_HDR})

include(FetchContent)

# ====================== DuckDB: platform-specific binaries
# ======================

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

FetchContent_Declare(
  libduckdb
  URL "${DUCKDB_URL}"
  URL_HASH "SHA256=${DUCKDB_SHA}")
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
  message(FATAL_ERROR "DuckDB dylib not found in ${libduckdb_SOURCE_DIR}")
endif()
if(NOT EXISTS "${_duckdb_inc}/duckdb.hpp")
  message(FATAL_ERROR "duckdb.hpp not found in ${libduckdb_SOURCE_DIR}")
endif()

add_library(duckdb SHARED IMPORTED GLOBAL)
set_target_properties(duckdb PROPERTIES IMPORTED_LOCATION "${_duckdb_lib}" INTERFACE_INCLUDE_DIRECTORIES "${_duckdb_inc}")

get_filename_component(_duckdb_libdir "${_duckdb_lib}" DIRECTORY)
set(_duckdb_rpath_dirs "${_duckdb_libdir}")

qt_add_executable(memly MANUAL_FINALIZATION ${ALL_SOURCES} ${CMAKE_SOURCE_DIR}/src/Qt/App/main.cpp)

if(COMMAND qt_policy)
  qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(
  memly
  URI
  Memly
  VERSION
  1.0
  RESOURCE_PREFIX
  "/"
  QML_FILES
  ${QT_QML})

qt_add_resources(
  memly
  "Sql"
  PREFIX
  "/Memly"
  BASE
  "${CMAKE_SOURCE_DIR}/src/Sql"
  FILES
  ${SQL_FILES})

target_include_directories(memly PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_compile_features(memly PUBLIC cxx_std_23)
set_target_properties(memly PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

target_compile_options(
  memly
  PRIVATE -Wall
          -Wextra
          -Wunused
          -pedantic
          -Werror
          $<$<CONFIG:Debug>:-fsanitize=address;
          -fno-omit-frame-pointer>)

target_link_options(memly PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)

target_link_libraries(memly PRIVATE Qt6::Quick Qt6::Qml Qt6::Network Qt6::Core duckdb)
set_target_properties(memly PROPERTIES BUILD_RPATH "${_duckdb_rpath_dirs}")

qt_import_qml_plugins(memly)
qt_finalize_executable(memly)

# ======================== Tests: CTest + GoogleTest ===========================

include(CTest)
find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/*.cpp")
file(GLOB TEST_MACOS_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/MacOS/*.cpp")

set(CORE_SOURCES ${UTIL_CPP} ${CORE_CPP} ${TS_CPP} ${APP_CPP})

add_executable(test-memly ${CORE_SOURCES} ${TEST_SOURCES})

# Add macOS-only test sources
target_sources(test-memly PRIVATE $<$<PLATFORM_ID:Darwin>:${TEST_MACOS_SOURCES}>)

target_compile_definitions(test-memly PRIVATE CMAKE_GENERATED_PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

target_include_directories(test-memly PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_compile_features(test-memly PUBLIC cxx_std_23)

target_compile_options(test-memly PRIVATE -Wall -Wextra -Wunused -pedantic -Werror $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>)

target_link_options(test-memly PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)

target_link_libraries(test-memly PRIVATE GTest::gtest_main Qt6::Quick Qt6::Qml Qt6::Network Qt6::Core duckdb)

set_target_properties(test-memly PROPERTIES BUILD_RPATH "${_duckdb_rpath_dirs}")

gtest_discover_tests(test-memly)
