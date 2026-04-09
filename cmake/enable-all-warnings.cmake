FUNCTION(ENABLE_ALL_WARNINGS target)
  IF(MSVC)
    TARGET_COMPILE_OPTIONS(
      ${target}
      PRIVATE
        /W4 /WX
    )
  ELSE()
    TARGET_COMPILE_OPTIONS(
      ${target}
      PRIVATE
        -Wall -Werror
    )
  ENDIF()
ENDFUNCTION()
