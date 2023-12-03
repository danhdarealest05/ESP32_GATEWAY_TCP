#include <Arduino.h>
#include <Wiegand.h>
#include <AsyncElegantOTA.h>
#include <Wiegand.h>
#include <WiFi.h>
#include "ArduinoJson.h"

#include <WiFi.h>
#include "ModbusIP_ESP8266.h"
// #include <ESP32_Master.h>
#include <ESP32_Slaver.h>

void setup()
{
  appInit();
}

void loop()
{
  appRun();
}