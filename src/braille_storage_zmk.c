// SPDX-License-Identifier: MIT
#ifdef __ZEPHYR__
#include <braille_storage.h>
#include <drivers/behavior.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/settings/settings.h>

int braille_storage_resolve(struct zmk_behavior_binding *binding) {
  const struct device *device = zmk_behavior_get_binding(binding->behavior_dev);
  if (!device || !device_is_ready(device))
    return -ENODEV;
  binding->behavior_dev = device->name;
  return zmk_behavior_validate_binding(binding);
}
int braille_storage_write(unsigned slot, const uint8_t *data, size_t size) {
  char key[24];
  snprintf(key, sizeof(key), "braille/r/%u", slot);
  return settings_save_one(key, data, size);
}
int braille_storage_delete(unsigned slot) {
  char key[24];
  snprintf(key, sizeof(key), "braille/r/%u", slot);
  return settings_delete(key);
}
int braille_storage_write_mode(const char *name) {
  return settings_save_one("braille/mode", name, strlen(name));
}
static int storage_set(const char *name, size_t len, settings_read_cb read_cb,
                       void *arg) {
  if (!strcmp(name, "mode")) {
    char mode[32];
    if (!len || len >= sizeof(mode))
      return 0;
    if (read_cb(arg, mode, len) != (ssize_t)len)
      return 0;
    mode[len] = 0;
    if (memchr(mode, 0, len))
      return 0;
    braille_storage_load_mode(mode);
    return 0;
  }
  if (strncmp(name, "r/", 2))
    return -ENOENT;
  char *end;
  unsigned long slot = strtoul(name + 2, &end, 10);
  if (name[2] == 0 || *end || slot >= BRAILLE_STORAGE_SLOTS)
    return 0;
  /* A malformed/oversize record occupies its slot and remains untouched. */
  uint8_t data[BRAILLE_STORAGE_RECORD_MAX];
  if (!len)
    return 0; /* settings tombstone */
  if (len > sizeof(data) || read_cb(arg, data, len) != (ssize_t)len) {
    braille_storage_load_record((unsigned)slot, NULL, 0);
    return 0;
  }
  braille_storage_load_record((unsigned)slot, data, len);
  return 0;
}
static int storage_commit(void) {
  braille_storage_finish_load();
  return 0;
}
SETTINGS_STATIC_HANDLER_DEFINE(braille, "braille", NULL, storage_set,
                               storage_commit, NULL);

#endif /* __ZEPHYR__ */
