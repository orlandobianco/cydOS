#include <TFT_eSPI.h>
#include <lvgl.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <SdFat.h>
#include "settings.h"
#include "SD_utils.h"
#include "settings_WIFI.h"
#include "ui.h"

extern TFT_eSPI tft;
extern SdFat sd;

#define NVS_NAMESPACE "storage"

static lv_obj_t *slider;
static lv_obj_t *label_brightness;

void initSettings() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void saveBrightness(int brightness) {
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_i32(my_handle, "brightness", brightness));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

int loadBrightness() {
    nvs_handle_t my_handle;
    int32_t brightness = 100;
    ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READONLY, &my_handle));
    ESP_ERROR_CHECK(nvs_get_i32(my_handle, "brightness", &brightness));
    nvs_close(my_handle);
    return brightness;
}

void brightness_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int brightness = lv_slider_get_value(slider);
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", brightness);
    lv_label_set_text(label_brightness, buf);
    saveBrightness(brightness);
    // Note: Adjust TFT brightness if supported by hardware
}

void showDisplaySettings(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Brightness:");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    slider = lv_slider_create(scr);
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 40);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, loadBrightness(), LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, brightness_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    label_brightness = lv_label_create(scr);
    lv_obj_align(label_brightness, LV_ALIGN_TOP_MID, 0, 70);
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", lv_slider_get_value(slider));
    lv_label_set_text(label_brightness, buf);

    drawNavBar();
}

void showSDCardSettings(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    uint64_t total_bytes = sd.card()->sectorCount() * 512;
    uint64_t used_bytes = total_bytes - (sd.vol()->freeClusterCount() * sd.vol()->sectorsPerCluster() * 512);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text_fmt(label, "SD Card Usage: %llu / %llu bytes", used_bytes, total_bytes);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *bar = lv_bar_create(scr);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 40);
    lv_bar_set_range(bar, 0, total_bytes);
    lv_bar_set_value(bar, used_bytes, LV_ANIM_OFF);

    drawNavBar();
}

void backup_create_cb(lv_event_t *e) {
    // Implement backup creation logic here
    showSettings(); // Go back to settings after the action
}

void backup_restore_cb(lv_event_t *e) {
    // Implement backup restoration logic here
    showSettings(); // Go back to settings after the action
}

void showBackupSettings(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *create_btn = lv_btn_create(scr);
    lv_obj_set_size(create_btn, 100, 40);
    lv_obj_align(create_btn, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_t *create_label = lv_label_create(create_btn);
    lv_label_set_text(create_label, "Create Backup");
    lv_obj_add_event_cb(create_btn, backup_create_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *restore_btn = lv_btn_create(scr);
    lv_obj_set_size(restore_btn, 100, 40);
    lv_obj_align(restore_btn, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_t *restore_label = lv_label_create(restore_btn);
    lv_label_set_text(restore_label, "Restore Backup");
    lv_obj_add_event_cb(restore_btn, backup_restore_cb, LV_EVENT_CLICKED, NULL);

    drawNavBar();
}

void showSettings(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 240, 280);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *btn;

    btn = lv_list_add_btn(list, NULL, "Connectivity");
    lv_obj_add_event_cb(btn, showConnectivity, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list, NULL, "Display");
    lv_obj_add_event_cb(btn, showDisplaySettings, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list, NULL, "SD Card");
    lv_obj_add_event_cb(btn, showSDCardSettings, LV_EVENT_CLICKED, NULL);

    btn = lv_list_add_btn(list, NULL, "Backup");
    lv_obj_add_event_cb(btn, showBackupSettings, LV_EVENT_CLICKED, NULL);

    drawNavBar();
}

void showConnectivity(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 240, 280);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *btn;

    btn = lv_list_add_btn(list, NULL, "WiFi");
    lv_obj_add_event_cb(btn, showWiFiSettings, LV_EVENT_CLICKED, NULL);

    drawNavBar();
}