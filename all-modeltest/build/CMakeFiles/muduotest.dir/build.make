# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/codeCpp/ChatServer/all-modeltest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/codeCpp/ChatServer/all-modeltest/build

# Include any dependencies generated for this target.
include CMakeFiles/muduotest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/muduotest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/muduotest.dir/flags.make

CMakeFiles/muduotest.dir/muduotest.cpp.o: CMakeFiles/muduotest.dir/flags.make
CMakeFiles/muduotest.dir/muduotest.cpp.o: ../muduotest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/codeCpp/ChatServer/all-modeltest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/muduotest.dir/muduotest.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/muduotest.dir/muduotest.cpp.o -c /home/codeCpp/ChatServer/all-modeltest/muduotest.cpp

CMakeFiles/muduotest.dir/muduotest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/muduotest.dir/muduotest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/codeCpp/ChatServer/all-modeltest/muduotest.cpp > CMakeFiles/muduotest.dir/muduotest.cpp.i

CMakeFiles/muduotest.dir/muduotest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/muduotest.dir/muduotest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/codeCpp/ChatServer/all-modeltest/muduotest.cpp -o CMakeFiles/muduotest.dir/muduotest.cpp.s

# Object files for target muduotest
muduotest_OBJECTS = \
"CMakeFiles/muduotest.dir/muduotest.cpp.o"

# External object files for target muduotest
muduotest_EXTERNAL_OBJECTS =

../bin/muduotest: CMakeFiles/muduotest.dir/muduotest.cpp.o
../bin/muduotest: CMakeFiles/muduotest.dir/build.make
../bin/muduotest: CMakeFiles/muduotest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/codeCpp/ChatServer/all-modeltest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/muduotest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/muduotest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/muduotest.dir/build: ../bin/muduotest

.PHONY : CMakeFiles/muduotest.dir/build

CMakeFiles/muduotest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/muduotest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/muduotest.dir/clean

CMakeFiles/muduotest.dir/depend:
	cd /home/codeCpp/ChatServer/all-modeltest/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/codeCpp/ChatServer/all-modeltest /home/codeCpp/ChatServer/all-modeltest /home/codeCpp/ChatServer/all-modeltest/build /home/codeCpp/ChatServer/all-modeltest/build /home/codeCpp/ChatServer/all-modeltest/build/CMakeFiles/muduotest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/muduotest.dir/depend

