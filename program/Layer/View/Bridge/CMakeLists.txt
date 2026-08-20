file(GLOB MEMLY_VIEW_BRIDGE_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_VIEW_BRIDGE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_view_bridge Memly.Bridge INTERFACES ${MEMLY_VIEW_BRIDGE_MODULE_INTERFACES} SOURCES ${MEMLY_VIEW_BRIDGE_SOURCES}
)
target_link_libraries(
  memly_view_bridge
  PUBLIC Qt6::Quick memly_application_invalidation memly_application_service memly_presentation_controller
         memly_presentation_model
  PRIVATE memly_support_exception
)
