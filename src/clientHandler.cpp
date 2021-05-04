#include <clientHandler.h>
#include <Arduino.h>

ClientHandler::ClientHandler(WiFiClient &client) : client(client) {}

void ClientHandler::handle(struct power_data *data) {
    consumeRequest();

    client.println("HTTP/1.0 200 Oki-Doki");
    client.println("Connection: close");
    client.println("Server: RawrHTTP on Wemos D1");
    client.println("Content-Type: application/json");
    client.println("");
    client.printf(
            R"({"current": %.4lf, "power": %.4lf, "voltage": %.1lf, "voltage_source": "%s"})",
            data->current, data->power, data->voltage, data->voltage_source
    );
    client.flush();
    client.stop();
}

void ClientHandler::consumeRequest() {
    while (hasRequest()) {
        client.read();
    }
}

bool ClientHandler::hasRequest() {
    if (client.available())
        return true;
    delay(100);
    return client.available();
}
