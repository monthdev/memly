file(GLOB MEMLY_DATABASE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Database/*.hpp")
file(GLOB MEMLY_DATABASE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")
file(GLOB MEMLY_DATABASE_PRIVATE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp")
file(GLOB MEMLY_DATABASE_BOOTSTRAP_SQL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/Sql/Bootstrap/*.sql")
file(GLOB MEMLY_DATABASE_INSERT_SQL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/Sql/Insert/*.sql")
file(GLOB MEMLY_DATABASE_SELECT_SQL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/Sql/Select/*.sql")
file(GLOB MEMLY_DATABASE_VERSION_SQL CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/Sql/Version/*.sql")

memly_add_component_library(
  memly_infrastructure_database
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  HEADERS
  ${MEMLY_DATABASE_HEADERS}
  SOURCES
  ${MEMLY_DATABASE_SOURCES}
  ${MEMLY_DATABASE_PRIVATE_HEADERS}
)
target_include_directories(
  memly_infrastructure_database BEFORE PUBLIC "${_duckdb_wrapper_dir}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source"
)
target_include_directories(memly_infrastructure_database SYSTEM PUBLIC "${_duckdb_inc}")
target_link_libraries(
  memly_infrastructure_database
  PUBLIC duckdb memly_support_exception
  PRIVATE memly_support_qt_app
)
set_target_properties(
  memly_infrastructure_database
  PROPERTIES BUILD_RPATH "${_duckdb_libdir}"
)
qt_add_resources(
  memly_infrastructure_database
  "DatabaseMigrationSql"
  PREFIX
  "/Sql"
  BASE
  "${CMAKE_CURRENT_SOURCE_DIR}/source/Sql"
  FILES
  ${MEMLY_DATABASE_BOOTSTRAP_SQL}
  ${MEMLY_DATABASE_INSERT_SQL}
  ${MEMLY_DATABASE_SELECT_SQL}
  ${MEMLY_DATABASE_VERSION_SQL}
)
