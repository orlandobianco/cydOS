#include "ui.h"
#include "home_screen.h"
#include "settings.h"
#include "event_handlers.h"
#include "utils.h"
#include <lvgl.h>
#include "main.h"


//basic ui elements like navbar anche status bar // call them when needed
// Handler per il pulsante della torcia


void drawNavBar() {
    short int dockiconsize = 50;
    short int dockmargin = 55;
    lv_obj_t *parent = lv_scr_act();

    // DOCK
    lv_obj_t *slide_menu = lv_obj_create(parent);
    lv_obj_set_size(slide_menu, 240, dockmargin);  // Ridurre l'altezza del menu
    lv_obj_set_style_bg_color(slide_menu, lv_color_hex(0xD3D3D3), 0);  // Colore grigio chiaro
    lv_obj_align(slide_menu, LV_ALIGN_BOTTOM_MID,0,0);  // Posizionare il menu appena sopra la barra
    lv_obj_clear_flag(slide_menu, LV_OBJ_FLAG_SCROLLABLE);  // Disabilita lo scrolling per il menu

    // TORCH
    lv_obj_t *torch_btn = lv_btn_create(slide_menu);
    lv_obj_set_size(torch_btn, dockiconsize, dockiconsize);  // Rendere i pulsanti più piccoli
    lv_obj_add_event_cb(torch_btn, torch_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align(torch_btn, LV_ALIGN_LEFT_MID, 10, 0);  // Aggiungere margine per evitare che i pulsanti si tocchino
    lv_obj_set_style_bg_color(torch_btn, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro iniziale
    lv_obj_t *torch_label = lv_label_create(torch_btn);
    lv_label_set_text(torch_label, "Torch");  // Utilizzare una rappresentazione testuale semplice
    lv_obj_center(torch_label);

    // HOME
    lv_obj_t *home_BTN = lv_btn_create(slide_menu);
    lv_obj_set_size(home_BTN, dockiconsize, dockiconsize);  // Rendere i pulsanti più piccoli
    lv_obj_align(home_BTN, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(home_BTN, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro
    lv_obj_add_event_cb(home_BTN, home_button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *home_BTN_label = lv_label_create(home_BTN);
    lv_label_set_text(home_BTN_label, "Home");  // Placeholder text
    lv_obj_center(home_BTN_label);

    // back
    lv_obj_t *placeholder2_btn = lv_btn_create(slide_menu);
    lv_obj_set_size(placeholder2_btn, dockiconsize, dockiconsize);  // Rendere i pulsanti più piccoli
    lv_obj_align(placeholder2_btn, LV_ALIGN_RIGHT_MID, -10, 0);  // Aggiungere margine per evitare che i pulsanti si tocchino
    lv_obj_set_style_bg_color(placeholder2_btn, lv_color_hex(0xA9A9A9), 0);  // Colore grigio chiaro
    lv_obj_t *placeholder2_label = lv_label_create(placeholder2_btn);
    lv_label_set_text(placeholder2_label, "back");  // Placeholder text
    lv_obj_center(placeholder2_label);
}
