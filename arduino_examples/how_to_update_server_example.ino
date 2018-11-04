#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>

//...

const int postingInterval = 3 * 60 * 1000; // post data every 3 mins
unsigned long previousMillis = 0;

void setup() {
    //....
}

void loop() {
    //....

    // If interval time has passed since the last connection, Publish data to Sinric       
    if((millis() - previousMillis >= postingInterval)) {
      previousMillis = millis();  // Remember the time

      // Publish to server eg:
      //    setPowerStateOnServer
      //    setTargetTemperatureOnServer
    }

    delay(10);  
}

void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}

void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}
