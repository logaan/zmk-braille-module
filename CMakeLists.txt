# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_FEATURE_BRAILLE)
  zephyr_library()
  # ZMK v0.3 keeps the behavior API in the application's private include tree.
  zephyr_library_include_directories(include ${APPLICATION_SOURCE_DIR}/include)
  zephyr_library_sources(
    src/behavior_braille.c
    src/braille_engine.c
    src/braille_rules_grade1.c
    src/braille_rules_nav.c
    src/braille_rules_computer8.c
  )
  zephyr_library_sources_ifdef(CONFIG_ZMK_BRAILLE_GRADE2 src/braille_rules_grade2.c)
endif()
