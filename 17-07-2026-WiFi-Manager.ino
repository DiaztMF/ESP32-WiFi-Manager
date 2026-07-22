#include "config.h"
#include "storage.h"
#include "portal.h"

enum Mode { MODE_AP, MODE_STA };
Mode currentMode = MODE_AP;

char savedSSID[64] = "";
char savedPass[64] = "";
unsigned long apStartTime = 0;
bool apTimedOut = false;

void onConfigSaved(const char* ssid, const char* pass) {
    saveConfig(ssid, pass);
}

bool connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSSID, savedPass);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
        delay(500);
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        attempts++;
    }
    digitalWrite(LED_PIN, LOW);

    return WiFi.status() == WL_CONNECTED;
}

void startAPMode() {
    currentMode = MODE_AP;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    IPAddress ip = WiFi.softAPIP();

    Serial.println("AP mode aktif");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("IP: ");
    Serial.println(ip);

    startWebServer(onConfigSaved);
    apStartTime = millis();
    apTimedOut = false;
}

void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    digitalWrite(LED_PIN, LOW);

    delay(500);
    Serial.println();
    Serial.println("ESP32 WiFi Manager starting...");

    loadConfig(savedSSID, savedPass, sizeof(savedSSID));

    if (strlen(savedSSID) > 0) {
        Serial.print("Mencoba connect ke: ");
        Serial.println(savedSSID);
        if (connectToWiFi()) {
            currentMode = MODE_STA;
            Serial.println("Berhasil connect!");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            digitalWrite(LED_PIN, HIGH);
        } else {
            Serial.println("Gagal connect, masuk AP mode");
            startAPMode();
        }
    } else {
        Serial.println("Tidak ada config tersimpan, masuk AP mode");
        startAPMode();
    }
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        delay(3000);
        if (digitalRead(BUTTON_PIN) == LOW) {
            Serial.println("Tombol reset ditekan 3 detik — menghapus config");
            clearConfig();
            ESP.restart();
        }
    }

    if (currentMode == MODE_AP) {
        handleClient();

        if (!apTimedOut && (millis() - apStartTime > AP_TIMEOUT_MS)) {
            Serial.println("AP timeout — restart");
            apTimedOut = true;
            ESP.restart();
        }
    }
}
