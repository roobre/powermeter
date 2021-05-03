#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EmonLib.h>

#ifndef WLAN_SSID
#define WLAN_SSID ""
#endif

#ifndef WLAN_PASSPHRASE
#define WLAN_PASSPHRASE ""
#endif

bool has_something_to_say(WiFiClient& c);

WiFiServer http(80);
EnergyMonitor emon;

const double voltage = 230.0;

void reconnect() {
    switch (WiFi.status()) {
        case WL_CONNECTED:
        case WL_IDLE_STATUS:
            return;
        default:
            break;
    }

    Serial.printf("Caught in status %d, reconnecting\n", WiFi.status());

    WiFi.begin(WLAN_SSID, WLAN_PASSPHRASE);

    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    http.begin();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Booting up");
    pinMode(A0, INPUT);
    emon.current(A0, 30);
}

void loop() {
    static unsigned long last_read = 0;
    static unsigned long sampling_time = 0;
    static double irms = 0;

    reconnect();

    if (millis() - last_read > 750) {
        last_read = millis();
        sampling_time = millis();
        irms = emon.calcIrms(1480);
        sampling_time = millis() - sampling_time;
    }


    if (http.hasClient()) {
        WiFiClient client = http.available();
        while (has_something_to_say(client)) {
            client.read();
        }
        client.println("HTTP/1.0 200 Oki-Doki");
        client.println("Connection: close");
        client.println("Server: RawrHTTP on Wemos D1");
        client.println("Content-Type: application/json");
        client.println("");
        client.printf(R"({"current": %.4lf, "power": %.4lf, "voltage": %.4lf, "voltage_source": "config", "sampling_period": %ld})", irms, irms * voltage, voltage, sampling_time);
        client.flush();
        client.stop();
    }
}

bool has_something_to_say(WiFiClient& c) {
    if (c.available())
        return true;
    delay(100);
    return c.available();
}
