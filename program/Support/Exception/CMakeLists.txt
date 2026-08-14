file(GLOB MEMLY_EXCEPTION_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Exception/*.hpp")
file(GLOB MEMLY_EXCEPTION_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_support_exception "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_EXCEPTION_HEADERS} SOURCES
  ${MEMLY_EXCEPTION_SOURCES}
)
target_link_libraries(
  memly_support_exception
  PRIVATE memly_support_qt_app
)
