cmake_minimum_required(VERSION 3.8)

project(orion)

add_definitions(-std=c++17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(spdlog REQUIRED)
find_package(stb REQUIRED)

file(GLOB SOURCES src/*.cpp)
add_library(orion SHARED ${SOURCES})
target_link_libraries(orion
	opengl::opengl
	glad::glad
	glfw::glfw 
	glm::glm
	Microsoft.GSL::GSL
	spdlog::spdlog
	stb::stb)
