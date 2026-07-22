# ESP32 WiFi Manager — Design

Change ESP32 WiFi SSID & password from a phone without re-flashing via Arduino IDE.

## Operation Modes

| Mode | Condition | Function |
|------|-----------|----------|
| AP (Config) | No WiFi saved / connection failed | Hotspot `ESP32-Config`, web server at `192.168.4.1` |
| STA (Normal) | WiFi connected | Normal mode, config persisted in NVS |

Transition: AP → user submits form → save to NVS → restart → STA. STA fails → back to AP.

## Architecture

```
17-07-2026-WiFi-Manager/
├── ESPWiFiManager.h           # Single-header library
├── 17-07-2026-WiFi-Manager.ino# Demo sketch
└── examples/
    └── ...                     # Usage examples
```

The library is self-contained in `ESPWiFiManager.h`. No external dependencies beyond ESP32 Arduino core.

## State Machine

```
setup():
  1. init Serial
  2. loadConfig() from NVS
  3. if (SSID exists) → try connecting to STA
  4. if (fail / no SSID) → start AP + web server

loop():
  if (AP mode):
    process DNS + HTTP
    if (timeout) → restart
  if (STA mode):
    idle — ready for user code
```

## Data Flow

```
Phone connects to ESP32-Config WiFi
        ↓
Captive portal / browser → 192.168.4.1
        ↓
Form with SSID dropdown + password input
        ↓
Submit POST /connect
        ↓
Save to NVS (Preferences)
        ↓
Restart ESP32
        ↓
STA mode — connect to new WiFi
  ↙        ↘
success     fail
  ↓           ↓
normal     back to AP mode
```

## Edge Cases

- **Wrong password** → STA fails → back to AP, old config cleared
- **Config lost** → hold BOOT 3s → clear NVS → AP mode
- **AP idle** → AP shuts down after 5 minutes, restart
- **Empty SSID** → HTTP validation rejects

## Success Criteria

- [ ] Phone connects to `ESP32-Config`, form page loads
- [ ] WiFi scan populates SSID dropdown
- [ ] Submit SSID/PW → ESP32 restarts → connects to WiFi
- [ ] Changing WiFi via AP mode works
- [ ] Reset button clears config
