file(GLOB MEMLY_VIEW_BRIDGE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Bridge/*.hpp")
file(GLOB MEMLY_VIEW_BRIDGE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_view_bridge "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_VIEW_BRIDGE_HEADERS} SOURCES
  ${MEMLY_VIEW_BRIDGE_SOURCES}
)
target_link_libraries(
  memly_view_bridge
  PUBLIC Qt6::Quick memly_application_invalidation memly_application_service memly_presentation_controller
         memly_presentation_model
  PRIVATE memly_support_exception
)
set_property(TARGET memly_view_bridge PROPERTY MEMLY_QML_REGISTRATION_HEADERS "${MEMLY_VIEW_BRIDGE_HEADERS}")
