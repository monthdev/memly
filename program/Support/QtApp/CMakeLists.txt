file(GLOB MEMLY_QT_APP_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_QT_APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_support_qt_app Memly.QtApp INTERFACES ${MEMLY_QT_APP_MODULE_INTERFACES} SOURCES ${MEMLY_QT_APP_SOURCES}
)
target_link_libraries(
  memly_support_qt_app
  PRIVATE Qt6::Core
)
