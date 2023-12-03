#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
AsyncWebServer server(80);

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "pass";
const char *PARAM_INPUT_3 = "ip";
const char *PARAM_INPUT_4 = "gateway";

String ssid;
String pass;
String ip;
String gateway;

// const char* ssidPath = "/ssid.txt";
// const char* passPath = "/pass.txt";
// const char* ipPath = "/ip.txt";
// const char* gatewayPath = "/gateway.txt";
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html><html lang="en"><head><style>body{text-align:center;padding:0;margin:0}h1{text-align:center;text-shadow:#00f;color:#008b8b}button.menu{width:60%;background-color:#4caf50;color:#fff;padding:30px 20px;margin:8px 0;border:none;border-radius:20px;cursor:pointer}p.value{font-size:80% color:cadetblue}</style><meta charset="UTF-8"><meta http-equiv="X-UA-Compatible" content="IE=edge"><meta name="viewport" content="width=device-width,initial-scale=1"><title>cấu hình hệ thống</title></head><body><h1>CẤU HÌNH HỆ THỐNG</h1><div><form action="/ethernet" method="POST"><button class="menu">Thiết lập kết nối mạng</button></form></div><br><div><form action="/update" method="GET"><button class="menu">Cập nhập phần mềm</button></form></div><br><div><form action="/recovery" method="POST"><button class="menu">Khôi phục cài đặt</button></form></div><br><div><form action="/reset" method="POST"><button class="menu">Khởi động lại</button></form></div></body></html>
)rawliteral";
const char ethernet_html[] PROGMEM = R"rawliteral(<!DOCTYPE html><html lang="en"><head> <style>body{text-align: center;}h1{text-align: center; text-shadow: blue; color: darkcyan;}input[type=text], select{width: 60%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 20px; box-sizing: border-box;}button[type=submit]{width: 60%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 20px; cursor: pointer;}</style> <meta charset="UTF-8"> <meta http-equiv="X-UA-Compatible" content="IE=edge"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>cấu hình hệ thống</title></head><body> <h1>CẤU HÌNH WIFI</h1> <form action="/getIP"> <div><input type="text" name="address" size="30" placeholder="IP Address"></div><div><input type="text" name="getway" size="30" placeholder="Getway"></div><div><input type="text" name="subnet" size="30" placeholder="Subnet"></div><div><input type="text" name="p_dns" size="30" placeholder="Primary DNS"></div><div><input type="text" name="s_dns" size="30" placeholder="Secondary DNS"></div><br><div><button type="submit">Save</button></div></form></body></html>
)rawliteral";
const char reset_html[] PROGMEM = R"rawliteral(<!DOCTYPE html><html lang="en"><head> <style>body{text-align: center; padding: 0; margin: 0;}h1{text-align: center; text-shadow: blue; color: darkcyan;}button.menu{width: 60%; background-color: #4CAF50; color: white; padding: 30px 20px; margin: 8px 0px; border: none; border-radius: 20px; cursor: pointer;}</style> <meta charset="UTF-8"> <meta http-equiv="X-UA-Compatible" content="IE=edge"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>cấu hình hệ thống</title></head><body> <h1>RESETED!</h1> <div><a href="/"><button class="menu">Trở về trang chủ</button></div><br></html>
)rawliteral";
const char recovery_html[] PROGMEM = R"rawliteral(<!DOCTYPE html><html lang="en"><head><style>body{text-align:center;padding:0;margin:0}h1{text-align:center;text-shadow:#00f;color:#008b8b}button.menu{width:60%;background-color:#4caf50;color:#fff;padding:30px 20px;margin:8px 0;border:none;border-radius:20px;cursor:pointer}</style><meta charset="UTF-8"><meta http-equiv="X-UA-Compatible" content="IE=edge"><meta name="viewport" content="width=device-width,initial-scale=1"><title>cấu hình hệ thống</title></head><body><h1>Thành công!</h1><div><form action="/reset" method="POST"><button class="menu">Khởi động lại</button></form></div></body></html>
)rawliteral";
const char save_html[] PROGMEM = R"rawliteral(<!DOCTYPE html><html lang="en"><head> <style>body{text-align: center; padding: 0; margin: 0;}h1{text-align: center; text-shadow: blue; color: darkcyan;}button.menu{width: 60%; background-color: #4CAF50; color: white; padding: 30px 20px; margin: 8px 0px; border: none; border-radius: 20px; cursor: pointer;}</style> <meta charset="UTF-8"> <meta http-equiv="X-UA-Compatible" content="IE=edge"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>cấu hình hệ thống</title></head><body> <h1>ĐÃ LƯU</h1> <div><a href="/"><button class="menu">Trở về trang chủ</button></div><br><div><form action="/reset" method="POST"><button class="menu">Khởi động lại</button></form></div><br></html>
)rawliteral";

// IPAddress localIP;
// //IPAddress localIP(192, 168, 1, 200); // hardcoded
const char *ssid = "MLTECH_SHOP";
const char *password = "mltech@2019";
IPAddress staticIP(192, 168, 1, 141);
IPAddress localGateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
// // Set your Gateway IP address
// IPAddress localGateway;
// //IPAddress localGateway(192, 168, 1, 1); //hardcoded
// IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000; // interval to wait for Wi-Fi connection (milliseconds)

// Initialize SPIFFS
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return String();
    }

    String fileContent;
    while (file.available())
    {
        fileContent = file.readStringUntil('\n');
        break;
    }
    return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
}

// Initialize WiFi
bool initWiFi()
{
    if (ssid == "" || ip == "")
    {
        Serial.println("Undefined SSID or IP address.");
        return false;
    }

    WiFi.mode(WIFI_STA);
    staticIP.fromString(ip.c_str());
    localGateway.fromString(gateway.c_str());

    if (!WiFi.config(staticIP, localGateway, subnet))
    {
        Serial.println("STA Failed to configure");
        return false;
    }
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.println("Connecting to WiFi...");

    unsigned long currentMillis = millis();
    previousMillis = currentMillis;

    while (WiFi.status() != WL_CONNECTED)
    {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            Serial.println("Failed to connect.");
            return false;
        }
    }

    Serial.println(WiFi.localIP());
    return true;
}

void setup()
{
    // Serial port for debugging purposes
    Serial.begin(115200);

    initSPIFFS();

    // Load values saved in SPIFFS
    ssid = readFile(SPIFFS, ssidPath);
    pass = readFile(SPIFFS, passPath);
    ip = readFile(SPIFFS, ipPath);
    gateway = readFile(SPIFFS, gatewayPath);
    Serial.println(ssid);
    Serial.println(pass);
    Serial.println(ip);
    Serial.println(gateway);

    if (initWiFi())
    {
        // // Route for root / web page
        // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        //   request->send(SPIFFS, "/index.html", "text/html", false, processor);
        // });
        // server.serveStatic("/", SPIFFS, "/");

        // // Route to set GPIO state to HIGH
        // server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        //   digitalWrite(ledPin, HIGH);
        //   request->send(SPIFFS, "/index.html", "text/html", false, processor);
        // });

        // // Route to set GPIO state to LOW
        // server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        //   digitalWrite(ledPin, LOW);
        //   request->send(SPIFFS, "/index.html", "text/html", false, processor);
        // });
        server.begin();
    }
    else
    {
        // Connect to Wi-Fi network with SSID and password
        Serial.println("Setting AP (Access Point)");
        // NULL sets an open Access Point
        WiFi.softAP("ESP-WIFI-MANAGER", NULL);

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        // Web Server Root URL
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(SPIFFS, "/wifimanager.html", "text/html"); });

        server.serveStatic("/", SPIFFS, "/");

        server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
                  {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(SPIFFS, gatewayPath, gateway.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(3000);
      ESP.restart(); });
        server.begin();
    }
}

void loop()
{
}