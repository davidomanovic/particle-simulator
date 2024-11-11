cmake_minimum_required(VERSION 3.10)
project(ParticleSimulation)

# Specify C standard
set(CMAKE_C_STANDARD 99)

# Add GLFW as a subdirectory
add_subdirectory(libs/glfw)

# Add include directories
include_directories(include)  # Your local project include directory
include_directories(libs/glfw/include)  # GLFW's include directory

# Define macros to prevent Windows header conflicts
add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -DGLFW_INCLUDE_NONE)

# Define executable and link libraries
add_executable(ParticleSimulation src/main.c src/particles.c src/verlet.c)

# Link GLFW and OpenGL
target_link_libraries(ParticleSimulation glfw)
target_link_libraries(ParticleSimulation opengl32)  # Link OpenGL on Windows