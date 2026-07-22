# ESP32 WiFi Manager

Single-header library untuk ganti SSID dan password WiFi ESP32 lewat HP via captive portal. Tanpa re-flash, tanpa aplikasi tambahan.

## Cara Pakai

Copy `ESPWiFiManager.h` ke folder sketch kamu:

```cpp
#include "ESPWiFiManager.h"

ESPWiFiManager wifi;

void setup() {
    Serial.begin(115200);
    wifi.begin();           // auto connect, fallback ke AP mode
}

void loop() {
    wifi.loop();            // wajib untuk captive portal
    // kode kamu di sini
}
```

Upload → HP connect ke `ESP32-Config` → notifikasi "Sign in to WiFi" muncul → isi SSID/password.

## API

| Method | Fungsi |
|--------|--------|
| `begin(apSSID, apPassword)` | Mulai — auto connect atau jadi hotspot |
| `loop()` | Wajib dipanggil di loop() untuk captive portal |
| `isConnected()` | `true` kalau udah connect ke WiFi |
| `getIP()` | IP address saat ini |
| `resetConfig()` | Hapus config WiFi dan restart |

## Cara Ganti WiFi

Tekan **GPIO0 (BOOT)** selama 3 detik → config terhapus → ESP32 restart ke AP mode → ulang proses dari HP.

## File Structure

```
ESPWiFiManager.h          # Library — satu file, include aja
examples/
├── basic/basic.ino       # Minimal usage
└── sensor-dht/sensor-dht.ino  # Contoh dengan DHT11
```

## Tech Stack

ESP32 Arduino Core — semua library built-in (`WiFi.h`, `WebServer.h`, `DNSServer.h`, `Preferences.h`).

## Lisensi

MIT — bebas dipake, diubah, dan disebarin.
