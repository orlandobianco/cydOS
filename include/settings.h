#ifndef SETTINGS_H
#define SETTINGS_H

#include <lvgl.h>

void showSettings(lv_event_t *e = nullptr);
void initSettings();
void saveBrightness(int brightness);
int loadBrightness();
void showConnectivity(lv_event_t *e = nullptr);
void showDisplaySettings(lv_event_t *e = nullptr);
void showSensorsSettings(lv_event_t *e = nullptr);
void showSDCardSettings(lv_event_t *e = nullptr);
void showBackupSettings(lv_event_t *e = nullptr);

#endif // SETTINGS_H