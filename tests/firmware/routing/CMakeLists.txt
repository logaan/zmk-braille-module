# SPDX-License-Identifier: MIT
# Test-only edge observer/completion spy; production detector remains linked.
zephyr_link_libraries(-Wl,--wrap=braille_chord_key_event -Wl,--wrap=braille_chord_resolved)
zephyr_library()
zephyr_library_include_directories(../../../include ${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources(edge_spy.c)
