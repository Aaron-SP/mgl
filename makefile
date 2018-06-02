# MGL Makefile - CYGWIN | LINUX

# Automatically generate variable name lists
CGRAPH_SOURCES = $(wildcard source/cpp/*.cpp) $(source/platform/min/glew.cpp)

# Convert the .cpp files to .o files
OBJGRAPH_SOURCES = $(CGRAPH_SOURCES:.cpp=.o)

# Query the freetype2 package config for the include directory
FREETYPE2_INCLUDE = $(shell pkg-config freetype2 --cflags)

# Linker parameters
ifeq ($(OS),Windows_NT)
	# 64 bit
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        MGL_PATH = C:/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/include/mgl
    endif

	#64 bit
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        MGL_PATH = C:/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/include/mgl
    else
	#32 bit
        MGL_PATH = C:/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/mgl
    endif

	LINKER = -lopengl32 -lgdi32 -lmingw32 -lfreetype.dll -lOpenAL32.dll -lvorbisfile.dll
else
	MGL_PATH = /usr/include/mgl
	LINKER = -lX11 -lGL -lfreetype -lopenal -lvorbisfile
endif

# Include directories
LIB_SOURCES = -Isource/file -Isource/geom -Isource/math -Isource/platform -Isource/renderer -Isource/scene -Isource/sound $(FREETYPE2_INCLUDE)
TEST_SOURCES = -Itest/file -Itest/geom -Itest/math -Itest/platform -Itest/renderer -Itest/scene -Itest/sound
BENCH_SOURCES = -Ibench/math -Ibench/geom -Ibench/scene -Ibench/file

# Enable GS rendering
ifdef MGL_VB43
	MGL_VB43 = -DMGL_VB43
endif

# Compile parameters
PARAMS = -s -std=c++14 -Wall -O3 -march=native -fomit-frame-pointer -freciprocal-math -ffast-math --param max-inline-insns-auto=100 --param early-inlining-insns=200 $(MGL_VB43)
EXTRA = source/platform/min/glew.cpp
WL_INCLUDE = -DGLEW_STATIC
TEST_AL = test/al_test.cpp
TEST_GL = test/gl_test.cpp
TEST_WL = $(EXTRA) test/wl_test.cpp
EX1 = $(EXTRA) example/programs/ex1.cpp
EX2 = $(EXTRA) example/programs/ex2.cpp
EX3 = $(EXTRA) example/programs/ex3.cpp
EX4 = $(EXTRA) example/programs/ex4.cpp
EX5 = $(EXTRA) example/programs/ex5.cpp
EX6 = $(EXTRA) example/programs/ex6.cpp
EX7 = $(EXTRA) example/programs/ex7.cpp
EX8 = $(EXTRA) example/programs/ex8.cpp
EX9 = $(EXTRA) example/programs/ex9.cpp
EX10 = $(EXTRA) example/programs/ex10.cpp

# Override if MGL_DESTDIR specified
ifdef MGL_DESTDIR
	MGL_PATH = $(MGL_DESTDIR)/mgl
endif

# Default run target
default: tests benchmarks examples

# All run targets
install:
	mkdir -p $(MGL_PATH)
	cp -r source/* $(MGL_PATH)
uninstall:
	rm -rI $(MGL_PATH)
lib: $(OBJGRAPH_SOURCES)
	ar rvs bin/libmin.a $(OBJGRAPH_SOURCES)
al_test:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) -Itest $(PARAMS) $(TEST_AL) -o bin/al_test $(LINKER) 2> "al_test.txt"
gl_test:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) -Itest $(PARAMS) $(TEST_GL) -o bin/gl_test $(LINKER) 2> "gl_test.txt"
wl_test:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) -Itest $(WL_INCLUDE) $(PARAMS) $(TEST_WL) -o bin/wl_test $(LINKER) 2> "wl_test.txt"
tests: al_test gl_test wl_test
	
benchmarks:
	g++ $(LIB_SOURCES) $(BENCH_SOURCES) -Ibench $(PARAMS) bench/gl_bench.cpp -o bin/gl_bench $(LINKER) 2> "gcc_bench.txt"
example1:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX1) -o bin/ex1 $(LINKER) 2> "min_ex1.txt"
example2:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX2) -o bin/ex2 $(LINKER) 2> "min_ex2.txt"
example3:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX3) -o bin/ex3 $(LINKER) 2> "min_ex3.txt"
example4:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX4) -o bin/ex4 $(LINKER) 2> "min_ex4.txt"
example5:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX5) -o bin/ex5 $(LINKER) 2> "min_ex5.txt"
example6:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX6) -o bin/ex6 $(LINKER) 2> "min_ex6.txt"
example7:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX7) -o bin/ex7 $(LINKER) 2> "min_ex7.txt"
example8:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX8) -o bin/ex8 $(LINKER) 2> "min_ex8.txt"
example9:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX9) -o bin/ex9 $(LINKER) 2> "min_ex9.txt"
example10:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) $(WL_INCLUDE) $(PARAMS) $(EX10) -o bin/ex10 $(LINKER) 2> "min_ex10.txt"
examples: example1 example2 example3 example4 example5 example6 example7 example8 example9 example10
	

# pattern matching .cpp
%.o: %.cpp
	g++ $(LIB_SOURCES) $(PARAMS) $(WL_INCLUDE) -c $< -o $@ 2> "gcc.txt"

# clean targets
clean: clean_junk clean_source clean_tests clean_benchmarks clean_bin
clean_junk:
	rm -f *.txt
clean_source:
	rm -f source/cpp/*.o
	rm -f source/platform/*.o
clean_tests:
	rm -f test/*.o
clean_benchmarks:
	rm -f bench/*.o
clean_bin:
	rm -f bin/*
