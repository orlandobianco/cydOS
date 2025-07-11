
#include "explorer.h"
#include <lvgl.h>
#include "launcher.h"
#include "event_handlers.h"
#include "utils.h"

// Static variables moved from header
static bool is_initialized = false;
static char current_path[128] = "/";

// Accessor functions for the static variables
bool get_explorer_initialized() {
    return is_initialized;
}

void set_explorer_initialized(bool value) {
    is_initialized = value;
}

char* get_current_path() {
    return current_path;
}

void set_current_path(const char* path) {
    strncpy(current_path, path, sizeof(current_path) - 1);
    current_path[sizeof(current_path) - 1] = '\0';
}

static void free_file_name(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    char *fileName = (char *)lv_obj_get_user_data(btn);
    if (fileName) {
        free(fileName);
        lv_obj_set_user_data(btn, NULL);
    }
}

void showFileExplorer(lv_event_t *e) {
    if (!is_initialized) {
        if (!init_sd_card()) {
            showError("SD init failed!");
            return;
        }
        is_initialized = true;
    }

    SdFile dir;
    if (!open_dir(dir, current_path)) {
        showError("Failed to open current directory");
        return;
    }

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 240, 280);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

  

    std::vector<FileInfo> files = list_files_in_dir(dir);
    for (const auto &file : files) {
        char file_info[128];
        snprintf(file_info, sizeof(file_info), "%s (%d KB)", file.name, file.size / 1024);
        lv_obj_t *btn = lv_list_add_btn(list, NULL, file_info);
        char *file_name_copy = strdup(file.name);
        lv_obj_set_user_data(btn, file_name_copy);
        lv_obj_add_event_cb(btn, dir_event_handler, LV_EVENT_CLICKED, file_name_copy);
        lv_obj_add_event_cb(btn, free_file_name, LV_EVENT_DELETE, NULL);
    }

    dir.close();

    drawExplorerNavBar();
}

void dir_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *dirName = (const char *)lv_obj_get_user_data(btn);

    // Use safe path join to prevent buffer overflow and path traversal
    char new_path[256];  // Increased buffer size
    if (!safe_path_join(new_path, sizeof(new_path), current_path, dirName)) {
        showError("Invalid directory name or path too long");
        return;
    }

    strncpy(current_path, new_path, sizeof(current_path) - 1);
    current_path[sizeof(current_path) - 1] = '\0';  // Ensure null termination

    showFileExplorer();
}

void create_dir_event_handler(lv_event_t *e) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Enter directory name:");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *kb = lv_keyboard_create(scr);
    lv_obj_set_size(kb, 240, 100);
    lv_obj_align(kb, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *ta = lv_textarea_create(scr);
    lv_obj_set_size(ta, 200, 40);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 40);
    lv_textarea_set_one_line(ta, true);

    lv_keyboard_set_textarea(kb, ta);

    lv_obj_t *yes_btn = lv_btn_create(scr);
    lv_obj_set_size(yes_btn, 80, 40);
    lv_obj_align(yes_btn, LV_ALIGN_CENTER, -60, 90);
    lv_obj_t *yes_label = lv_label_create(yes_btn);
    lv_label_set_text(yes_label, "Create");
    lv_obj_set_user_data(yes_btn, ta);
    lv_obj_add_event_cb(yes_btn, confirm_create_dir_event_handler, LV_EVENT_CLICKED, ta);

    lv_obj_t *no_btn = lv_btn_create(scr);
    lv_obj_set_size(no_btn, 80, 40);
    lv_obj_align(no_btn, LV_ALIGN_CENTER, 60, 90);
    lv_obj_t *no_label = lv_label_create(no_btn);
    lv_label_set_text(no_label, "Cancel");
    lv_obj_add_event_cb(no_btn, showFileExplorer, LV_EVENT_CLICKED, NULL);

    drawExplorerNavBar();
}

void confirm_create_dir_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *ta = (lv_obj_t *)lv_obj_get_user_data(btn);
    const char *dirName = lv_textarea_get_text(ta);

    // Validate directory name using utility function
    if (!validate_path_component(dirName)) {
        showError("Invalid directory name");
        return;
    }

    if (create_directory(current_path, dirName)) {
        showFileExplorer();
    } else {
        showError("Failed to create directory");
    }
}

void back_to_parent_dir(lv_event_t *e) {
    // Find the last slash and null-terminate there to go to the parent directory
    char *last_slash = strrchr(current_path, '/');
    if (last_slash != nullptr && last_slash != current_path) {
        *last_slash = '\0';
    } else {
        // If we are at the root directory, do nothing or handle accordingly
        strcpy(current_path, "/");
    }
    showFileExplorer();
}

//navbar for file explorer witch back button, home button and new dir button (like a +)
void drawExplorerNavBar() {
    short int dockiconsize = 50;
    short int dockmargin = 55;
    lv_obj_t *parent = lv_scr_act();

    // DOCK
    lv_obj_t *slide_menu = lv_obj_create(parent);
    lv_obj_set_size(slide_menu, 240, dockmargin);  // Ridurre l'altezza del menu
    lv_obj_set_style_bg_color(slide_menu, lv_color_hex(0xD3D3D3), 0);  // Colore grigio chiaro
    lv_obj_align(slide_menu, LV_ALIGN_BOTTOM_MID,0,0);  // Posizionare il menu appena sopra la barra
    lv_obj_clear_flag(slide_menu, LV_OBJ_FLAG_SCROLLABLE);  // Disabilita lo scrolling per il menu

    // ADD DIR
    lv_obj_t *add_dir_btn = lv_btn_create(slide_menu);
    lv_obj_set_size(add_dir_btn, dockiconsize, dockiconsize);  // Rendere i pulsanti più piccoli
    lv_obj_add_event_cb(add_dir_btn, create_dir_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align(add_dir_btn, LV_ALIGN_LEFT_MID, 10, 0);  // Aggiungere margine per evitare che i pulsanti si tocchino
    lv_obj_set_style_bg_color(add_dir_btn, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro iniziale
    lv_obj_t *add_dir_label = lv_label_create(add_dir_btn);
    lv_label_set_text(add_dir_label, LV_SYMBOL_PLUS);  // Utilizzare una rappresentazione testuale semplice
    lv_obj_center(add_dir_label);

    // HOME
    lv_obj_t *home_BTN = lv_btn_create(slide_menu);
    lv_obj_set_size(home_BTN, dockiconsize, dockiconsize);  // Rendere i pulsanti più piccoli
    lv_obj_align(home_BTN, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(home_BTN, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro
    lv_obj_add_event_cb(home_BTN, home_button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *home_BTN_label = lv_label_create(home_BTN);
    lv_label_set_text(home_BTN_label, LV_SYMBOL_HOME);  // Placeholder text
    lv_obj_center(home_BTN_label);

    // back
    lv_obj_t *back_btn = lv_btn_create(slide_menu);
    lv_obj_set_size(back_btn, dockiconsize, dockiconsize);  // Rendere i pulsanti più piccoli
    lv_obj_align(back_btn, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro
    lv_obj_add_event_cb(back_btn, back_to_parent_dir, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_REFRESH);  // Placeholder text
    //center label
    lv_obj_center(back_label);
}
    