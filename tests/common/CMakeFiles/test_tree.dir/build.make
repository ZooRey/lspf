# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/hgfs/opensource/lspf

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/hgfs/opensource/lspf

# Include any dependencies generated for this target.
include tests/common/CMakeFiles/test_tree.dir/depend.make

# Include the progress variables for this target.
include tests/common/CMakeFiles/test_tree.dir/progress.make

# Include the compile flags for this target's objects.
include tests/common/CMakeFiles/test_tree.dir/flags.make

tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o: tests/common/CMakeFiles/test_tree.dir/flags.make
tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o: tests/common/test_tree.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/hgfs/opensource/lspf/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o"
	cd /mnt/hgfs/opensource/lspf/tests/common && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_tree.dir/test_tree.cpp.o -c /mnt/hgfs/opensource/lspf/tests/common/test_tree.cpp

tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_tree.dir/test_tree.cpp.i"
	cd /mnt/hgfs/opensource/lspf/tests/common && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/hgfs/opensource/lspf/tests/common/test_tree.cpp > CMakeFiles/test_tree.dir/test_tree.cpp.i

tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_tree.dir/test_tree.cpp.s"
	cd /mnt/hgfs/opensource/lspf/tests/common && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/hgfs/opensource/lspf/tests/common/test_tree.cpp -o CMakeFiles/test_tree.dir/test_tree.cpp.s

tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.requires:

.PHONY : tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.requires

tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.provides: tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.requires
	$(MAKE) -f tests/common/CMakeFiles/test_tree.dir/build.make tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.provides.build
.PHONY : tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.provides

tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.provides.build: tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o


# Object files for target test_tree
test_tree_OBJECTS = \
"CMakeFiles/test_tree.dir/test_tree.cpp.o"

# External object files for target test_tree
test_tree_EXTERNAL_OBJECTS =

tests/test/bin/test_tree: tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o
tests/test/bin/test_tree: tests/common/CMakeFiles/test_tree.dir/build.make
tests/test/bin/test_tree: build_release/lib/libcommon.a
tests/test/bin/test_tree: tests/common/CMakeFiles/test_tree.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/hgfs/opensource/lspf/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../test/bin/test_tree"
	cd /mnt/hgfs/opensource/lspf/tests/common && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_tree.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/common/CMakeFiles/test_tree.dir/build: tests/test/bin/test_tree

.PHONY : tests/common/CMakeFiles/test_tree.dir/build

tests/common/CMakeFiles/test_tree.dir/requires: tests/common/CMakeFiles/test_tree.dir/test_tree.cpp.o.requires

.PHONY : tests/common/CMakeFiles/test_tree.dir/requires

tests/common/CMakeFiles/test_tree.dir/clean:
	cd /mnt/hgfs/opensource/lspf/tests/common && $(CMAKE_COMMAND) -P CMakeFiles/test_tree.dir/cmake_clean.cmake
.PHONY : tests/common/CMakeFiles/test_tree.dir/clean

tests/common/CMakeFiles/test_tree.dir/depend:
	cd /mnt/hgfs/opensource/lspf && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/hgfs/opensource/lspf /mnt/hgfs/opensource/lspf/tests/common /mnt/hgfs/opensource/lspf /mnt/hgfs/opensource/lspf/tests/common /mnt/hgfs/opensource/lspf/tests/common/CMakeFiles/test_tree.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/common/CMakeFiles/test_tree.dir/depend

