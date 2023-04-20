CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(laskin C CXX)

OPTION(
  LASKIN_ENABLE_CLI
  "Whether CLI interpreter should be built or not."
  ON
)
OPTION(
  LASKIN_ENABLE_GUI
  "Whether GUI interpreter should be built or not."
  OFF
)

IF(LASKIN_ENABLE_CLI)
  ADD_SUBDIRECTORY(cli)
ENDIF()
IF(LASKIN_ENABLE_GUI)
  ADD_SUBDIRECTORY(gui)
ENDIF()
