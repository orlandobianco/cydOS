#ifndef EXPLORER_H
#define EXPLORER_H


#include <TFT_eSPI.h>
#include <SdFat.h>
#include "lvgl.h"
#include "SD_utils.h"

#include "ui.h" // Assuming ui.h includes the keyboard functionality

extern TFT_eSPI tft;
extern SdFat sd;
static bool is_initialized = false;
static char current_path[128] = "/";
void showError(const char *msg);
void showFileExplorer(lv_event_t *e = nullptr);
void dir_event_handler(lv_event_t *e);
void create_dir_event_handler(lv_event_t *e);
void confirm_create_dir_event_handler(lv_event_t *e);
void back_to_parent_dir(lv_event_t *e);
void drawExplorerNavBar();

#endif // EXPLORER_H