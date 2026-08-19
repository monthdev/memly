file(GLOB MEMLY_SERVICE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Service/*.hpp")
file(GLOB MEMLY_SERVICE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_application_service "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_SERVICE_HEADERS} SOURCES
  ${MEMLY_SERVICE_SOURCES}
)
target_link_libraries(
  memly_application_service
  PUBLIC memly_application_index_cache
  PRIVATE memly_application_domain memly_infrastructure_repository
)
