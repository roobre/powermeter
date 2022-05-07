#pragma once

#include <sender.h>
#include <ArduinoHA.h>
#include <WiFiClient.h>

class HASSSender : Sender {
public:
    HASSSender(WiFiClient &client);
    void begin(const char* mqttBroker);
    void begin(const char* mqttBroker, const char* username, const char* password);
    void send(struct power_data*) override;
    void uid(uint8_t* id, size_t len);
    void interval(long interval_sec);

private:
    bool shouldSend() const;

    HADevice device;
    HAMqtt mqtt;
    HASensor current;
    HASensor voltage;
    HASensor power;
    uint64_t lastSent = 0;
    unsigned long interval_sec = 30;
};
