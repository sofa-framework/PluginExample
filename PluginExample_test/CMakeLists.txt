cmake_minimum_required(VERSION 3.12)

project(PluginExample_test)

# find_package(PluginExample REQUIRED)
find_package(Threads REQUIRED)
find_package(Sofa.Testing REQUIRED)

set(SOURCE_FILES
    MyBehaviorModel_test.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.Testing PluginExample Threads::Threads)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
