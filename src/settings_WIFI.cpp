#include "settings_WIFI.h"
#include "WIFI_utils.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <vector>
#include <Arduino.h>
#include "ui.h"

extern TFT_eSPI tft;

static lv_obj_t *wifi_list;

// Forward declare the function
void connect_to_wifi_event_cb(lv_event_t *e);

void prompt_for_password(const char* ssid) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text_fmt(label, "Password for %s:", ssid);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *ta = lv_textarea_create(scr);
    lv_obj_set_size(ta, 200, 40);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_one_line(ta, true);

    lv_obj_t *kb = lv_keyboard_create(scr);
    lv_obj_set_size(kb, 240, 100);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_textarea(kb, ta);

    lv_obj_t *connect_btn = lv_btn_create(scr);
    lv_obj_set_size(connect_btn, 40, 40);
    lv_obj_align(connect_btn, LV_ALIGN_CENTER, 0, 90);
    lv_obj_t *connect_label = lv_label_create(connect_btn);
    lv_label_set_text(connect_label, LV_SYMBOL_OK);
    lv_obj_set_user_data(connect_btn, (void *)ssid); // Cast const char* to void*
    lv_obj_add_event_cb(connect_btn, connect_to_wifi_event_cb, LV_EVENT_CLICKED, ta);
}

void connect_to_wifi_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *ta = (lv_obj_t *)lv_event_get_user_data(e);
    const char *ssid = (const char *)lv_obj_get_user_data(btn);

    Serial.printf("connect_to_wifi_event_cb called with ssid: %p, ta: %p\n", ssid, ta);

    if (ssid == nullptr) {
        Serial.println("SSID is null, returning.");
        return;
    }

    if (ta != nullptr && lv_obj_check_type(ta, &lv_textarea_class)) {
        // User input from text area
        const char *password = lv_textarea_get_text(ta);
        if (password == nullptr || strlen(password) == 0) {
            Serial.println("Password is null or empty, returning.");
            return;
        }
        Serial.printf("Attempting to connect to SSID: %s with password: %s\n", ssid, password);
        saveWiFiCredentials(ssid, password);
        if (connectToNetwork(ssid, password)) {
            Serial.println("Connection successful");
            lv_obj_t *scr = lv_scr_act();
            lv_obj_clean(scr);
            lv_obj_t *label = lv_label_create(scr);
            lv_label_set_text(label, "Connected successfully!");
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            delay(2000); // Show the message for 2 seconds
            showWiFiSettings();
        } else {
            Serial.println("Connection failed");
            lv_obj_t *label = lv_label_create(lv_scr_act());
            lv_label_set_text(label, "Failed to connect to WiFi");
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            // Free the memory allocated for the SSID
            free((void *)ssid);
        }
    } else {
        // Load saved password
        char password[64] = "";
        Serial.printf("Loading saved password for SSID: %s\n", ssid);
        if (loadWiFiCredentials(ssid, password, sizeof(password))) {
            Serial.printf("Connecting to SSID: %s with saved password\n", ssid);
            if (connectToNetwork(ssid, password)) {
                Serial.println("Connection successful");
                lv_obj_t *scr = lv_scr_act();
                lv_obj_clean(scr);
                lv_obj_t *label = lv_label_create(scr);
                lv_label_set_text(label, "Connected successfully!");
                lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
                delay(2000); // Show the message for 2 seconds
                showWiFiSettings();
            } else {
                Serial.println("Connection failed");
                lv_obj_t *label = lv_label_create(lv_scr_act());
                lv_label_set_text(label, "Failed to connect to WiFi");
                lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
                // Free the memory allocated for the SSID
                free((void *)ssid);
            }
        } else {
            Serial.println("No saved password found, prompting for input");
            prompt_for_password(ssid);
        }
    }
}

void showAvailableNetworks() {
    std::vector<String> networks = scanNetworks();
    Serial.println("Available Networks:");
    for (const auto &network : networks) {
        Serial.println(network);
        lv_obj_t *btn = lv_list_add_btn(wifi_list, NULL, network.c_str());
        char *network_copy = strdup(network.c_str());
        if (network_copy == nullptr) {
            Serial.println("Memory allocation failed for network_copy");
            continue;
        }
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