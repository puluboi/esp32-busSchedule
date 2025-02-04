#include <Arduino.h>
#include "./drivers/LCD12864RSPI.h"
#include "fetchBusData.hpp"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <algorithm>


//Width 93mm Height 70mm distance between the holes: 61mm and 84.3mm.2mm hole diameter
//Screen 53.6mm x 78.8mm
const char* ssid = "150cc"; 
char password[] = "AHCPR9F97HUWJ"; //"AHCPR9F97HUWJ" for password to 150cc
const char* stopId = "HSL:2222209";
const char* stopId2 = "HSL:2222208";
bool depart = true;
// put function declarations here:
unsigned char str[] = "Hello, world!";

Stop stop;
Stop stop2; // The second stop that has busses only if nighttime.
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
    if(depart && stop.isNight()){ // only display busdata from stop2 when night and departing...
      stop2.displayBusData(depart);
    }
    stop.displayBusData(depart);
  }
  stop.fetchBusData(stopId);
  if(stop.isNight()){
    stop2.fetchBusData(stopId2);
  }
  
}

