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
