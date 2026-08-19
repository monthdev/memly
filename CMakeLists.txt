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

find_program(MEMLY_INCLUDE_WHAT_YOU_USE_EXECUTABLE NAMES include-what-you-use REQUIRED)
execute_process(
  COMMAND "${MEMLY_INCLUDE_WHAT_YOU_USE_EXECUTABLE}" --version
  RESULT_VARIABLE MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_RESULT
  OUTPUT_VARIABLE MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_OUTPUT
  ERROR_VARIABLE MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_ERROR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_RESULT EQUAL 0)
  message(FATAL_ERROR "Unable to read Include What You Use version: ${MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_ERROR}")
endif()
string(REGEX MATCH "clang version ([0-9]+)" MEMLY_INCLUDE_WHAT_YOU_USE_CLANG_VERSION_MATCH
             "${MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_OUTPUT}"
)
if(NOT MEMLY_INCLUDE_WHAT_YOU_USE_CLANG_VERSION_MATCH OR NOT CMAKE_MATCH_1 STREQUAL "${MEMLY_LLVM_MAJOR_VERSION}")
  message(FATAL_ERROR "Memly requires Include What You Use built against LLVM Clang ${MEMLY_LLVM_MAJOR_VERSION}.x; "
                      "found ${MEMLY_INCLUDE_WHAT_YOU_USE_VERSION_OUTPUT}"
  )
endif()
cmake_path(GET MEMLY_INCLUDE_WHAT_YOU_USE_EXECUTABLE PARENT_PATH MEMLY_INCLUDE_WHAT_YOU_USE_BINARY_DIRECTORY)
find_program(
  MEMLY_INCLUDE_WHAT_YOU_USE_DRIVER_EXECUTABLE
  NAMES iwyu_tool.py
  HINTS "${MEMLY_INCLUDE_WHAT_YOU_USE_BINARY_DIRECTORY}"
  NO_DEFAULT_PATH REQUIRED
)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

file(GLOB_RECURSE MEMLY_AUTHORED_FILE_PATHS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/*"
     "${CMAKE_SOURCE_DIR}/test/*"
)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tool/verify_source_extensions.py")
execute_process(
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tool/verify_source_extensions.py" "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE MEMLY_SOURCE_EXTENSION_VERIFICATION_RESULT
  ERROR_VARIABLE MEMLY_SOURCE_EXTENSION_VERIFICATION_ERROR
  ERROR_STRIP_TRAILING_WHITESPACE
)
if(NOT MEMLY_SOURCE_EXTENSION_VERIFICATION_RESULT EQUAL 0)
  message(FATAL_ERROR "${MEMLY_SOURCE_EXTENSION_VERIFICATION_ERROR}")
endif()

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

find_package(Qt6 ${MEMLY_QT_MIN_VERSION} REQUIRED COMPONENTS Core Qml Quick)
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

# Adds a static component and registers its complete C++ surface with the global lint and verification gates.
function(memly_add_component_library MEMLY_COMPONENT_TARGET MEMLY_COMPONENT_INCLUDE_DIRECTORY)
  cmake_parse_arguments(PARSE_ARGV 2 MEMLY_COMPONENT "" "" "HEADERS;PRIVATE_HEADERS;SOURCES")

  foreach(MEMLY_COMPONENT_HEADER IN LISTS MEMLY_COMPONENT_HEADERS MEMLY_COMPONENT_PRIVATE_HEADERS)
    if(NOT MEMLY_COMPONENT_HEADER MATCHES "[.]hpp$")
      message(FATAL_ERROR "Memly target ${MEMLY_COMPONENT_TARGET} classifies a non-.hpp file as a header: "
                          "${MEMLY_COMPONENT_HEADER}"
      )
    endif()
  endforeach()
  foreach(MEMLY_COMPONENT_SOURCE IN LISTS MEMLY_COMPONENT_SOURCES)
    if(NOT MEMLY_COMPONENT_SOURCE MATCHES "[.]cpp$")
      message(FATAL_ERROR "Memly target ${MEMLY_COMPONENT_TARGET} classifies a non-.cpp file as a source: "
                          "${MEMLY_COMPONENT_SOURCE}"
      )
    endif()
  endforeach()

  add_library("${MEMLY_COMPONENT_TARGET}" STATIC)
  target_sources(
    "${MEMLY_COMPONENT_TARGET}" PRIVATE ${MEMLY_COMPONENT_SOURCES}
    PUBLIC FILE_SET HEADERS BASE_DIRS "${MEMLY_COMPONENT_INCLUDE_DIRECTORY}" FILES ${MEMLY_COMPONENT_HEADERS}
  )
  if(MEMLY_COMPONENT_PRIVATE_HEADERS)
    target_sources(
      "${MEMLY_COMPONENT_TARGET}"
      PRIVATE FILE_SET
              memly_private_headers
              TYPE
              HEADERS
              BASE_DIRS
              "${CMAKE_CURRENT_SOURCE_DIR}/source"
              FILES
              ${MEMLY_COMPONENT_PRIVATE_HEADERS}
    )
    set_target_properties(
      "${MEMLY_COMPONENT_TARGET}" PROPERTIES VERIFY_PRIVATE_HEADER_SETS ON PRIVATE_HEADER_SETS_TO_VERIFY
                                                                           memly_private_headers
    )
  endif()
  target_link_libraries("${MEMLY_COMPONENT_TARGET}" PRIVATE memly_asan)
  set_target_properties("${MEMLY_COMPONENT_TARGET}" PROPERTIES AUTOMOC OFF VERIFY_INTERFACE_HEADER_SETS ON)
  set_property(GLOBAL APPEND PROPERTY MEMLY_COMPONENT_TARGETS "${MEMLY_COMPONENT_TARGET}")
  set_property(
    GLOBAL APPEND PROPERTY MEMLY_REGISTERED_CXX_FILES ${MEMLY_COMPONENT_HEADERS} ${MEMLY_COMPONENT_PRIVATE_HEADERS}
                           ${MEMLY_COMPONENT_SOURCES}
  )
endfunction()

add_subdirectory(program)

get_property(MEMLY_REGISTERED_CXX_FILES GLOBAL PROPERTY MEMLY_REGISTERED_CXX_FILES)
file(
  GLOB_RECURSE
  MEMLY_AUTHORED_CXX_FILES
  CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/program/*.cpp"
  "${CMAKE_SOURCE_DIR}/program/*.hpp"
  "${CMAKE_SOURCE_DIR}/test/*.cpp"
  "${CMAKE_SOURCE_DIR}/test/*.hpp"
)
foreach(MEMLY_AUTHORED_CXX_FILE IN LISTS MEMLY_AUTHORED_CXX_FILES)
  if(NOT MEMLY_AUTHORED_CXX_FILE IN_LIST MEMLY_REGISTERED_CXX_FILES)
    message(FATAL_ERROR "Memly C++ file is not assigned to a target: ${MEMLY_AUTHORED_CXX_FILE}")
  endif()
endforeach()

set(MEMLY_CXX_TRANSLATION_UNIT_FILES ${MEMLY_REGISTERED_CXX_FILES})
list(FILTER MEMLY_CXX_TRANSLATION_UNIT_FILES INCLUDE REGEX "[.]cpp$")
set(MEMLY_CXX_HEADER_FILES ${MEMLY_REGISTERED_CXX_FILES})
list(FILTER MEMLY_CXX_HEADER_FILES INCLUDE REGEX "[.]hpp$")

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
  string(CONCAT MEMLY_PRIVATE_HEADER_VERIFICATION_CONDITION "$<AND:" "$<COMPILE_LANG_AND_ID:CXX,Clang>,"
                "$<STREQUAL:$<TARGET_PROPERTY:NAME>,${MEMLY_COMPONENT_TARGET}_verify_private_header_sets>>"
  )
  foreach(MEMLY_CLANG_WARNING_OPTION IN LISTS MEMLY_CLANG_WARNING_OPTIONS)
    target_compile_options(
      "${MEMLY_COMPONENT_TARGET}"
      INTERFACE "$<${MEMLY_INTERFACE_HEADER_VERIFICATION_CONDITION}:${MEMLY_CLANG_WARNING_OPTION}>"
    )
    target_compile_options(
      "${MEMLY_COMPONENT_TARGET}"
      PRIVATE "$<${MEMLY_PRIVATE_HEADER_VERIFICATION_CONDITION}:${MEMLY_CLANG_WARNING_OPTION}>"
    )
  endforeach()
endforeach()

set(MEMLY_INCLUDE_WHAT_YOU_USE_STAMP "${CMAKE_BINARY_DIR}/.memly-include-what-you-use.stamp")
add_custom_command(
  OUTPUT "${MEMLY_INCLUDE_WHAT_YOU_USE_STAMP}"
  COMMAND
    "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tool/run_include_what_you_use.py"
    "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/.iwyu.imp" --output-format clang
    --include-what-you-use-driver "${MEMLY_INCLUDE_WHAT_YOU_USE_DRIVER_EXECUTABLE}" --include-what-you-use-executable
    "${MEMLY_INCLUDE_WHAT_YOU_USE_EXECUTABLE}" --project-root "${CMAKE_SOURCE_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E touch "${MEMLY_INCLUDE_WHAT_YOU_USE_STAMP}"
  DEPENDS "${CMAKE_SOURCE_DIR}/.iwyu.imp"
          "${CMAKE_SOURCE_DIR}/tool/run_include_what_you_use.py"
          "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${MEMLY_INCLUDE_WHAT_YOU_USE_DRIVER_EXECUTABLE}"
          "${MEMLY_INCLUDE_WHAT_YOU_USE_EXECUTABLE}"
          ${MEMLY_CXX_TRANSLATION_UNIT_FILES}
          ${MEMLY_CXX_HEADER_FILES}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Run the mandatory implementation-associated header Include What You Use gate before compilation"
  VERBATIM
)
add_custom_target(
  memly-include-what-you-use DEPENDS "${MEMLY_INCLUDE_WHAT_YOU_USE_STAMP}"
  COMMENT "Run the mandatory implementation-associated header Include What You Use gate"
)

set(MEMLY_CLANG_TIDY_STAMP "${CMAKE_BINARY_DIR}/.memly-clang-tidy.stamp")
add_custom_command(
  OUTPUT "${MEMLY_CLANG_TIDY_STAMP}"
  COMMAND
    "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}" -p "${CMAKE_BINARY_DIR}" -clang-tidy-binary
    "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}" -config-file "${CMAKE_SOURCE_DIR}/.clang-tidy" -header-filter
    "^${CMAKE_SOURCE_DIR}/(program|test)/" -quiet -hide-progress "^${CMAKE_SOURCE_DIR}/(program|test)/.*[.]cpp$"
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
add_dependencies(memly-clang-tidy memly-include-what-you-use)

foreach(MEMLY_COMPONENT_TARGET IN LISTS MEMLY_COMPONENT_TARGETS)
  add_dependencies("${MEMLY_COMPONENT_TARGET}" memly-clang-tidy)
endforeach()
add_dependencies(memly_qml memly-clang-tidy)
add_dependencies(memly memly-clang-tidy)

add_custom_target(memly-header-verification DEPENDS all_verify_header_sets COMMENT "Compile every header independently")
add_dependencies(memly memly-header-verification)
