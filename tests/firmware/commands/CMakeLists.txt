# SPDX-License-Identifier: MIT
# Observe real HID reports, always forwarding to the real endpoint implementation.
zephyr_link_libraries(-Wl,--wrap=zmk_endpoints_send_report)
zephyr_library()
zephyr_library_include_directories(../../../include ${APPLICATION_SOURCE_DIR}/include)
zephyr_library_sources(fixture.c)
