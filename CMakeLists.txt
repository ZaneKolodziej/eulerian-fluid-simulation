cmake_minimum_required(VERSION  3.16)
project(EulerianFluidSimulation_Iter2 CXX C) # dont know if the C is needed, this line was copied from the older reference file

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ImGui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
)

#Gather the source files (whatever that means?)

#Automatically collect all .cpp files present in src/
file (GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(SOURCES
    ${PROJECT_SOURCES}

    #glad source
    external/glad/src/glad.c
    #ImGui Sources
    external/ImGui/imgui.cpp
    external/ImGui/imgui_draw.cpp
    external/ImGui/imgui_tables.cpp
    external/ImGui/imgui_widgets.cpp
    external/ImGui/backends/imgui_impl_glfw.cpp
    external/ImGui/backends/imgui_impl_opengl3.cpp
    #glfw sources
    

)

#Executable Target
add_executable(${PROJECT_NAME} ${SOURCES})

#Compile Definitions & Linker Flages

#Link directly to required system libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    gdi32
    winmm
    glfw3
)

#Link directly to required system directories
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib-mingw-w64
)

#copy Shaders Directory to Build Output on Build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)