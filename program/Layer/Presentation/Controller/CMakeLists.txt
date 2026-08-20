file(GLOB MEMLY_CONTROLLER_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_CONTROLLER_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_presentation_controller Memly.Controller INTERFACES ${MEMLY_CONTROLLER_MODULE_INTERFACES} SOURCES
  ${MEMLY_CONTROLLER_SOURCES}
)
target_link_libraries(
  memly_presentation_controller
  PUBLIC Qt6::Core memly_application_invalidation memly_presentation_model
  PRIVATE memly_application_service memly_support_exception
)
