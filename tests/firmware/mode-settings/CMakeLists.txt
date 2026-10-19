# SPDX-License-Identifier: MIT
# Observe the downstream gate without redefining the production engine symbols.
zephyr_link_libraries(-Wl,--wrap=braille_chord_resolved)
zephyr_library()
zephyr_library_include_directories(../../../include ${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources(fixture.c)
