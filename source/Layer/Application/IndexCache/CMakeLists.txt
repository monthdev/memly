file(GLOB MEMLY_INDEX_CACHE_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/module/*.mpp")

memly_add_component(Memly.IndexCache Memly.IndexCache PUBLIC_MODULE_INTERFACES ${MEMLY_INDEX_CACHE_MODULE_INTERFACES})
memly_link_component_dependencies(Memly.IndexCache PUBLIC Memly.Domain)
