#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <WiFi.h>
#include <vector> // Include vector header

void initWiFi();
void enableWiFi();
void disableWiFi();
bool isWiFiEnabled();
String getCurrentNetworkInfo();
std::vector<String> scanNetworks();
bool connectToNetwork(const char* ssid, const char* password);
void saveWiFiCredentials(const char* ssid, const char* password);
bool loadWiFiCredentials(const char* ssid, char* password, size_t maxLen);

#endif // WIFI_UTILS_H