#include "settings_WIFI.h"
#include "WIFI_utils.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <vector>
#include "ui.h"
extern TFT_eSPI tft;

static lv_obj_t *wifi_list;

void connect_to_wifi_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *ssid = (const char *)lv_obj_get_user_data(btn);

    char password[64] = "";
    if (!loadWiFiCredentials(ssid, password, sizeof(password))) {
        // Show a keyboard for the user to input the password
        lv_obj_t *scr = lv_scr_act();
        lv_obj_clean(scr);

        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text_fmt(label, "Password for %s:", ssid);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

        lv_obj_t *kb = lv_keyboard_create(scr);
        lv_obj_set_size(kb, 240, 100);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

        lv_obj_t *ta = lv_textarea_create(scr);
        lv_obj_set_size(ta, 200, 40);
        lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 40);
        lv_textarea_set_one_line(ta, true);

        lv_keyboard_set_textarea(kb, ta);

        lv_obj_t *connect_btn = lv_btn_create(scr);
        lv_obj_set_size(connect_btn, 40, 40);
        lv_obj_align(connect_btn, LV_ALIGN_CENTER, 0, 90);
        lv_obj_t *connect_label = lv_label_create(connect_btn);
        lv_label_set_text(connect_label, LV_SYMBOL_OK);
        lv_obj_set_user_data(connect_btn, ta);
        lv_obj_add_event_cb(connect_btn, connect_to_wifi_event_cb, LV_EVENT_CLICKED, ta);
    } else {
        // Connect to WiFi using the saved password
        if (connectToNetwork(ssid, password)) {
            showWiFiSettings();
        } else {
            // Handle connection failure
            lv_obj_t *label = lv_label_create(lv_scr_act());
            lv_label_set_text(label, "Failed to connect to WiFi");
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        }
    }
}

void showAvailableNetworks() {
    std::vector<String> networks = scanNetworks();
    for (const auto &network : networks) {
        lv_obj_t *btn = lv_list_add_btn(wifi_list, NULL, network.c_str());
        char *network_copy = strdup(network.c_str());
        lv_obj_set_user_data(btn, network_copy);
        lv_obj_add_event_cb(btn, connect_to_wifi_event_cb, LV_EVENT_CLICKED, network_copy);
    }
}

void wifi_enable_event_cb(lv_event_t *e) {
    enableWiFi();
    showWiFiSettings();
}

void wifi_disable_event_cb(lv_event_t *e) {
    disableWiFi();
    showWiFiSettings();
}

void wifi_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    if (lv_obj_has_state(sw, LV_STATE_CHECKED)) {
        enableWiFi();
    } else {
        disableWiFi();
    }
    showWiFiSettings();
}

void showWiFiSettings(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *sw = lv_switch_create(scr);
    lv_obj_align(sw, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_add_event_cb(sw, wifi_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    if (isWiFiEnabled()) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);

        lv_obj_t *info_label = lv_label_create(scr);
        lv_label_set_text_fmt(info_label, "Current Network:\n%s", getCurrentNetworkInfo().c_str());
        lv_obj_align(info_label, LV_ALIGN_TOP_MID, 0, 60);

        wifi_list = lv_list_create(scr);
        lv_obj_set_size(wifi_list, 240, 160);
        lv_obj_align(wifi_list, LV_ALIGN_BOTTOM_MID, 0, -10);

        showAvailableNetworks();
    } else {
        lv_obj_clear_state(sw, LV_STATE_CHECKED);
    }

    drawNavBar();
}