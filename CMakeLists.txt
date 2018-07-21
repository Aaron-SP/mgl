cmake_minimum_required(VERSION 3.6.2)
project( MGL )

# Set the build flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_BUILD_TYPE Release)

# Fast Math
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
		add_compile_options(-fomit-frame-pointer -freciprocal-math -ffast-math)
        # add_compile_options(--param max-inline-insns-auto=100)
        # add_compile_options(--param early-inlining-insns=200)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
		add_compile_options(/fp:fast)
endif()

include_directories( source/file )
include_directories( source/geom )
include_directories( source/math )
include_directories( source/platform )
include_directories( source/renderer )
include_directories( source/scene )
include_directories( source/sound )
include_directories( source/util )
add_subdirectory( examples )
