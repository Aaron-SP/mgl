# MGL Makefile - CYGWIN | LINUX

# Automatically generate variable name lists
CGRAPH_SOURCES = $(wildcard source/cpp/*.cpp)

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
BIN_EX11 = bin/ex11
BIN_EX12 = bin/ex12
BIN_AL_TEST = bin/al_test
BIN_BENCH = bin/gl_bench
BIN_GL_TEST = bin/gl_test
BIN_WL_TEST = bin/wl_test

# Linker parameters
ifeq ($(OS),Windows_NT)
	# 64 bit
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        MGL_DESTDIR ?= $(shell cygpath -m /usr/x86_64-w64-mingw32/sys-root/mingw/include/mgl)
    endif

	#64 bit
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        MGL_DESTDIR ?= $(shell cygpath -m /usr/x86_64-w64-mingw32/sys-root/mingw/include/mgl)
    else
	#32 bit
        MGL_DESTDIR ?= $(shell cygpath -m /usr/i686-w64-mingw32/sys-root/mingw/include/mgl)
    endif

	# Link library settings
	LINKER = -lgdi32 -lopengl32 -lfreetype.dll -lOpenAL32.dll -lvorbisfile.dll -lglew32.dll
	STATIC = $(LINKER) -static -lmingw32 -static-libgcc -static-libstdc++ -Wl,--as-needed
	DYNAMIC = -Wl,-Bdynamic $(LINKER) -lmingw32 -Wl,--as-needed
else
	MGL_DESTDIR ?= /usr/include/mgl

	# Link library settings
	LINKER = -lX11 -lGL -lfreetype -lopenal -lvorbisfile -lGLEW
	STATIC = $(LINKER) -Wl,-Bstatic -pthread -static-libgcc -static-libstdc++ -Wl,--as-needed
	DYNAMIC = -Wl,-Bdynamic $(LINKER) -pthread -Wl,--as-needed
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
TEST_AL = $(LIB_SOURCES) $(TEST_SOURCES) -Itest test/al_test.cpp
TEST_GL = $(LIB_SOURCES) $(TEST_SOURCES) -Itest test/gl_test.cpp
TEST_WL = $(LIB_SOURCES) $(TEST_SOURCES) -Itest test/wl_test.cpp
TEST_BENCH = $(LIB_SOURCES) $(BENCH_SOURCES) -Ibench bench/gl_bench.cpp
EXFLAGS = $(LIB_SOURCES) $(TEST_SOURCES)
EX1 = $(EXFLAGS) examples/ex1.cpp
EX2 = $(EXFLAGS) examples/ex2.cpp
EX3 = $(EXFLAGS) examples/ex3.cpp
EX4 = $(EXFLAGS) examples/ex4.cpp
EX5 = $(EXFLAGS) examples/ex5.cpp
EX6 = $(EXFLAGS) examples/ex6.cpp
EX7 = $(EXFLAGS) examples/ex7.cpp
EX8 = $(EXFLAGS) examples/ex8.cpp
EX9 = $(EXFLAGS) examples/ex9.cpp
EX10 = $(EXFLAGS) examples/ex10.cpp
EX11 = $(EXFLAGS) examples/ex11.cpp
EX12 = $(EXFLAGS) examples/ex12.cpp

# Enable GS rendering
ifdef MGL_GS_RENDER
	CXXFLAGS += -DMGL_GS_RENDER
endif

# Enable opengl43 features
ifdef MGL_VB43
	CXXFLAGS += -DMGL_VB43
endif

# Enable testing sizeof and alignment
ifdef MGL_TEST_ALIGN
	CXXFLAGS += -DMGL_TEST_ALIGN
endif

# Default run target
all: tests benchmarks examples
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
example11: $(BIN_EX11)
example12: $(BIN_EX12)
examples: $(BIN_EX1) $(BIN_EX2) $(BIN_EX3) $(BIN_EX4) $(BIN_EX5) $(BIN_EX6) $(BIN_EX7) $(BIN_EX8) $(BIN_EX9) $(BIN_EX10) $(BIN_EX11) $(BIN_EX12)
install:
	mkdir -p $(MGL_DESTDIR)
	cp -r source/* $(MGL_DESTDIR)
lib: $(OBJGRAPH_SOURCES)
	ar rvs bin/libmin.a $(OBJGRAPH_SOURCES)
tests: $(BIN_AL_TEST) $(BIN_GL_TEST) $(BIN_WL_TEST)
al_test: $(BIN_AL_TEST)
gl_test: $(BIN_GL_TEST)
wl_test: $(BIN_WL_TEST)
uninstall:
	rm -rI $(MGL_DESTDIR)
$(BIN_AL_TEST):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_AL) -o $@ $(DYNAMIC)
$(BIN_BENCH):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_BENCH) -o $@ $(DYNAMIC)
$(BIN_GL_TEST):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_GL) -o $@ $(DYNAMIC)
$(BIN_WL_TEST):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(TEST_WL) -o $@ $(DYNAMIC)
$(BIN_EX1):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX1) -o $@ $(DYNAMIC)
$(BIN_EX2):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX2) -o $@ $(DYNAMIC)
$(BIN_EX3):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX3) -o $@ $(DYNAMIC)
$(BIN_EX4):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX4) -o $@ $(DYNAMIC)
$(BIN_EX5):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX5) -o $@ $(DYNAMIC)
$(BIN_EX6):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX6) -o $@ $(DYNAMIC)
$(BIN_EX7):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX7) -o $@ $(DYNAMIC)
$(BIN_EX8):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX8) -o $@ $(DYNAMIC)
$(BIN_EX9):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX9) -o $@ $(DYNAMIC)
$(BIN_EX10):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX10) -o $@ $(DYNAMIC)
$(BIN_EX11):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX11) -o $@ $(DYNAMIC)
$(BIN_EX12):
	$(CXX) $(SYMBOLS) $(CXXFLAGS) $(EX12) -o $@ $(DYNAMIC)

# pattern matching .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIB_SOURCES) -c $< -o $@
clean:
	rm -rf bin/*
	rm -f bench/*.o
	rm -rf cmake-build/*
	rm -f source/cpp/*.o
	rm -f source/platform/*.o
	rm -f test/*.o
