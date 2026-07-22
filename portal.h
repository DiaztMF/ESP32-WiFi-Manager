#ifndef PORTAL_H
#define PORTAL_H

#include <WebServer.h>
#include <DNSServer.h>
#include <WiFi.h>

WebServer server(80);
DNSServer dns;

void (*saveCallback)(const char*, const char*);

String buildConfigPage() {
    int n = WiFi.scanNetworks();
    String options = "";
    for (int i = 0; i < n; i++) {
        String ssid = WiFi.SSID(i);
        options += "<option value=\"" + ssid + "\">" + ssid + "</option>";
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
select,input{width:100%;padding:12px;border:2px solid#e0e0e0;border-radius:10px;font-size:15px;margin-bottom:20px;background:#fafafa}
select:focus,input:focus{outline:none;border-color:#2563eb;background:#fff}
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
s.textContent='Berhasil! ESP32 akan restart...'
setTimeout(()=>{document.body.innerHTML='<div style="text-align:center;padding:40px;font-family:sans-serif"><h2>Tersambung</h2><p>ESP32 sedang restart...</p></div>'},1500)
}else{
s.className='status error'
s.textContent='Gagal: '+t
}
}catch(e){
s.className='status error'
s.textContent='Gagal terhubung ke ESP32'
}
}
</script>
</body>
</html>
)rawliteral";
    return html;
}

void handleRoot() {
    server.send(200, "text/html", buildConfigPage());
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

void handleNotFound() {
    server.send(200, "text/html", buildConfigPage());
}

void startWebServer(void (*cb)(const char*, const char*)) {
    saveCallback = cb;

    dns.start(53, "*", WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/connect", HTTP_POST, handleConnect);
    server.onNotFound(handleNotFound);
    server.begin();
}

void handleClient() {
    dns.processNextRequest();
    server.handleClient();
}

#endif
