file(GLOB MEMLY_SERVICE_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_SERVICE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_application_service Memly.Service INTERFACES ${MEMLY_SERVICE_MODULE_INTERFACES} SOURCES
  ${MEMLY_SERVICE_SOURCES}
)
target_link_libraries(
  memly_application_service
  PUBLIC memly_application_index_cache memly_infrastructure_repository
  PRIVATE memly_application_domain
)
