/*
 * Contoh: WiFi + DHT11 sensor
 * 
 * Copy ESPWiFiManager.h ke folder sketch ini.
 * Install library: Tools → Manage Libraries → "DHT sensor library" by Adafruit
 */

#include "ESPWiFiManager.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

ESPWiFiManager wifi;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();

    wifi.begin();  // auto connect atau AP mode kalau gagal
}

void loop() {
    wifi.loop();

    if (wifi.isConnected()) {
        float t = dht.readTemperature();
        float h = dht.readHumidity();

        Serial.print("Suhu: ");
        Serial.print(t);
        Serial.print("C, Kelembaban: ");
        Serial.print(h);
        Serial.println("%");

        delay(2000);
    }
}
