/*
 * Minimal example — ESP32 WiFi Manager
 * 
 * Copy ESPWiFiManager.h ke folder sketch ini,
 * atau install sebagai library Arduino.
 */

#include <ESPWiFiManager.h>

ESPWiFiManager wifi;

void setup() {
    Serial.begin(115200);

    wifi.begin();  // auto connect atau AP mode

    if (wifi.isConnected()) {
        Serial.print("IP: ");
        Serial.println(wifi.getIP());
    }
}

void loop() {
    wifi.loop();  // wajib untuk captive portal

    // kode kamu di sini
}
