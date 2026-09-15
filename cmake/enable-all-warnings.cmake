FUNCTION(ENABLE_ALL_WARNINGS target)
  IF(MSVC)
    TARGET_COMPILE_OPTIONS(
      ${target}
      PRIVATE
        /W4 /WX
        /wd4146 # GMP: unary minus on unsigned type
        /wd4244 # GMP: mp_limb_t to unsigned long conversion
    )
  ELSE()
    TARGET_COMPILE_OPTIONS(
      ${target}
      PRIVATE
        -Wall -Werror
    )
  ENDIF()
ENDFUNCTION()
