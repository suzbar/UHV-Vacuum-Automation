# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/szb/projects/vcSamplePositioningAppQt5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/szb/projects/vcSamplePositioningAppQt5/build

# Include any dependencies generated for this target.
include CMakeFiles/vcSamplePositioningApp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vcSamplePositioningApp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vcSamplePositioningApp.dir/flags.make

CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o: CMakeFiles/vcSamplePositioningApp.dir/flags.make
CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o: ../vcinputwindow.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/szb/projects/vcSamplePositioningAppQt5/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o -c /home/szb/projects/vcSamplePositioningAppQt5/vcinputwindow.cpp

CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/szb/projects/vcSamplePositioningAppQt5/vcinputwindow.cpp > CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.i

CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/szb/projects/vcSamplePositioningAppQt5/vcinputwindow.cpp -o CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.s

CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.requires:
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.requires

CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.provides: CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.requires
	$(MAKE) -f CMakeFiles/vcSamplePositioningApp.dir/build.make CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.provides.build
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.provides

CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.provides.build: CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o

CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o: CMakeFiles/vcSamplePositioningApp.dir/flags.make
CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o: ../main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/szb/projects/vcSamplePositioningAppQt5/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o -c /home/szb/projects/vcSamplePositioningAppQt5/main.cpp

CMakeFiles/vcSamplePositioningApp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcSamplePositioningApp.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/szb/projects/vcSamplePositioningAppQt5/main.cpp > CMakeFiles/vcSamplePositioningApp.dir/main.cpp.i

CMakeFiles/vcSamplePositioningApp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcSamplePositioningApp.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/szb/projects/vcSamplePositioningAppQt5/main.cpp -o CMakeFiles/vcSamplePositioningApp.dir/main.cpp.s

CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.requires

CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.provides: CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/vcSamplePositioningApp.dir/build.make CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.provides

CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.provides.build: CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o

CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o: CMakeFiles/vcSamplePositioningApp.dir/flags.make
CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o: ../thordrive.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/szb/projects/vcSamplePositioningAppQt5/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o -c /home/szb/projects/vcSamplePositioningAppQt5/thordrive.cpp

CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/szb/projects/vcSamplePositioningAppQt5/thordrive.cpp > CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.i

CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/szb/projects/vcSamplePositioningAppQt5/thordrive.cpp -o CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.s

CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.requires:
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.requires

CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.provides: CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.requires
	$(MAKE) -f CMakeFiles/vcSamplePositioningApp.dir/build.make CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.provides.build
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.provides

CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.provides.build: CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o

CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o: CMakeFiles/vcSamplePositioningApp.dir/flags.make
CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o: ../vpUeyeFrameGrabber.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/szb/projects/vcSamplePositioningAppQt5/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o -c /home/szb/projects/vcSamplePositioningAppQt5/vpUeyeFrameGrabber.cpp

CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/szb/projects/vcSamplePositioningAppQt5/vpUeyeFrameGrabber.cpp > CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.i

CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/szb/projects/vcSamplePositioningAppQt5/vpUeyeFrameGrabber.cpp -o CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.s

CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.requires:
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.requires

CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.provides: CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.requires
	$(MAKE) -f CMakeFiles/vcSamplePositioningApp.dir/build.make CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.provides.build
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.provides

CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.provides.build: CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o

CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o: CMakeFiles/vcSamplePositioningApp.dir/flags.make
CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o: vcSamplePositioningApp_automoc.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/szb/projects/vcSamplePositioningAppQt5/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o -c /home/szb/projects/vcSamplePositioningAppQt5/build/vcSamplePositioningApp_automoc.cpp

CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/szb/projects/vcSamplePositioningAppQt5/build/vcSamplePositioningApp_automoc.cpp > CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.i

CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/szb/projects/vcSamplePositioningAppQt5/build/vcSamplePositioningApp_automoc.cpp -o CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.s

CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.requires:
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.requires

CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.provides: CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.requires
	$(MAKE) -f CMakeFiles/vcSamplePositioningApp.dir/build.make CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.provides.build
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.provides

CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.provides.build: CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o

# Object files for target vcSamplePositioningApp
vcSamplePositioningApp_OBJECTS = \
"CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o" \
"CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o" \
"CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o" \
"CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o" \
"CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o"

# External object files for target vcSamplePositioningApp
vcSamplePositioningApp_EXTERNAL_OBJECTS =

vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o
vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o
vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o
vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o
vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o
vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/build.make
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_vs.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_visual_features.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_vision.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_tt_mi.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_tt.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_me.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_mbt.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_klt.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_blob.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_sensor.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_robot.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_io.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_gui.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_detection.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_core.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_ar.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_vs.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_visual_features.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_vision.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_tt_mi.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_tt.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_me.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_mbt.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_klt.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_blob.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_sensor.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_robot.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_io.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_gui.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_detection.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_core.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_ar.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/libimalib.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.2.1
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_vision.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_visual_features.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_me.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_blob.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_io.so.3.0.0
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libjpeg.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libpng.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_thread.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_system.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libOgreMain.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_thread.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_system.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libOgreMain.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libOIS.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libGLU.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libGL.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libSM.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libICE.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libX11.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libXext.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libCoin.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libv4l2.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libv4lconvert.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libdc1394.so
vcSamplePositioningApp: /usr/local/lib/x86_64-linux-gnu/libvisp_core.so.3.0.0
vcSamplePositioningApp: /usr/local/lib/libopencv_xphoto.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_xobjdetect.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_ximgproc.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_xfeatures2d.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_tracking.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_text.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_surface_matching.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_structured_light.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_stereo.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_saliency.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_rgbd.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_reg.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_plot.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_optflow.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_line_descriptor.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_hdf.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_fuzzy.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_face.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_dpm.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_dnn.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_datasets.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_cvv.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_ccalib.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_bioinspired.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_bgsegm.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_aruco.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_videostab.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_videoio.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_video.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_superres.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_stitching.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_shape.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_photo.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_objdetect.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_ml.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_imgproc.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_highgui.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_flann.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_features2d.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_core.so.3.1.0
vcSamplePositioningApp: /usr/local/lib/libopencv_calib3d.so.3.1.0
vcSamplePositioningApp: /usr/lib/libgsl.so
vcSamplePositioningApp: /usr/lib/libgslcblas.so
vcSamplePositioningApp: /usr/lib/liblapack.so
vcSamplePositioningApp: /usr/lib/libblas.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libxml2.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libz.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libpthread.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libm.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libnsl.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libdmtx.so
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.2.1
vcSamplePositioningApp: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.2.1
vcSamplePositioningApp: CMakeFiles/vcSamplePositioningApp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable vcSamplePositioningApp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vcSamplePositioningApp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vcSamplePositioningApp.dir/build: vcSamplePositioningApp
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/build

CMakeFiles/vcSamplePositioningApp.dir/requires: CMakeFiles/vcSamplePositioningApp.dir/vcinputwindow.cpp.o.requires
CMakeFiles/vcSamplePositioningApp.dir/requires: CMakeFiles/vcSamplePositioningApp.dir/main.cpp.o.requires
CMakeFiles/vcSamplePositioningApp.dir/requires: CMakeFiles/vcSamplePositioningApp.dir/thordrive.cpp.o.requires
CMakeFiles/vcSamplePositioningApp.dir/requires: CMakeFiles/vcSamplePositioningApp.dir/vpUeyeFrameGrabber.cpp.o.requires
CMakeFiles/vcSamplePositioningApp.dir/requires: CMakeFiles/vcSamplePositioningApp.dir/vcSamplePositioningApp_automoc.cpp.o.requires
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/requires

CMakeFiles/vcSamplePositioningApp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vcSamplePositioningApp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/clean

CMakeFiles/vcSamplePositioningApp.dir/depend:
	cd /home/szb/projects/vcSamplePositioningAppQt5/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/szb/projects/vcSamplePositioningAppQt5 /home/szb/projects/vcSamplePositioningAppQt5 /home/szb/projects/vcSamplePositioningAppQt5/build /home/szb/projects/vcSamplePositioningAppQt5/build /home/szb/projects/vcSamplePositioningAppQt5/build/CMakeFiles/vcSamplePositioningApp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vcSamplePositioningApp.dir/depend
