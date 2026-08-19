add_subdirectory(Support/QtApp)
add_subdirectory(Support/Exception)
add_subdirectory(Layer/Application/Domain)
add_subdirectory(Layer/Application/IndexCache)
add_subdirectory(Layer/Application/Invalidation)
add_subdirectory(Layer/Infrastructure/Database)
add_subdirectory(Layer/Infrastructure/Repository)
add_subdirectory(Layer/Application/Service)
add_subdirectory(Layer/Presentation/Model)
add_subdirectory(Layer/Presentation/Controller)
add_subdirectory(Layer/View/Bridge)
add_subdirectory(Layer/View/Qml)
add_subdirectory(CompositionRoot)

set(MEMLY_EXECUTABLE_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

set_property(GLOBAL APPEND PROPERTY MEMLY_REGISTERED_CXX_FILES ${MEMLY_EXECUTABLE_SOURCE_FILES})

qt_add_executable(memly MANUAL_FINALIZATION ${MEMLY_EXECUTABLE_SOURCE_FILES})

target_link_libraries(
  memly
  PRIVATE Qt6::Core
          Qt6::Qml
          Qt6::Quick
          memly_composition_root
          memly_qml_plugin
          memly_support_exception
          memly_support_qt_app
          memly_asan
)

qt_finalize_executable(memly)
