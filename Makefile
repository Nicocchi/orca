# export LD_LIBRARY_PATH := $(HOME)/Development/cpp/music-player/lib

ARCH := $(firstword $(subst -, ,$(shell $(CC) -dumpmachine)))
ifeq ($(ARCH),x86_64)
else ifeq ($(ARCH),aarch64)
else ifneq ($(findstring $(ARCH),arm),)
ARCH := armhf
else
ARCH := x86
endif

# Program
EXECUTABLE	:= orca

# Compiler and Linker
CC	:= clang++
LD	:= clang++

# The Target Binary Program
TARGET	:= main

# The Directories, Source, Includes, Objects, Binary and Resources
MODULES	:= third_party third_party/imgui third_party/portable-file-dialogs third_party/fonts third_party/miniaudio /usr/include/freetype2
BIN		:= bin
SRC_DIR		:= $(addprefix src/,$(MODULES))
INCLUDE_DIR		:= $(addprefix include/,$(MODULES))
BUILD_DIR		:= $(addprefix build/,$(MODULES))
LIB_DIR		:= $(addprefix lib/,$(MODULES))

SRC		:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ		:= $(patsubst src/%.cpp,build/%.o,$(SRC))
INCLUDE		:= $(addprefix -I , $(INCLUDE_DIR))
LIB		::= $(addprefix -L , $(LIB_DIR))

SOURCES := $(shell find $(SRCDIR) -name "*.$(SFILES)")

# Flags, Libraries and Includes
CXX		  := clang++
CXX_FLAGS := -Wall -Wextra -std=c++17 `pkg-config --cflags glfw3 freetype2` -O2
LIBRARIES	:= -ltag -lsqlite3 -lGL `pkg-config --static --libs glfw3 freetype2` -ldl -lm -lpthread
vpath %.cpp $(SRC_DIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
# Default Make
all: $(BIN)/$(EXECUTABLE)

dev: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

run:
	clear
	cd $(BIN) && ./$(EXECUTABLE)

bear: clean
	bear -- make
	echo "Generated compile_commands.json"

$(BIN)/$(EXECUTABLE): src/*.cpp $(SRC)
	$(CXX) $(CXX_FLAGS) -I include $(INCLUDE) $^ $(LIBRARIES) -o $@

install:
	@echo "** Installing..."
	sudo cp $(BIN)/$(EXECUTABLE) /usr/bin

uninstall:
	@echo "** Uninstalling..."
	sudo rm -f /usr/bin/$(EXECUTABLE)

clean:
	-rm $(BIN)/orca

