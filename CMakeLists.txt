# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_FEATURE_BRAILLE)
  zephyr_include_directories(include)
  zephyr_library()
  # ZMK v0.3 keeps the behavior API in the application's private include tree.
  zephyr_library_include_directories(include ${APPLICATION_SOURCE_DIR}/include)
  zephyr_library_sources(
    src/behavior_braille_select.c
    src/behavior_braille.c
    src/braille_bindings.c
    src/braille_display.c
    src/braille_chord.c
    src/braille_settings.c
    src/braille_engine.c
    src/braille_context_events.c
    src/braille_dispatch.c
    src/braille_tables.c
    src/braille_rules_grade1.c
    src/braille_rules_nav.c
    src/braille_rules_computer8.c
  )
  zephyr_library_sources_ifdef(CONFIG_ZMK_BRAILLE_STORAGE src/braille_storage.c src/braille_storage_zmk.c)
  if(CONFIG_ZMK_BRAILLE_SETTINGS_UI)
    set(BRAILLE_KEYCODES ${CMAKE_CURRENT_BINARY_DIR}/braille_keycodes.inc)
    execute_process(COMMAND ${PYTHON_EXECUTABLE}
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate-braille-keycodes.py
      ${APPLICATION_SOURCE_DIR}/include/dt-bindings/zmk/keys.h ${BRAILLE_KEYCODES}
      COMMAND_ERROR_IS_FATAL ANY)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
      ${APPLICATION_SOURCE_DIR}/include/dt-bindings/zmk/keys.h
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate-braille-keycodes.py)
    zephyr_library_include_directories(${CMAKE_CURRENT_BINARY_DIR})
    zephyr_library_sources(src/braille_editor.c src/braille_editor_zmk.c src/braille_search.c)
  endif()
  zephyr_library_sources_ifdef(CONFIG_ZMK_BRAILLE_WIDGET src/braille_widget.c src/braille_small_screen.c src/braille_small_canvas.c)
  # Computer8 shorthand shares this data unit and depends on GRADE2 in Kconfig.
  zephyr_library_sources_ifdef(CONFIG_ZMK_BRAILLE_GRADE2 src/braille_rules_grade2.c)
endif()
