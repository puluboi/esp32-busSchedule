#include "fetchBusData.hpp"

void Stop::fetchBusData(const char* stopId) {
  Serial.println("Fetching data!");
  if (WiFi.status() == WL_CONNECTED) {  // Check if we have wifi connection
    HTTPClient http;
    http.begin(
        "https://api.digitransit.fi/routing/v2/hsl/gtfs/v1");  // the api
                                                               // address here's
                                                               // the query
    String query = String(
        R"({"query":"{stop(id:\"HSL:2222209\"){name stoptimesWithoutPatterns{scheduledArrival realtimeArrival arrivalDelay scheduledDeparture realtimeDeparture departureDelay realtime realtimeState serviceDay headsign}}}"})");
    // add headers to the htpp
    http.addHeader("Content-Type", "application/json");
    http.addHeader("digitransit-subscription-key",
                   "183da02a389c4408a350927c329dccf3");  // subscription key
    int httpResponseCode = http.POST(query);  // make a POST request to the API

    if (httpResponseCode > 0) {  // check if successful
      payload = http.getString();
      processBusData(payload);
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end();

  } else {
    Serial.println("WiFi not connected");
  }
}

void Stop::processBusData(const String& data) {
  // Implement data processing logic here
  Serial.println("Received bus data:");
  Serial.println(data);
}

void Stop::displayBusData(bool departing) {
  // Extract the time of the next stop from the payload
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  JsonObject stop = doc["data"]["stop"];
  JsonArray stoptimes =
      stop["stoptimesWithoutPatterns"];  // get the stoptimes from the payload
  // Filter out the ones with headsign "Otaniemi"
  String headerstr = "Departing";
  if (departing) {  // if we're checking departing busses, the header should be
                    // correct
    for (int i = 0; i < stoptimes.size(); i++) {
      if (String((const char*)stoptimes[i]["headsign"]) == "Otaniemi") {
        stoptimes.remove(i);
        i--;  // Adjust index after removal
      }
    }
  } else {
    headerstr = "Arriving";
    for (int i = 0; i < stoptimes.size(); i++) {
      if (String((const char*)stoptimes[i]["headsign"]) ==
          "Matinkylä (M) via Tapiola (M)") {
        stoptimes.remove(i);
        i--;  // Adjust index after removal
      }
    }
  }

  if (stoptimes.size() > 0) {
    LCDA.DisplayString(0, 0, (unsigned char*)headerstr.c_str(),
                       headerstr.length());
    for (size_t i = 0; i < std::min(stoptimes.size(), (size_t)3); i++) {
      int nextStopTimehr = ((int)stoptimes[i]["realtimeDeparture"] / 3600) %
                           24;  // format the time, which in the payload is
                                // displayed in seconds since midnight.
      int nextStopTimemin = (int)stoptimes[i]["realtimeArrival"] % 3600 / 60;
      String minStr = nextStopTimemin < 10
                          ? "0" + String(nextStopTimemin)
                          : String(nextStopTimemin);  // If minutes under 10,
                                                      // add a zero to the front
      String realtime = stoptimes[i]["realtime"] ? "" : "~";
      String headSign = stoptimes[i]["headsign"].as<String>().substring(0, 3);
      String timeString = headSign + " " + realtime + " " +
                          String(nextStopTimehr) + ":" + minStr;
      LCDA.DisplayString(i + 1, 0, (unsigned char*)timeString.c_str(),
                         timeString.length());
      Serial.println("updated succesfully");
    }

  } else {
    Serial.println("No upcoming stops");
  }
}
bool Stop::isNight() {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  JsonObject stop = doc["data"]["stop"];
  JsonArray stoptimes = stop["stoptimesWithoutPatterns"];
  int nextStopTimehr = ((int)stoptimes[0]["realtimeDeparture"] / 3600);
  return nextStopTimehr>=24;
}