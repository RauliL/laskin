CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(laskin-gui CXX)

FIND_PACKAGE(PkgConfig)

PKG_CHECK_MODULES(GTKMM gtkmm-3.0 REQUIRED)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/FindGMP.cmake)
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../common/cmake/paths.cmake)

LINK_DIRECTORIES(
  ${GTKMM_LIBRARY_DIRS}
)

ADD_EXECUTABLE(
  laskin-gui
  ${COMMON_SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/line-display.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/line-editor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stack-display.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp
)

TARGET_COMPILE_OPTIONS(
  laskin-gui
  PRIVATE
    -Wall -Werror
)

TARGET_COMPILE_FEATURES(
  laskin-gui
  PRIVATE
    cxx_std_17
)

TARGET_INCLUDE_DIRECTORIES(
  laskin-gui
  PRIVATE
    ${GMP_INCLUDE_DIR}
    ${GMPXX_INCLUDE_DIR}
    ${GTKMM_INCLUDE_DIRS}
    ${COMMON_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

TARGET_LINK_LIBRARIES(
  laskin-gui
  ${GMP_LIBRARIES}
  ${GMPXX_LIBRARIES}
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
