# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_FEATURE_BRAILLE)
  zephyr_library()
  zephyr_library_sources(src/braille.c)
endif()
