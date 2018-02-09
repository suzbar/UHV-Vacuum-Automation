cmake_minimum_required(VERSION 2.8.7)
  project(vcSamplePositioningApp)

  # Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Widgets finds its own dependencies (QtGui and QtCore).
  find_package(Qt5Widgets REQUIRED)

  # The Qt5Widgets_INCLUDES also includes the include directories for
  # dependencies QtCore and QtGui
  include_directories(${Qt5Widgets_INCLUDES})

  # We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
  add_definitions(${Qt5Widgets_DEFINITIONS})
  
  # Executables fail to build with Qt 5 in the default configuration
  # without -fPIE. We add that here.
  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

  # generate ui header file
  QT5_WRAP_UI(UIS_HDRS vcinputwindow.ui)

  # Redundant - works same with or without this
  # generate moc files
  # QT5_WRAP_CPP(moc_srcs applicationcontroller.h)
  # ViSP config
  FIND_PACKAGE(VISP REQUIRED)
  IF(VISP_FOUND) 
    add_definitions(${VISP_DEFINITIONS})
    include_directories(${VISP_INCLUDE_DIRS})
    link_directories(${VISP_LIBRARY_DIRS})
    link_libraries(${VISP_LIBRARIES})
    INCLUDE(${VISP_USE_FILE})
  ENDIF(VISP_FOUND)

  set(LINKEDLIBSPATH "/usr/local/lib/" "/usr/lib/x86_64-linux-gnu/")
  set(EXTERNIMALIBS "/usr/local/lib/libimalib.so" "/usr/lib/x86_64-linux-gnu/libboost_filesystem.so")
  # Add include paths
  include_directories( "/usr/include/")
  include_directories( "/usr/local/include/")
  link_directories( ${LINKEDLIBSPATH} )
  link_libraries(${EXTERNIMALIBS})
  set(vcSamplePositioningApp_SRCS vcinputwindow.cpp main.cpp thordrive.cpp vpUeyeFrameGrabber.cpp applicationcontroller.cpp )
  # set(GCC_COVERAGE_COMPILE_FLAGS "-Ofast")
  # add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
  add_executable(vcSamplePositioningApp ${vcSamplePositioningApp_SRCS} ${UIS_HDRS})

  # The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
  target_link_libraries(vcSamplePositioningApp ${Qt5Widgets_LIBRARIES})
