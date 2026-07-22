# ESP32 WiFi Manager Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax.

**Goal:** ESP32 dapat ganti SSID/password WiFi lewat HP via captive portal tanpa re-flash.

**Architecture:** State machine dengan 2 mode — AP (config) dan STA (normal). Web server embedded menyajikan form config. Konfigurasi disimpan di NVS Preferences.

**Tech Stack:** Arduino ESP32 core (`WiFi.h`, `WebServer.h`, `Preferences.h`)

## Global Constraints

- File encoding: UTF-8 without BOM
- Bahasa: Indonesia (comments, HTML labels)
- Hanya pakai library bawaan ESP32 — no external dependencies
- HTML/CSS/JS inline dalam string — no SPIFFS
- AP SSID: `ESP32-Config`
- AP IP: `192.168.4.1`
- Port: 80

---

### Task 1: `config.h` — Konstanta Global

**Files:**
- Create: `17-07-2026-WiFi-Manager/config.h`

**Interfaces:**
- Consumes: —
- Produces: `AP_SSID`, `AP_PASSWORD`, `AP_TIMEOUT_MS`, `BUTTON_PIN`, `LED_PIN`, `BAUD_RATE`

- [ ] **Buat config.h**

```cpp
#ifndef CONFIG_H
#define CONFIG_H

#define AP_SSID         "ESP32-Config"
#define AP_PASSWORD     ""
#define AP_TIMEOUT_MS   300000  // 5 menit
#define BUTTON_PIN      0       // GPIO0 (boot button)
#define LED_PIN         2       // GPIO2 (built-in LED)
#define BAUD_RATE       115200

#endif
```

---

### Task 2: `storage.h` — NVS Preferences

**Files:**
- Create: `17-07-2026-WiFi-Manager/storage.h`

**Interfaces:**
- Consumes: `config.h`
- Produces: `loadConfig(ssid, pass, len)`, `saveConfig(ssid, pass)`, `clearConfig()`

- [ ] **Buat storage.h**

```cpp
#ifndef STORAGE_H
#define STORAGE_H

#include <Preferences.h>

Preferences prefs;

void loadConfig(char* ssid, char* pass, size_t len) {
    prefs.begin("wifi-config", true);
    String s = prefs.getString("ssid", "");
    String p = prefs.getString("pass", "");
    s.toCharArray(ssid, len);
    p.toCharArray(pass, len);
    prefs.end();
}

void saveConfig(const char* ssid, const char* pass) {
    prefs.begin("wifi-config", false);
    prefs.putString("ssid", ssid);
    prefs.putString("pass", pass);
    prefs.end();
}

void clearConfig() {
    prefs.begin("wifi-config", false);
    prefs.clear();
    prefs.end();
}

#endif
```

---

### Task 3: `webserver.h` — Web Server + Halaman Config

**Files:**
- Create: `17-07-2026-WiFi-Manager/webserver.h`

**Interfaces:**
- Consumes: `config.h`, `storage.h`. Callback: `onSave(ssid, pass)`
- Produces: `startWebServer()`, `handleClient()`, `stopWebServer()`

- [ ] **Buat webserver.h**

```cpp
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <WebServer.h>
#include <WiFi.h>

WebServer server(80);

void (*saveCallback)(const char*, const char*);

void handleRoot() {
    int n = WiFi.scanNetworks();
    String options = "";
    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);
        String signal = (rssi > -67) ? "📶" : (rssi > -80) ? "📡" : "📡";
        options += "<option value=\"" + ssid + "\">" + signal + " " + ssid + "</option>";
    }

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Config WiFi ESP32</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif;background:#f5f5f5;display:flex;justify-content:center;align-items:center;min-height:100vh;padding:16px}
.card{background:#fff;border-radius:16px;padding:32px;width:100%;max-width:400px;box-shadow:0 4px 24px rgba(0,0,0,.1)}
h1{font-size:22px;margin-bottom:8px;color:#1a1a1a}
p{font-size:14px;color:#666;margin-bottom:24px}
label{display:block;font-size:14px;font-weight:600;margin-bottom:6px;color:#333}
select,input{width:100%;padding:12px;border:2px solid#e0e0e0;border-radius:10px;font-size:15px;margin-bottom:20px;background:#fafafa;transition:border-color .2s}
select:focus,input:focus{outline:none;border-color:#2563eb;background:#fff}
button{width:100%;padding:14px;background:#2563eb;color:#fff;border:none;border-radius:10px;font-size:16px;font-weight:600;cursor:pointer;transition:background .2s}
button:hover{background:#1d4ed8}
.status{margin-top:16px;padding:12px;border-radius:10px;text-align:center;font-size:14px;display:none}
.status.error{display:block;background:#fee2e2;color:#dc2626}
.status.ok{display:block;background:#dcfce7;color:#16a34a}
</style>
</head>
<body>
<div class="card">
<h1>⬡ Konfigurasi WiFi</h1>
<p>Pilih jaringan WiFi dan masukkan password</p>
<form id="form">
<label for="ssid">Jaringan WiFi</label>
<select id="ssid" name="ssid" required>
<option value="">-- Pilih jaringan --</option>
)rawliteral";
    html += options;
    html += R"rawliteral(
</select>
<label for="pass">Password</label>
<input type="password" id="pass" name="pass" placeholder="Masukkan password">
<button type="submit">Hubungkan</button>
</form>
<div id="status" class="status"></div>
</div>
<script>
document.getElementById('form').onsubmit=async function(e){
e.preventDefault()
const s=document.getElementById('status')
s.className='status'
s.textContent='Menghubungkan...'
s.style.display='block'
try{
const r=await fetch('/connect',{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:'ssid='+encodeURIComponent(document.getElementById('ssid').value)+'&pass='+encodeURIComponent(document.getElementById('pass').value)
})
const t=await r.text()
if(r.ok){
s.className='status ok'
s.textContent='✓ Berhasil! ESP32 akan restart...'
setTimeout(()=>{document.body.innerHTML='<div style="text-align:center;padding:40px;font-family:sans-serif"><h2>✓ Tersambung</h2><p>ESP32 sedang restart...</p></div>'},1500)
}else{
s.className='status error'
s.textContent='✗ '+t
}
}catch(e){
s.className='status error'
s.textContent='✗ Gagal terhubung ke ESP32'
}
}
</script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

void handleConnect() {
    if (!server.hasArg("ssid") || server.arg("ssid").length() == 0) {
        server.send(400, "text/plain", "SSID tidak boleh kosong");
        return;
    }

    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    saveCallback(ssid.c_str(), pass.c_str());

    server.send(200, "text/plain", "OK");
    delay(500);
    ESP.restart();
}

void startWebServer(void (*cb)(const char*, const char*)) {
    saveCallback = cb;
    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.begin();
}

void handleClient() {
    server.handleClient();
}

void stopWebServer() {
    server.stop();
}

#endif
```

---

### Task 4: `17-07-2026-WiFi-Manager.ino` — State Machine Utama

**Files:**
- Create: `17-07-2026-WiFi-Manager/17-07-2026-WiFi-Manager.ino`

**Interfaces:**
- Consumes: `config.h`, `storage.h`, `webserver.h`
- Produces: Complete ESP32 firmware

- [ ] **Buat main sketch**

```cpp
#include "config.h"
#include "storage.h"
#include "webserver.h"

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
```

- [ ] **Verify sketch compiles**

Buka di Arduino IDE, pilih board ESP32, klik Verify (✓). Pastikan tidak ada error.

---

### Task 5: Upload & Test

**Files:** None (flashing hardware)

- [ ] **Upload ke ESP32**

Arduino IDE → pilih port → Upload (→).

- [ ] **Test AP mode**

1. Buka WiFi HP — cari `ESP32-Config`, connect (tanpa password)
2. Buka browser → `192.168.4.1` — halaman form muncul
3. Pastikan daftar SSID terisi

- [ ] **Test connect**

1. Pilih WiFi rumah, isi password, klik Hubungkan
2. ESP32 restart — LED menyala (indikasi STA mode)
3. Cek Serial monitor untuk IP yang didapat

- [ ] **Test ganti WiFi**

1. Tekan tombol BOOT (GPIO0) selama 3 detik
2. Config terhapus, ESP32 restart ke AP mode
3. Ulang test AP mode & connect dengan WiFi berbeda
