#include <TFT_eSPI.h>
#include <SdFat.h>
#include "lvgl.h"
#include "utils.h"
#include "event_handlers.h"
#include "ui.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_partition.h"

extern TFT_eSPI tft;

SdFat sd;
static bool is_initialized = false;

void showError(const char *msg);
void showLauncher();
void install_event_handler(lv_event_t *e);
void confirm_install_event_handler(lv_event_t *e);
void ota_task(void *pvParameter);
esp_err_t flash_binary(const char *path, const esp_partition_t *partition);

void showError(const char *msg) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, msg);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, 20);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_add_event_cb(back_btn, home_button_event_handler, LV_EVENT_CLICKED, NULL);
    drawNavBar();
    Serial.println("Error displayed with navbar.");
}

void showLauncher() {
    if (!is_initialized) {
        if (!sd.begin(SS, SD_SCK_MHZ(32))) {
            Serial.println("Failed to initialize SD card");
            showError("SD init failed!");
            return;
        }
        is_initialized = true;
    }

    SdFile appsDir;
    if (!appsDir.open("/apps")) {
        if (sd.mkdir("/apps")) {
            Serial.println("apps dir created");
        } else {
            Serial.println("Failed to create apps dir");
            showError("Failed to create apps dir");
            return;
        }
    } else {
        appsDir.close();
    }

    if (!appsDir.open("/apps")) {
        Serial.println("Failed to open apps directory");
        showError("apps dir open failed!");
        return;
    }

    Serial.println("Opened apps directory successfully");

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 240, 280);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

    appsDir.rewind();
    SdFile entry;

    while (entry.openNext(&appsDir, O_RDONLY)) {
        if (entry.isDir()) {
            char dirName[64];
            entry.getName(dirName, sizeof(dirName));

            lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, dirName);
            char *dirNameCopy = strdup(dirName);
            lv_obj_set_user_data(btn, dirNameCopy);
            lv_obj_add_event_cb(btn, install_event_handler, LV_EVENT_CLICKED, dirNameCopy);
            Serial.printf("Created button for directory: %s\n", dirName);
        }
        entry.close();
    }

    appsDir.close();
    drawNavBar();
    Serial.println("SD card directories listed!");
}

void install_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *dirName = (const char *)lv_obj_get_user_data(btn);

    Serial.printf("Selected directory: %s\n", dirName);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text_fmt(label, "Files in %s:", dirName);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *file_list = lv_list_create(scr);
    lv_obj_set_size(file_list, 240, 180);
    lv_obj_align(file_list, LV_ALIGN_CENTER, 0, 20);

    SdFile dir;
    char full_dir_path[128];
    snprintf(full_dir_path, sizeof(full_dir_path), "/apps/%s", dirName);

    if (!dir.open(full_dir_path)) {
        showError("Failed to open selected directory!");
        return;
    }

    dir.rewind();
    SdFile file;
    char fileName[64];

    while (file.openNext(&dir, O_RDONLY)) {
        file.getName(fileName, sizeof(fileName));
        lv_list_add_btn(file_list, NULL, fileName);
        Serial.printf("Found file: %s\n", fileName);
        file.close();
    }

    dir.close();

    lv_obj_t *yes_btn = lv_btn_create(scr);
    lv_obj_set_size(yes_btn, 80, 40);
    lv_obj_align(yes_btn, LV_ALIGN_CENTER, -60, 90);
    lv_obj_t *yes_label = lv_label_create(yes_btn);
    lv_label_set_text(yes_label, "Install");
    lv_obj_set_user_data(yes_btn, (void *)dirName);
    lv_obj_add_event_cb(yes_btn, confirm_install_event_handler, LV_EVENT_CLICKED, (void *)dirName);

    lv_obj_t *no_btn = lv_btn_create(scr);
    lv_obj_set_size(no_btn, 80, 40);
    lv_obj_align(no_btn, LV_ALIGN_CENTER, 60, 90);
    lv_obj_t *no_label = lv_label_create(no_btn);
    lv_label_set_text(no_label, "Cancel");
    lv_obj_add_event_cb(no_btn, home_button_event_handler, LV_EVENT_CLICKED, NULL);

    drawNavBar();
}

void confirm_install_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *dirName = (const char *)lv_obj_get_user_data(btn);

    Serial.printf("Confirmed install from directory: %s\n", dirName);

    xTaskCreatePinnedToCore(ota_task, "ota_task", 8192, (void *)dirName, 5, NULL, 1);
}

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
                showError("Failed to flash bootloader!");
                vTaskDelete(NULL);
                return;
            }
        }
    }

    // Flash app0 (the main firmware) - mandatory
    snprintf(full_path, sizeof(full_path), "/apps/%s/firmware.bin", dir_name);
    Serial.printf("Checking for firmware: %s\n", full_path);
    if (!sd.exists(full_path)) {
        showError("Firmware file not found!");
        vTaskDelete(NULL);
        return;
    }
    Serial.println("Firmware found, flashing...");

    const esp_partition_t *firmware_partition = esp_ota_get_next_update_partition(NULL);
    if (firmware_partition) {
        Serial.printf("Firmware partition size: %d\n", firmware_partition->size);
        if (flash_binary(full_path, firmware_partition) != ESP_OK) {
            showError("Failed to flash firmware!");
            vTaskDelete(NULL);
            return;
        }

        // Set the boot partition to the newly flashed firmware partition
        esp_err_t err = esp_ota_set_boot_partition(firmware_partition);
        if (err != ESP_OK) {
            showError("Failed to set boot partition!");
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
                showError("Failed to flash boot_app0!");
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