file(GLOB MEMLY_DOMAIN_MODULE_INTERFACES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/export/*.cppm")
file(GLOB MEMLY_DOMAIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_module(
  memly_application_domain Memly.Domain INTERFACES ${MEMLY_DOMAIN_MODULE_INTERFACES} SOURCES ${MEMLY_DOMAIN_SOURCES}
)
target_link_libraries(
  memly_application_domain
  PRIVATE ICU::i18n ICU::uc memly_support_exception
)
