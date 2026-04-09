CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(laskin-gui CXX)

INCLUDE(../cmake/enable-all-warnings.cmake)

FIND_PACKAGE(PkgConfig)

PKG_CHECK_MODULES(GTKMM gtkmm-3.0 REQUIRED)

LINK_DIRECTORIES(
  ${GTKMM_LIBRARY_DIRS}
)

ADD_EXECUTABLE(
  laskin-gui
  ./src/context.cpp
  ./src/dictionary-display.cpp
  ./src/line-display.cpp
  ./src/line-editor.cpp
  ./src/main.cpp
  ./src/stack-display.cpp
  ./src/utils.cpp
  ./src/window.cpp
)

ENABLE_ALL_WARNINGS(laskin-gui)

TARGET_COMPILE_FEATURES(
  laskin-gui
  PRIVATE
    cxx_std_17
)

TARGET_INCLUDE_DIRECTORIES(
  laskin-gui
  PRIVATE
    ${GTKMM_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(
  laskin-gui
  laskin
  ${MPFR_LIBRARIES}
  ${GTKMM_LIBRARIES}
)

INSTALL(
  TARGETS
    laskin-gui
  RUNTIME DESTINATION
    bin
)

INSTALL(
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/laskin-gui.desktop
  DESTINATION
    share/applications
)

INSTALL(
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/icons/laskin-gui.svg
  DESTINATION
    share/icons/hicolor/scalable/apps
)

FOREACH(ICON_SIZE 16 32 48 64 128 256)
  INSTALL(
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/icons/laskin-gui${ICON_SIZE}.png
    DESTINATION
      share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps
    RENAME
      laskin-gui.png
  )
ENDFOREACH()
