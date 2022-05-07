#pragma once

#include <httpHandler.h>
#include <Client.h>

class Sender {
public:
    virtual void send(struct power_data*) = 0;
    virtual ~Sender();
};

class PrometheusSender : public Sender {
public:
    explicit PrometheusSender(Client& _client);
    void send(struct power_data*) override;

private:
    Client& client;
};

class JsonSender : public Sender{
public:
    explicit JsonSender(Client& _client);
    void send(struct power_data*) override;

private:
    Client& client;
};
