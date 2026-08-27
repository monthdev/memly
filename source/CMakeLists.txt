add_subdirectory(Support/QtApp)
add_subdirectory(Support/Exception)
add_subdirectory(Layer/Application/Domain)
add_subdirectory(Layer/Application/IndexCache)
add_subdirectory(Layer/Infrastructure/Database)
add_subdirectory(Layer/Infrastructure/Repository)
add_subdirectory(Layer/Application/Invalidation)
add_subdirectory(Layer/Application/Service)
add_subdirectory(Layer/Presentation/Model)
add_subdirectory(Layer/Presentation/Controller)
add_subdirectory(Layer/View/Bridge)
add_subdirectory(Layer/View/Qml)
add_subdirectory(CompositionRoot)

set(MEMLY_EXECUTABLE_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

qt_add_executable(memly MANUAL_FINALIZATION ${MEMLY_EXECUTABLE_SOURCE_FILES})
memly_configure_authored_cxx_files(memly ${MEMLY_EXECUTABLE_SOURCE_FILES})
set_property(TARGET memly PROPERTY MEMLY_CLANG_TIDY_FILES ${MEMLY_EXECUTABLE_SOURCE_FILES})
set_property(TARGET memly PROPERTY MEMLY_CLANG_TIDY_DEPENDS_ON_OWNING_TARGET FALSE)
set_target_properties(
  memly
  PROPERTIES CXX_SCAN_FOR_MODULES ON
)

memly_link_complete_component_closure(
  memly
  COMPONENTS
  Memly.CompositionRoot
  Memly.Bridge
  Memly.Controller
  Memly.Invalidation
  Memly.Model
  Memly.Service
  Memly.IndexCache
  Memly.Repository
  Memly.Database
  Memly.Domain
  Memly.Exception
  Memly.QtApp
  LINK_DEPENDENCIES
  memly_qml_plugin
  Qt6::Core
  Qt6::Qml
  Qt6::Quick
  ICU::i18n
  ICU::uc
  duckdb
  memly_sanitizers
)

qt_finalize_executable(memly)
