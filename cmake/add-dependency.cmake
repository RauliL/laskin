INCLUDE(FetchContent)

# Fetches a dependency and adds it as a subdirectory excluded from the default
# build. Subprojects that guard install() with
# CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR will not install.
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

# Fetches a header-only dependency without add_subdirectory, so upstream
# install() rules are never registered. Creates an INTERFACE library with the
# given alias and include directory (relative to the fetched source root).
FUNCTION(ADD_HEADER_ONLY_DEPENDENCY name repository tag alias include_dir)
  FetchContent_Declare(
    ${name}
    GIT_REPOSITORY ${repository}
    GIT_TAG ${tag}
  )
  FetchContent_GetProperties(${name})
  IF(NOT ${name}_POPULATED)
    FetchContent_Populate(${name})
    STRING(REPLACE "::" "_" _target_name ${alias})
    ADD_LIBRARY(${_target_name} INTERFACE)
    ADD_LIBRARY(${alias} ALIAS ${_target_name})
    TARGET_INCLUDE_DIRECTORIES(
      ${_target_name}
      SYSTEM
      INTERFACE
        "${${name}_SOURCE_DIR}/${include_dir}"
    )
  ENDIF()
ENDFUNCTION()
