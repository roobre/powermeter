#pragma once

#include <Client.h>
#include <map>

struct power_data {
    double current;
    double voltage;
    double power;
    const char* voltage_source;
};

// Answers requests from clients
class ClientHandler {
public:
    explicit ClientHandler(Client& client);
    void handle(struct power_data* data);

private:
    std::map<String, String> consumeHeaders();
    Client& client;
};
