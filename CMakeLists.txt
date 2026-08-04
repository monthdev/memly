cmake_minimum_required(VERSION 4.4)
project(memly LANGUAGES C CXX)

set(MEMLY_LLVM_MAJOR_VERSION 22)

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

if(NOT CMAKE_GENERATOR MATCHES "Ninja|Makefiles")
  message(FATAL_ERROR "Memly requires a Ninja or Makefiles generator for the required clang-tidy lint build")
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
  set(MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy.cmd")
  file(GENERATE OUTPUT "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
       CONTENT "@echo off\r\n\"${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}\" --experimental-custom-checks %*\r\n"
  )
else()
  set(MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE "${CMAKE_BINARY_DIR}/memly-clang-tidy")
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

set(APP_MAIN_CPP "${CMAKE_SOURCE_DIR}/program/main.cpp")

file(GLOB COMPOSITION_ROOT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/CompositionRoot/*.cpp")
file(GLOB COMPOSITION_ROOT_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/CompositionRoot/*.[Hh]*")

file(GLOB LAYER_APPLICATION_DOMAIN_DECK_CONSTRAINT_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Deck/Constraint/*.cpp"
)
file(GLOB LAYER_APPLICATION_DOMAIN_DECK_CONSTRAINT_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Deck/Constraint/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_DOMAIN_DECK_INDEX_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Deck/Index/*.cpp"
)
file(GLOB LAYER_APPLICATION_DOMAIN_DECK_INDEX_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Deck/Index/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_DOMAIN_LANGUAGE_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Language/*.cpp"
)
file(GLOB LAYER_APPLICATION_DOMAIN_LANGUAGE_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Language/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_DOMAIN_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/ReviewSession/*.cpp"
)
file(GLOB LAYER_APPLICATION_DOMAIN_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/ReviewSession/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_DOMAIN_SCHEDULER_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Scheduler/*.cpp"
)
file(GLOB LAYER_APPLICATION_DOMAIN_SCHEDULER_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Scheduler/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_DOMAIN_TEXT_DATA_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Text/Data/*.cpp"
)
file(GLOB LAYER_APPLICATION_DOMAIN_TEXT_DATA_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Domain/Text/Data/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_INDEX_CACHE_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/IndexCache/*.cpp"
)
file(GLOB LAYER_APPLICATION_INDEX_CACHE_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/IndexCache/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_INDEX_CACHE_DECK_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/IndexCache/Deck/*.cpp"
)
file(GLOB LAYER_APPLICATION_INDEX_CACHE_DECK_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/IndexCache/Deck/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_INVALIDATION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Invalidation/*.cpp"
)
file(GLOB LAYER_APPLICATION_INVALIDATION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Invalidation/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_SERVICE_DECK_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Service/Deck/*.cpp"
)
file(GLOB LAYER_APPLICATION_SERVICE_DECK_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Service/Deck/*.[Hh]*"
)
file(GLOB LAYER_APPLICATION_SERVICE_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Service/ReviewSession/*.cpp"
)
file(GLOB LAYER_APPLICATION_SERVICE_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Application/Service/ReviewSession/*.[Hh]*"
)

file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_DATABASE_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Database/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_DATABASE_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Database/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_DECK_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Deck/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_DECK_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Deck/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_LIBRARY_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Library/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_LIBRARY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Library/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_MIGRATION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Migration/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_MIGRATION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Migration/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/ReviewSession/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/ReviewSession/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_SEED_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Seed/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_SEED_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Seed/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_DECK_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Repository/Deck/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_DECK_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Repository/Deck/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_LIBRARY_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Repository/Library/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_LIBRARY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Repository/Library/*.[Hh]*"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Repository/ReviewSession/*.cpp"
)
file(GLOB LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Repository/ReviewSession/*.[Hh]*"
)

file(GLOB SQL_DECK_SELECT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Deck/Select/*.sql"
)
file(GLOB SQL_DECK_UPDATE_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Deck/Update/*.sql"
)
file(GLOB SQL_DECK_INSERT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Deck/Insert/*.sql"
)
file(GLOB SQL_DECK_DELETE_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Deck/Delete/*.sql"
)
file(GLOB SQL_LIBRARY_SELECT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Library/Select/*.sql"
)
file(GLOB SQL_MIGRATION_BOOTSTRAP_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Migration/Bootstrap/*.sql"
)
file(GLOB SQL_MIGRATION_VERSION_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Migration/Version/*.sql"
)
file(GLOB SQL_MIGRATION_SELECT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Migration/Select/*.sql"
)
file(GLOB SQL_MIGRATION_INSERT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Migration/Insert/*.sql"
)
file(GLOB SQL_REVIEW_SESSION_SELECT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/ReviewSession/Select/*.sql"
)
file(GLOB SQL_REVIEW_SESSION_UPDATE_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/ReviewSession/Update/*.sql"
)
file(GLOB SQL_REVIEW_SESSION_INSERT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/ReviewSession/Insert/*.sql"
)
file(GLOB SQL_REVIEW_SESSION_DELETE_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/ReviewSession/Delete/*.sql"
)
file(GLOB SQL_SEED_INSERT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql/Seed/Insert/*.sql"
)

file(GLOB LAYER_PRESENTATION_CONTROLLER_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Presentation/Controller/*.cpp"
)
file(GLOB LAYER_PRESENTATION_CONTROLLER_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/Presentation/Controller/*.[Hh]*"
)
file(GLOB LAYER_PRESENTATION_MODEL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/Layer/Presentation/Model/*.cpp")
file(GLOB LAYER_PRESENTATION_MODEL_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/Layer/Presentation/Model/*.[Hh]*")

file(GLOB LAYER_VIEW_BRIDGE_CONTROLLER_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/View/Bridge/Controller/*.cpp"
)
file(GLOB LAYER_VIEW_BRIDGE_CONTROLLER_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/View/Bridge/Controller/*.[Hh]*"
)
file(GLOB LAYER_VIEW_BRIDGE_PROXY_MODEL_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/View/Bridge/ProxyModel/*.cpp"
)
file(GLOB LAYER_VIEW_BRIDGE_PROXY_MODEL_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Layer/View/Bridge/ProxyModel/*.[Hh]*"
)

set(QML_DIR "${CMAKE_SOURCE_DIR}/program/Layer/View/Qml")
file(GLOB QML_ROOT CONFIGURE_DEPENDS "${QML_DIR}/*.qml")
file(GLOB QML_COMPONENT CONFIGURE_DEPENDS "${QML_DIR}/Component/*.qml")
file(GLOB QML_SCREEN_CARD CONFIGURE_DEPENDS "${QML_DIR}/Screen/Card/*.qml")
file(GLOB QML_SECTION_ADD_CARD CONFIGURE_DEPENDS "${QML_DIR}/Section/AddCard/*.qml")
file(GLOB QML_SECTION_BROWSE CONFIGURE_DEPENDS "${QML_DIR}/Section/Browse/*.qml")
file(GLOB QML_SECTION_DECKS CONFIGURE_DEPENDS "${QML_DIR}/Section/Decks/*.qml")
set(QT_QML ${QML_ROOT} ${QML_COMPONENT} ${QML_SCREEN_CARD} ${QML_SECTION_ADD_CARD} ${QML_SECTION_BROWSE}
           ${QML_SECTION_DECKS}
)
foreach(qml_file IN LISTS QT_QML)
  file(RELATIVE_PATH rel_qml "${QML_DIR}" "${qml_file}")
  set_source_files_properties("${qml_file}" PROPERTIES QT_RESOURCE_ALIAS "${rel_qml}")
endforeach()

file(GLOB SUPPORT_RUNTIME_EXCEPTION_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/Support/Runtime/Exception/*.cpp")
file(GLOB SUPPORT_RUNTIME_EXCEPTION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Support/Runtime/Exception/*.[Hh]*"
)
file(GLOB SUPPORT_RUNTIME_QT_APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/Support/Runtime/QtApp/*.cpp")
file(GLOB SUPPORT_RUNTIME_QT_APP_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/program/Support/Runtime/QtApp/*.[Hh]*")
file(GLOB SUPPORT_SPECIAL_MEMBER_POLICY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/program/Support/SpecialMemberPolicy/*.[Hh]*"
)

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

add_library(memly_sql_resources OBJECT)
set_target_properties(
  memly_sql_resources
  PROPERTIES AUTOMOC OFF
)

qt_add_resources(
  memly_sql_resources
  "Sql"
  PREFIX
  "/Sql"
  BASE
  "${CMAKE_SOURCE_DIR}/program/Layer/Infrastructure/Persistence/Sql"
  FILES
  ${SQL_DECK_SELECT_SQL}
  ${SQL_DECK_UPDATE_SQL}
  ${SQL_DECK_INSERT_SQL}
  ${SQL_DECK_DELETE_SQL}
  # ${SQL_LIBRARY_SELECT_SQL}
  ${SQL_MIGRATION_BOOTSTRAP_SQL}
  ${SQL_MIGRATION_VERSION_SQL}
  ${SQL_MIGRATION_SELECT_SQL}
  ${SQL_MIGRATION_INSERT_SQL}
  # ${SQL_REVIEW_SESSION_SELECT_SQL} ${SQL_REVIEW_SESSION_UPDATE_SQL} ${SQL_REVIEW_SESSION_INSERT_SQL}
  # ${SQL_REVIEW_SESSION_DELETE_SQL}
  ${SQL_SEED_INSERT_SQL}
)

set(MEMLY_CORE_SOURCE_FILES
    ${LAYER_APPLICATION_DOMAIN_DECK_CONSTRAINT_CPP}
    ${LAYER_APPLICATION_DOMAIN_DECK_CONSTRAINT_HDR}
    ${LAYER_APPLICATION_DOMAIN_DECK_INDEX_CPP}
    ${LAYER_APPLICATION_DOMAIN_DECK_INDEX_HDR}
    ${LAYER_APPLICATION_DOMAIN_LANGUAGE_CPP}
    ${LAYER_APPLICATION_DOMAIN_LANGUAGE_HDR}
    ${LAYER_APPLICATION_DOMAIN_REVIEW_SESSION_CPP}
    ${LAYER_APPLICATION_DOMAIN_REVIEW_SESSION_HDR}
    ${LAYER_APPLICATION_DOMAIN_SCHEDULER_CPP}
    ${LAYER_APPLICATION_DOMAIN_SCHEDULER_HDR}
    ${LAYER_APPLICATION_DOMAIN_TEXT_DATA_CPP}
    ${LAYER_APPLICATION_DOMAIN_TEXT_DATA_HDR}
    ${LAYER_APPLICATION_INDEX_CACHE_CPP}
    ${LAYER_APPLICATION_INDEX_CACHE_HDR}
    ${LAYER_APPLICATION_INDEX_CACHE_DECK_CPP}
    ${LAYER_APPLICATION_INDEX_CACHE_DECK_HDR}
    ${LAYER_APPLICATION_INVALIDATION_CPP}
    ${LAYER_APPLICATION_INVALIDATION_HDR}
    ${LAYER_APPLICATION_SERVICE_DECK_CPP}
    ${LAYER_APPLICATION_SERVICE_DECK_HDR}
    ${LAYER_APPLICATION_SERVICE_REVIEW_SESSION_CPP}
    ${LAYER_APPLICATION_SERVICE_REVIEW_SESSION_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_DATABASE_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_DATABASE_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_DECK_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_DECK_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_LIBRARY_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_LIBRARY_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_MIGRATION_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_MIGRATION_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_REVIEW_SESSION_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_REVIEW_SESSION_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_SEED_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_SQL_SEED_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_DECK_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_DECK_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_LIBRARY_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_LIBRARY_HDR}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_REVIEW_SESSION_CPP}
    ${LAYER_INFRASTRUCTURE_PERSISTENCE_REPOSITORY_REVIEW_SESSION_HDR}
    ${LAYER_PRESENTATION_CONTROLLER_CPP}
    ${LAYER_PRESENTATION_CONTROLLER_HDR}
    ${LAYER_PRESENTATION_MODEL_CPP}
    ${LAYER_PRESENTATION_MODEL_HDR}
    ${SUPPORT_RUNTIME_EXCEPTION_CPP}
    ${SUPPORT_RUNTIME_EXCEPTION_HDR}
    ${SUPPORT_RUNTIME_QT_APP_CPP}
    ${SUPPORT_RUNTIME_QT_APP_HDR}
    ${SUPPORT_SPECIAL_MEMBER_POLICY_HDR}
)

add_library(memly_core STATIC ${MEMLY_CORE_SOURCE_FILES} $<TARGET_OBJECTS:memly_sql_resources>)

target_include_directories(memly_core BEFORE PUBLIC "${CMAKE_SOURCE_DIR}/program" "${_duckdb_wrapper_dir}")

target_include_directories(memly_core SYSTEM PUBLIC "${_duckdb_inc}")

target_link_libraries(
  memly_core
  PUBLIC Qt6::Core
         Qt6::Network
         Qt6::Qml
         Qt6::Quick
         duckdb
         memly_asan
         ICU::uc
  PRIVATE ICU::i18n
)

set_target_properties(
  memly_core
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)

set(MEMLY_QML_SOURCE_FILES ${LAYER_VIEW_BRIDGE_CONTROLLER_CPP} ${LAYER_VIEW_BRIDGE_CONTROLLER_HDR}
                           ${LAYER_VIEW_BRIDGE_PROXY_MODEL_CPP} ${LAYER_VIEW_BRIDGE_PROXY_MODEL_HDR}
)

qt_add_qml_module(
  memly_qml
  STATIC
  URI
  Memly
  VERSION
  1.0
  PLUGIN_TARGET
  memly_qml_plugin
  OUTPUT_DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/Memly"
  SOURCES
  ${MEMLY_QML_SOURCE_FILES}
  QML_FILES
  ${QT_QML}
)

target_include_directories(
  memly_qml BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/program" "${CMAKE_SOURCE_DIR}/program/Layer/View/Bridge/Controller"
                           "${CMAKE_SOURCE_DIR}/program/Layer/View/Bridge/ProxyModel"
)

target_link_libraries(
  memly_qml
  PRIVATE Qt6::Quick memly_asan
)

set(MEMLY_EXECUTABLE_SOURCE_FILES "${APP_MAIN_CPP}" ${COMPOSITION_ROOT_CPP} ${COMPOSITION_ROOT_HDR})

qt_add_executable(memly MANUAL_FINALIZATION ${MEMLY_EXECUTABLE_SOURCE_FILES})

target_link_libraries(
  memly
  PRIVATE memly_qml_plugin memly_core memly_asan
)

set_target_properties(
  memly
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)

qt_finalize_executable(memly)

if(BUILD_TESTING)
  add_custom_target(memly-test COMMENT "Memly tests are not currently implemented")
endif()

set(MEMLY_CXX_SOURCE_FILES ${MEMLY_CORE_SOURCE_FILES} ${MEMLY_QML_SOURCE_FILES} ${MEMLY_EXECUTABLE_SOURCE_FILES})
set(MEMLY_CXX_TRANSLATION_UNIT_FILES ${MEMLY_CXX_SOURCE_FILES})
list(FILTER MEMLY_CXX_TRANSLATION_UNIT_FILES INCLUDE REGEX "\\.(cc|cpp|cxx)$")

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

set(MEMLY_CLANG_TIDY_STAMP "${CMAKE_BINARY_DIR}/.memly-clang-tidy.stamp")
add_custom_command(
  OUTPUT "${MEMLY_CLANG_TIDY_STAMP}"
  COMMAND
    "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}" -p "${CMAKE_BINARY_DIR}" -clang-tidy-binary
    "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}" -config-file "${CMAKE_SOURCE_DIR}/.clang-tidy" -header-filter
    "^${CMAKE_SOURCE_DIR}/(program|test)/" -quiet "^${CMAKE_SOURCE_DIR}/(program|test)/.*\\.cpp$"
  COMMAND "${CMAKE_COMMAND}" -E touch "${MEMLY_CLANG_TIDY_STAMP}"
  DEPENDS "${CMAKE_SOURCE_DIR}/.clang-tidy" "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}" "${MEMLY_LLVM_CLANG_TIDY_WRAPPER_EXECUTABLE}"
          "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}" ${MEMLY_CXX_SOURCE_FILES}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Run clang-tidy on Memly C++ translation units"
  VERBATIM
)

set_property(SOURCE ${MEMLY_CXX_TRANSLATION_UNIT_FILES} APPEND PROPERTY OBJECT_DEPENDS "${MEMLY_CLANG_TIDY_STAMP}")
