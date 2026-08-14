cmake_minimum_required(VERSION 4.4)
project(memly LANGUAGES C CXX)

set(MEMLY_LLVM_MAJOR_VERSION 22)
set(MEMLY_PRETTIER_VERSION 3.9.6)

if(CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
  message(FATAL_ERROR "Memly's Android and iOS toolchain contracts have not been defined yet")
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(Darwin|Linux|Windows)$")
  message(FATAL_ERROR "Memly currently supports desktop builds targeting macOS, Linux, or Windows")
endif()

foreach(MEMLY_COMPILER_LANGUAGE IN ITEMS C CXX)
  set(MEMLY_COMPILER_ID_VARIABLE "CMAKE_${MEMLY_COMPILER_LANGUAGE}_COMPILER_ID")
  set(MEMLY_COMPILER_VERSION_VARIABLE "CMAKE_${MEMLY_COMPILER_LANGUAGE}_COMPILER_VERSION")
  if(NOT "${${MEMLY_COMPILER_ID_VARIABLE}}" STREQUAL "Clang")
    message(
      FATAL_ERROR
        "Memly desktop builds require upstream LLVM Clang ${MEMLY_LLVM_MAJOR_VERSION}.x "
        "for ${MEMLY_COMPILER_LANGUAGE}; "
        "configured compiler is ${${MEMLY_COMPILER_ID_VARIABLE}} ${${MEMLY_COMPILER_VERSION_VARIABLE}}"
    )
  endif()
  string(REGEX MATCH "^[0-9]+" MEMLY_COMPILER_MAJOR_VERSION "${${MEMLY_COMPILER_VERSION_VARIABLE}}")
  if(NOT MEMLY_COMPILER_MAJOR_VERSION STREQUAL "${MEMLY_LLVM_MAJOR_VERSION}")
    message(
      FATAL_ERROR
        "Memly desktop builds require LLVM Clang ${MEMLY_LLVM_MAJOR_VERSION}.x for ${MEMLY_COMPILER_LANGUAGE}; "
        "configured version is ${${MEMLY_COMPILER_VERSION_VARIABLE}}"
    )
  endif()
endforeach()

if(NOT CMAKE_C_COMPILER_VERSION VERSION_EQUAL CMAKE_CXX_COMPILER_VERSION)
  message(FATAL_ERROR "Memly requires matching LLVM C and C++ compilers; configured versions are "
                      "${CMAKE_C_COMPILER_VERSION} and ${CMAKE_CXX_COMPILER_VERSION}"
  )
endif()

if(WIN32)
  set(MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT MSVC)
else()
  set(MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT GNU)
endif()
foreach(MEMLY_COMPILER_LANGUAGE IN ITEMS C CXX)
  set(MEMLY_COMPILER_FRONTEND_VARIANT_VARIABLE "CMAKE_${MEMLY_COMPILER_LANGUAGE}_COMPILER_FRONTEND_VARIANT")
  if(NOT "${${MEMLY_COMPILER_FRONTEND_VARIANT_VARIABLE}}" STREQUAL "${MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT}")
    message(
      FATAL_ERROR "Memly requires Clang's ${MEMLY_REQUIRED_CLANG_FRONTEND_VARIANT} frontend on ${CMAKE_SYSTEM_NAME}; "
                  "configured ${MEMLY_COMPILER_LANGUAGE} frontend is ${${MEMLY_COMPILER_FRONTEND_VARIANT_VARIABLE}}"
    )
  endif()
endforeach()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(QT_QML_GENERATE_QMLLS_INI ON)

include(CTest)
include(FetchContent)

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
  message(FATAL_ERROR "Memly requires a Ninja generator for its mandatory full-tree Clang-Tidy gate")
endif()

find_program(MEMLY_PRETTIER_EXECUTABLE NAMES prettier REQUIRED)
execute_process(
  COMMAND "${MEMLY_PRETTIER_EXECUTABLE}" --version
  RESULT_VARIABLE MEMLY_PRETTIER_VERSION_RESULT
  OUTPUT_VARIABLE MEMLY_PRETTIER_VERSION_OUTPUT
  ERROR_VARIABLE MEMLY_PRETTIER_VERSION_ERROR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT MEMLY_PRETTIER_VERSION_RESULT EQUAL 0)
  message(FATAL_ERROR "Unable to read Prettier version: ${MEMLY_PRETTIER_VERSION_ERROR}")
endif()
if(NOT "${MEMLY_PRETTIER_VERSION_OUTPUT}" STREQUAL "${MEMLY_PRETTIER_VERSION}")
  message(FATAL_ERROR "Memly requires Prettier ${MEMLY_PRETTIER_VERSION}; "
                      "found ${MEMLY_PRETTIER_VERSION_OUTPUT} at ${MEMLY_PRETTIER_EXECUTABLE}"
  )
endif()

cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH MEMLY_LLVM_BINARY_DIRECTORY)
cmake_path(GET MEMLY_LLVM_BINARY_DIRECTORY PARENT_PATH MEMLY_LLVM_INSTALL_DIRECTORY)
cmake_path(GET CMAKE_C_COMPILER PARENT_PATH MEMLY_LLVM_C_BINARY_DIRECTORY)
if(NOT MEMLY_LLVM_C_BINARY_DIRECTORY STREQUAL MEMLY_LLVM_BINARY_DIRECTORY)
  message(FATAL_ERROR "Memly requires clang and clang++ from the same LLVM installation")
endif()

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

if(WIN32)
  set(MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy-driver.cmd")
  file(GENERATE OUTPUT "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
       CONTENT "@echo off\r\n\"${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}\" --experimental-custom-checks %*\r\n"
  )

  set(MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy-header-driver.cmd")
  string(
    CONCAT MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_CONTENT
           "@echo off\r\n"
           "setlocal\r\n"
           "set \"MemlyClangTidyOutputFile=%TEMP%\\memly-clang-tidy-%RANDOM%-%RANDOM%.txt\"\r\n"
           "call \"${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}\" %* 2>\"%MemlyClangTidyOutputFile%\"\r\n"
           "set \"MemlyClangTidyResult=%ERRORLEVEL%\"\r\n"
           "findstr /V /R /C:\"^\\[[0-9][0-9]*/[0-9][0-9]*\\] Processing file \" "
           "\"%MemlyClangTidyOutputFile%\" 1>&2\r\n"
           "del /Q \"%MemlyClangTidyOutputFile%\"\r\n"
           "exit /B %MemlyClangTidyResult%\r\n"
  )
  file(GENERATE OUTPUT "${MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_EXECUTABLE}"
       CONTENT "${MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_CONTENT}"
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

  set(MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy-header-driver")
  string(
    CONCAT MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_CONTENT
           "#!/bin/sh\n"
           "ClangTidyOutputFile=\"\$(mktemp \"\${TMPDIR:-/tmp}/memly-clang-tidy-header-output.XXXXXX\")\" || exit 2\n"
           "trap 'rm -f \"\$ClangTidyOutputFile\"' EXIT HUP INT TERM\n"
           "\"${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}\" \"\$@\" 2>\"\$ClangTidyOutputFile\"\n"
           "ClangTidyResult=\$?\n"
           "if ! sed '/^\\[[0-9][0-9]*\\\/[0-9][0-9]*\\] Processing file /d' \"\$ClangTidyOutputFile\" >&2; then\n"
           "  exit 2\n"
           "fi\n"
           "exit \"\$ClangTidyResult\"\n"
  )
  file(
    GENERATE
    OUTPUT "${MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_EXECUTABLE}"
    CONTENT "${MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_CONTENT}"
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

find_package(Qt6 ${MEMLY_QT_MIN_VERSION} REQUIRED COMPONENTS Core Network Qml Quick)
find_package(ICU REQUIRED COMPONENTS uc i18n)
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
if(DEFINED FETCHCONTENT_BASE_DIR)
  set(_duckdb_fetchcontent_base_dir "${FETCHCONTENT_BASE_DIR}")
else()
  set(_duckdb_fetchcontent_base_dir "${CMAKE_BINARY_DIR}/_deps")
endif()

set(_duckdb_fetchcontent_source_dir "${_duckdb_fetchcontent_base_dir}/libduckdb-src")
set(_duckdb_needs_download FALSE)
if(NOT EXISTS "${_duckdb_fetchcontent_source_dir}")
  set(_duckdb_needs_download TRUE)
  message(STATUS "Fetching DuckDB ${DUCKDB_VERSION} from ${DUCKDB_URL}")
endif()

FetchContent_MakeAvailable(libduckdb)
if(_duckdb_needs_download)
  message(STATUS "Downloaded DuckDB ${DUCKDB_VERSION} to ${libduckdb_SOURCE_DIR}")
endif()

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

add_library(memly_asan INTERFACE)
target_compile_options(
  memly_asan
  INTERFACE $<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer>
)
target_link_options(
  memly_asan INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>
)

# Adds a static component and registers its complete C++ surface with the global lint and verification gates.
function(memly_add_component_library MEMLY_COMPONENT_TARGET MEMLY_COMPONENT_INCLUDE_DIRECTORY)
  cmake_parse_arguments(PARSE_ARGV 2 MEMLY_COMPONENT "" "" "HEADERS;SOURCES")
  add_library("${MEMLY_COMPONENT_TARGET}" STATIC)
  target_sources(
    "${MEMLY_COMPONENT_TARGET}" PRIVATE ${MEMLY_COMPONENT_SOURCES}
    PUBLIC FILE_SET HEADERS BASE_DIRS "${MEMLY_COMPONENT_INCLUDE_DIRECTORY}" FILES ${MEMLY_COMPONENT_HEADERS}
  )
  target_link_libraries("${MEMLY_COMPONENT_TARGET}" PRIVATE memly_asan)
  set_target_properties("${MEMLY_COMPONENT_TARGET}" PROPERTIES AUTOMOC OFF VERIFY_INTERFACE_HEADER_SETS ON)
  set_property(GLOBAL APPEND PROPERTY MEMLY_COMPONENT_TARGETS "${MEMLY_COMPONENT_TARGET}")
  set_property(GLOBAL APPEND PROPERTY MEMLY_CXX_SOURCE_FILES ${MEMLY_COMPONENT_HEADERS} ${MEMLY_COMPONENT_SOURCES})
endfunction()

add_subdirectory(program)

if(BUILD_TESTING)
  add_custom_target(memly-test COMMENT "Memly tests are not currently implemented")
endif()

get_property(MEMLY_CXX_SOURCE_FILES GLOBAL PROPERTY MEMLY_CXX_SOURCE_FILES)
file(
  GLOB_RECURSE
  MEMLY_PROGRAM_HEADERS
  CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/program/*.h"
  "${CMAKE_SOURCE_DIR}/program/*.hh"
  "${CMAKE_SOURCE_DIR}/program/*.hpp"
  "${CMAKE_SOURCE_DIR}/program/*.hxx"
)
foreach(MEMLY_PROGRAM_HEADER IN LISTS MEMLY_PROGRAM_HEADERS)
  if(NOT MEMLY_PROGRAM_HEADER IN_LIST MEMLY_CXX_SOURCE_FILES)
    message(FATAL_ERROR "Memly header is not assigned to a target: ${MEMLY_PROGRAM_HEADER}")
  endif()
endforeach()

set(MEMLY_CXX_TRANSLATION_UNIT_FILES ${MEMLY_CXX_SOURCE_FILES})
list(FILTER MEMLY_CXX_TRANSLATION_UNIT_FILES INCLUDE REGEX "\\.(cc|cpp|cxx)$")
set(MEMLY_CXX_HEADER_FILES ${MEMLY_CXX_SOURCE_FILES})
list(FILTER MEMLY_CXX_HEADER_FILES INCLUDE REGEX "\\.(h|hh|hpp|hxx)$")

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
      /clang:-Wno-disabled-macro-expansion
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
      -Wno-disabled-macro-expansion
      -Wno-covered-switch-default
      -Werror
  )
endif()
foreach(MEMLY_CLANG_WARNING_OPTION IN LISTS MEMLY_CLANG_WARNING_OPTIONS)
  set_property(
    SOURCE ${MEMLY_CXX_TRANSLATION_UNIT_FILES} APPEND
    PROPERTY COMPILE_OPTIONS "$<$<COMPILE_LANG_AND_ID:CXX,Clang>:${MEMLY_CLANG_WARNING_OPTION}>"
  )
endforeach()

get_property(MEMLY_COMPONENT_TARGETS GLOBAL PROPERTY MEMLY_COMPONENT_TARGETS)
foreach(MEMLY_COMPONENT_TARGET IN LISTS MEMLY_COMPONENT_TARGETS)
  string(CONCAT MEMLY_INTERFACE_HEADER_VERIFICATION_CONDITION "$<AND:" "$<COMPILE_LANG_AND_ID:CXX,Clang>,"
                "$<STREQUAL:$<TARGET_PROPERTY:NAME>,${MEMLY_COMPONENT_TARGET}_verify_interface_header_sets>>"
  )
  foreach(MEMLY_CLANG_WARNING_OPTION IN LISTS MEMLY_CLANG_WARNING_OPTIONS)
    target_compile_options(
      "${MEMLY_COMPONENT_TARGET}"
      INTERFACE "$<${MEMLY_INTERFACE_HEADER_VERIFICATION_CONDITION}:${MEMLY_CLANG_WARNING_OPTION}>"
    )
  endforeach()
endforeach()

set(MEMLY_CLANG_TIDY_STAMP "${CMAKE_BINARY_DIR}/.memly-clang-tidy.stamp")
add_custom_command(
  OUTPUT "${MEMLY_CLANG_TIDY_STAMP}"
  COMMAND
    "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}" -p "${CMAKE_BINARY_DIR}" -clang-tidy-binary
    "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}" -config-file "${CMAKE_SOURCE_DIR}/.clang-tidy" -header-filter
    "^${CMAKE_SOURCE_DIR}/(program|test)/" -quiet -hide-progress "^${CMAKE_SOURCE_DIR}/(program|test)/.*\\.cpp$"
  COMMAND "${MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_EXECUTABLE}" -p "${CMAKE_BINARY_DIR}" -config-file
          "${CMAKE_SOURCE_DIR}/.clang-tidy" -header-filter "^$" -quiet ${MEMLY_CXX_HEADER_FILES}
  COMMAND "${CMAKE_COMMAND}" -E touch "${MEMLY_CLANG_TIDY_STAMP}"
  DEPENDS "${CMAKE_SOURCE_DIR}/.clang-tidy"
          "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}"
          "${MEMLY_LLVM_CLANG_TIDY_HEADER_WRAPPER_EXECUTABLE}"
          "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
          "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}"
          ${MEMLY_CXX_TRANSLATION_UNIT_FILES}
          ${MEMLY_CXX_HEADER_FILES}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Run the mandatory full-tree Clang-Tidy gate before compilation"
  VERBATIM
)
add_custom_target(
  memly-clang-tidy DEPENDS "${MEMLY_CLANG_TIDY_STAMP}" COMMENT "Run the mandatory full-tree Clang-Tidy gate"
)

foreach(MEMLY_COMPONENT_TARGET IN LISTS MEMLY_COMPONENT_TARGETS)
  add_dependencies("${MEMLY_COMPONENT_TARGET}" memly-clang-tidy)
endforeach()
add_dependencies(memly_qml memly-clang-tidy)
add_dependencies(memly memly-clang-tidy)

add_custom_target(
  memly-public-header-verification DEPENDS all_verify_interface_header_sets
  COMMENT "Compile every public header independently"
)
add_dependencies(memly memly-public-header-verification)
