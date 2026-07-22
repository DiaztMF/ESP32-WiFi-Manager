/*
 * ESP32 WiFi Manager — Demo
 *
 * How to use:
 *   1. Upload this sketch to ESP32
 *   2. Open phone WiFi, connect to "ESP32-Config"
 *   3. "Sign in to WiFi" notification appears — tap it
 *   4. Select your WiFi, enter password, submit
 *   5. ESP32 restarts and connects to your WiFi
 *
 * To change WiFi: hold BOOT button for 3 seconds
 */

#include "ESPWiFiManager.h"

ESPWiFiManager wifi;

void setup() {
    Serial.begin(115200);

    wifi.begin("ESP32-Config", "");

    Serial.print("Status: ");
    Serial.println(wifi.isConnected() ? "Connected" : "AP Mode");
    Serial.print("IP: ");
    Serial.println(wifi.getIP());
}

void loop() {
    wifi.loop();
}
