file(GLOB MEMLY_INDEX_CACHE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/IndexCache/*.hpp")
file(GLOB MEMLY_INDEX_CACHE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_application_index_cache "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_INDEX_CACHE_HEADERS} SOURCES
  ${MEMLY_INDEX_CACHE_SOURCES}
)
target_link_libraries(
  memly_application_index_cache
  PUBLIC memly_application_domain
)
