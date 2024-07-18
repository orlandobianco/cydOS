#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Bitbang.h>
#include "home_screen.h"
#include "utils.h" // Include il file di intestazione
#include "main.h"

//inizialize the SPI class
SPIClass mySpi = SPIClass(VSPI);
XPT2046_Bitbang ts = XPT2046_Bitbang(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK, XPT2046_CS);

uint16_t touchScreenMinimumY = 200, touchScreenMaximumY = 3700, touchScreenMinimumX = 240, touchScreenMaximumX = 3800;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[240 * 320 / 10];

TFT_eSPI tft = TFT_eSPI(240, 320);
static bool touchPressed = false;

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp_drv);
}
// Touchpad read callback function
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    TouchPoint p = ts.getTouch();  // Use getTouch() function and correct type
    Serial.printf("TouchPoint x: %d, y: %d\n", p.x, p.y);  // Debugging line
    if (p.x != 0 && p.y != 0) {  // Check if touch is detected
        Serial.printf("Raw touch coordinates: x=%d, y=%d\n", p.x, p.y);
        data->point.x = 240 - p.y;  // Invert X coordinate
        data->point.y = p.x;
        data->state = LV_INDEV_STATE_PR;
        Serial.printf("Using inverted touch coordinates: x=%d, y=%d, state=PR\n", data->point.x, data->point.y);
        if (!touchPressed) {
            touchPressed = true;
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
        Serial.println("Touch released, state=REL");
        if (touchPressed) {
            touchPressed = false;
        }
    }

    digitalWrite(XPT2046_CS, HIGH);  // Deselect touchscreen
}

void flushDisplay() {
    tft.fillScreen(TFT_BLACK);  // Riempie lo schermo con il colore nero
    lv_obj_clean(lv_scr_act()); // Pulisce lo schermo attivo di LVGL
}

void setup() {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(0);
    mySpi.begin(25, 39, 32, 33);
    ts.begin();  // No parameters needed

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);

  
    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 240 * 320 / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 320;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
//start UI
    drawHomeScreen();
}

void loop() {
    lv_timer_handler();
    lv_task_handler();  // Call LVGL task handler
    delay(5);
}