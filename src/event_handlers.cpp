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

void home_button_event_handler(lv_event_t * e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    Serial.println("home button event handler activated");
    drawHomeScreen();
    }


//NAVBAR event handlers
void torch_event_handler(lv_event_t * e) {
    lv_obj_t *btn = lv_event_get_target(e);
    if (lv_obj_get_state(btn) & LV_STATE_CHECKED) {
        lv_obj_clear_state(btn, LV_STATE_CHECKED);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro
        lv_obj_set_style_text_color(lv_obj_get_child(btn, 0), lv_color_white(), 0);  // Testo bianco
        torchOFF();
    
    } else {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), 0);  // Colore blu
        lv_obj_set_style_text_color(lv_obj_get_child(btn, 0), lv_color_hex(0xFFFF00), 0);  // Testo giallo
        torchON();
    }
}

