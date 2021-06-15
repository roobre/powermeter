#include <sender.h>
#include <ESP8266WiFi.h>

Sender::~Sender() {}

void PrometheusSender::send(struct power_data *data, WiFiClient &client) {
    client.println("HTTP/1.0 200 Oki-Doki");
    client.println("Connection: close");
    client.println("Server: RawrHTTP on Wemos D1");
    client.println("Content-Type: text/plain; version=0.0.4");
    client.println("");

    client.print("# HELP pm_current Instant current draw, averaged.\n");
    client.print("# TYPE pm_current gauge\n");
    client.printf("pm_current %.4lf\n", data->current);

    client.print("# HELP pm_voltage Instant voltage.\n");
    client.print("# TYPE pm_voltage gauge\n");
    client.printf("pm_voltage{source=\"%s\"} %.4lf\n", data->voltage_source, data->voltage);

    client.print("# HELP pm_power Instant power draw, averaged.\n");
    client.print("# TYPE pm_power gauge\n");
    client.printf("pm_power %.4lf\n", data->power);
}

void JsonSender::send(struct power_data *data, WiFiClient &client) {
    client.println("HTTP/1.0 200 Oki-Doki");
    client.println("Connection: close");
    client.println("Server: RawrHTTP on Wemos D1");
    client.println("Content-Type: application/json");
    client.println("");

    client.printf(
            R"({"current": %.4lf, "power": %.4lf, "voltage": %.1lf, "voltage_source": "%s"})",
            data->current, data->power, data->voltage, data->voltage_source
    );
}
