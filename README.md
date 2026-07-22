# ESP32 WiFi Manager

Single-header library for changing ESP32 WiFi SSID and password from a phone via captive portal. No re-flash, no extra apps.

## Usage

Copy `ESPWiFiManager.h` into your sketch folder:

```cpp
#include "ESPWiFiManager.h"

ESPWiFiManager wifi;

void setup() {
    Serial.begin(115200);
    wifi.begin();           // auto connect, fallback to AP mode
}

void loop() {
    wifi.loop();            // required for captive portal
    // your code here
}
```

Upload → phone connects to `ESP32-Config` → "Sign in to WiFi" notification appears → enter SSID/password.

## API

| Method | Description |
|--------|-------------|
| `begin(apSSID, apPassword)` | Start — auto connect or become a hotspot |
| `loop()` | Must be called in loop() for captive portal |
| `isConnected()` | `true` when connected to WiFi |
| `getIP()` | Current IP address |
| `resetConfig()` | Erase WiFi config and restart |

## Changing WiFi

Hold **GPIO0 (BOOT)** for 3 seconds → config cleared → ESP32 restarts to AP mode → repeat setup from phone.

## File Structure

```
ESPWiFiManager.h          # Library — single file, just include it
examples/
├── basic/basic.ino       # Minimal usage
└── sensor-dht/sensor-dht.ino  # DHT11 integration example
```

## Tech Stack

ESP32 Arduino Core — all built-in libraries (`WiFi.h`, `WebServer.h`, `DNSServer.h`, `Preferences.h`).

## License

MIT — free to use, modify, and distribute.
