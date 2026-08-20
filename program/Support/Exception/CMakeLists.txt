file(GLOB MEMLY_EXCEPTION_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_EXCEPTION_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_support_exception Memly.Exception INTERFACES ${MEMLY_EXCEPTION_MODULE_INTERFACES} SOURCES
  ${MEMLY_EXCEPTION_SOURCES}
)
target_link_libraries(
  memly_support_exception
  PRIVATE memly_support_qt_app
)
