file(GLOB MEMLY_QT_APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/QtApp/*.hpp")
file(GLOB MEMLY_QT_APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_support_qt_app "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_QT_APP_HEADERS} SOURCES
  ${MEMLY_QT_APP_SOURCES}
)
target_link_libraries(
  memly_support_qt_app
  PRIVATE Qt6::Core
)
