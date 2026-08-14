file(GLOB MEMLY_CONTROLLER_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Controller/*.hpp")
file(GLOB MEMLY_CONTROLLER_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_presentation_controller "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_CONTROLLER_HEADERS} SOURCES
  ${MEMLY_CONTROLLER_SOURCES}
)
target_link_libraries(
  memly_presentation_controller
  PUBLIC Qt6::Core memly_application_invalidation memly_presentation_model
  PRIVATE memly_application_service memly_support_exception
)
