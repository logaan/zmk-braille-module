// SPDX-License-Identifier: MIT

#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zmk_feature_braille);

static int braille_init(void) {
    LOG_INF("Hello from the ZMK Braille module!");
    return 0;
}

SYS_INIT(braille_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
