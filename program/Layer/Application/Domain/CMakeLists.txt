file(GLOB MEMLY_DOMAIN_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Memly/Domain/*.hpp")
file(GLOB MEMLY_DOMAIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

memly_add_component_library(
  memly_application_domain "${CMAKE_CURRENT_SOURCE_DIR}/include" HEADERS ${MEMLY_DOMAIN_HEADERS} SOURCES
  ${MEMLY_DOMAIN_SOURCES}
)
target_link_libraries(
  memly_application_domain
  PUBLIC ICU::uc
  PRIVATE ICU::i18n memly_support_exception
)
