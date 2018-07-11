# MGL Makefile - CYGWIN | LINUX

# Automatically generate variable name lists
CGRAPH_SOURCES = $(wildcard source/cpp/*.cpp) $(source/platform/min/glew.cpp)

# Convert the .cpp files to .o files
OBJGRAPH_SOURCES = $(CGRAPH_SOURCES:.cpp=.o)

# Query the freetype2 package config for the include directory
FREETYPE2_INCLUDE = $(shell pkg-config freetype2 --cflags)

# Compile binaries
BIN_EX1 = bin/ex1
BIN_EX2 = bin/ex2
BIN_EX3 = bin/ex3
BIN_EX4 = bin/ex4
BIN_EX5 = bin/ex5
BIN_EX6 = bin/ex6
BIN_EX7 = bin/ex7
BIN_EX8 = bin/ex8
BIN_EX9 = bin/ex9
BIN_EX10 = bin/ex10
BIN_AL_TEST = bin/al_test
BIN_BENCH = bin/gl_bench
BIN_GL_TEST = bin/gl_test
BIN_WL_TEST = bin/wl_test

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

	# Link library settings
	LINKER = -lgdi32 -lopengl32 -lfreetype.dll -lOpenAL32.dll -lvorbisfile.dll
	STATIC = $(LINKER) -static -lmingw32 -static-libgcc -static-libstdc++ -Wl,--as-needed
	DYNAMIC = -Wl,-Bdynamic $(LINKER) -lmingw32 -Wl,--as-needed
else
	MGL_PATH = /usr/include/mgl

	# Link library settings
	LINKER = -lX11 -lGL -lfreetype -lopenal -lvorbisfile
	STATIC = $(LINKER) -Wl,-Bstatic -pthread -static-libgcc -static-libstdc++ -Wl,--as-needed
	DYNAMIC = -Wl,-Bdynamic $(LINKER) -pthread -Wl,--as-needed
endif

# Override if MGL_DESTDIR specified
ifdef MGL_DESTDIR
	MGL_PATH = $(MGL_DESTDIR)/mgl
endif

# Include directories
LIB_SOURCES = -Isource/file -Isource/geom -Isource/math -Isource/platform -Isource/renderer -Isource/scene -Isource/sound -Isource/util $(FREETYPE2_INCLUDE)
TEST_SOURCES = -Itest/file -Itest/geom -Itest/math -Itest/platform -Itest/renderer -Itest/scene -Itest/sound -Itest/util
BENCH_SOURCES = -Ibench/math -Ibench/geom -Ibench/scene -Ibench/file

# Compile flags
WARNFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter 
DEBUGFLAGS = -std=c++14 $(WARNFLAGS) -O1
RELEASEFLAGS = -std=c++14 $(WARNFLAGS) -O3 -fomit-frame-pointer -freciprocal-math -ffast-math --param max-inline-insns-auto=100 --param early-inlining-insns=200

# Set architecture
ifeq ($(BUILD),debug)
	CXXFLAGS += $(DEBUGFLAGS)
	SYMBOLS = -g
else
ifeq ($(BUILD),arch32)
	CXXFLAGS += $(RELEASEFLAGS) -m32
	SYMBOLS = -s
else
ifeq ($(BUILD),arch64)
	CXXFLAGS += $(RELEASEFLAGS) -m64
	SYMBOLS = -s
else
	CXXFLAGS += $(RELEASEFLAGS) -march=native
	SYMBOLS = -s
endif
endif
endif

# Build targets
CPP_O = -DGLEW_STATIC $(LIB_SOURCES)
GLEW = -DGLEW_STATIC source/platform/min/glew.cpp
TEST_AL = $(LIB_SOURCES) $(TEST_SOURCES) -Itest test/al_test.cpp
TEST_GL = $(LIB_SOURCES) $(TEST_SOURCES) -Itest test/gl_test.cpp
TEST_WL = $(LIB_SOURCES) $(TEST_SOURCES) -Itest $(GLEW) test/wl_test.cpp
TEST_BENCH = $(LIB_SOURCES) $(BENCH_SOURCES) -Ibench bench/gl_bench.cpp
EXFLAGS = $(LIB_SOURCES) $(TEST_SOURCES) $(GLEW)
EX1 = $(EXFLAGS) example/programs/ex1.cpp
EX2 = $(EXFLAGS) example/programs/ex2.cpp
EX3 = $(EXFLAGS) example/programs/ex3.cpp
EX4 = $(EXFLAGS) example/programs/ex4.cpp
EX5 = $(EXFLAGS) example/programs/ex5.cpp
EX6 = $(EXFLAGS) example/programs/ex6.cpp
EX7 = $(EXFLAGS) example/programs/ex7.cpp
EX8 = $(EXFLAGS) example/programs/ex8.cpp
EX9 = $(EXFLAGS) example/programs/ex9.cpp
EX10 = $(EXFLAGS) example/programs/ex10.cpp

# Enable GS rendering
ifdef MGL_GS_RENDER
	CXXFLAGS += -DMGL_GS_RENDER
endif

# Enable opengl43 features
ifdef MGL_VB43
	CXXFLAGS += -DMGL_VB43
endif

# Default run target
default: tests benchmarks examples
benchmarks: $(BIN_BENCH)
example1: $(BIN_EX1)
example2: $(BIN_EX2)
example3: $(BIN_EX3)
example4: $(BIN_EX4)
example5: $(BIN_EX5)
example6: $(BIN_EX6)
example7: $(BIN_EX7)
example8: $(BIN_EX8)
example9: $(BIN_EX9)
example10: $(BIN_EX10)
examples: $(BIN_EX1) $(BIN_EX2) $(BIN_EX3) $(BIN_EX4) $(BIN_EX5) $(BIN_EX6) $(BIN_EX7) $(BIN_EX8) $(BIN_EX9) $(BIN_EX10)
install:
	mkdir -p $(MGL_PATH)
	cp -r source/* $(MGL_PATH)
lib: $(OBJGRAPH_SOURCES)
	ar rvs bin/libmin.a $(OBJGRAPH_SOURCES)
tests: $(BIN_AL_TEST) $(BIN_GL_TEST) $(BIN_WL_TEST)
uninstall:
	rm -rI $(MGL_PATH)
$(BIN_AL_TEST):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_AL) -o $@ $(DYNAMIC) 2> "al_test.txt"
$(BIN_BENCH):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_BENCH) -o $@ $(DYNAMIC) 2> "gcc_bench.txt"
$(BIN_GL_TEST):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_GL) -o $@ $(DYNAMIC) 2> "gl_test.txt"
$(BIN_WL_TEST):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_WL) -o $@ $(DYNAMIC) 2> "wl_test.txt"
$(BIN_EX1):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX1) -o $@ $(DYNAMIC) 2> "ex1.txt"
$(BIN_EX2):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX2) -o $@ $(DYNAMIC) 2> "ex2.txt"
$(BIN_EX3):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX3) -o $@ $(DYNAMIC) 2> "ex3.txt"
$(BIN_EX4):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX4) -o $@ $(DYNAMIC) 2> "ex4.txt"
$(BIN_EX5):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX5) -o $@ $(DYNAMIC) 2> "ex5.txt"
$(BIN_EX6):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX6) -o $@ $(DYNAMIC) 2> "ex6.txt"
$(BIN_EX7):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX7) -o $@ $(DYNAMIC) 2> "ex7.txt"
$(BIN_EX8):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX8) -o $@ $(DYNAMIC) 2> "ex8.txt"
$(BIN_EX9):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX9) -o $@ $(DYNAMIC) 2> "ex9.txt"
$(BIN_EX10):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX10) -o $@ $(DYNAMIC) 2> "ex10.txt"

# pattern matching .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPP_O) -c $< -o $@ 2> "gcc.txt"

# clean targets
clean:
	rm -f *.txt
	rm -f source/cpp/*.o
	rm -f source/platform/*.o
	rm -f test/*.o
	rm -f bench/*.o
	rm -f bin/*
