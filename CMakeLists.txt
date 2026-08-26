cmake_minimum_required(VERSION 4.4)
project(memly LANGUAGES CXX)

set(MEMLY_LLVM_MAJOR_VERSION 22)

if(CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
  message(FATAL_ERROR "Memly's Android and iOS toolchain contracts have not been defined yet")
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(Darwin|Linux|Windows)$")
  message(FATAL_ERROR "Memly currently supports desktop builds targeting macOS, Linux, or Windows")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "Memly desktop builds require upstream LLVM Clang ${MEMLY_LLVM_MAJOR_VERSION}.x; "
                      "configured compiler is ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
  )
endif()
string(REGEX MATCH "^[0-9]+" MEMLY_COMPILER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
if(NOT MEMLY_COMPILER_MAJOR_VERSION STREQUAL "${MEMLY_LLVM_MAJOR_VERSION}")
  message(FATAL_ERROR "Memly desktop builds require LLVM Clang ${MEMLY_LLVM_MAJOR_VERSION}.x; "
                      "configured version is ${CMAKE_CXX_COMPILER_VERSION}"
  )
endif()

if(WIN32)
  set(MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT MSVC)
else()
  set(MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT GNU)
endif()
if(NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "${MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT}")
  message(
    FATAL_ERROR "Memly requires Clang's ${MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT} frontend on ${CMAKE_SYSTEM_NAME}; "
                "configured frontend is ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}"
  )
endif()

if(NOT CMAKE_CXX_COMPILER_LINKER_ID STREQUAL "LLD")
  message(
    FATAL_ERROR
      "Memly desktop builds require LLVM LLD ${MEMLY_LLVM_MAJOR_VERSION}.x; "
      "configured linker is ${CMAKE_CXX_COMPILER_LINKER_ID} ${CMAKE_CXX_COMPILER_LINKER_VERSION}. "
      "Configure with CMAKE_LINKER_TYPE=LLD."
  )
endif()
string(REGEX MATCH "^[0-9]+" MEMLY_LINKER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_LINKER_VERSION}")
if(NOT MEMLY_LINKER_MAJOR_VERSION STREQUAL "${MEMLY_LLVM_MAJOR_VERSION}")
  message(FATAL_ERROR "Memly desktop builds require LLVM LLD ${MEMLY_LLVM_MAJOR_VERSION}.x; "
                      "configured version is ${CMAKE_CXX_COMPILER_LINKER_VERSION}"
  )
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(QT_QML_GENERATE_QMLLS_INI ON)

include(FetchContent)

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
  message(FATAL_ERROR "Memly requires a Ninja generator for its mandatory full-tree Clang-Tidy gate")
endif()

set(CMAKE_CXX_COMPILE_OPTIONS_IPO "-flto=full")
include(CheckIPOSupported)
check_ipo_supported(LANGUAGES CXX)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  add_compile_options(
    "$<$<CONFIG:Release>:/clang:-fvisibility=hidden>" "$<$<CONFIG:Release>:/clang:-fvisibility-inlines-hidden>"
    "$<$<CONFIG:Release>:/clang:-fwhole-program-vtables>" "$<$<CONFIG:Release>:/clang:-fvirtual-function-elimination>"
  )
  add_link_options("$<$<CONFIG:Release>:/opt:lldlto=3>" "$<$<CONFIG:Release>:/opt:lldltocgo=3>")
else()
  add_compile_options(
    "$<$<CONFIG:Release>:-fvisibility=hidden>" "$<$<CONFIG:Release>:-fvisibility-inlines-hidden>"
    "$<$<CONFIG:Release>:-fwhole-program-vtables>" "$<$<CONFIG:Release>:-fvirtual-function-elimination>"
  )
  add_link_options("$<$<CONFIG:Release>:LINKER:--lto-O3>" "$<$<CONFIG:Release>:LINKER:--lto-CGO3>")
endif()

cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH MEMLY_LLVM_BINARY_DIRECTORY)
cmake_path(GET MEMLY_LLVM_BINARY_DIRECTORY PARENT_PATH MEMLY_LLVM_INSTALL_DIRECTORY)

# Finds a tool in the configured LLVM installation and verifies its major version.
function(memly_require_llvm_tool MEMLY_LLVM_TOOL_EXECUTABLE_VARIABLE MEMLY_LLVM_TOOL_NAME)
  find_program(
    "${MEMLY_LLVM_TOOL_EXECUTABLE_VARIABLE}"
    NAMES ${ARGN}
    HINTS "${MEMLY_LLVM_BINARY_DIRECTORY}"
    NO_DEFAULT_PATH REQUIRED
  )
  cmake_path(GET ${MEMLY_LLVM_TOOL_EXECUTABLE_VARIABLE} PARENT_PATH MEMLY_LLVM_TOOL_BINARY_DIRECTORY)
  if(NOT MEMLY_LLVM_TOOL_BINARY_DIRECTORY STREQUAL MEMLY_LLVM_BINARY_DIRECTORY)
    message(FATAL_ERROR "Memly requires ${MEMLY_LLVM_TOOL_NAME} from the configured LLVM compiler installation")
  endif()
  execute_process(
    COMMAND "${${MEMLY_LLVM_TOOL_EXECUTABLE_VARIABLE}}" --version
    RESULT_VARIABLE MEMLY_LLVM_TOOL_VERSION_RESULT
    OUTPUT_VARIABLE MEMLY_LLVM_TOOL_VERSION_OUTPUT
    ERROR_VARIABLE MEMLY_LLVM_TOOL_VERSION_ERROR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT MEMLY_LLVM_TOOL_VERSION_RESULT EQUAL 0)
    message(FATAL_ERROR "Unable to read ${MEMLY_LLVM_TOOL_NAME} version: ${MEMLY_LLVM_TOOL_VERSION_ERROR}")
  endif()
  string(REGEX MATCH "version ([0-9]+)" MEMLY_LLVM_TOOL_VERSION_MATCH "${MEMLY_LLVM_TOOL_VERSION_OUTPUT}")
  if(NOT MEMLY_LLVM_TOOL_VERSION_MATCH OR NOT CMAKE_MATCH_1 STREQUAL "${MEMLY_LLVM_MAJOR_VERSION}")
    message(
      FATAL_ERROR
        "Memly requires ${MEMLY_LLVM_TOOL_NAME} ${MEMLY_LLVM_MAJOR_VERSION}.x from the configured LLVM toolchain; "
        "found ${MEMLY_LLVM_TOOL_VERSION_OUTPUT}"
    )
  endif()
  set(${MEMLY_LLVM_TOOL_EXECUTABLE_VARIABLE} "${${MEMLY_LLVM_TOOL_EXECUTABLE_VARIABLE}}" PARENT_SCOPE)
endfunction()

memly_require_llvm_tool(MEMLY_LLVM_CLANG_TIDY_EXECUTABLE clang-tidy "clang-tidy-${MEMLY_LLVM_MAJOR_VERSION}" clang-tidy)
memly_require_llvm_tool(MEMLY_LLVM_CLANGD_EXECUTABLE clangd "clangd-${MEMLY_LLVM_MAJOR_VERSION}" clangd)
memly_require_llvm_tool(
  MEMLY_LLVM_CLANG_FORMAT_EXECUTABLE clang-format "clang-format-${MEMLY_LLVM_MAJOR_VERSION}" clang-format
)
find_program(
  MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE
  NAMES "run-clang-tidy-${MEMLY_LLVM_MAJOR_VERSION}" run-clang-tidy run-clang-tidy.py
  HINTS "${MEMLY_LLVM_BINARY_DIRECTORY}" "${MEMLY_LLVM_INSTALL_DIRECTORY}/share/clang"
  NO_DEFAULT_PATH REQUIRED
)
set(MEMLY_CLANG_TIDY_JOBS 4 CACHE STRING "Maximum concurrent Clang-Tidy processes")
if(NOT MEMLY_CLANG_TIDY_JOBS MATCHES "^[1-9][0-9]*$")
  message(FATAL_ERROR "MEMLY_CLANG_TIDY_JOBS must be a positive integer")
endif()
set_property(GLOBAL APPEND PROPERTY JOB_POOLS "memly_clang_tidy=${MEMLY_CLANG_TIDY_JOBS}")

find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(MEMLY_CMAKE_TOOL_DIRECTORY "${CMAKE_SOURCE_DIR}/tool/cmake")

# Runs a lint/build source verifier during configuration and tracks changes to its implementation.
function(memly_run_source_verifier MEMLY_VERIFIER_FILE_NAME)
  set(MEMLY_VERIFIER_PATH "${MEMLY_CMAKE_TOOL_DIRECTORY}/${MEMLY_VERIFIER_FILE_NAME}")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${MEMLY_VERIFIER_PATH}")
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${MEMLY_VERIFIER_PATH}" "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE MEMLY_VERIFIER_RESULT
    ERROR_VARIABLE MEMLY_VERIFIER_ERROR
    ERROR_STRIP_TRAILING_WHITESPACE
  )
  if(NOT MEMLY_VERIFIER_RESULT EQUAL 0)
    message(FATAL_ERROR "${MEMLY_VERIFIER_ERROR}")
  endif()
endfunction()

# Makes authored-file additions, removals, and renames rerun the source-extension verifier.
file(GLOB_RECURSE MEMLY_SOURCE_POLICY_CHANGE_TRACKING_PATHS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/*"
     "${CMAKE_SOURCE_DIR}/test/*"
)
memly_run_source_verifier(verify_source_extensions.py)

if(WIN32)
  set(MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy-driver.cmd")
  file(GENERATE OUTPUT "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
       CONTENT "@echo off\r\n\"${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}\" --experimental-custom-checks %*\r\n"
  )
else()
  set(MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy-driver")
  file(
    GENERATE
    OUTPUT "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
    CONTENT "#!/bin/sh\nexec \"${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}\" --experimental-custom-checks \"$@\"\n"
            FILE_PERMISSIONS
            OWNER_READ
            OWNER_WRITE
            OWNER_EXECUTE
            GROUP_READ
            GROUP_EXECUTE
            WORLD_READ
            WORLD_EXECUTE
            NEWLINE_STYLE
            UNIX
  )
endif()

set(MEMLY_QT_MIN_VERSION 6.11)

# Ensures an explicitly selected ICU installation supplies every discovered header and library.
function(memly_validate_explicit_icu_root)
  if(NOT ICU_ROOT)
    return()
  endif()

  get_filename_component(MEMLY_ICU_ROOT_REAL_PATH "${ICU_ROOT}" REALPATH)
  set(MEMLY_ICU_DISCOVERED_PATHS ${ICU_INCLUDE_DIRS} ${ICU_LIBRARIES})
  list(FILTER MEMLY_ICU_DISCOVERED_PATHS EXCLUDE REGEX "^(debug|general|optimized)$")
  list(REMOVE_DUPLICATES MEMLY_ICU_DISCOVERED_PATHS)
  foreach(MEMLY_ICU_DISCOVERED_PATH IN LISTS MEMLY_ICU_DISCOVERED_PATHS)
    get_filename_component(MEMLY_ICU_DISCOVERED_REAL_PATH "${MEMLY_ICU_DISCOVERED_PATH}" REALPATH)
    cmake_path(
      IS_PREFIX MEMLY_ICU_ROOT_REAL_PATH "${MEMLY_ICU_DISCOVERED_REAL_PATH}" NORMALIZE
      MEMLY_ICU_DISCOVERED_PATH_IS_WITHIN_ROOT
    )
    if(NOT MEMLY_ICU_DISCOVERED_PATH_IS_WITHIN_ROOT)
      message(FATAL_ERROR "ICU discovery resolved ${MEMLY_ICU_DISCOVERED_REAL_PATH} outside explicit ICU_ROOT "
                          "${MEMLY_ICU_ROOT_REAL_PATH}"
      )
    endif()
  endforeach()
endfunction()

find_package(Qt6 ${MEMLY_QT_MIN_VERSION} REQUIRED COMPONENTS Core Qml Quick)
find_package(ICU REQUIRED COMPONENTS uc i18n)
memly_validate_explicit_icu_root()
qt_standard_project_setup(REQUIRES ${MEMLY_QT_MIN_VERSION})

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  add_custom_target(
    memly-documentation
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/Doxyfile"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Generate Doxygen documentation"
    VERBATIM
  )
else()
  message(STATUS "Doxygen not found; memly-documentation target will not be available")
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

FetchContent_Declare(
  libduckdb
  URL "${DUCKDB_URL}"
  URL_HASH "SHA256=${DUCKDB_SHA}"
  DOWNLOAD_NO_PROGRESS TRUE
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

message(STATUS "Found DuckDB: ${_duckdb_lib} (found version \"${DUCKDB_VERSION}\")")

set(_duckdb_wrapper_dir "${CMAKE_BINARY_DIR}/duckdb_wrapper")
file(MAKE_DIRECTORY "${_duckdb_wrapper_dir}")
file(GENERATE OUTPUT "${_duckdb_wrapper_dir}/duckdb.hpp"
     CONTENT "#pragma GCC system_header\n#include \"${_duckdb_inc}/duckdb.hpp\"\n" NEWLINE_STYLE UNIX
)

add_library(duckdb SHARED IMPORTED)
set_target_properties(
  duckdb
  PROPERTIES IMPORTED_LOCATION "${_duckdb_lib}" INTERFACE_INCLUDE_DIRECTORIES "${_duckdb_inc}"
)

add_library(memly_asan INTERFACE)
target_compile_options(
  memly_asan
  INTERFACE $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>
)
target_link_options(
  memly_asan INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>
)

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  set(MEMLY_CLANG_WARNING_OPTIONS
      /clang:-Weverything
      /clang:-pedantic
      /clang:-Wno-c++98-compat
      /clang:-Wno-c++98-compat-pedantic
      /clang:-Wno-c++98-compat-bind-to-temporary-copy
      /clang:-Wno-pre-c++14-compat
      /clang:-Wno-pre-c++17-compat
      /clang:-Wno-pre-c++20-compat
      /clang:-Wno-pre-c++23-compat
      /clang:-Wno-pre-c++26-compat
      /clang:-Wno-padded
      /clang:-Wno-covered-switch-default
      /WX
  )
else()
  set(MEMLY_CLANG_WARNING_OPTIONS
      -Weverything
      -pedantic
      -Wno-c++98-compat
      -Wno-c++98-compat-pedantic
      -Wno-c++98-compat-bind-to-temporary-copy
      -Wno-pre-c++14-compat
      -Wno-pre-c++17-compat
      -Wno-pre-c++20-compat
      -Wno-pre-c++23-compat
      -Wno-pre-c++26-compat
      -Wno-padded
      -Wno-covered-switch-default
      -Werror
  )
endif()

# Registers authored C++ files and applies Memly's strict warning policy in the directory scope where their owning
# target was created.
function(memly_configure_authored_cxx_files MEMLY_TARGET)
  foreach(MEMLY_CLANG_WARNING_OPTION IN LISTS MEMLY_CLANG_WARNING_OPTIONS)
    set_property(
      SOURCE ${ARGN} TARGET_DIRECTORY "${MEMLY_TARGET}" APPEND
      PROPERTY COMPILE_OPTIONS "$<$<COMPILE_LANG_AND_ID:CXX,Clang>:${MEMLY_CLANG_WARNING_OPTION}>"
    )
  endforeach()
  set_property(GLOBAL APPEND PROPERTY MEMLY_REGISTERED_CXX_FILES ${ARGN})
  set_property(TARGET "${MEMLY_TARGET}" APPEND PROPERTY MEMLY_AUTHORED_CXX_FILES ${ARGN})
endfunction()

# Computes and validates one component-owned named module interface.
function(memly_get_module_interface_metadata MEMLY_MODULE_NAME_OUTPUT MEMLY_MODULE_VISIBILITY_OUTPUT
         MEMLY_MODULE_PREFIX MEMLY_MODULE_INTERFACE
)
  if(NOT MEMLY_MODULE_INTERFACE MATCHES "[.]cppm$")
    message(FATAL_ERROR "Memly module interface must use .cppm: ${MEMLY_MODULE_INTERFACE}")
  endif()

  string(REPLACE "\\" "/" MEMLY_MODULE_INTERFACE_PATH "${MEMLY_MODULE_INTERFACE}")
  cmake_path(GET MEMLY_MODULE_INTERFACE_PATH STEM MEMLY_MODULE_INTERFACE_STEM)
  if(MEMLY_MODULE_INTERFACE_PATH MATCHES "/module/[^/]+[.]cppm$")
    set(MEMLY_MODULE_VISIBILITY PUBLIC)
    set(MEMLY_EXPECTED_MODULE_NAME "${MEMLY_MODULE_PREFIX}.${MEMLY_MODULE_INTERFACE_STEM}")
  elseif(MEMLY_MODULE_INTERFACE_PATH MATCHES "/module/Internal/_[^/]+[.]cppm$")
    set(MEMLY_MODULE_VISIBILITY PRIVATE)
    string(REGEX REPLACE "^_" "" MEMLY_INTERNAL_MODULE_INTERFACE_STEM "${MEMLY_MODULE_INTERFACE_STEM}")
    set(MEMLY_EXPECTED_MODULE_NAME "${MEMLY_MODULE_PREFIX}.Internal.${MEMLY_INTERNAL_MODULE_INTERFACE_STEM}")
  else()
    message(FATAL_ERROR "Memly module interfaces must be stored directly under module/ or use an _ filename prefix "
                        "directly under module/Internal/: ${MEMLY_MODULE_INTERFACE}"
    )
  endif()

  file(STRINGS "${MEMLY_MODULE_INTERFACE}" MEMLY_MODULE_DECLARATION_LINES
       REGEX "^[ \t]*export[ \t]+module[ \t]+Memly([.][A-Za-z_][A-Za-z0-9_]*)+[ \t]*;"
  )
  list(LENGTH MEMLY_MODULE_DECLARATION_LINES MEMLY_MODULE_DECLARATION_COUNT)
  if(NOT MEMLY_MODULE_DECLARATION_COUNT EQUAL 1)
    message(FATAL_ERROR "Memly module interface ${MEMLY_MODULE_INTERFACE} must declare exactly one named module")
  endif()
  list(GET MEMLY_MODULE_DECLARATION_LINES 0 MEMLY_MODULE_DECLARATION_LINE)
  string(REGEX REPLACE "^[ \t]*export[ \t]+module[ \t]+([^ \t;]+)[ \t]*;.*$" "\\1" MEMLY_MODULE_NAME
                       "${MEMLY_MODULE_DECLARATION_LINE}"
  )
  if(NOT MEMLY_MODULE_NAME STREQUAL MEMLY_EXPECTED_MODULE_NAME)
    message(FATAL_ERROR "Memly interface ${MEMLY_MODULE_INTERFACE} must declare ${MEMLY_EXPECTED_MODULE_NAME}, not "
                        "${MEMLY_MODULE_NAME}"
    )
  endif()

  set(${MEMLY_MODULE_NAME_OUTPUT} "${MEMLY_MODULE_NAME}" PARENT_SCOPE)
  set(${MEMLY_MODULE_VISIBILITY_OUTPUT} "${MEMLY_MODULE_VISIBILITY}" PARENT_SCOPE)
endfunction()

# Finds the target, visibility, and interface that own one registered named module.
function(memly_get_module_provider MEMLY_MODULE_TARGET_OUTPUT MEMLY_MODULE_VISIBILITY_OUTPUT
         MEMLY_MODULE_INTERFACE_OUTPUT MEMLY_MODULE_NAME
)
  get_property(MEMLY_MODULE_NAMES GLOBAL PROPERTY MEMLY_MODULE_NAMES)
  get_property(MEMLY_MODULE_PROVIDER_TARGETS GLOBAL PROPERTY MEMLY_MODULE_PROVIDER_TARGETS)
  get_property(MEMLY_MODULE_VISIBILITIES GLOBAL PROPERTY MEMLY_MODULE_VISIBILITIES)
  get_property(MEMLY_MODULE_INTERFACES GLOBAL PROPERTY MEMLY_MODULE_INTERFACE_FILES)
  list(FIND MEMLY_MODULE_NAMES "${MEMLY_MODULE_NAME}" MEMLY_MODULE_INDEX)
  if(MEMLY_MODULE_INDEX EQUAL -1)
    message(FATAL_ERROR "Unknown Memly named module: ${MEMLY_MODULE_NAME}")
  endif()
  list(GET MEMLY_MODULE_PROVIDER_TARGETS "${MEMLY_MODULE_INDEX}" MEMLY_MODULE_TARGET)
  list(GET MEMLY_MODULE_VISIBILITIES "${MEMLY_MODULE_INDEX}" MEMLY_MODULE_VISIBILITY)
  list(GET MEMLY_MODULE_INTERFACES "${MEMLY_MODULE_INDEX}" MEMLY_MODULE_INTERFACE)
  set(${MEMLY_MODULE_TARGET_OUTPUT} "${MEMLY_MODULE_TARGET}" PARENT_SCOPE)
  set(${MEMLY_MODULE_VISIBILITY_OUTPUT} "${MEMLY_MODULE_VISIBILITY}" PARENT_SCOPE)
  set(${MEMLY_MODULE_INTERFACE_OUTPUT} "${MEMLY_MODULE_INTERFACE}" PARENT_SCOPE)
endfunction()

# Validates that an implementation unit belongs to an interface owned by its component target.
function(memly_validate_module_implementation MEMLY_COMPONENT_TARGET MEMLY_MODULE_SOURCE)
  if(NOT MEMLY_MODULE_SOURCE MATCHES "[.]cpp$")
    message(FATAL_ERROR "Memly module implementation must use .cpp: ${MEMLY_MODULE_SOURCE}")
  endif()
  file(STRINGS "${MEMLY_MODULE_SOURCE}" MEMLY_MODULE_DECLARATION_LINES
       REGEX "^[ \t]*module[ \t]+Memly([.][A-Za-z_][A-Za-z0-9_]*)+[ \t]*;"
  )
  list(LENGTH MEMLY_MODULE_DECLARATION_LINES MEMLY_MODULE_DECLARATION_COUNT)
  if(NOT MEMLY_MODULE_DECLARATION_COUNT EQUAL 1)
    message(FATAL_ERROR "Memly implementation ${MEMLY_MODULE_SOURCE} must declare exactly one named module")
  endif()
  list(GET MEMLY_MODULE_DECLARATION_LINES 0 MEMLY_MODULE_DECLARATION_LINE)
  string(REGEX REPLACE "^[ \t]*module[ \t]+([^ \t;]+)[ \t]*;.*$" "\\1" MEMLY_MODULE_NAME
                       "${MEMLY_MODULE_DECLARATION_LINE}"
  )
  memly_get_module_provider(MEMLY_PROVIDER_TARGET MEMLY_MODULE_VISIBILITY MEMLY_MODULE_INTERFACE "${MEMLY_MODULE_NAME}")
  if(NOT MEMLY_PROVIDER_TARGET STREQUAL MEMLY_COMPONENT_TARGET)
    message(FATAL_ERROR "Memly implementation ${MEMLY_MODULE_SOURCE} belongs to ${MEMLY_MODULE_NAME}, which is "
                        "owned by ${MEMLY_PROVIDER_TARGET} instead of ${MEMLY_COMPONENT_TARGET}"
    )
  endif()

  set(MEMLY_MODULE_SOURCE_PATH "${MEMLY_MODULE_SOURCE}")
  cmake_path(GET MEMLY_MODULE_SOURCE_PATH STEM MEMLY_MODULE_SOURCE_STEM)
  set(MEMLY_MODULE_INTERFACE_PATH "${MEMLY_MODULE_INTERFACE}")
  cmake_path(GET MEMLY_MODULE_INTERFACE_PATH STEM MEMLY_MODULE_INTERFACE_STEM)
  if(NOT MEMLY_MODULE_SOURCE_STEM STREQUAL MEMLY_MODULE_INTERFACE_STEM)
    message(FATAL_ERROR "Memly implementation ${MEMLY_MODULE_SOURCE} does not correspond to ${MEMLY_MODULE_INTERFACE}")
  endif()
  string(REPLACE "\\" "/" MEMLY_MODULE_SOURCE_PATH "${MEMLY_MODULE_SOURCE}")
  if(MEMLY_MODULE_VISIBILITY STREQUAL "PRIVATE")
    if(NOT MEMLY_MODULE_SOURCE_PATH MATCHES "/module/Internal/_[^/]+[.]cpp$")
      message(FATAL_ERROR "Internal module implementations must use an _ filename prefix directly under "
                          "module/Internal/: ${MEMLY_MODULE_SOURCE}"
      )
    endif()
  elseif(NOT MEMLY_MODULE_SOURCE_PATH MATCHES "/module/[^/]+[.]cpp$")
    message(FATAL_ERROR "Public module implementation units must be stored directly under module/: "
                        "${MEMLY_MODULE_SOURCE}"
    )
  endif()
endfunction()

# Pushes one preprocessor condition onto a Memly import scan.
function(memly_push_import_scan_condition MEMLY_CONDITION_STATE)
  list(APPEND MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK "${MEMLY_PREPROCESSOR_BRANCH_ACTIVE}")
  list(APPEND MEMLY_PREPROCESSOR_CONDITION_STATE_STACK "${MEMLY_CONDITION_STATE}")
  if(MEMLY_CONDITION_STATE STREQUAL "ALWAYS_SKIPPED")
    set(MEMLY_PREPROCESSOR_BRANCH_ACTIVE FALSE)
  endif()
  set(MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK "${MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK}" PARENT_SCOPE)
  set(MEMLY_PREPROCESSOR_CONDITION_STATE_STACK "${MEMLY_PREPROCESSOR_CONDITION_STATE_STACK}" PARENT_SCOPE)
  set(MEMLY_PREPROCESSOR_BRANCH_ACTIVE "${MEMLY_PREPROCESSOR_BRANCH_ACTIVE}" PARENT_SCOPE)
endfunction()

# Selects an elif or else branch while conservatively scanning unknown conditions.
function(memly_select_import_scan_alternative MEMLY_IMPORTER_FILE MEMLY_IMPORTER_LINE MEMLY_ALTERNATIVE_KIND)
  list(LENGTH MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK MEMLY_PREPROCESSOR_NESTING_DEPTH)
  if(MEMLY_PREPROCESSOR_NESTING_DEPTH EQUAL 0)
    message(FATAL_ERROR "Unmatched #${MEMLY_ALTERNATIVE_KIND} in Memly source: ${MEMLY_IMPORTER_FILE}")
  endif()
  list(GET MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK -1 MEMLY_PARENT_ACTIVE)
  list(GET MEMLY_PREPROCESSOR_CONDITION_STATE_STACK -1 MEMLY_CONDITION_STATE)
  if(MEMLY_ALTERNATIVE_KIND STREQUAL "else")
    if(MEMLY_PARENT_ACTIVE AND NOT MEMLY_CONDITION_STATE STREQUAL "ALWAYS_TAKEN")
      set(MEMLY_BRANCH_ACTIVE TRUE)
    else()
      set(MEMLY_BRANCH_ACTIVE FALSE)
    endif()
    set(MEMLY_CONDITION_STATE "ALWAYS_TAKEN")
  elseif(NOT MEMLY_PARENT_ACTIVE OR MEMLY_CONDITION_STATE STREQUAL "ALWAYS_TAKEN" OR MEMLY_IMPORTER_LINE MATCHES
                                                                                     "^#[ \t]*elif[ \t]+0([ \t]|$)"
  )
    set(MEMLY_BRANCH_ACTIVE FALSE)
  else()
    set(MEMLY_BRANCH_ACTIVE TRUE)
    if(MEMLY_IMPORTER_LINE MATCHES "^#[ \t]*elif[ \t]+1([ \t]|$)")
      set(MEMLY_CONDITION_STATE "ALWAYS_TAKEN")
    else()
      set(MEMLY_CONDITION_STATE "CONDITIONAL")
    endif()
  endif()
  list(POP_BACK MEMLY_PREPROCESSOR_CONDITION_STATE_STACK)
  list(APPEND MEMLY_PREPROCESSOR_CONDITION_STATE_STACK "${MEMLY_CONDITION_STATE}")
  set(MEMLY_PREPROCESSOR_CONDITION_STATE_STACK "${MEMLY_PREPROCESSOR_CONDITION_STATE_STACK}" PARENT_SCOPE)
  set(MEMLY_PREPROCESSOR_BRANCH_ACTIVE "${MEMLY_BRANCH_ACTIVE}" PARENT_SCOPE)
endfunction()

# Pops one completed preprocessor condition from a Memly import scan.
function(memly_pop_import_scan_condition MEMLY_IMPORTER_FILE)
  list(LENGTH MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK MEMLY_PREPROCESSOR_NESTING_DEPTH)
  if(MEMLY_PREPROCESSOR_NESTING_DEPTH EQUAL 0)
    message(FATAL_ERROR "Unmatched #endif in Memly source: ${MEMLY_IMPORTER_FILE}")
  endif()
  list(POP_BACK MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK MEMLY_PREPROCESSOR_BRANCH_ACTIVE)
  list(POP_BACK MEMLY_PREPROCESSOR_CONDITION_STATE_STACK)
  set(MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK "${MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK}" PARENT_SCOPE)
  set(MEMLY_PREPROCESSOR_CONDITION_STATE_STACK "${MEMLY_PREPROCESSOR_CONDITION_STATE_STACK}" PARENT_SCOPE)
  set(MEMLY_PREPROCESSOR_BRANCH_ACTIVE "${MEMLY_PREPROCESSOR_BRANCH_ACTIVE}" PARENT_SCOPE)
endfunction()

# Reads the exact Memly named modules that can be active in one authored translation unit.
function(memly_read_imported_module_names MEMLY_OUTPUT_VARIABLE MEMLY_IMPORTER_FILE)
  file(STRINGS "${MEMLY_IMPORTER_FILE}" MEMLY_IMPORTER_LINES)
  set(MEMLY_IMPORTED_MODULE_NAMES)
  set(MEMLY_PREPROCESSOR_BRANCH_ACTIVE TRUE)
  set(MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK)
  set(MEMLY_PREPROCESSOR_CONDITION_STATE_STACK)
  foreach(MEMLY_IMPORTER_LINE IN LISTS MEMLY_IMPORTER_LINES)
    string(STRIP "${MEMLY_IMPORTER_LINE}" MEMLY_STRIPPED_IMPORTER_LINE)
    if(MEMLY_STRIPPED_IMPORTER_LINE MATCHES "^#[ \t]*if[ \t]+0([ \t]|$)")
      set(MEMLY_CONDITION_STATE "ALWAYS_SKIPPED")
    elseif(MEMLY_STRIPPED_IMPORTER_LINE MATCHES "^#[ \t]*if[ \t]+1([ \t]|$)")
      set(MEMLY_CONDITION_STATE "ALWAYS_TAKEN")
    elseif(MEMLY_STRIPPED_IMPORTER_LINE MATCHES "^#[ \t]*(if|ifdef|ifndef)([ \t]|$)")
      set(MEMLY_CONDITION_STATE "CONDITIONAL")
    elseif(MEMLY_STRIPPED_IMPORTER_LINE MATCHES "^#[ \t]*elif([ \t]|$)")
      memly_select_import_scan_alternative("${MEMLY_IMPORTER_FILE}" "${MEMLY_STRIPPED_IMPORTER_LINE}" elif)
      continue()
    elseif(MEMLY_STRIPPED_IMPORTER_LINE MATCHES "^#[ \t]*else([ \t]|$)")
      memly_select_import_scan_alternative("${MEMLY_IMPORTER_FILE}" "${MEMLY_STRIPPED_IMPORTER_LINE}" else)
      continue()
    elseif(MEMLY_STRIPPED_IMPORTER_LINE MATCHES "^#[ \t]*endif([ \t]|$)")
      memly_pop_import_scan_condition("${MEMLY_IMPORTER_FILE}")
      continue()
    elseif(MEMLY_PREPROCESSOR_BRANCH_ACTIVE AND MEMLY_STRIPPED_IMPORTER_LINE MATCHES
                                                "^(export[ \t]+)?import[ \t]+(Memly[.][A-Za-z0-9_.]+)[ \t]*;"
    )
      list(APPEND MEMLY_IMPORTED_MODULE_NAMES "${CMAKE_MATCH_2}")
      continue()
    else()
      continue()
    endif()
    memly_push_import_scan_condition("${MEMLY_CONDITION_STATE}")
  endforeach()
  if(MEMLY_PREPROCESSOR_PARENT_ACTIVE_STACK)
    message(FATAL_ERROR "Unterminated preprocessor conditional in Memly source: ${MEMLY_IMPORTER_FILE}")
  endif()
  list(REMOVE_DUPLICATES MEMLY_IMPORTED_MODULE_NAMES)
  set(${MEMLY_OUTPUT_VARIABLE} ${MEMLY_IMPORTED_MODULE_NAMES} PARENT_SCOPE)
endfunction()

# Resolves a Memly importer's transitive exact named-module closure to the interface files that define it.
function(memly_collect_imported_module_interface_closure MEMLY_OUTPUT_VARIABLE MEMLY_IMPORTER_FILE)
  get_property(MEMLY_MODULE_NAMES GLOBAL PROPERTY MEMLY_MODULE_NAMES)
  get_property(MEMLY_MODULE_INTERFACE_FILES GLOBAL PROPERTY MEMLY_MODULE_INTERFACE_FILES)
  set(MEMLY_PENDING_IMPORTER_FILES "${MEMLY_IMPORTER_FILE}")
  set(MEMLY_VISITED_MODULE_NAMES)
  set(MEMLY_IMPORTED_MODULE_INTERFACES)
  if(MEMLY_IMPORTER_FILE MATCHES "[.]cpp$")
    file(STRINGS "${MEMLY_IMPORTER_FILE}" MEMLY_IMPLEMENTATION_MODULE_DECLARATION_LINES
         REGEX "^[ \t]*module[ \t]+Memly([.][A-Za-z_][A-Za-z0-9_]*)+[ \t]*;"
    )
    list(LENGTH MEMLY_IMPLEMENTATION_MODULE_DECLARATION_LINES MEMLY_IMPLEMENTATION_MODULE_DECLARATION_COUNT)
    if(MEMLY_IMPLEMENTATION_MODULE_DECLARATION_COUNT GREATER 1)
      message(FATAL_ERROR "Memly implementation ${MEMLY_IMPORTER_FILE} declares more than one named module")
    endif()
    if(MEMLY_IMPLEMENTATION_MODULE_DECLARATION_COUNT EQUAL 1)
      list(GET MEMLY_IMPLEMENTATION_MODULE_DECLARATION_LINES 0 MEMLY_IMPLEMENTATION_MODULE_DECLARATION_LINE)
      string(REGEX REPLACE "^[ \t]*module[ \t]+([^ \t;]+)[ \t]*;.*$" "\\1" MEMLY_IMPLEMENTATION_MODULE_NAME
                           "${MEMLY_IMPLEMENTATION_MODULE_DECLARATION_LINE}"
      )
      list(FIND MEMLY_MODULE_NAMES "${MEMLY_IMPLEMENTATION_MODULE_NAME}" MEMLY_IMPLEMENTATION_MODULE_INDEX)
      if(MEMLY_IMPLEMENTATION_MODULE_INDEX EQUAL -1)
        message(FATAL_ERROR "Memly implementation ${MEMLY_IMPORTER_FILE} belongs to an unregistered module: "
                            "${MEMLY_IMPLEMENTATION_MODULE_NAME}"
        )
      endif()
      list(GET MEMLY_MODULE_INTERFACE_FILES "${MEMLY_IMPLEMENTATION_MODULE_INDEX}"
           MEMLY_IMPLEMENTATION_MODULE_INTERFACE
      )
      list(APPEND MEMLY_VISITED_MODULE_NAMES "${MEMLY_IMPLEMENTATION_MODULE_NAME}")
      list(APPEND MEMLY_IMPORTED_MODULE_INTERFACES "${MEMLY_IMPLEMENTATION_MODULE_INTERFACE}")
      list(APPEND MEMLY_PENDING_IMPORTER_FILES "${MEMLY_IMPLEMENTATION_MODULE_INTERFACE}")
    endif()
  endif()
  while(MEMLY_PENDING_IMPORTER_FILES)
    list(POP_FRONT MEMLY_PENDING_IMPORTER_FILES MEMLY_PENDING_IMPORTER_FILE)
    memly_read_imported_module_names(MEMLY_IMPORTED_MODULE_NAMES "${MEMLY_PENDING_IMPORTER_FILE}")
    foreach(MEMLY_IMPORTED_MODULE_NAME IN LISTS MEMLY_IMPORTED_MODULE_NAMES)
      if(MEMLY_IMPORTED_MODULE_NAME IN_LIST MEMLY_VISITED_MODULE_NAMES)
        continue()
      endif()
      list(APPEND MEMLY_VISITED_MODULE_NAMES "${MEMLY_IMPORTED_MODULE_NAME}")
      list(FIND MEMLY_MODULE_NAMES "${MEMLY_IMPORTED_MODULE_NAME}" MEMLY_IMPORTED_MODULE_INDEX)
      if(MEMLY_IMPORTED_MODULE_INDEX EQUAL -1)
        message(FATAL_ERROR "Memly importer ${MEMLY_PENDING_IMPORTER_FILE} names an unregistered module: "
                            "${MEMLY_IMPORTED_MODULE_NAME}"
        )
      endif()
      list(GET MEMLY_MODULE_INTERFACE_FILES "${MEMLY_IMPORTED_MODULE_INDEX}" MEMLY_IMPORTED_MODULE_INTERFACE)
      list(APPEND MEMLY_IMPORTED_MODULE_INTERFACES "${MEMLY_IMPORTED_MODULE_INTERFACE}")
      list(APPEND MEMLY_PENDING_IMPORTER_FILES "${MEMLY_IMPORTED_MODULE_INTERFACE}")
    endforeach()
  endwhile()
  list(REMOVE_DUPLICATES MEMLY_IMPORTED_MODULE_INTERFACES)
  set(${MEMLY_OUTPUT_VARIABLE} ${MEMLY_IMPORTED_MODULE_INTERFACES} PARENT_SCOPE)
endfunction()

# Adds one static component target with distinct public and internal module-interface file sets.
function(memly_add_component MEMLY_COMPONENT_TARGET MEMLY_MODULE_PREFIX)
  cmake_parse_arguments(
    PARSE_ARGV 2 MEMLY_COMPONENT "" "" "PUBLIC_MODULE_INTERFACES;INTERNAL_MODULE_INTERFACES;SOURCES"
  )
  if(MEMLY_COMPONENT_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unexpected arguments for Memly component ${MEMLY_COMPONENT_TARGET}: "
                        "${MEMLY_COMPONENT_UNPARSED_ARGUMENTS}"
    )
  endif()
  if(TARGET "${MEMLY_COMPONENT_TARGET}")
    message(FATAL_ERROR "Memly component target ${MEMLY_COMPONENT_TARGET} is provided more than once")
  endif()
  if(NOT MEMLY_COMPONENT_PUBLIC_MODULE_INTERFACES)
    message(FATAL_ERROR "Memly component ${MEMLY_COMPONENT_TARGET} must export at least one named module")
  endif()

  add_library("${MEMLY_COMPONENT_TARGET}" STATIC)
  target_sources(
    "${MEMLY_COMPONENT_TARGET}" PUBLIC FILE_SET memly_public_module_interfaces TYPE CXX_MODULES FILES
                                       ${MEMLY_COMPONENT_PUBLIC_MODULE_INTERFACES}
  )
  if(MEMLY_COMPONENT_INTERNAL_MODULE_INTERFACES)
    target_sources(
      "${MEMLY_COMPONENT_TARGET}" PRIVATE FILE_SET memly_internal_module_interfaces TYPE CXX_MODULES FILES
                                          ${MEMLY_COMPONENT_INTERNAL_MODULE_INTERFACES}
    )
  endif()
  if(MEMLY_COMPONENT_SOURCES)
    target_sources("${MEMLY_COMPONENT_TARGET}" PRIVATE ${MEMLY_COMPONENT_SOURCES})
  endif()

  foreach(MEMLY_MODULE_INTERFACE IN LISTS MEMLY_COMPONENT_PUBLIC_MODULE_INTERFACES
                                          MEMLY_COMPONENT_INTERNAL_MODULE_INTERFACES
  )
    memly_get_module_interface_metadata(
      MEMLY_MODULE_NAME MEMLY_MODULE_VISIBILITY "${MEMLY_MODULE_PREFIX}" "${MEMLY_MODULE_INTERFACE}"
    )
    get_property(MEMLY_MODULE_NAMES GLOBAL PROPERTY MEMLY_MODULE_NAMES)
    if(MEMLY_MODULE_NAME IN_LIST MEMLY_MODULE_NAMES)
      message(FATAL_ERROR "Memly named module ${MEMLY_MODULE_NAME} is provided more than once")
    endif()
    if(MEMLY_MODULE_INTERFACE IN_LIST MEMLY_COMPONENT_PUBLIC_MODULE_INTERFACES AND NOT MEMLY_MODULE_VISIBILITY STREQUAL
                                                                                   "PUBLIC"
    )
      message(FATAL_ERROR "Internal module ${MEMLY_MODULE_NAME} was registered as a public component module")
    endif()
    if(MEMLY_MODULE_INTERFACE IN_LIST MEMLY_COMPONENT_INTERNAL_MODULE_INTERFACES AND NOT MEMLY_MODULE_VISIBILITY
                                                                                     STREQUAL "PRIVATE"
    )
      message(FATAL_ERROR "Public module ${MEMLY_MODULE_NAME} was registered as an internal component module")
    endif()
    set_property(GLOBAL APPEND PROPERTY MEMLY_MODULE_NAMES "${MEMLY_MODULE_NAME}")
    set_property(GLOBAL APPEND PROPERTY MEMLY_MODULE_PROVIDER_TARGETS "${MEMLY_COMPONENT_TARGET}")
    set_property(GLOBAL APPEND PROPERTY MEMLY_MODULE_VISIBILITIES "${MEMLY_MODULE_VISIBILITY}")
    set_property(GLOBAL APPEND PROPERTY MEMLY_MODULE_INTERFACE_FILES "${MEMLY_MODULE_INTERFACE}")
  endforeach()
  foreach(MEMLY_MODULE_SOURCE IN LISTS MEMLY_COMPONENT_SOURCES)
    memly_validate_module_implementation("${MEMLY_COMPONENT_TARGET}" "${MEMLY_MODULE_SOURCE}")
  endforeach()

  set(MEMLY_COMPONENT_AUTHORED_CXX_FILES ${MEMLY_COMPONENT_PUBLIC_MODULE_INTERFACES}
                                         ${MEMLY_COMPONENT_INTERNAL_MODULE_INTERFACES} ${MEMLY_COMPONENT_SOURCES}
  )
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${MEMLY_COMPONENT_AUTHORED_CXX_FILES})
  set_target_properties("${MEMLY_COMPONENT_TARGET}" PROPERTIES AUTOMOC OFF CXX_SCAN_FOR_MODULES ON)
  memly_configure_authored_cxx_files("${MEMLY_COMPONENT_TARGET}" ${MEMLY_COMPONENT_AUTHORED_CXX_FILES})
  set_property(TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_CLANG_TIDY_FILES ${MEMLY_COMPONENT_AUTHORED_CXX_FILES})
  set_property(
    TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_PUBLIC_MODULE_INTERFACES
                                                ${MEMLY_COMPONENT_PUBLIC_MODULE_INTERFACES}
  )
  set_property(
    TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_INTERNAL_MODULE_INTERFACES
                                                ${MEMLY_COMPONENT_INTERNAL_MODULE_INTERFACES}
  )
  set_property(TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_MODULE_IMPLEMENTATION_UNITS ${MEMLY_COMPONENT_SOURCES})
  set_property(GLOBAL APPEND PROPERTY MEMLY_COMPONENT_TARGETS "${MEMLY_COMPONENT_TARGET}")
endfunction()

# Declares one component's compile dependencies while exposing only its genuine public dependency interface.
function(memly_link_component_dependencies MEMLY_COMPONENT_TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 MEMLY_DEPENDENCIES "" "" "PUBLIC;PRIVATE")
  if(MEMLY_DEPENDENCIES_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unexpected dependencies for Memly component ${MEMLY_COMPONENT_TARGET}: "
                        "${MEMLY_DEPENDENCIES_UNPARSED_ARGUMENTS}"
    )
  endif()
  get_property(MEMLY_COMPONENT_TARGETS GLOBAL PROPERTY MEMLY_COMPONENT_TARGETS)
  if(NOT MEMLY_COMPONENT_TARGET IN_LIST MEMLY_COMPONENT_TARGETS)
    message(FATAL_ERROR "Memly component dependencies were declared for an unregistered target: "
                        "${MEMLY_COMPONENT_TARGET}"
    )
  endif()
  get_property(
    MEMLY_COMPONENT_DEPENDENCIES_DECLARED TARGET "${MEMLY_COMPONENT_TARGET}"
    PROPERTY MEMLY_COMPONENT_DEPENDENCIES_DECLARED
  )
  if(MEMLY_COMPONENT_DEPENDENCIES_DECLARED)
    message(FATAL_ERROR "Memly component dependencies were declared more than once: ${MEMLY_COMPONENT_TARGET}")
  endif()

  foreach(MEMLY_COMPONENT_DEPENDENCY IN LISTS MEMLY_DEPENDENCIES_PUBLIC MEMLY_DEPENDENCIES_PRIVATE)
    if(NOT TARGET "${MEMLY_COMPONENT_DEPENDENCY}")
      message(FATAL_ERROR "Memly component ${MEMLY_COMPONENT_TARGET} names an unknown dependency: "
                          "${MEMLY_COMPONENT_DEPENDENCY}"
      )
    endif()
  endforeach()
  foreach(MEMLY_PUBLIC_COMPONENT_DEPENDENCY IN LISTS MEMLY_DEPENDENCIES_PUBLIC)
    if(MEMLY_PUBLIC_COMPONENT_DEPENDENCY IN_LIST MEMLY_DEPENDENCIES_PRIVATE)
      message(FATAL_ERROR "Memly component ${MEMLY_COMPONENT_TARGET} declares ${MEMLY_PUBLIC_COMPONENT_DEPENDENCY} "
                          "as both public and private"
      )
    endif()
  endforeach()

  if(MEMLY_DEPENDENCIES_PUBLIC)
    target_link_libraries("${MEMLY_COMPONENT_TARGET}" PUBLIC ${MEMLY_DEPENDENCIES_PUBLIC})
  endif()
  target_link_libraries("${MEMLY_COMPONENT_TARGET}" PRIVATE memly_asan ${MEMLY_DEPENDENCIES_PRIVATE})

  # Static-library PRIVATE dependencies normally escape through $<LINK_ONLY:...> entries. Curating the outward interface
  # keeps their BMIs unavailable to indirect component consumers; the executable supplies the full link closure
  # explicitly.
  if(MEMLY_DEPENDENCIES_PUBLIC)
    set_property(TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY INTERFACE_LINK_LIBRARIES ${MEMLY_DEPENDENCIES_PUBLIC})
  else()
    set_property(TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY INTERFACE_LINK_LIBRARIES "")
  endif()
  set_property(TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_COMPONENT_DEPENDENCIES_DECLARED TRUE)
endfunction()

# Rejects direct component edges that are unused or more visible than their imports require.
function(memly_validate_component_dependency_usage MEMLY_COMPONENT_TARGET)
  list(REMOVE_DUPLICATES MEMLY_IMPORTED_PROVIDER_TARGETS)
  list(REMOVE_DUPLICATES MEMLY_PUBLICLY_IMPORTED_PROVIDER_TARGETS)
  foreach(MEMLY_DIRECT_TARGET_DEPENDENCY IN LISTS MEMLY_DIRECT_TARGET_DEPENDENCIES)
    if(NOT MEMLY_DIRECT_TARGET_DEPENDENCY IN_LIST MEMLY_COMPONENT_TARGETS)
      continue()
    endif()
    if(NOT MEMLY_DIRECT_TARGET_DEPENDENCY IN_LIST MEMLY_IMPORTED_PROVIDER_TARGETS)
      message(FATAL_ERROR "Component ${MEMLY_COMPONENT_TARGET} declares ${MEMLY_DIRECT_TARGET_DEPENDENCY} as a "
                          "direct target dependency, but no active source file imports one of its modules"
      )
    endif()
    if(MEMLY_DIRECT_TARGET_DEPENDENCY IN_LIST MEMLY_PUBLIC_TARGET_DEPENDENCIES
       AND NOT MEMLY_DIRECT_TARGET_DEPENDENCY IN_LIST MEMLY_PUBLICLY_IMPORTED_PROVIDER_TARGETS
    )
      message(FATAL_ERROR "Component ${MEMLY_COMPONENT_TARGET} declares ${MEMLY_DIRECT_TARGET_DEPENDENCY} as a "
                          "public target dependency, but only target-private files import its modules"
      )
    endif()
  endforeach()
endfunction()

# Rejects Memly imports that exceed the importing component's explicitly declared direct target graph.
function(memly_validate_component_import_graph)
  get_property(MEMLY_COMPONENT_TARGETS GLOBAL PROPERTY MEMLY_COMPONENT_TARGETS)
  foreach(MEMLY_COMPONENT_TARGET IN LISTS MEMLY_COMPONENT_TARGETS)
    set(MEMLY_IMPORTED_PROVIDER_TARGETS)
    set(MEMLY_PUBLICLY_IMPORTED_PROVIDER_TARGETS)
    get_property(MEMLY_DIRECT_TARGET_DEPENDENCIES TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY LINK_LIBRARIES)
    get_property(MEMLY_PUBLIC_TARGET_DEPENDENCIES TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY INTERFACE_LINK_LIBRARIES)
    get_property(
      MEMLY_PUBLIC_MODULE_INTERFACES TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_PUBLIC_MODULE_INTERFACES
    )
    get_property(
      MEMLY_INTERNAL_MODULE_INTERFACES TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_INTERNAL_MODULE_INTERFACES
    )
    get_property(
      MEMLY_MODULE_IMPLEMENTATION_UNITS TARGET "${MEMLY_COMPONENT_TARGET}" PROPERTY MEMLY_MODULE_IMPLEMENTATION_UNITS
    )

    foreach(MEMLY_IMPORTER_FILE IN LISTS MEMLY_PUBLIC_MODULE_INTERFACES MEMLY_INTERNAL_MODULE_INTERFACES
                                         MEMLY_MODULE_IMPLEMENTATION_UNITS
    )
      if(MEMLY_IMPORTER_FILE IN_LIST MEMLY_PUBLIC_MODULE_INTERFACES)
        set(MEMLY_IMPORTER_VISIBILITY PUBLIC)
      else()
        set(MEMLY_IMPORTER_VISIBILITY PRIVATE)
      endif()
      memly_read_imported_module_names(MEMLY_IMPORTED_MODULE_NAMES "${MEMLY_IMPORTER_FILE}")
      foreach(MEMLY_IMPORTED_MODULE_NAME IN LISTS MEMLY_IMPORTED_MODULE_NAMES)
        memly_get_module_provider(
          MEMLY_PROVIDER_TARGET MEMLY_IMPORTED_MODULE_VISIBILITY MEMLY_IMPORTED_MODULE_INTERFACE
          "${MEMLY_IMPORTED_MODULE_NAME}"
        )
        if(MEMLY_PROVIDER_TARGET STREQUAL MEMLY_COMPONENT_TARGET)
          if(MEMLY_IMPORTER_VISIBILITY STREQUAL "PUBLIC" AND MEMLY_IMPORTED_MODULE_VISIBILITY STREQUAL "PRIVATE")
            message(FATAL_ERROR "Public module interface ${MEMLY_IMPORTER_FILE} imports internal module "
                                "${MEMLY_IMPORTED_MODULE_NAME}"
            )
          endif()
          continue()
        endif()
        if(MEMLY_IMPORTED_MODULE_VISIBILITY STREQUAL "PRIVATE")
          message(
            FATAL_ERROR
              "Component ${MEMLY_COMPONENT_TARGET} imports ${MEMLY_IMPORTED_MODULE_NAME} from "
              "${MEMLY_PROVIDER_TARGET}, but internal modules may only be imported by their owner: "
              "${MEMLY_IMPORTER_FILE}"
          )
        endif()
        list(APPEND MEMLY_IMPORTED_PROVIDER_TARGETS "${MEMLY_PROVIDER_TARGET}")
        if(MEMLY_IMPORTER_VISIBILITY STREQUAL "PUBLIC")
          list(APPEND MEMLY_PUBLICLY_IMPORTED_PROVIDER_TARGETS "${MEMLY_PROVIDER_TARGET}")
        endif()
        if(NOT MEMLY_PROVIDER_TARGET IN_LIST MEMLY_DIRECT_TARGET_DEPENDENCIES)
          message(
            FATAL_ERROR
              "Component ${MEMLY_COMPONENT_TARGET} imports ${MEMLY_IMPORTED_MODULE_NAME} from "
              "${MEMLY_PROVIDER_TARGET} without declaring a direct target dependency: " "${MEMLY_IMPORTER_FILE}"
          )
        endif()
        if(MEMLY_IMPORTER_VISIBILITY STREQUAL "PUBLIC" AND NOT MEMLY_PROVIDER_TARGET IN_LIST
                                                           MEMLY_PUBLIC_TARGET_DEPENDENCIES
        )
          message(
            FATAL_ERROR
              "Public module interface ${MEMLY_IMPORTER_FILE} imports ${MEMLY_IMPORTED_MODULE_NAME}, "
              "but ${MEMLY_COMPONENT_TARGET} does not declare ${MEMLY_PROVIDER_TARGET} as a public "
              "target dependency"
          )
        endif()
      endforeach()
    endforeach()
    memly_validate_component_dependency_usage("${MEMLY_COMPONENT_TARGET}")
  endforeach()
endfunction()

# Links every registered static component into the final consumer in an explicit caller-supplied order.
function(memly_link_complete_component_closure MEMLY_CONSUMER_TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 MEMLY_CLOSURE "" "" "COMPONENTS;LINK_DEPENDENCIES")
  if(MEMLY_CLOSURE_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unexpected complete component closure arguments for ${MEMLY_CONSUMER_TARGET}: "
                        "${MEMLY_CLOSURE_UNPARSED_ARGUMENTS}"
    )
  endif()

  get_property(MEMLY_REGISTERED_COMPONENT_TARGETS GLOBAL PROPERTY MEMLY_COMPONENT_TARGETS)
  set(MEMLY_DECLARED_COMPONENT_TARGETS ${MEMLY_CLOSURE_COMPONENTS})
  list(REMOVE_DUPLICATES MEMLY_DECLARED_COMPONENT_TARGETS)
  foreach(MEMLY_REGISTERED_COMPONENT_TARGET IN LISTS MEMLY_REGISTERED_COMPONENT_TARGETS)
    if(NOT MEMLY_REGISTERED_COMPONENT_TARGET IN_LIST MEMLY_DECLARED_COMPONENT_TARGETS)
      message(FATAL_ERROR "Final target ${MEMLY_CONSUMER_TARGET} omits component ${MEMLY_REGISTERED_COMPONENT_TARGET}")
    endif()
    get_property(
      MEMLY_COMPONENT_DEPENDENCIES_DECLARED TARGET "${MEMLY_REGISTERED_COMPONENT_TARGET}"
      PROPERTY MEMLY_COMPONENT_DEPENDENCIES_DECLARED
    )
    if(NOT MEMLY_COMPONENT_DEPENDENCIES_DECLARED)
      message(FATAL_ERROR "Memly component has no explicit dependency declaration: "
                          "${MEMLY_REGISTERED_COMPONENT_TARGET}"
      )
    endif()
  endforeach()
  foreach(MEMLY_DECLARED_COMPONENT_TARGET IN LISTS MEMLY_DECLARED_COMPONENT_TARGETS)
    if(NOT MEMLY_DECLARED_COMPONENT_TARGET IN_LIST MEMLY_REGISTERED_COMPONENT_TARGETS)
      message(FATAL_ERROR "Final target ${MEMLY_CONSUMER_TARGET} names an unregistered component: "
                          "${MEMLY_DECLARED_COMPONENT_TARGET}"
      )
    endif()
  endforeach()
  foreach(MEMLY_CLOSURE_LINK_DEPENDENCY IN LISTS MEMLY_CLOSURE_LINK_DEPENDENCIES)
    if(NOT TARGET "${MEMLY_CLOSURE_LINK_DEPENDENCY}")
      message(FATAL_ERROR "Final target ${MEMLY_CONSUMER_TARGET} names an unknown link dependency: "
                          "${MEMLY_CLOSURE_LINK_DEPENDENCY}"
      )
    endif()
  endforeach()

  target_link_libraries(
    "${MEMLY_CONSUMER_TARGET}" PRIVATE ${MEMLY_CLOSURE_COMPONENTS} ${MEMLY_CLOSURE_LINK_DEPENDENCIES}
  )
endfunction()

# Gives the database component access to Memly's configured DuckDB header.
function(memly_configure_duckdb_header MEMLY_COMPONENT_TARGET)
  target_include_directories("${MEMLY_COMPONENT_TARGET}" BEFORE PRIVATE "${_duckdb_wrapper_dir}")
endfunction()

set(MEMLY_BUILD_TIMING_FILE "${CMAKE_BINARY_DIR}/build-start-time.txt")
set(MEMLY_CLANG_TIDY_TIMING_FILE "${CMAKE_BINARY_DIR}/clang-tidy-start-time.txt")
set(MEMLY_TIMING_SCRIPT "${MEMLY_CMAKE_TOOL_DIRECTORY}/report_elapsed_time.cmake")
add_custom_target(
  memly-build-timing-start
  COMMAND "${CMAKE_COMMAND}" -DMEMLY_TIMING_ACTION=Start "-DMEMLY_TIMING_FILE=${MEMLY_BUILD_TIMING_FILE}" -P
          "${MEMLY_TIMING_SCRIPT}"
  BYPRODUCTS "${MEMLY_BUILD_TIMING_FILE}"
  COMMENT "Start the build timer"
  VERBATIM
)

add_subdirectory(program)
memly_validate_component_import_graph()

get_property(MEMLY_REGISTERED_CXX_FILES GLOBAL PROPERTY MEMLY_REGISTERED_CXX_FILES)
file(
  GLOB_RECURSE
  MEMLY_AUTHORED_CXX_FILES
  CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/program/*.cpp"
  "${CMAKE_SOURCE_DIR}/program/*.cppm"
  "${CMAKE_SOURCE_DIR}/test/*.cpp"
  "${CMAKE_SOURCE_DIR}/test/*.cppm"
)
foreach(MEMLY_AUTHORED_CXX_FILE IN LISTS MEMLY_AUTHORED_CXX_FILES)
  if(NOT MEMLY_AUTHORED_CXX_FILE IN_LIST MEMLY_REGISTERED_CXX_FILES)
    message(FATAL_ERROR "Memly C++ file is not assigned to a target: ${MEMLY_AUTHORED_CXX_FILE}")
  endif()
endforeach()

set(MEMLY_DEPRECATED_C_HEADER_STAMP_DIRECTORY "${CMAKE_BINARY_DIR}/deprecated-c-header-stamps")
set(MEMLY_DEPRECATED_C_HEADER_VERIFIER "${MEMLY_CMAKE_TOOL_DIRECTORY}/verify_deprecated_c_headers.py")
file(MAKE_DIRECTORY "${MEMLY_DEPRECATED_C_HEADER_STAMP_DIRECTORY}")
set(MEMLY_DEPRECATED_C_HEADER_STAMPS)
foreach(MEMLY_AUTHORED_CXX_FILE IN LISTS MEMLY_AUTHORED_CXX_FILES)
  cmake_path(
    RELATIVE_PATH MEMLY_AUTHORED_CXX_FILE BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE
    MEMLY_AUTHORED_CXX_RELATIVE_FILE
  )
  string(SHA256 MEMLY_AUTHORED_CXX_FILE_HASH "${MEMLY_AUTHORED_CXX_RELATIVE_FILE}")
  set(MEMLY_DEPRECATED_C_HEADER_STAMP
      "${MEMLY_DEPRECATED_C_HEADER_STAMP_DIRECTORY}/${MEMLY_AUTHORED_CXX_FILE_HASH}.stamp"
  )
  add_custom_command(
    OUTPUT "${MEMLY_DEPRECATED_C_HEADER_STAMP}"
    COMMAND "${Python3_EXECUTABLE}" "${MEMLY_DEPRECATED_C_HEADER_VERIFIER}" "${CMAKE_SOURCE_DIR}"
            "${MEMLY_AUTHORED_CXX_FILE}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${MEMLY_DEPRECATED_C_HEADER_STAMP}"
    DEPENDS "${MEMLY_AUTHORED_CXX_FILE}" "${MEMLY_DEPRECATED_C_HEADER_VERIFIER}"
    COMMENT "Check deprecated C headers ${MEMLY_AUTHORED_CXX_RELATIVE_FILE}"
    VERBATIM
  )
  list(APPEND MEMLY_DEPRECATED_C_HEADER_STAMPS "${MEMLY_DEPRECATED_C_HEADER_STAMP}")
endforeach()
add_custom_target(
  memly-deprecated-c-header-verification ALL DEPENDS ${MEMLY_DEPRECATED_C_HEADER_STAMPS}
  COMMENT "Check deprecated C headers"
)
add_dependencies(memly-build-timing-start memly-deprecated-c-header-verification)

get_property(MEMLY_COMPONENT_TARGETS GLOBAL PROPERTY MEMLY_COMPONENT_TARGETS)
set(MEMLY_CLANG_TIDY_TARGETS ${MEMLY_COMPONENT_TARGETS} memly)
set(MEMLY_TIMED_BUILD_TARGETS ${MEMLY_COMPONENT_TARGETS} memly_qml memly_qml_plugin memly)
foreach(MEMLY_TIMED_BUILD_TARGET IN LISTS MEMLY_TIMED_BUILD_TARGETS)
  add_dependencies("${MEMLY_TIMED_BUILD_TARGET}" memly-build-timing-start)
endforeach()
set(MEMLY_CLANG_TIDY_COMPILATION_DATABASE_DIRECTORY "${CMAKE_BINARY_DIR}/clang-tidy-compilation-database")
set(MEMLY_CLANG_TIDY_COMPILATION_DATABASE "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE_DIRECTORY}/compile_commands.json")
set(MEMLY_CLANG_TIDY_STAMP_DIRECTORY "${CMAKE_BINARY_DIR}/clang-tidy-stamps")
set(MEMLY_CLANG_TIDY_PROFILE_REPORT "${CMAKE_BINARY_DIR}/clang-tidy-profile.txt")
file(MAKE_DIRECTORY "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE_DIRECTORY}" "${MEMLY_CLANG_TIDY_STAMP_DIRECTORY}")

add_custom_command(
  OUTPUT "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE}"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE}"
  DEPENDS "${CMAKE_BINARY_DIR}/compile_commands.json"
  COMMENT "Update the content-stable Clang-Tidy compilation database"
  VERBATIM
)
add_custom_target(
  memly-clang-tidy-compilation-database DEPENDS "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE}"
  COMMENT "Prepare the content-stable Clang-Tidy compilation database"
)

add_custom_target(
  memly-clang-tidy-build-ready
  COMMAND "${CMAKE_COMMAND}" -DMEMLY_TIMING_ACTION=Report "-DMEMLY_TIMING_FILE=${MEMLY_BUILD_TIMING_FILE}"
          -DMEMLY_TIMING_LABEL=Build -P "${MEMLY_TIMING_SCRIPT}"
  COMMAND "${CMAKE_COMMAND}" -DMEMLY_TIMING_ACTION=Start "-DMEMLY_TIMING_FILE=${MEMLY_CLANG_TIDY_TIMING_FILE}" -P
          "${MEMLY_TIMING_SCRIPT}"
  BYPRODUCTS "${MEMLY_CLANG_TIDY_TIMING_FILE}"
  COMMENT "Report the build duration and start the Clang-Tidy timer"
  VERBATIM
)
add_dependencies(memly-clang-tidy-build-ready ${MEMLY_CLANG_TIDY_TARGETS})

set(MEMLY_CLANG_TIDY_STAMPS)
set(MEMLY_CLANG_TIDY_PROFILE_FILES)
foreach(MEMLY_CLANG_TIDY_TARGET IN LISTS MEMLY_CLANG_TIDY_TARGETS)
  get_property(MEMLY_TARGET_CLANG_TIDY_FILES TARGET "${MEMLY_CLANG_TIDY_TARGET}" PROPERTY MEMLY_CLANG_TIDY_FILES)
  list(APPEND MEMLY_CLANG_TIDY_PROFILE_FILES ${MEMLY_TARGET_CLANG_TIDY_FILES})
  foreach(MEMLY_CLANG_TIDY_FILE IN LISTS MEMLY_TARGET_CLANG_TIDY_FILES)
    memly_collect_imported_module_interface_closure(
      MEMLY_CLANG_TIDY_INVALIDATION_DEPENDENCIES "${MEMLY_CLANG_TIDY_FILE}"
    )
    list(REMOVE_ITEM MEMLY_CLANG_TIDY_INVALIDATION_DEPENDENCIES "${MEMLY_CLANG_TIDY_FILE}")
    list(REMOVE_DUPLICATES MEMLY_CLANG_TIDY_INVALIDATION_DEPENDENCIES)
    cmake_path(
      RELATIVE_PATH MEMLY_CLANG_TIDY_FILE BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE
      MEMLY_CLANG_TIDY_RELATIVE_FILE
    )
    string(SHA256 MEMLY_CLANG_TIDY_FILE_HASH "${MEMLY_CLANG_TIDY_RELATIVE_FILE}")
    set(MEMLY_CLANG_TIDY_FILE_STAMP
        "${MEMLY_CLANG_TIDY_STAMP_DIRECTORY}/${MEMLY_CLANG_TIDY_TARGET}-${MEMLY_CLANG_TIDY_FILE_HASH}.stamp"
    )
    add_custom_command(
      OUTPUT "${MEMLY_CLANG_TIDY_FILE_STAMP}"
      COMMAND
        "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}" -p "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE_DIRECTORY}"
        "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy" "--header-filter=^${CMAKE_SOURCE_DIR}/(program|test)/" -quiet
        "${MEMLY_CLANG_TIDY_FILE}"
      COMMAND "${CMAKE_COMMAND}" -E touch "${MEMLY_CLANG_TIDY_FILE_STAMP}"
      DEPENDS "${MEMLY_CLANG_TIDY_FILE}"
              ${MEMLY_CLANG_TIDY_INVALIDATION_DEPENDENCIES}
              memly-clang-tidy-build-ready
              "${CMAKE_SOURCE_DIR}/.clang-tidy"
              "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE}"
              "${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}"
              "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      COMMENT "Clang-Tidy ${MEMLY_CLANG_TIDY_RELATIVE_FILE}"
      JOB_POOL memly_clang_tidy
      VERBATIM
    )
    list(APPEND MEMLY_CLANG_TIDY_STAMPS "${MEMLY_CLANG_TIDY_FILE_STAMP}")
  endforeach()
endforeach()

list(REMOVE_DUPLICATES MEMLY_CLANG_TIDY_PROFILE_FILES)
list(SORT MEMLY_CLANG_TIDY_PROFILE_FILES)
list(LENGTH MEMLY_CLANG_TIDY_PROFILE_FILES MEMLY_CLANG_TIDY_PROFILE_FILE_COUNT)
set(MEMLY_CLANG_TIDY_PROFILE_INPUT_ARGUMENTS)
foreach(MEMLY_CLANG_TIDY_PROFILE_FILE IN LISTS MEMLY_CLANG_TIDY_PROFILE_FILES)
  list(APPEND MEMLY_CLANG_TIDY_PROFILE_INPUT_ARGUMENTS --input-file "${MEMLY_CLANG_TIDY_PROFILE_FILE}")
endforeach()
add_custom_target(
  memly-clang-tidy ALL
  COMMAND "${CMAKE_COMMAND}" -DMEMLY_TIMING_ACTION=Report "-DMEMLY_TIMING_FILE=${MEMLY_CLANG_TIDY_TIMING_FILE}"
          -DMEMLY_TIMING_LABEL=Clang-Tidy -P "${MEMLY_TIMING_SCRIPT}"
  DEPENDS ${MEMLY_CLANG_TIDY_STAMPS}
  COMMENT "Report the mandatory Clang-Tidy gate duration"
  VERBATIM
)
add_dependencies(memly-clang-tidy memly-clang-tidy-build-ready)

add_custom_target(
  memly-clang-tidy-profile
  COMMAND
    "${CMAKE_COMMAND}" -E env PYTHONUNBUFFERED=1 "${Python3_EXECUTABLE}"
    "${MEMLY_CMAKE_TOOL_DIRECTORY}/run_and_capture_output.py" "${MEMLY_CLANG_TIDY_PROFILE_REPORT}" --source-root
    "${CMAKE_SOURCE_DIR}" --config-file "${CMAKE_SOURCE_DIR}/.clang-tidy" --compilation-database
    "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE}" --tool "${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}" --metadata
    "CMake version: ${CMAKE_VERSION}" --metadata "CMake generator: ${CMAKE_GENERATOR}" --metadata
    "Build type: ${CMAKE_BUILD_TYPE}" --metadata
    "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})" --metadata
    "C++ compiler frontend: ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" --metadata
    "Linker: ${CMAKE_CXX_COMPILER_LINKER_ID} ${CMAKE_CXX_COMPILER_LINKER_VERSION}" --metadata
    "Target system: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}" --metadata
    "Clang-Tidy workers: ${MEMLY_CLANG_TIDY_JOBS}" --metadata "Lint inputs: ${MEMLY_CLANG_TIDY_PROFILE_FILE_COUNT}"
    ${MEMLY_CLANG_TIDY_PROFILE_INPUT_ARGUMENTS} -- "${CMAKE_COMMAND}" -E time "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}"
    -j "${MEMLY_CLANG_TIDY_JOBS}" -p "${MEMLY_CLANG_TIDY_COMPILATION_DATABASE_DIRECTORY}" -clang-tidy-binary
    "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}" -config-file "${CMAKE_SOURCE_DIR}/.clang-tidy" -header-filter
    "^${CMAKE_SOURCE_DIR}/(program|test)/" -quiet -enable-check-profile ${MEMLY_CLANG_TIDY_PROFILE_FILES}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Profile the effective full-tree Clang-Tidy check set"
  USES_TERMINAL VERBATIM
)
add_dependencies(memly-clang-tidy-profile memly-clang-tidy-compilation-database ${MEMLY_CLANG_TIDY_TARGETS})
