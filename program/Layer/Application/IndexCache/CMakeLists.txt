file(GLOB MEMLY_INDEX_CACHE_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")

memly_add_component_module(
  memly_application_index_cache Memly.IndexCache INTERFACES ${MEMLY_INDEX_CACHE_MODULE_INTERFACES}
)
target_link_libraries(
  memly_application_index_cache
  PUBLIC memly_application_domain
)
