cmake_minimum_required(VERSION 3.12)
project(PluginExample VERSION 1.0 LANGUAGES CXX)

# Find and load CMake configuration of packages containing this plugin's dependencies
## Mandatory dependencies
find_package(Sofa.Component.Visual REQUIRED) # Needed for VisualModelImpl

## Optional dependencies
sofa_find_package(Sofa.GUI.Qt QUIET)
sofa_find_package(Qt5 COMPONENTS Core QUIET) # Dependency to Qt5Core (needed for qt5_wrap_cpp)

# List all files
set(PLUGINEXAMPLE_SRC_DIR src/PluginExample)
set(HEADER_FILES
    ${PLUGINEXAMPLE_SRC_DIR}/config.h.in
    ${PLUGINEXAMPLE_SRC_DIR}/MyBehaviorModel.h
    ${PLUGINEXAMPLE_SRC_DIR}/MyVisualModel.h
    ${PLUGINEXAMPLE_SRC_DIR}/MyMappingPendulumInPlane.h
    ${PLUGINEXAMPLE_SRC_DIR}/MyMappingPendulumInPlane.inl
    ${PLUGINEXAMPLE_SRC_DIR}/MyProjectiveConstraintSet.h
    ${PLUGINEXAMPLE_SRC_DIR}/MyProjectiveConstraintSet.inl
    )
set(HEADER_FILES_TO_MOC
    )
set(SOURCE_FILES
    ${PLUGINEXAMPLE_SRC_DIR}/initPluginExample.cpp
    ${PLUGINEXAMPLE_SRC_DIR}/MyBehaviorModel.cpp
    ${PLUGINEXAMPLE_SRC_DIR}/MyVisualModel.cpp
    ${PLUGINEXAMPLE_SRC_DIR}/MyMappingPendulumInPlane.cpp
    ${PLUGINEXAMPLE_SRC_DIR}/MyProjectiveConstraintSet.cpp
    )
set(README_FILES
    README.md
    )

if(Qt5Core_FOUND AND Sofa.GUI.Qt_FOUND)
    message(STATUS "GUI components have been detected, enable GUI-related components")
    list(APPEND HEADER_FILES_TO_MOC
        ${PLUGINEXAMPLE_SRC_DIR}/MyDataWidgetUnsigned.h
    )
    list(APPEND SOURCE_FILES
        ${PLUGINEXAMPLE_SRC_DIR}/MyDataWidgetUnsigned.cpp
    )

    # [Qt] Create moc code.
    qt5_wrap_cpp(MOCCED_HEADER_FILES ${HEADER_FILES_TO_MOC})
endif()


# Create the plugin library.
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES_TO_MOC} ${MOCCED_HEADER_FILES} ${HEADER_FILES} ${SOURCE_FILES} ${README_FILES})

# Link the plugin library to its dependency(ies).
target_link_libraries(${PROJECT_NAME} Sofa.Component.Visual)
# Link with the optional GUI dependencies.
if(Qt5Core_FOUND AND Sofa.GUI.Qt_FOUND)
    target_link_libraries(${PROJECT_NAME} Sofa.GUI.Qt)
endif()

# Create package Config, Version & Target files.
# Deploy the headers, resources, scenes & examples.
# Set the plugin 'relocatable' if built within SOFA.
# --> see SofaMacros.cmake
sofa_create_package_with_targets(
    PACKAGE_NAME ${PROJECT_NAME}
    PACKAGE_VERSION ${PROJECT_VERSION}
    TARGETS ${PROJECT_NAME} AUTO_SET_TARGET_PROPERTIES
    INCLUDE_SOURCE_DIR "src"
    INCLUDE_INSTALL_DIR ${PROJECT_NAME}
    RELOCATABLE "plugins"
    )

# Tests
# If SOFA_BUILD_TESTS exists and is OFF, then these tests will be auto-disabled
cmake_dependent_option(PLUGINEXAMPLE_BUILD_TESTS "Compile the automatic tests" ON "SOFA_BUILD_TESTS OR NOT DEFINED SOFA_BUILD_TESTS" OFF)
if(PLUGINEXAMPLE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(PluginExample_test)
endif()
