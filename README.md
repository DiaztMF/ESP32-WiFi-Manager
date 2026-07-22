# ESP32 WiFi Manager

Ganti SSID dan password WiFi ESP32 lewat HP tanpa perlu re-flash dari Arduino IDE.

## Cara Kerja

ESP32 punya dua mode:

- **AP Mode** — ESP32 jadi hotspot `ESP32-Config`. HP connect ke WiFi ini, otomatis muncul notifikasi "Sign in to WiFi". Buka halaman config, pilih jaringan, isi password, submit.
- **STA Mode** — ESP32 connect ke WiFi yang udah disimpan. LED biru nyala tanda berhasil.

Kalau mau ganti WiFi, tekan tombol BOOT (GPIO0) selama 3 detik — config terhapus, ESP32 restart ke AP mode.

## File Structure

```
17-07-2026-WiFi-Manager/
├── 17-07-2026-WiFi-Manager.ino   # State machine: setup/loop
├── config.h                        # Konstanta (SSID AP, pin, timeout)
├── storage.h                       # Baca/tulis config ke NVS
├── portal.h                        # Web server + DNS spoofing + HTML
├── DESIGN.md                       # Dokumen desain
├── PLAN.md                         # Rencana implementasi
└── README.md
```

## Tech Stack

- **ESP32 Arduino Core** — WiFi, WebServer, Preferences, DNSServer
- Semua library **built-in** — tanpa install tambahan

## Cara Upload

1. Buka `17-07-2026-WiFi-Manager.ino` di Arduino IDE
2. **Tools → Board → ESP32 Dev Module**
3. **Tools → Port →** pilih port ESP32
4. Klik **Upload**
5. Buka **Serial Monitor** (115200 baud) untuk lihat log

## Test

| Langkah | Hasil |
|---------|-------|
| HP cari WiFi → `ESP32-Config` | Muncul tanpa password |
| Connect ke `ESP32-Config` | Notifikasi "Sign in to WiFi" muncul otomatis |
| Buka halaman config | Form dengan daftar SSID hasil scan |
| Pilih WiFi, isi password, submit | ESP32 restart |
| Cek Serial Monitor | IP address muncul, LED nyala |
| Tekan BOOT 3 detik | Config terhapus, balik AP mode |

## License

MIT
