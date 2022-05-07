#include <Arduino.h>
#include <Client.h>
#include <ESP8266WiFi.h>
#include <movingAverage.h>
#include <httpHandler.h>
#include <currentSampler.h>
#include <hassSender.h>

#ifndef WLAN_SSID
#define WLAN_SSID ""
#endif

#ifndef WLAN_PASSPHRASE
#define WLAN_PASSPHRASE ""
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER "mqtt.localdomain"
#endif
#ifndef MQTT_USER
#define MQTT_USER ""
#endif
#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD ""
#endif

const double config_voltage = 230.0;

const int onboard_led_pin = D4;
const int busy_led_pin = D2;

void busy(bool busy);

void wlan_loop(WiFiServer& server);

WiFiServer http(80);
MovingAverage currentAvg(64);
CurrentSampler currentSampler(A0, 15);

// Other senders are stateless and instantiated by Handler, but HASSSender is
// stateful so we manage it here.
WiFiClient client;
HASSSender hass(client);

void setup() {
    Serial.begin(115200);
    Serial.println("Booting up");

    pinMode(A0, INPUT); // ADC
    pinMode(onboard_led_pin, OUTPUT);
    pinMode(busy_led_pin, OUTPUT);

    digitalWrite(onboard_led_pin, HIGH); // Active on low
}

void loop() {
    wlan_loop(http);

    if (currentSampler.ready()) {
        auto sample = currentSampler.sample();
        currentAvg.sample(sample.value);
    }

    struct power_data data = {
            .current = currentAvg.average(),
            .voltage = config_voltage,
            .power = 0,
            .voltage_source = "config",
    };
    data.power = data.current * data.voltage;

    hass.send(&data);

    if (http.hasClient()) {
        busy(true);

        WiFiClient httpClient = http.available();
        ClientHandler handler(httpClient);

        handler.handle(&data);

        busy(false);
    }
}

void wlan_loop(WiFiServer& server) {
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

    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    hass.uid(mac, WL_MAC_ADDR_LENGTH);
    hass.begin(MQTT_SERVER, MQTT_USER, MQTT_PASSWORD);

    busy(false);
}

void inline busy(bool busy) {
    digitalWrite(busy_led_pin, busy);
}
