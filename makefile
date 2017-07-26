# MGL Makefile - CYGWIN | LINUX

# Automatically generate variable name lists
CGRAPH_SOURCES = $(wildcard source/cpp/*.cpp) $(source/platform/min/glew.cpp)

# Convert the .cpp files to .o files
OBJGRAPH_SOURCES = $(CGRAPH_SOURCES:.cpp=.o)

# Query the freetype2 package config for the include directory
FREETYPE2_INCLUDE = $(shell freetype-config --cflags)

# Include directories
LIB_SOURCES = -Isource/file -Isource/geom -Isource/math -Isource/platform -Isource/scene -Isource/renderer $(FREETYPE2_INCLUDE)
TEST_SOURCES = -Itest/file -Itest/geom -Itest/math -Itest/platform -Itest/scene -Itest/renderer
BENCH_SOURCES = -Ibench/math -Ibench/geom -Ibench/scene -Ibench/file

# Compile parameters
PARAMS = -std=c++14 -Wall -O3 -march=native -fomit-frame-pointer -freciprocal-math -ffast-math --param max-inline-insns-auto=100 --param early-inlining-insns=200
EXTRA = source/platform/min/glew.cpp
WL_INCLUDE = -DGLEW_STATIC
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

# Linker parameters
ifeq ($(OS),Windows_NT)
	MGL_PATH = C:/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/mgl
	LINKER = -lopengl32 -lgdi32 -lmingw32 -lfreetype.dll
else
	MGL_PATH = /usr/include/mgl
	LINKER = -lX11 -lGL -lfreetype
endif

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
tests:
	g++ $(LIB_SOURCES) $(TEST_SOURCES) -Itest $(PARAMS) $(TEST_GL) -o bin/gl_test $(LINKER) 2> "gl_test.txt"
	g++ $(LIB_SOURCES) $(TEST_SOURCES) -Itest $(WL_INCLUDE) $(PARAMS) $(TEST_WL) -o bin/wl_test $(LINKER) 2> "wl_test.txt"
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
examples: example1 example2 example3 example4 example5 example6 example7 example8 example9
	

# pattern matching .cpp
%.o: %.cpp
	g++ $(LIB_SOURCES) $(PARAMS) $(WL_INCLUDE) -c $< -o $@ 2> "gcc.txt"

# clean targets
clean: clean_junk clean_source clean_tests clean_benchmarks clean_examples
clean_junk:
	rm -f gcc.txt
	rm -f gcc_bench.txt
	rm -f gl_test.txt
	rm -f wl_test.txt
	rm -f min_ex1.txt
	rm -f min_ex2.txt
	rm -f min_ex3.txt
	rm -f min_ex4.txt
	rm -f min_ex5.txt
	rm -f min_ex6.txt
	rm -f min_ex7.txt
	rm -f min_ex8.txt
	rm -f min_ex9.txt
clean_source:
	rm -f source/cpp/*.o
	rm -f source/platform/*.o
	rm -f bin/libmin.a
clean_tests:
	rm -f test/*.o
	rm -f bin/gl_test
	rm -f bin/wl_test
clean_benchmarks:
	rm -f bench/*.o
	rm -f bin/gl_bench
clean_examples:
	rm -f bin/ex1
	rm -f bin/ex2
	rm -f bin/ex3
	rm -f bin/ex4
	rm -f bin/ex5
	rm -f bin/ex6
	rm -f bin/ex7
	rm -f bin/ex8
	rm -f bin/ex9
