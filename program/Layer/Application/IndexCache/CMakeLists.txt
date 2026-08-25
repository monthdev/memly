file(GLOB MEMLY_INDEX_CACHE_EXPORTED_MODULES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")

memly_add_component(Memly.IndexCache Memly.IndexCache PUBLIC_MODULES ${MEMLY_INDEX_CACHE_EXPORTED_MODULES})
memly_link_component_dependencies(Memly.IndexCache PUBLIC Memly.Domain)
