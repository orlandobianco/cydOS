#ifndef SETTINGS_H
#define SETTINGS_H

#include <lvgl.h> // Include LVGL headers

void showSettings(lv_event_t *e = nullptr); // Allow default value for e
void initSettings();
void saveBrightness(int brightness);
int loadBrightness();
void showConnectivity(lv_event_t *e = nullptr); // Allow default value for e
void showDisplaySettings(lv_event_t *e = nullptr); // Allow default value for e
void showSensorsSettings(lv_event_t *e = nullptr); // Allow default value for e
void showSDCardSettings(lv_event_t *e = nullptr); // Allow default value for e
void showBackupSettings(lv_event_t *e = nullptr); // Allow default value for e

#endif // SETTINGS_H