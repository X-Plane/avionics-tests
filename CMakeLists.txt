cmake_minimum_required(VERSION 3.12)
project(avionics VERSION 1.0 LANGUAGES C CXX)
include(cmake/xplm.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SDK_ROOT "${PROJECT_SOURCE_DIR}/SDK")

if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version" FORCE)
endif()

use_static_libc()
find_xplane_sdk(${SDK_ROOT} 411)
find_package(OpenGL REQUIRED)

add_xplane_plugin(avionics
    src/plugin.c
	src/stock_override.c
	src/custom_device.c
    src/SystemGL.h
)
target_link_libraries(avionics PUBLIC xplm ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES})

