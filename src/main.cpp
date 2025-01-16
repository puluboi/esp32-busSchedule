#include <Arduino.h>
#include "./drivers/LCD12864RSPI.h"
#include "fetchBusData.hpp"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <algorithm>

const char* ssid = "*****";
char password[] = "*********";
const char* stopId = "HSL:2222209";
bool depart = true;
// put function declarations here:
unsigned char str[] = "Hello, world!";

Stop stop;
void setup()
{
  LCDA.initDriverPin(SCK,D3,MOSI); 
  LCDA.Initialise(); // INIT SCREEN  
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  delay(200);
  LCDA.DisplayString(0,0,(unsigned char *)"Connecting...", 12);
  while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
  delay(100);
  Serial.println("Starting the program!");
  LCDA.CLEAR();
  LCDA.DisplayString(0,0,(unsigned char *)"Booting...", 10);
  stop.fetchBusData(stopId);
  stop.displayBusData(depart);
}


void loop() {
  for (size_t i = 0; i < 24; i++)
  {
    delay(5000);
    if(depart){
      depart = false;
    }
    else{
      depart = true;
    }
    LCDA.CLEAR();
    stop.displayBusData(depart);
  }
  stop.fetchBusData(stopId);
  
}

