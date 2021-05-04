#pragma once

#include <ESP8266WiFi.h>

struct power_data {
    double current;
    double voltage;
    double power;
    const char* voltage_source;
};

// Answers requests from clients
class ClientHandler {
public:
    explicit ClientHandler(WiFiClient& client);
    void handle(struct power_data* data);

private:
    void consumeRequest();
    bool hasRequest();
    WiFiClient& client;
};
