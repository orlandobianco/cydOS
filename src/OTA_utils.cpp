#include "OTA_utils.h"
#include "SD_utils.h"
#include "lvgl.h"

extern SdFat sd;

void ota_task(void *pvParameter) {
    const char *dir_name = (const char *)pvParameter;
    char full_path[128];

    // Flash bootloader if available
    snprintf(full_path, sizeof(full_path), "/apps/%s/bootloader.bin", dir_name);
    Serial.printf("Checking for bootloader: %s\n", full_path);
    if (sd.exists(full_path)) {
        Serial.println("Bootloader found, flashing...");
        const esp_partition_t* bootloader_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
        if (bootloader_partition) {
            Serial.printf("Bootloader partition size: %d\n", bootloader_partition->size);
            if (flash_binary(full_path, bootloader_partition) != ESP_OK) {
                Serial.println("Failed to flash bootloader!");
                vTaskDelete(NULL);
                return;
            }
        }
    }

    // Flash app0 (the main firmware) - mandatory
    snprintf(full_path, sizeof(full_path), "/apps/%s/firmware.bin", dir_name);
    Serial.printf("Checking for firmware: %s\n", full_path);
    if (!sd.exists(full_path)) {
        Serial.println("Firmware file not found!");
        vTaskDelete(NULL);
        return;
    }
    Serial.println("Firmware found, flashing...");

    const esp_partition_t *firmware_partition = esp_ota_get_next_update_partition(NULL);
    if (firmware_partition) {
        Serial.printf("Firmware partition size: %d\n", firmware_partition->size);
        if (flash_binary(full_path, firmware_partition) != ESP_OK) {
            Serial.println("Failed to flash firmware!");
            vTaskDelete(NULL);
            return;
        }

        // Set the boot partition to the newly flashed firmware partition
        esp_err_t err = esp_ota_set_boot_partition(firmware_partition);
        if (err != ESP_OK) {
            Serial.println("Failed to set boot partition!");
            vTaskDelete(NULL);
            return;
        }
    }

    // Flash boot_app0 if available
    snprintf(full_path, sizeof(full_path), "/apps/%s/boot_app0.bin", dir_name);
    Serial.printf("Checking for boot_app0: %s\n", full_path);
    if (sd.exists(full_path)) {
        Serial.println("boot_app0 found, flashing...");
        const esp_partition_t* boot_app0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
        if (boot_app0_partition) {
            Serial.printf("boot_app0 partition size: %d\n", boot_app0_partition->size);
            if (flash_binary(full_path, boot_app0_partition) != ESP_OK) {
                Serial.println("Failed to flash boot_app0!");
                vTaskDelete(NULL);
                return;
            }
        }
    }

    ESP_LOGI("OTA", "OTA update complete, restarting...");
    esp_restart();
}

esp_err_t flash_binary(const char *path, const esp_partition_t *partition) {
    SdFile file;
    if (!file.open(path, O_RDONLY)) {
        ESP_LOGE("OTA", "Failed to open file for reading: %s", path);
        return ESP_FAIL;
    }

    // Check if the firmware size exceeds the partition size
    uint32_t fileSize = file.fileSize();
    if (fileSize > partition->size) {
        file.close();
        ESP_LOGE("OTA", "Firmware size (%d) exceeds partition size (%d)", fileSize, partition->size);
        return ESP_ERR_INVALID_SIZE;
    }

    esp_ota_handle_t ota_handle;
    esp_err_t err = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        file.close();
        ESP_LOGE("OTA", "esp_ota_begin failed: %s", esp_err_to_name(err));
        return err;
    }

    char buf[1024];
    int bytes_read;
    while ((bytes_read = file.read(buf, sizeof(buf))) > 0) {
        err = esp_ota_write(ota_handle, buf, bytes_read);
        if (err != ESP_OK) {
            file.close();
            ESP_LOGE("OTA", "esp_ota_write failed: %s", esp_err_to_name(err));
            return err;
        }
    }

    file.close();
    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE("OTA", "esp_ota_end failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}