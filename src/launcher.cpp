#include <TFT_eSPI.h>
#include <SdFat.h>
#include "lvgl.h"
#include "utils.h"
#include "event_handlers.h"

extern TFT_eSPI tft;

SdFat sd;
static bool is_initialized = false;

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
    lv_obj_add_event_cb(back_btn, back_button_event_handler, LV_EVENT_CLICKED, NULL);

    Serial.println("Error displayed with back button.");
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

    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_add_event_cb(back_btn, back_button_event_handler, LV_EVENT_CLICKED, NULL);
    Serial.println("Back button created and event handler added.");

    appsDir.rewind();
    SdFile entry;
    int y = 60;
    bool binFileFound = false;

    while (entry.openNext(&appsDir, O_RDONLY)) {
        char fileName[64];
        entry.getName(fileName, sizeof(fileName));
        if (!entry.isDir() && String(fileName).endsWith(".bin")) {
            binFileFound = true;

            lv_obj_t *btn = lv_btn_create(scr);
            lv_obj_set_size(btn, 200, 40);
            lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y);
            lv_obj_t *btn_label = lv_label_create(btn);
            lv_label_set_text(btn_label, fileName);

            char *fileNameCopy = strdup(fileName);
            lv_obj_set_user_data(btn, fileNameCopy);
            lv_obj_add_event_cb(btn, launcher_event_handler, LV_EVENT_CLICKED, fileNameCopy);
            Serial.printf("Created button for %s\n", fileName);

            y += 50;
        }
        entry.close();
    }

    appsDir.close();

    if (!binFileFound) {
        lv_obj_t *no_app_label = lv_label_create(scr);
        lv_label_set_text(no_app_label, "NO APP AVAILABLE");
        lv_obj_align(no_app_label, LV_ALIGN_CENTER, 0, 0);
    }

    Serial.println("SD card files listed!");
}