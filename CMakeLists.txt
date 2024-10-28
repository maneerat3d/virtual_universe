cmake_minimum_required(VERSION 3.24)
project(virtuniverse)

# Find libraries
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)

# Create the library target
add_executable(${PROJECT_NAME} src/main.cpp)

# Link libraries to the target
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
