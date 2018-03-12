INCLUDES= -Icommands -I../../include -I../../ext/tinycrypt/lib/include
LIBS=../../ext/tinycrypt/lib/libtinycrypt.a

FLAGS= -DSIMPLE_MANIFEST -DWITH_TINYCRYPT

CC ?= gcc
CXX ?= g++
CFLAGS=-Wall -g $(INCLUDES) $(FLAGS)
CXXFLAGS=-Wall -g -std=c++11 $(INCLUDES) $(FLAGS)

CPPSRC= $(wildcard commands/*.cpp ./*.cpp)
OBJ= $(patsubst %.cpp, %.o, $(CPPSRC))

CSRC+=../../src/memory/manifest.c ../../src/security/tinycrypt.c
CSRC+=../../src/memory/simple_manifest_impl.c
OBJ+= $(patsubst %.c, %.o, $(CSRC))

firmware_tool: $(OBJ)
	echo $(OBJ)
	$(CXX) $(CXXFLAGS) $(LIBS) $(OBJ) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) firmware_tool
