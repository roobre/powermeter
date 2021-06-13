#pragma once

#include <clientHandler.h>
#include <ESP8266WiFi.h>

class Sender {
public:
    virtual void send(struct power_data*, WiFiClient&) = 0;
};

class PrometheusSender : public Sender {
    void send(struct power_data*, WiFiClient&) override;
};

class JsonSender : public Sender{
    void send(struct power_data*, WiFiClient&) override;
};
