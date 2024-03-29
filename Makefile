CXX = g++
CXXFLAGS = -O2 -g -Wall -std=c++0x -fopenmp

# Strict compiler options
CXXFLAGS += -Wformat-security -Wignored-qualifiers -Winit-self \
		-Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith \
		-Wtype-limits -Wempty-body -Wlogical-op \
		-Wmissing-field-initializers -Wctor-dtor-privacy \
		-Wnon-virtual-dtor -Wstrict-null-sentinel  \
		-Woverloaded-virtual -Wsign-promo -Wextra -pedantic \

# Directories with source code
SRC_DIR = src
INCLUDE_DIR = include

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
DEP_DIR = $(BUILD_DIR)/deps

# Bridge - place for main project to pull headers and compiled libraries.
# Headers must be placed into BRIDGE_LIBRARY_DIR before dependency generation
# proceed. Libraries in BRIDGE_LIBRARY_DIR will be required on linkage step.
BRIDGE_DIR = bridge
BRIDGE_MAKE = bridge/Makefile
BRIDGE_INCLUDE_DIR = bridge/include
BRIDGE_LIBRARY_DIR = bridge/lib
CONFIG_DIR = bridge/config

# Which libraries to prepare for usage: targets must be defined in BRIDGE_MAKE.
BRIDGE_TARGETS = glm easybmp assimp

# Link libraries gcc flag: library will be searched with prefix "lib".
LDFLAGS = -leasybmp -lassimp

# Add headers dirs to gcc search path
CXXFLAGS += -I $(INCLUDE_DIR) -I $(BRIDGE_INCLUDE_DIR)
# Add path with compiled libraries to gcc search path
CXXFLAGS += -L $(BRIDGE_LIBRARY_DIR)

# Helper macros
# subst is sensitive to leading spaces in arguments.
make_path = $(addsuffix $(1), $(basename $(subst $(2), $(3), $(4))))
# Takes path list with source files and returns pathes to related objects.
src_to_obj = $(call make_path,.o, $(SRC_DIR), $(OBJ_DIR), $(1))
# Takes path list with object files and returns pathes to related dep. file.
# Dependency files will be generated with gcc -MM.
src_to_dep = $(call make_path,.d, $(SRC_DIR), $(DEP_DIR), $(1))

# All source files in our project that must be built into movable object code.
CXXFILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJFILES := $(call src_to_obj, $(CXXFILES))

# Default target (make without specified target).
.DEFAULT_GOAL := all

# Alias to make all targets.
.PHONY: all
all: $(BIN_DIR)/main	

# Suppress makefile rebuilding.
Makefile: ;

# "Hack" to restrict order of actions:
# 1. Make bridge files target.
# 2. Generate and include dependency information.
ifneq ($(MAKECMDGOALS), clean)
-include bridge.touch
endif

# bridge.touch contains redirect to dependency generation Makefile.
bridge.touch: $(wildcard $(BRIDGE_INCLUDE_DIR)/*) \
		$(wildcard $(BRIDGE_LIBRARY_DIR)/*)
	mkdir -p $(BRIDGE_INCLUDE_DIR) $(BRIDGE_LIBRARY_DIR)
	make -C $(dir $(BRIDGE_MAKE)) -f $(notdir $(BRIDGE_MAKE)) $(BRIDGE_TARGETS)
	mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DEP_DIR)
	echo "include deps.mk" > $@

# Rules for compiling targets
$(BIN_DIR)/main: $(OBJFILES) bridge.touch	
	$(CXX) $(CXXFLAGS) $(filter %.o, $^) -o $@ $(LDFLAGS)

# Pattern for generating dependency description files (*.d)
$(DEP_DIR)/%.d: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -E -MM -MT $(call src_to_obj, $<) -MT $@ -MF $@ $<

# Pattern for compiling object files (*.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $(call src_to_obj, $<) $<

# Fictive target
.PHONY: clean
# Delete all temprorary and binary files
clean:
	make -C $(dir $(BRIDGE_MAKE)) -f $(notdir $(BRIDGE_MAKE)) clean
	rm -rf $(BUILD_DIR) $(BRIDGE_INCLUDE_DIR) $(BRIDGE_LIBRARY_DIR)
	rm -f bridge.touch

# If you still have "WTF?!" feeling, try reading teaching book
# by Mashechkin & Co. http://unicorn.ejudge.ru/instr.pdf
