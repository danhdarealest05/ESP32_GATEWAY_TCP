#include <Arduino.h>
#include <web_config.h>
#include <WiFi.h>

// const char *ssid = "MLTECH_SHOP";
// const char *password = "mltech@2019";
// IPAddress staticIP(192, 168, 1, 141);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);

WiFiServer wifiServer(502);

void appInit();
void appRun();

void appInit()
{
    Serial.begin(9600);
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting ...");
    }
    // managerInit();
    Serial.println("Connected !!!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    wifiServer.begin();
}

void appRun()
{
    String data = "";
    WiFiClient client = wifiServer.available();
    if (client)
    {
        while (client.connected())
        {
            while (client.available() > 0)
            {
                char c = client.read(); // lệnh đọc tín hiệu
                // client.write(c); lệnh gửi lại
                data += c;
                delay(5);
            }
            delay(10);
            if (data != "")
            {
                Serial.println(data);
            }
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}