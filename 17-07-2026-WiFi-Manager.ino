/*
 * ESP32 WiFi Manager — Demo
 * 
 * Cara pakai:
 *   1. Upload sketch ini ke ESP32
 *   2. Buka WiFi HP, connect ke "ESP32-Config"
 *   3. Notifikasi "Sign in to WiFi" muncul — tap
 *   4. Pilih WiFi rumah, isi password, submit
 *   5. ESP32 restart dan connect ke WiFi
 * 
 * Ganti WiFi lain: tekan BOOT 3 detik
 */

#include "ESPWiFiManager.h"

ESPWiFiManager wifi;

void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    wifi.begin("ESP32-Config", "");

    Serial.print("Status: ");
    Serial.println(wifi.isConnected() ? "Connected" : "AP Mode");
    Serial.print("IP: ");
    Serial.println(wifi.getIP());
}

void loop() {
    wifi.loop();

    if (wifi.isConnected()) {
        digitalWrite(2, HIGH);
    }
}
