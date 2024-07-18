#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include <lvgl.h>

void settings_event_handler(lv_event_t *e); //apri settings button
void launcher_event_handler(lv_event_t *e); //apri launcher button
void touch_event_handler(lv_event_t *e); //tocco sun file.bin
void back_button_event_handler(lv_event_t * e); //tocco back button da scheramta bin launcher a home screen

#endif // EVENT_HANDLERS_H