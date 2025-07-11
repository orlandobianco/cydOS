#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <WiFi.h>
#include <vector> // Include vector header

// Connection states
enum WiFiConnectionState {
    WIFI_STATE_IDLE,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_FAILED
};

void initWiFi();
void enableWiFi();
void disableWiFi();
bool isWiFiEnabled();
String getCurrentNetworkInfo();
std::vector<String> scanNetworks();
bool connectToNetwork(const char* ssid, const char* password);
void saveWiFiCredentials(const char* ssid, const char* password);
bool loadWiFiCredentials(const char* ssid, char* password, size_t maxLen);

// Async WiFi functions
void startAsyncConnect(const char* ssid, const char* password);
WiFiConnectionState checkConnectionStatus();
void updateWiFiConnection();
int getConnectionProgress();

#endif // WIFI_UTILS_H