#include <clientHandler.h>
#include <sender.h>
#include <Arduino.h>
#include <map>

ClientHandler::ClientHandler(WiFiClient &client) : client(client) {}

void ClientHandler::handle(struct power_data *data) {
    auto headers = consumeHeaders();

    Sender* sender;
    if (headers["Accept"].indexOf("json") != -1) {
        sender = new JsonSender();
    } else {
        sender = new PrometheusSender();
    }

    sender->send(data, this->client);

    client.flush();
    client.stop();
}

std::map<String, String> ClientHandler::consumeHeaders() {
    String contentType;
    std::map<String, String> headers;

    const auto headerDelimiter = ": ";
    for (;;) {
        auto hdr = client.readStringUntil('\n');
        hdr.trim();

        if (hdr.length() == 0) {
            break;
        }

        auto dPos = hdr.indexOf(headerDelimiter);
        if (dPos == -1) {
            continue;
        }

        headers[hdr.substring(0, dPos)] = hdr.substring(dPos + strlen(headerDelimiter));
    }

    return headers;
}
