INCLUDES= -Icommands -I../../include -I../../ext/tinycrypt/lib/include
LIBS=../../ext/tinycrypt/lib/libtinycrypt.a

FLAGS= -DSIMPLE_MANIFEST -DWITH_TINYCRYPT

CC=gcc
CXX=g++
CFLAGS=-Wall -g $(INCLUDES) $(FLAGS)
CXXFLAGS=-Wall -g -std=c++11 $(INCLUDES) $(FLAGS)

CPPSRC= $(wildcard commands/*.cpp ./*.cpp)
OBJ= $(patsubst src/%.cpp, obj/%.o, $(CPPSRC))

CSRC= ../../ext/tinycrypt/lib/source/ecc_platform_specific.c
CSRC+=../../src/memory/manifest.c ../../src/security/tinycrypt.c
CSRC+=../../src/memory/simple_manifest_impl.c
OBJ+= $(patsubst src/%.c, obj/%.o, $(CSRC))

firmware_tool: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LIBS) $(OBJ) -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj
