cmake_minimum_required(VERSION 4.4)
project(memly LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(QT_QML_GENERATE_QMLLS_INI ON)

include(CTest)
include(FetchContent)

if(NOT CMAKE_GENERATOR MATCHES "Ninja|Makefiles")
  message(FATAL_ERROR "Memly requires a Ninja or Makefiles generator for the required clang-tidy lint build")
endif()

find_program(
  MEMLY_LLVM_CLANG_TIDY_EXECUTABLE NAMES clang-tidy HINTS "/opt/homebrew/opt/llvm/bin" "/usr/local/opt/llvm/bin"
                                                          REQUIRED
)
cmake_path(GET MEMLY_LLVM_CLANG_TIDY_EXECUTABLE PARENT_PATH MEMLY_LLVM_BINARY_DIRECTORY)
find_program(
  MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE
  NAMES run-clang-tidy
  HINTS "${MEMLY_LLVM_BINARY_DIRECTORY}"
  NO_DEFAULT_PATH REQUIRED
)

set(MEMLY_QT_MIN_VERSION 6.11)

find_package(Qt6 ${MEMLY_QT_MIN_VERSION} REQUIRED COMPONENTS Core Network Qml Quick)
find_package(ICU REQUIRED COMPONENTS uc i18n)
qt_standard_project_setup(REQUIRES ${MEMLY_QT_MIN_VERSION})

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  add_custom_target(
    memly-docs
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/Doxyfile"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Generate Doxygen documentation"
    VERBATIM
  )
else()
  message(STATUS "Doxygen not found; memly-docs target will not be available")
endif()

set(APP_MAIN_CPP "${CMAKE_SOURCE_DIR}/src/main.cpp")

file(GLOB BOOTSTRAP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Bootstrap/*.cpp")
file(GLOB BOOTSTRAP_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Bootstrap/*.[Hh]*")

file(GLOB APPLICATION_DOMAIN_DECK_CONSTRAINT_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Deck/Constraint/*.cpp"
)
file(GLOB APPLICATION_DOMAIN_DECK_CONSTRAINT_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Deck/Constraint/*.[Hh]*"
)
file(GLOB APPLICATION_DOMAIN_DECK_INDEX_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Deck/Index/*.cpp"
)
file(GLOB APPLICATION_DOMAIN_DECK_INDEX_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Deck/Index/*.[Hh]*"
)
file(GLOB APPLICATION_DOMAIN_LANGUAGE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/Domain/Language/*.cpp")
file(GLOB APPLICATION_DOMAIN_LANGUAGE_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Language/*.[Hh]*"
)
file(GLOB APPLICATION_DOMAIN_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/ReviewSession/*.cpp"
)
file(GLOB APPLICATION_DOMAIN_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/ReviewSession/*.[Hh]*"
)
file(GLOB APPLICATION_DOMAIN_SCHEDULER_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Scheduler/*.cpp"
)
file(GLOB APPLICATION_DOMAIN_SCHEDULER_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Scheduler/*.[Hh]*"
)
file(GLOB APPLICATION_DOMAIN_TEXT_DATA_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Text/Data/*.cpp"
)
file(GLOB APPLICATION_DOMAIN_TEXT_DATA_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Domain/Text/Data/*.[Hh]*"
)
file(GLOB APPLICATION_INDEX_CACHE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/IndexCache/*.cpp")
file(GLOB APPLICATION_INDEX_CACHE_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/IndexCache/*.[Hh]*")
file(GLOB APPLICATION_INDEX_CACHE_DECK_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/IndexCache/Deck/*.cpp"
)
file(GLOB APPLICATION_INDEX_CACHE_DECK_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/IndexCache/Deck/*.[Hh]*"
)
file(GLOB APPLICATION_INVALIDATION_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/Invalidation/*.cpp")
file(GLOB APPLICATION_INVALIDATION_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/Invalidation/*.[Hh]*")
file(GLOB APPLICATION_SERVICE_DECK_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/Service/Deck/*.cpp")
file(GLOB APPLICATION_SERVICE_DECK_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Application/Service/Deck/*.[Hh]*")
file(GLOB APPLICATION_SERVICE_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Service/ReviewSession/*.cpp"
)
file(GLOB APPLICATION_SERVICE_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Application/Service/ReviewSession/*.[Hh]*"
)

file(GLOB INFRASTRUCTURE_DATABASE_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Database/*.cpp")
file(GLOB INFRASTRUCTURE_DATABASE_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Database/*.[Hh]*")
file(GLOB INFRASTRUCTURE_SQL_DECK_MUTATION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Deck/Mutation/*.cpp"
)
file(GLOB INFRASTRUCTURE_SQL_DECK_MUTATION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Deck/Mutation/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_SQL_DECK_QUERY_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Deck/Query/*.cpp"
)
file(GLOB INFRASTRUCTURE_SQL_DECK_QUERY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Deck/Query/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_SQL_LIBRARY_QUERY_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Library/Query/*.cpp"
)
file(GLOB INFRASTRUCTURE_SQL_LIBRARY_QUERY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Library/Query/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_SQL_MIGRATION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Migration/*.cpp"
)
file(GLOB INFRASTRUCTURE_SQL_MIGRATION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Migration/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_SQL_REVIEW_SESSION_MUTATION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/ReviewSession/Mutation/*.cpp"
)
file(GLOB INFRASTRUCTURE_SQL_REVIEW_SESSION_MUTATION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/ReviewSession/Mutation/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_SQL_REVIEW_SESSION_QUERY_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/ReviewSession/Query/*.cpp"
)
file(GLOB INFRASTRUCTURE_SQL_REVIEW_SESSION_QUERY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/ReviewSession/Query/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_SQL_SEED_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Seed/*.cpp")
file(GLOB INFRASTRUCTURE_SQL_SEED_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Seed/*.[Hh]*")
file(GLOB INFRASTRUCTURE_STORE_DECK_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Store/Deck/*.cpp")
file(GLOB INFRASTRUCTURE_STORE_DECK_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Store/Deck/*.[Hh]*")
file(GLOB INFRASTRUCTURE_STORE_LIBRARY_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Store/Library/*.cpp"
)
file(GLOB INFRASTRUCTURE_STORE_LIBRARY_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Store/Library/*.[Hh]*"
)
file(GLOB INFRASTRUCTURE_STORE_REVIEW_SESSION_CPP CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Store/ReviewSession/*.cpp"
)
file(GLOB INFRASTRUCTURE_STORE_REVIEW_SESSION_HDR CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Store/ReviewSession/*.[Hh]*"
)

file(GLOB PRESENTATION_CONTROLLER_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Presentation/Controller/*.cpp")
file(GLOB PRESENTATION_CONTROLLER_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Presentation/Controller/*.[Hh]*")
file(GLOB PRESENTATION_MODEL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Presentation/Model/*.cpp")
file(GLOB PRESENTATION_MODEL_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Presentation/Model/*.[Hh]*")

file(GLOB SUPPORT_RUNTIME_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/Runtime/*.cpp")
file(GLOB SUPPORT_RUNTIME_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/Runtime/*.[Hh]*")
file(GLOB SUPPORT_RUNTIME_QT_APP_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/Runtime/QtApp/*.cpp")
file(GLOB SUPPORT_RUNTIME_QT_APP_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/Runtime/QtApp/*.[Hh]*")
file(GLOB SUPPORT_TYPE_POLICY_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Support/TypePolicy/*.[Hh]*")

file(GLOB VIEW_BRIDGE_CONTROLLER_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/View/Bridge/Controller/*.cpp")
file(GLOB VIEW_BRIDGE_CONTROLLER_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/View/Bridge/Controller/*.[Hh]*")
file(GLOB VIEW_BRIDGE_PROXY_MODEL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/View/Bridge/ProxyModel/*.cpp")
file(GLOB VIEW_BRIDGE_PROXY_MODEL_HDR CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/View/Bridge/ProxyModel/*.[Hh]*")

set(QML_DIR "${CMAKE_SOURCE_DIR}/src/View/Qml")
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

file(GLOB SQL_DECK_MUTATION_STATEMENT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Deck/Mutation/Statement/*.sql"
)
file(GLOB SQL_DECK_QUERY_STATEMENT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Deck/Query/Statement/*.sql"
)
file(GLOB SQL_LIBRARY_QUERY_STATEMENT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Library/Query/Statement/*.sql"
)
file(GLOB SQL_MIGRATION_STATEMENT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Migration/Statement/*.sql"
)
file(GLOB SQL_REVIEW_SESSION_MUTATION_STATEMENT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/ReviewSession/Mutation/Statement/*.sql"
)
file(GLOB SQL_REVIEW_SESSION_QUERY_STATEMENT_SQL CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/ReviewSession/Query/Statement/*.sql"
)
file(GLOB SQL_SEED_STATEMENT_SQL CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql/Seed/Statement/*.sql")

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
  "${CMAKE_SOURCE_DIR}/src/Infrastructure/Sql"
  FILES
  ${SQL_DECK_MUTATION_STATEMENT_SQL}
  ${SQL_DECK_QUERY_STATEMENT_SQL}
  ${SQL_LIBRARY_QUERY_STATEMENT_SQL}
  ${SQL_MIGRATION_STATEMENT_SQL}
  ${SQL_REVIEW_SESSION_MUTATION_STATEMENT_SQL}
  ${SQL_REVIEW_SESSION_QUERY_STATEMENT_SQL}
  ${SQL_SEED_STATEMENT_SQL}
)

set(MEMLY_CORE_SOURCE_FILES
    ${APPLICATION_DOMAIN_DECK_CONSTRAINT_CPP}
    ${APPLICATION_DOMAIN_DECK_CONSTRAINT_HDR}
    ${APPLICATION_DOMAIN_DECK_INDEX_CPP}
    ${APPLICATION_DOMAIN_DECK_INDEX_HDR}
    ${APPLICATION_DOMAIN_LANGUAGE_CPP}
    ${APPLICATION_DOMAIN_LANGUAGE_HDR}
    ${APPLICATION_DOMAIN_REVIEW_SESSION_CPP}
    ${APPLICATION_DOMAIN_REVIEW_SESSION_HDR}
    ${APPLICATION_DOMAIN_SCHEDULER_CPP}
    ${APPLICATION_DOMAIN_SCHEDULER_HDR}
    ${APPLICATION_DOMAIN_TEXT_DATA_CPP}
    ${APPLICATION_DOMAIN_TEXT_DATA_HDR}
    ${APPLICATION_INDEX_CACHE_CPP}
    ${APPLICATION_INDEX_CACHE_HDR}
    ${APPLICATION_INDEX_CACHE_DECK_CPP}
    ${APPLICATION_INDEX_CACHE_DECK_HDR}
    ${APPLICATION_INVALIDATION_CPP}
    ${APPLICATION_INVALIDATION_HDR}
    ${APPLICATION_SERVICE_DECK_CPP}
    ${APPLICATION_SERVICE_DECK_HDR}
    ${APPLICATION_SERVICE_REVIEW_SESSION_CPP}
    ${APPLICATION_SERVICE_REVIEW_SESSION_HDR}
    ${INFRASTRUCTURE_DATABASE_CPP}
    ${INFRASTRUCTURE_DATABASE_HDR}
    ${INFRASTRUCTURE_SQL_DECK_MUTATION_CPP}
    ${INFRASTRUCTURE_SQL_DECK_MUTATION_HDR}
    ${INFRASTRUCTURE_SQL_DECK_QUERY_CPP}
    ${INFRASTRUCTURE_SQL_DECK_QUERY_HDR}
    ${INFRASTRUCTURE_SQL_LIBRARY_QUERY_CPP}
    ${INFRASTRUCTURE_SQL_LIBRARY_QUERY_HDR}
    ${INFRASTRUCTURE_SQL_MIGRATION_CPP}
    ${INFRASTRUCTURE_SQL_MIGRATION_HDR}
    ${INFRASTRUCTURE_SQL_REVIEW_SESSION_MUTATION_CPP}
    ${INFRASTRUCTURE_SQL_REVIEW_SESSION_MUTATION_HDR}
    ${INFRASTRUCTURE_SQL_REVIEW_SESSION_QUERY_CPP}
    ${INFRASTRUCTURE_SQL_REVIEW_SESSION_QUERY_HDR}
    ${INFRASTRUCTURE_SQL_SEED_CPP}
    ${INFRASTRUCTURE_SQL_SEED_HDR}
    ${INFRASTRUCTURE_STORE_DECK_CPP}
    ${INFRASTRUCTURE_STORE_DECK_HDR}
    ${INFRASTRUCTURE_STORE_LIBRARY_CPP}
    ${INFRASTRUCTURE_STORE_LIBRARY_HDR}
    ${INFRASTRUCTURE_STORE_REVIEW_SESSION_CPP}
    ${INFRASTRUCTURE_STORE_REVIEW_SESSION_HDR}
    ${PRESENTATION_CONTROLLER_CPP}
    ${PRESENTATION_CONTROLLER_HDR}
    ${PRESENTATION_MODEL_CPP}
    ${PRESENTATION_MODEL_HDR}
    ${SUPPORT_RUNTIME_CPP}
    ${SUPPORT_RUNTIME_HDR}
    ${SUPPORT_RUNTIME_QT_APP_CPP}
    ${SUPPORT_RUNTIME_QT_APP_HDR}
    ${SUPPORT_TYPE_POLICY_HDR}
)

add_library(memly_core STATIC ${MEMLY_CORE_SOURCE_FILES} $<TARGET_OBJECTS:memly_sql_resources>)

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
         ICU::uc
  PRIVATE ICU::i18n
)

set_target_properties(
  memly_core
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)

set(MEMLY_QML_SOURCE_FILES ${VIEW_BRIDGE_CONTROLLER_CPP} ${VIEW_BRIDGE_CONTROLLER_HDR} ${VIEW_BRIDGE_PROXY_MODEL_CPP}
                           ${VIEW_BRIDGE_PROXY_MODEL_HDR}
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
  memly_qml BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/View/Bridge/Controller"
                           "${CMAKE_SOURCE_DIR}/src/View/Bridge/ProxyModel"
)

target_link_libraries(
  memly_qml
  PRIVATE Qt6::Quick memly_warnings memly_asan
)

set(MEMLY_EXECUTABLE_SOURCE_FILES "${APP_MAIN_CPP}" ${BOOTSTRAP_CPP} ${BOOTSTRAP_HDR})

qt_add_executable(memly MANUAL_FINALIZATION ${MEMLY_EXECUTABLE_SOURCE_FILES})

target_link_libraries(
  memly
  PRIVATE memly_qml_plugin memly_core memly_warnings memly_asan
)

set_target_properties(
  memly
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)

qt_finalize_executable(memly)

if(BUILD_TESTING)
  add_custom_target(memly-test)
endif()

set(MEMLY_CXX_SOURCE_FILES ${MEMLY_CORE_SOURCE_FILES} ${MEMLY_QML_SOURCE_FILES} ${MEMLY_EXECUTABLE_SOURCE_FILES})
set(MEMLY_CXX_TRANSLATION_UNIT_FILES ${MEMLY_CXX_SOURCE_FILES})
list(FILTER MEMLY_CXX_TRANSLATION_UNIT_FILES INCLUDE REGEX "\\.(cc|cpp|cxx)$")

set(MEMLY_CLANG_TIDY_STAMP "${CMAKE_BINARY_DIR}/.memly-clang-tidy.stamp")
add_custom_command(
  OUTPUT "${MEMLY_CLANG_TIDY_STAMP}"
  COMMAND
    "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}" -p "${CMAKE_BINARY_DIR}" -clang-tidy-binary
    "${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}" -config-file "${CMAKE_SOURCE_DIR}/.clang-tidy" -header-filter
    "^${CMAKE_SOURCE_DIR}/(src|test)/" -quiet "^${CMAKE_SOURCE_DIR}/(src|test)/.*\\.cpp$"
  COMMAND "${CMAKE_COMMAND}" -E touch "${MEMLY_CLANG_TIDY_STAMP}"
  DEPENDS "${CMAKE_SOURCE_DIR}/.clang-tidy" "${CMAKE_BINARY_DIR}/compile_commands.json"
          "${MEMLY_LLVM_CLANG_TIDY_EXECUTABLE}" "${MEMLY_LLVM_RUN_CLANG_TIDY_EXECUTABLE}" ${MEMLY_CXX_SOURCE_FILES}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Run clang-tidy on Memly C++ translation units"
  VERBATIM
)

set_property(SOURCE ${MEMLY_CXX_TRANSLATION_UNIT_FILES} APPEND PROPERTY OBJECT_DEPENDS "${MEMLY_CLANG_TIDY_STAMP}")
