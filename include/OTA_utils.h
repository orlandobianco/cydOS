#ifndef OTA_UTILS_H
#define OTA_UTILS_H

#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "SdFat.h"

void ota_task(void *pvParameter);
esp_err_t flash_binary(const char *path, const esp_partition_t *partition);

#endif // OTA_UTILS_H