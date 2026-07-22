# ESP32 WiFi Manager

Single-header library for changing ESP32 WiFi SSID and password from a phone via captive portal. No re-flash, no extra apps.

## Installation

### Arduino IDE (ZIP)

1. Download the [latest release](https://github.com/DiaztMF/ESP32-WiFi-Manager/releases) ZIP
2. Arduino IDE → `Sketch → Include Library → Add .ZIP Library...`
3. Select the downloaded ZIP

### Manual (copy file)

Copy `src/ESPWiFiManager.h` into your sketch folder.

## Usage

```cpp
#include <ESPWiFiManager.h>

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
ESP32-WiFi-Manager/
├── library.properties      # Arduino library metadata
├── src/
│   └── ESPWiFiManager.h    # Library — single header
├── examples/
│   ├── basic/basic.ino     # Minimal usage
│   └── sensor-dht/sensor-dht.ino  # DHT11 integration
├── keywords.txt            # Arduino IDE syntax highlighting
└── README.md
```

## Tech Stack

ESP32 Arduino Core — all built-in libraries (`WiFi.h`, `WebServer.h`, `DNSServer.h`, `Preferences.h`).

## License

MIT — free to use, modify, and distribute.
