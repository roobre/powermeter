#include <hassSender.h>
#include <WiFiClient.h>

HASSSender::HASSSender(WiFiClient &client) :
        device(),
        mqtt(client, device),
        current("current"),
        voltage("voltage"),
        power("power") {
    device.setName("Power Meter");
    device.setManufacturer("Roobre");
    device.setSoftwareVersion(__DATE__);

    current.setUnitOfMeasurement("A");
    current.setDeviceClass("current");
    current.setIcon("mdi:current-ac");
    current.setName("Current");

    voltage.setUnitOfMeasurement("V");
    voltage.setDeviceClass("voltage");
    voltage.setIcon("mdi:current-ac");
    voltage.setName("Voltage");

    power.setUnitOfMeasurement("W");
    power.setDeviceClass("power");
    power.setIcon("mdi:gauge");
    power.setName("Instant Power");
}

void HASSSender::uid(uint8_t* id, size_t len) {
    device.setUniqueId(id, len);
#ifdef PM_DEBUG_SERIAL
    Serial.printf("Setting UID to '%s'\n", device.getUniqueId());
#endif
}

void HASSSender::begin(const char *mqttBroker) {
    mqtt.begin(mqttBroker);
}

void HASSSender::begin(const char *mqttBroker, const char *username, const char *password) {
    mqtt.begin(mqttBroker, username, password);
}

void HASSSender::send(struct power_data *pdata) {
    mqtt.loop();

    if (!shouldSend()) {
        return;
    }

#ifdef PM_DEBUG_SERIAL
    Serial.println("Sending update to HASS...");
#endif

    bool sent = true;
    sent &= current.setValue(pdata->current);
    sent &= voltage.setValue(pdata->voltage);
    sent &= power.setValue(pdata->power, 0);

#ifdef PM_DEBUG_SERIAL
    if (!sent) {
        Serial.println("Error sending updates to HASS");
    } else {
        Serial.println("Updates sent");
    }
#endif

    lastSent = millis();
}

bool HASSSender::shouldSend() const {
    return lastSent == 0 || millis() > (lastSent + interval_sec * 1000);
}

void HASSSender::interval(long _interval_sec) {
    interval_sec = _interval_sec;
}
