#include "event_handlers.h"
#include "settings.h"
#include "launcher.h"
#include <Arduino.h>
#include "home_screen.h"
#include <utils.h>

void settings_event_handler(lv_event_t *e) {
    Serial.println("Settings button clicked");
    showSettings();
}

void launcher_event_handler(lv_event_t *e) {
    Serial.println("Launcher button clicked");
    showLauncher();
}

void touch_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        const char *fileName = (const char*)lv_event_get_user_data(e);
        Serial.printf("User pressed %s\n", fileName);
    }
}

void back_button_event_handler(lv_event_t * e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    Serial.println("Back button event handler activated");
    drawHomeScreen();
    }
