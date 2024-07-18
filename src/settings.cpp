#include "settings.h"
#include <lvgl.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

void showSettings() {
    //redraw the entire screen
    tft.fillScreen(TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Settings:");
    tft.setCursor(10, 50);
    tft.println("1: WiFi");
    tft.setCursor(10, 90);
    tft.println("2: Bluetooth");
    tft.setCursor(10, 130);
    tft.println("3: Display");
    tft.setCursor(10, 170);
    tft.println("4: System Info");
    // Codice per mostrare le impostazioni
}

void launchSettings() {
    // Codice per lanciare le impostazioni, se diverso da showSettings
}