#include <lvgl.h>
#include "home_screen.h"
#include "event_handlers.h"
#include "utils.h"
#include <Arduino.h>
#include "ui.h"

void drawHomeScreen() {
    lv_obj_t *scr = lv_scr_act();
    short int iconsize = 67;
    // Imposta lo sfondo nero per la schermata principale
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);

    // Barra di stato
    lv_obj_t *status_bar = lv_obj_create(scr);
    lv_obj_set_size(status_bar, 240, 30);
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);  // Disabilita lo scrolling
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_t *time_label = lv_label_create(status_bar);
    lv_label_set_text(time_label, "12:00");
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    // Contenitore per la griglia di pulsanti
    lv_obj_t *btn_cont = lv_obj_create(scr);
    lv_obj_set_size(btn_cont, 240, 290);  // Altezza ridotta per lasciare spazio alla barra di stato
    lv_obj_align(btn_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(btn_cont, lv_color_black(), 0);  // Imposta lo sfondo nero per il contenitore
    lv_obj_clear_flag(btn_cont, LV_OBJ_FLAG_SCROLLABLE);  // Disabilita lo scrolling
    lv_obj_set_layout(btn_cont, LV_LAYOUT_GRID);  // Imposta il layout a griglia
    lv_obj_set_style_pad_all(btn_cont, 10, 0);  // Aggiungi padding intorno al contenitore

    // Configura la griglia
    static lv_coord_t col_dsc[] = {iconsize, iconsize, iconsize, LV_GRID_TEMPLATE_LAST};  // Colonne della griglia
    static lv_coord_t row_dsc[] = {iconsize, iconsize, iconsize, LV_GRID_TEMPLATE_LAST};  // Righe della griglia

    lv_obj_set_grid_dsc_array(btn_cont, col_dsc, row_dsc);

    // Crea i pulsanti e aggiungili alla griglia
    lv_obj_t *btn1 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn1, iconsize, iconsize);
    lv_obj_add_event_cb(btn1, settings_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);  // Colonna 0, riga 0
    lv_obj_t *label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "Settings");
    lv_obj_center(label1);  // Centra la scritta

    lv_obj_t *btn2 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn2, iconsize, iconsize);
    lv_obj_add_event_cb(btn2, launcher_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn2, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);  // Colonna 1, riga 0
    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "Launcher");
    lv_obj_center(label2);  // Centra la scritta

    lv_obj_t *btn3 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn3, iconsize, iconsize);
    // lv_obj_add_event_cb(btn3, another_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn3, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);  // Colonna 2, riga 0
    lv_obj_t *label3 = lv_label_create(btn3);
    lv_label_set_text(label3, "Another");
    lv_obj_center(label3);  // Centra la scritta

    lv_obj_t *btn4 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn4, iconsize, iconsize);
    lv_obj_add_event_cb(btn4, settings_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn4, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);  // Colonna 0, riga 1
    lv_obj_t *label4 = lv_label_create(btn4);
    lv_label_set_text(label4, "Settings");
    lv_obj_center(label4);  // Centra la scritta

    lv_obj_t *btn5 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn5, iconsize, iconsize);
    lv_obj_add_event_cb(btn5, launcher_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn5, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);  // Colonna 1, riga 1
    lv_obj_t *label5 = lv_label_create(btn5);
    lv_label_set_text(label5, "Launcher");
    lv_obj_center(label5);  // Centra la scritta

    lv_obj_t *btn6 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn6, iconsize, iconsize);
    // lv_obj_add_event_cb(btn6, another_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn6, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);  // Colonna 2, riga 1
    lv_obj_t *label6 = lv_label_create(btn6);
    lv_label_set_text(label6, "Another");
    lv_obj_center(label6);  // Centra la scritta

    lv_obj_t *btn7 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn7, iconsize, iconsize);
    lv_obj_add_event_cb(btn7, settings_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn7, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);  // Colonna 0, riga 2
    lv_obj_t *label7 = lv_label_create(btn7);
    lv_label_set_text(label7, "Settings");
    lv_obj_center(label7);  // Centra la scritta

    lv_obj_t *btn8 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn8, iconsize, iconsize);
    lv_obj_add_event_cb(btn8, launcher_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn8, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);  // Colonna 1, riga 2
    lv_obj_t *label8 = lv_label_create(btn8);
    lv_label_set_text(label8, "Launcher");
    lv_obj_center(label8);  // Centra la scritta

    lv_obj_t *btn9 = lv_btn_create(btn_cont);
    lv_obj_set_size(btn9, iconsize, iconsize);
    // lv_obj_add_event_cb(btn9, another_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(btn9, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);  // Colonna 2, riga 2
    lv_obj_t *label9 = lv_label_create(btn9);
    lv_label_set_text(label9, "Another");
    lv_obj_center(label9);  // Centra la scritta
    // Disegna il menu a comparsa
    drawNavBar();
}