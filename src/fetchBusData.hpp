#include <WiFi.h>
#include <HTTPClient.h>
#include <vector>
#include "./drivers/LCD12864RSPI.h"
#include <ArduinoJson.h>
class Stop{
public:
void fetchBusData(const char* stopId);
void displayBusData( bool departing);
private:
void processBusData(const String& data);
String payload;
};
