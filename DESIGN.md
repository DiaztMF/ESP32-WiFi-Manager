# ESP32 WiFi Manager — Captive Portal

Ganti SSID & password WiFi ESP32 lewat HP tanpa re-flash Arduino IDE.

## Mode Operasi

| Mode | Kondisi | Fungsi |
|------|---------|--------|
| AP (Config) | WiFi belum diset / gagal konek | Hotspot `ESP32-Config`, web server di `192.168.4.1` |
| STA (Normal) | Berhasil konek WiFi | Mode normal, WiFi config tersimpan |

Transisi: AP → user submit form → simpan NVS → restart → STA. STA gagal → balik AP.

## Arsitektur

```
17-07-2026-WiFi-Manager/
├── 17-07-2026-WiFi-Manager.ino   # setup/loop, state machine
├── config.h                        # Pin, timeout, AP SSID, dll
├── webserver.h                     # Web server, handler, halaman config
└── storage.h                       # Baca/tulis NVS via Preferences
```

## Komponen Detail

### config.h
- `AP_SSID` = `"ESP32-Config"`
- `AP_TIMEOUT` = 300 detik (AP mati otomatis kalau gak ada interaksi)
- Pin opsional: `BUTTON_PIN` untuk force AP mode

### storage.h
Pakai `Preferences.h` (built-in ESP32).
- Namespace: `"wifi-config"`
- Key: `"ssid"` (string), `"pass"` (string)
- Fungsi: `loadConfig()`, `saveConfig(ssid, pass)`, `clearConfig()`

### webserver.h
Web server di port 80, aktif cuma di AP mode.

Halaman config (HTML/CSS inline):
- Scan WiFi → dropdown SSID
- Input password
- Tombol "Connect"
- Submit POST ke `/connect` → simpan → restart

Halaman status opsional di STA mode:
- Nampilin IP yang didapat
- Info koneksi

### 17-07-2026-WiFi-Manager.ino (state machine)

```
setup():
  1. init Serial
  2. loadConfig() dari NVS
  3. if (ada SSID) → coba connect STA
  4. if (gagal / gak ada SSID) → start AP + web server
  
loop():
  if (AP mode):
    handleClient()
    if (timeout) → restart
  if (STA mode):
    // idle — siap untuk fungsi tambahan
  if (BUTTON_PIN ditekan > 3 detik):
    clearConfig() → restart
```

## Data Flow

```
User connect HP ke WiFi ESP32-Config
        ↓
Captive portal / buka browser → 192.168.4.1
        ↓
Form dengan dropdown SSID + input password
        ↓
Submit POST /connect
        ↓
Simpan ke NVS
        ↓
Restart ESP32
        ↓
STA mode — connect ke WiFi baru
  ↙        ↘
berhasil     gagal
  ↓           ↓
normal      balik AP mode
```

## Edge Cases & Error Handling

- **Password salah** → STA gagal → balik AP, config lama dihapus
- **ESP32 lupa config** → tombol reset (hold 3s) → clear NVS → AP mode
- **AP gak ada interaksi** → AP mati setelah 5 menit, restart ulang
- **SSID kosong / submit gak lengkap** → validasi di HTTP handler, tolak

## Success Criteria

- [ ] HP connect ke `ESP32-Config`, muncul halaman form
- [ ] SSID ter-scan & tampil di dropdown
- [ ] Submit SSID/PW → ESP32 restart → connect ke WiFi
- [ ] Ganti WiFi lagi via AP mode kalau perlu
- [ ] Tombol reset config
