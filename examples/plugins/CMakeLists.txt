ADD_SUBDIRECTORY(hello)

FIND_PACKAGE(SDL2 QUIET)
IF(SDL2_FOUND)
  ADD_SUBDIRECTORY(turtle)
ELSE()
  MESSAGE(
    STATUS
    "SDL2 not found; skipping laskin-turtle-plugin (install libsdl2-dev)"
  )
ENDIF()
