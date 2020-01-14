FIND_PATH(
  GMP_INCLUDE_DIR
  NAMES gmp.h
  DOC "The directory containing GMP include files."
)
FIND_PATH(
  GMPXX_INCLUDE_DIR
  NAMES gmpxx.h
  DOC "The directory containing GMPXX include files."
)
FIND_LIBRARY(
  GMP_LIBRARIES
  NAMES gmp libgmp
  DOC "Path to the GMP library."
)
FIND_LIBRARY(
  GMPXX_LIBRARIES
  NAMES gmpxx libgmpxx
  DOC "Path to the GMPXX library."
)
IF(NOT GMP_LIBRARIES OR NOT GMPXX_LIBRARIES)
  MESSAGE(FATAL_ERROR "Could not find GMPXX library.")
ENDIF()
