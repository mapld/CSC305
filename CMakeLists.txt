cmake_minimum_required(VERSION 2.8)
project(alrm_graphics)

#--- Load the common ICG configuration
include(common/config.cmake)
include(common/Eigen.cmake)
include(common/OpenGP.cmake)

#--- OpenGL configuration
include(common/OpenGL.cmake)
include(common/GLEW.cmake)
include(common/GLFW.cmake)

#--- Subprojects
#add_subdirectory(raytrace)
#add_subdirectory(rasterize)
#add_subdirectory(assign01)
#add_subdirectory(lab03)
add_subdirectory(Examples/Ex1_Simple)
