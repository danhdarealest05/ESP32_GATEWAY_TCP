#include <Arduino.h>
#include <WiFi.h>
#include "ModbusIP_ESP8266.h"
// #include <web_config.h>

const char *ssid = "MLTECH_SHOP";
const char *password = "mltech@2019";
const uint16_t port = 502;
const char *host = "192.168.1.141";

#define MODBUS_RX_PIN 16
#define MODBUS_TX_PIN 17

WiFiServer wifiServer(502);
ModbusIP mb;

void appInit();
void appRun();

void appInit()
{
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8E1, MODBUS_RX_PIN, MODBUS_TX_PIN);
    // managerInit();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting ...");
    }
    Serial.println("Connected !!!");
    Serial.print("Connect with IP: ");
    Serial.println(WiFi.localIP());

    mb.server();
    mb.addCoil(0, false, 10);
}

void appRun()
{
    WiFiClient client;
    mb.task();

    if (client.connect(host, port))
    {

        // if (result == node.ku8MBSuccess)
        // {
        //     for (j = 0; j < 6; j++)
        //     {
        //         data[j] = node.getResponseBuffer(j);
        //         client.print(data);
        //     }
        // }
    }
    Serial.println("Connection to host failed");
    delay(1000);
    // Serial.println("Connected to server successful!");
    // client.print("Hello from ESP32!");

    // Serial.println("Disconnecting...");
    client.stop();
    delay(10000);
}