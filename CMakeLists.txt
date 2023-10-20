cmake_minimum_required(VERSION 3.20)
project(FATE)

# Set C++ version
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(assimp)
add_subdirectory(glad)
add_subdirectory(json)

include(FetchContent)
FetchContent_Declare(cpr 
GIT_REPOSITORY https://github.com/libcpr/cpr.git          
GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba)
FetchContent_MakeAvailable(cpr)

find_package(OpenGL REQUIRED)

include_directories(
	include
	glfw/include
	glm/glm
	assimp/include
	imgui
	imgui/backends
	stb
	json/include
)

set(SOURCES
	src/animationSelector.cpp
	src/animator.cpp
	src/camera.cpp
	src/downloadManager.cpp
	src/fate.cpp
	src/main.cpp
	src/meshSelector.cpp
	src/model.cpp
	src/modelSelector.cpp
	src/renderable.cpp
	src/renderer.cpp
	src/shader.cpp
	src/stb_image.cpp
	src/stb_image_write.cpp
	src/viewport.cpp

	include/animationSelector.h
	include/animator.h
	include/camera.h
	include/constants.h
	include/downloadManager.h
	include/fate.h
	include/meshSelector.h
	include/model.h
	include/modelSelector.h
	include/renderable.h
	include/renderer.h
	include/shader.h
	include/viewport.h
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/shaders/vertex.glsl
                ${CMAKE_CURRENT_BINARY_DIR}/vertex.glsl)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/shaders/fragment.glsl
                ${CMAKE_CURRENT_BINARY_DIR}/fragment.glsl)

add_library(imgui
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui_demo.cpp
 	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC imgui)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
	imgui 
	glfw
	glm
 	assimp
 	glad
 	nlohmann_json::nlohmann_json
 	${OPENGL_LIBRARIES}
	cpr::cpr
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
	include
)
