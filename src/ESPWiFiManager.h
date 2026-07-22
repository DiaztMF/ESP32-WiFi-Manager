#ifndef ESPWiFiManager_h
#define ESPWiFiManager_h

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

class ESPWiFiManager {
public:
    ESPWiFiManager()
        : _server(80), _mode(MODE_AP), _apTimedOut(false), _apStartTime(0) {}

    void begin(const char* apSSID = "ESP32-Config", const char* apPassword = "") {
        _apSSID = apSSID;
        _apPassword = apPassword;
        _instance = this;

        _loadConfig();

        if (strlen(_ssid) > 0) {
            Serial.print("[WiFi] Connecting to: ");
            Serial.println(_ssid);
            if (_connect()) {
                _mode = MODE_STA;
                Serial.print("[WiFi] Connected! IP: ");
                Serial.println(WiFi.localIP());
                return;
            }
            Serial.println("[WiFi] Failed, starting AP mode");
        } else {
            Serial.println("[WiFi] No saved config, starting AP mode");
        }

        _startAP();
    }

    void loop() {
        if (_mode == MODE_AP) {
            _dns.processNextRequest();
            _server.handleClient();

            if (!_apTimedOut && (millis() - _apStartTime > 300000)) {
                _apTimedOut = true;
                ESP.restart();
            }
        }
    }

    bool isConnected() { return _mode == MODE_STA && WiFi.status() == WL_CONNECTED; }

    IPAddress getIP() {
        return _mode == MODE_STA ? WiFi.localIP() : WiFi.softAPIP();
    }

    void resetConfig() {
        _clearConfig();
        ESP.restart();
    }

private:
    enum Mode { MODE_AP, MODE_STA };

    Mode _mode;
    char _ssid[64] = "";
    char _pass[64] = "";
    const char* _apSSID;
    const char* _apPassword;
    WebServer _server;
    DNSServer _dns;
    unsigned long _apStartTime;
    bool _apTimedOut;

    static ESPWiFiManager* _instance;

    static void _handleRootStatic() { _instance->_handleRoot(); }
    static void _handleConnectStatic() { _instance->_handleConnect(); }
    static void _handleNotFoundStatic() { _instance->_handleNotFound(); }

    void _loadConfig() {
        Preferences prefs;
        prefs.begin("wifi-config", true);
        String s = prefs.getString("ssid", "");
        String p = prefs.getString("pass", "");
        s.toCharArray(_ssid, sizeof(_ssid));
        p.toCharArray(_pass, sizeof(_pass));
        prefs.end();
    }

    void _saveConfig(const char* ssid, const char* pass) {
        Preferences prefs;
        prefs.begin("wifi-config", false);
        prefs.putString("ssid", ssid);
        prefs.putString("pass", pass);
        prefs.end();
    }

    void _clearConfig() {
        Preferences prefs;
        prefs.begin("wifi-config", false);
        prefs.clear();
        prefs.end();
    }

    bool _connect() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _pass);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 40) {
            delay(500);
            attempts++;
        }

        return WiFi.status() == WL_CONNECTED;
    }

    void _startAP() {
        _mode = MODE_AP;
        WiFi.mode(WIFI_AP);
        WiFi.softAP(_apSSID, _apPassword);

        IPAddress ip = WiFi.softAPIP();
        Serial.println("[WiFi] AP mode active");
        Serial.print("[WiFi] SSID: ");
        Serial.println(_apSSID);
        Serial.print("[WiFi] IP: ");
        Serial.println(ip);

        _dns.start(53, "*", ip);

        _server.on("/", _handleRootStatic);
        _server.on("/connect", HTTP_POST, _handleConnectStatic);
        _server.onNotFound(_handleNotFoundStatic);
        _server.begin();

        _apStartTime = millis();
        _apTimedOut = false;
    }

    String _buildConfigPage() {
        int n = WiFi.scanNetworks();
        String options = "";
        for (int i = 0; i < n; i++) {
            String ssid = WiFi.SSID(i);
            options += "<option value=\"" + ssid + "\">" + ssid + "</option>";
        }

        return R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Config WiFi</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif;background:#f5f5f5;display:flex;justify-content:center;align-items:center;min-height:100vh;padding:16px}
.card{background:#fff;border-radius:16px;padding:32px;width:100%;max-width:400px;box-shadow:0 4px 24px rgba(0,0,0,.1)}
h1{font-size:22px;margin-bottom:8px;color:#1a1a1a}
p{font-size:14px;color:#666;margin-bottom:24px}
label{display:block;font-size:14px;font-weight:600;margin-bottom:6px;color:#333}
select,input{width:100%;padding:12px;border:2px solid#ddd;border-radius:10px;font-size:15px;margin-bottom:20px;background:#fafafa}
select:focus,input:focus{outline:none;border-color:#2563eb}
button{width:100%;padding:14px;background:#2563eb;color:#fff;border:none;border-radius:10px;font-size:16px;font-weight:600;cursor:pointer}
button:hover{background:#1d4ed8}
.status{margin-top:16px;padding:12px;border-radius:10px;text-align:center;font-size:14px;display:none}
.status.error{display:block;background:#fee2e2;color:#dc2626}
.status.ok{display:block;background:#dcfce7;color:#16a34a}
</style>
</head>
<body>
<div class="card">
<h1>Konfigurasi WiFi</h1>
<p>Pilih jaringan dan masukkan password</p>
<form id="form">
<label for="ssid">Jaringan</label>
<select id="ssid" name="ssid" required>
<option value="">-- Pilih --</option>
)rawliteral" + options + R"rawliteral(
</select>
<label for="pass">Password</label>
<input type="password" id="pass" name="pass" placeholder="Password">
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
const r=await fetch('/connect',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'ssid='+encodeURIComponent(document.getElementById('ssid').value)+'&pass='+encodeURIComponent(document.getElementById('pass').value)})
const t=await r.text()
if(r.ok){
s.className='status ok'
s.textContent='Berhasil! ESP32 restart...'
setTimeout(()=>{document.body.innerHTML='<h2>Tersambung</h2><p>ESP32 restart...</p>'},1500)
}else{s.className='status error';s.textContent='Gagal: '+t}
}catch(e){s.className='status error';s.textContent='Gagal terhubung'}
}
</script>
</body>
</html>
)rawliteral";
    }

    void _handleRoot() {
        _server.send(200, "text/html", _buildConfigPage());
    }

    void _handleConnect() {
        if (!_server.hasArg("ssid") || _server.arg("ssid").length() == 0) {
            _server.send(400, "text/plain", "SSID wajib diisi");
            return;
        }

        String ssid = _server.arg("ssid");
        String pass = _server.arg("pass");

        _saveConfig(ssid.c_str(), pass.c_str());
        _server.send(200, "text/plain", "OK");
        delay(500);
        ESP.restart();
    }

    void _handleNotFound() {
        _server.send(200, "text/html", _buildConfigPage());
    }
};

ESPWiFiManager* ESPWiFiManager::_instance = nullptr;

#endif
