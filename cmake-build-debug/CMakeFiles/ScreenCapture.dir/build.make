# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /cygdrive/c/Users/Andre/AppData/Local/JetBrains/CLion2021.2/cygwin_cmake/bin/cmake.exe

# The command to remove a file.
RM = /cygdrive/c/Users/Andre/AppData/Local/JetBrains/CLion2021.2/cygwin_cmake/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ScreenCapture.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/ScreenCapture.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ScreenCapture.dir/flags.make

CMakeFiles/ScreenCapture.dir/main.cpp.o: CMakeFiles/ScreenCapture.dir/flags.make
CMakeFiles/ScreenCapture.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ScreenCapture.dir/main.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ScreenCapture.dir/main.cpp.o -c /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/main.cpp

CMakeFiles/ScreenCapture.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ScreenCapture.dir/main.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/main.cpp > CMakeFiles/ScreenCapture.dir/main.cpp.i

CMakeFiles/ScreenCapture.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ScreenCapture.dir/main.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/main.cpp -o CMakeFiles/ScreenCapture.dir/main.cpp.s

CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.o: CMakeFiles/ScreenCapture.dir/flags.make
CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.o: ../ScreenRecorder.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.o"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.o -c /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/ScreenRecorder.cpp

CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/ScreenRecorder.cpp > CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.i

CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/ScreenRecorder.cpp -o CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.s

# Object files for target ScreenCapture
ScreenCapture_OBJECTS = \
"CMakeFiles/ScreenCapture.dir/main.cpp.o" \
"CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.o"

# External object files for target ScreenCapture
ScreenCapture_EXTERNAL_OBJECTS =

ScreenCapture.exe: CMakeFiles/ScreenCapture.dir/main.cpp.o
ScreenCapture.exe: CMakeFiles/ScreenCapture.dir/ScreenRecorder.cpp.o
ScreenCapture.exe: CMakeFiles/ScreenCapture.dir/build.make
ScreenCapture.exe: /cygdrive/c/msys64/usr/local/lib/libavcodec.dll.a
ScreenCapture.exe: /cygdrive/c/msys64/usr/local/lib/libavformat.dll.a
ScreenCapture.exe: /cygdrive/c/msys64/usr/local/lib/libavutil.dll.a
ScreenCapture.exe: /cygdrive/c/msys64/usr/local/lib/libavdevice.dll.a
ScreenCapture.exe: CMakeFiles/ScreenCapture.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ScreenCapture.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ScreenCapture.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ScreenCapture.dir/build: ScreenCapture.exe
.PHONY : CMakeFiles/ScreenCapture.dir/build

CMakeFiles/ScreenCapture.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ScreenCapture.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ScreenCapture.dir/clean

CMakeFiles/ScreenCapture.dir/depend:
	cd /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug /cygdrive/c/Users/Andre/CLionProjects/ScreenCapture/cmake-build-debug/CMakeFiles/ScreenCapture.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ScreenCapture.dir/depend
