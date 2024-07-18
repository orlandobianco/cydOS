
#include "explorer.h"
#include <lvgl.h>
#include "launcher.h"


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

    lv_obj_t *create_dir_btn = lv_btn_create(scr);
    lv_obj_set_size(create_dir_btn, 100, 40);
    lv_obj_align(create_dir_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_t *create_dir_label = lv_label_create(create_dir_btn);
    lv_label_set_text(create_dir_label, "New Dir");
    lv_obj_add_event_cb(create_dir_btn, create_dir_event_handler, LV_EVENT_CLICKED, NULL);

    std::vector<FileInfo> files = list_files_in_dir(dir);
    for (const auto &file : files) {
        char file_info[128];
        snprintf(file_info, sizeof(file_info), "%s (%d KB)", file.name, file.size / 1024);
        lv_obj_t *btn = lv_list_add_btn(list, NULL, file_info);
        char *file_name_copy = strdup(file.name);
        lv_obj_set_user_data(btn, file_name_copy);
        lv_obj_add_event_cb(btn, dir_event_handler, LV_EVENT_CLICKED, file_name_copy);
    }

    dir.close();

    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 60);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_add_event_cb(back_btn, back_to_parent_dir, LV_EVENT_CLICKED, NULL);

    drawNavBar();
}

void dir_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *dirName = (const char *)lv_obj_get_user_data(btn);

    char new_path[128];
    snprintf(new_path, sizeof(new_path), "%s/%s", current_path, dirName);
    strncpy(current_path, new_path, sizeof(current_path));

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

    drawNavBar();
}

void confirm_create_dir_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *ta = (lv_obj_t *)lv_obj_get_user_data(btn);
    const char *dirName = lv_textarea_get_text(ta);

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