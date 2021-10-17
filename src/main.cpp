#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <movingAverage.h>
#include <clientHandler.h>
#include <currentSampler.h>

#ifndef WLAN_SSID
#define WLAN_SSID ""
#endif

#ifndef WLAN_PASSPHRASE
#define WLAN_PASSPHRASE ""
#endif


const double voltage = 230.0;

const int onboard_led_pin = D4;
const int busy_led_pin = D2;

void busy(bool busy);

void wlan_reconnect(WiFiServer& server);

void setup() {
    Serial.begin(115200);
    Serial.println("Booting up");
    pinMode(A0, INPUT); // ADC
    pinMode(onboard_led_pin, OUTPUT);
    digitalWrite(onboard_led_pin, HIGH); // Active on low
    pinMode(busy_led_pin, OUTPUT);
    busy(true);
}

void loop() {
    static WiFiServer http(80);
    static MovingAverage currentAvg(64);
    static CurrentSampler currentSampler(A0, 30);

    wlan_reconnect(http);

    if (currentSampler.ready()) {
        auto sample = currentSampler.sample();
        currentAvg.sample(sample.value);
    }

    if (http.hasClient()) {
        busy(true);

        WiFiClient client = http.available();
        ClientHandler handler(client);

        struct power_data data = {
                .current = currentAvg.average(),
                .voltage = voltage,
                .power = 0,
                .voltage_source = "config",
        };
        data.power = data.current * data.voltage;

        handler.handle(&data);

        busy(false);
    }
}

void wlan_reconnect(WiFiServer& server) {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    busy(true);

    Serial.printf("Caught in status %d, reconnecting\n", WiFi.status());

    WiFi.mode(WIFI_STA);
    WiFi.begin(WLAN_SSID, WLAN_PASSPHRASE);

    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();

    busy(false);
}

void busy(bool busy) {
    digitalWrite(busy_led_pin, busy);
}
