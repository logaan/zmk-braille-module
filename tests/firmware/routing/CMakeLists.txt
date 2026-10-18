# SPDX-License-Identifier: MIT
# Test-only edge spy; the production module is discovered separately.
zephyr_library()
zephyr_library_include_directories(../../../include ${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources(edge_spy.c)
