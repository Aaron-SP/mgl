cmake_minimum_required(VERSION 2.8.12)
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

function( add_executable ex )

# Add new example and set output directory
add_executable( ${ex} ${ex}.cpp )
set_target_properties( ${ex} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin" )

endfunction ( add_executable )

# Add OpenGL support to executable
function( add_opengl ex )

# Find OpenGL on the system
find_package(OpenGL)
if(OPENGL_FOUND)
    target_include_directories( ${ex} PUBLIC ${OPENGL_INCLUDE_DIR} )
    target_link_libraries( ${ex} ${OPENGL_LIBRARIES} )
endif()

# Find GLEW on WINDOWS
find_package(GLEW)
if (GLEW_FOUND AND OPENGL_FOUND)
    target_include_directories( ${ex} PUBLIC ${GLEW_INCLUDE_DIRS} )
    target_link_libraries( ${ex} ${GLEW_LIBRARIES} )
endif()

if (NOT GLEW_FOUND OR NOT OPENGL_FOUND)

    # Fallback to legacy OPENGL detection
    message( STATUS "Falling back to legacy OPENGL and GLEW detection" )

    # Specify CYGWIN
    if ( CYGWIN )

        # 64 bits
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)

        # Find GLEW on CYGWIN
        find_library(glew
                NAMES libglew32.dll.a
                PATHS "C:/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/lib" )

        # Link with opengl
        target_link_libraries( ${ex} gdi32.dll )
        target_link_libraries( ${ex} opengl32.dll )

        # 32 bits
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        
        # Find GLEW on CYGWIN
        find_library(glew
                NAMES libglew32.dll.a
                PATHS "C:/cygwin/usr/i686-w64-mingw32/sys-root/mingw/lib" )

        # Link with opengl
        target_link_libraries( ${ex} gdi32.dll )
        target_link_libraries( ${ex} opengl32.dll )

        endif()

    else()

        # Find GLEW on UNIX
        find_library(glew 
                NAMES GLEW)

        # Link with opengl
        target_link_libraries( ${ex} X11 )
        target_link_libraries( ${ex} GL )

    endif()

    # Link with GLEW
    target_link_libraries( ${ex} ${glew} )
    message( STATUS "glew=${glew}" )

endif()

endfunction ( add_opengl )

# Add FREETYPE2 support to executable
function( add_freetype ex )

# Find FREETYPE
find_package(Freetype REQUIRED)
if (FREETYPE_FOUND)
    target_include_directories( ${ex} PUBLIC ${FREETYPE_INCLUDE_DIRS} )
    target_link_libraries( ${ex} ${FREETYPE_LIBRARIES} )
endif()

if (NOT FREETYPE_FOUND)

    # Specify CYGWIN
    if ( CYGWIN )

        # 64 bits
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)

        # Find GLEW on CYGWIN
        find_library(freetype
                NAMES libfreetype.dll.a
                PATHS "C:/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/lib" )

        # 32 bits
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        
        # Find FREETYPE on CYGWIN
        find_library(freetype
                NAMES libfreetype.dll.a
                PATHS "C:/cygwin/usr/i686-w64-mingw32/sys-root/mingw/lib" )

        endif()

        # Get FREETYPE include path
        set(MAKE_CMD "pkg-config")
        execute_process(COMMAND ${MAKE_CMD} "freetype2" "--cflags"
            RESULT_VARIABLE CMD_ERROR
            OUTPUT_VARIABLE CMD_OUTPUT )

    else()

        # Find FREETYPE on UNIX
        find_library(freetype
                NAMES freetype)

        # Get FREETYPE include path
        set(MAKE_CMD "pkg-config")
        execute_process(COMMAND ${MAKE_CMD} "freetype2" "--cflags"
            RESULT_VARIABLE CMD_ERROR
            OUTPUT_VARIABLE CMD_OUTPUT )

    endif()

    # Include FREETYPE path
    string(REPLACE "-I" "" CMD_OUTPUT ${CMD_OUTPUT})
    separate_arguments(CMD_OUTPUT UNIX_COMMAND "${CMD_OUTPUT}")
    target_include_directories( ${ex} PUBLIC ${CMD_OUTPUT} )

    # Link with freetype
    target_link_libraries( ${ex} ${freetype} )
    message( STATUS "freetype=${freetype}" )

endif()

endfunction ( add_freetype )

# Source files
include_directories( source/file )
include_directories( source/geom )
include_directories( source/math )
include_directories( source/platform )
include_directories( source/renderer )
include_directories( source/scene )
include_directories( source/sound )
include_directories( source/util )

# Test files
include_directories( test/file )
include_directories( test/geom )
include_directories( test/math )
include_directories( test/platform )
include_directories( test/renderer )
include_directories( test/scene )
include_directories( test/sound )
include_directories( test/util )

# CPP folders
add_subdirectory( examples )
add_subdirectory( test )
