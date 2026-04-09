INCLUDE(FetchContent)

# Wrapper for FetchContent that excludes dependency headers and other fluff
# from installation.
FUNCTION(ADD_DEPENDENCY name repository tag)
  FetchContent_Declare(
    ${name}
    GIT_REPOSITORY ${repository}
    GIT_TAG ${tag}
  )
  FetchContent_GetProperties(${name})
  IF(NOT ${name}_POPULATED)
    FetchContent_Populate(${name})
    ADD_SUBDIRECTORY(
      ${${name}_SOURCE_DIR}
      ${${name}_BINARY_DIR}
      EXCLUDE_FROM_ALL
    )
  ENDIF()
ENDFUNCTION()
