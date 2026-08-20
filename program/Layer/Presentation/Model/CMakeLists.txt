file(GLOB MEMLY_MODEL_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_MODEL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_presentation_model Memly.Model INTERFACES ${MEMLY_MODEL_MODULE_INTERFACES} SOURCES ${MEMLY_MODEL_SOURCES}
)
target_link_libraries(
  memly_presentation_model
  PUBLIC Qt6::Core memly_application_domain
  PRIVATE memly_support_exception
)
