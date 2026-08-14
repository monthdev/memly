file(GLOB MEMLY_MODEL_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Model/*.hpp")
file(GLOB MEMLY_MODEL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_presentation_model "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_MODEL_HEADERS} SOURCES
  ${MEMLY_MODEL_SOURCES}
)
target_link_libraries(
  memly_presentation_model
  PUBLIC Qt6::Core memly_application_domain
  PRIVATE memly_support_exception
)
