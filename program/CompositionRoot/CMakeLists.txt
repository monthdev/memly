file(GLOB MEMLY_COMPOSITION_ROOT_HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/CompositionRoot/*.hpp"
)
file(GLOB MEMLY_COMPOSITION_ROOT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_composition_root "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_COMPOSITION_ROOT_HEADERS} SOURCES
  ${MEMLY_COMPOSITION_ROOT_SOURCES}
)
target_link_libraries(
  memly_composition_root
  PUBLIC memly_application_service memly_infrastructure_database memly_infrastructure_repository
)
