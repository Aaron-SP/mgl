# MGL
Repository for the Minimal Graphics Library

Welcome to the Minimal Graphics Library! This is the prerelease alpha version 0.1.

This package is the source code distribution of the Minimal Graphics Library, hence abbreviated MGL. The library builds on linux and win32 platforms. 
A GNU makefile is available for compilation with GCC/MINGW for win32 platforms or GCC/X11 for linux platforms. 
The makefile should work for both environments without modification.

The GNU makefile contains various build targets.

- 'make' - builds all tests and examples.
- 'make benchmarks' - builds only benchmarks (gl_bench)
- 'make examples' - builds only examples (ex1-ex7)
- 'make tests' - builds only tests (gl_test, wl_test)
- 'make lib' - builds a static library (libmin.a)
- 'make clean' - cleans up all generated output files

These build targets have been tested for compilation on Arch Linux x64 and Windows 7 x64 platforms.

GLEW is used within the engine for adding OpenGL function support.
You can download GLEW at http://glew.sourceforge.net/

Additional dependencies that are required and not distributed in this package include:

- 'mingw32' - for win32 only
- 'freetype' - for win32 and linux
- 'X11' - for linux only

For Arch Linux users, the following pacman command should install all needed dependencies:

- pacman -S freetype2 xorg-server xorg-server-devel

To verify freetype has been installed properly, query it using the following command:

- freetype-config --cflags

To perform code formatting, run the formatting script command in the project root (clang is required):

- ./format