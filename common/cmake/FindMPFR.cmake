FIND_PATH(
  MPFR_INCLUDE_DIR
  NAMES mpfr.h
  DOC "The directory containing MPFR include files."
)

FIND_LIBRARY(
  MPFR_LIBRARIES
  NAMES mpfr libmpfr
  DOC "Path to the MPFR library."
)

IF(NOT MPFR_LIBRARIES)
  MESSAGE(FATAL_ERROR "Could not find MPFR library.")
ENDIF()
