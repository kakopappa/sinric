#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>

//...
void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

const int postingInterval = 3 * 60 * 1000; // post data every 3 mins
unsigned long previousMillis = 0;
WebSocketsClient webSocket;

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
#if ARDUINOJSON_VERSION_MAJOR == 5
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
  DynamicJsonDocument root(1024);
#endif        
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
#if ARDUINOJSON_VERSION_MAJOR == 5
  root.printTo(databuf);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
  serializeJson(root, databuf);
#endif  
  
  webSocket.sendTXT(databuf);
}


void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
#if ARDUINOJSON_VERSION_MAJOR == 5
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
  DynamicJsonDocument root(1024);
#endif        
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
#if ARDUINOJSON_VERSION_MAJOR == 5
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
  JsonObject valueObj = root.createNestedObject("value");
  JsonObject targetSetpoint = valueObj.createNestedObject("targetSetpoint");
#endif  
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
#if ARDUINOJSON_VERSION_MAJOR == 5
  root.printTo(databuf);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
  serializeJson(root, databuf);
#endif  
  
  webSocket.sendTXT(databuf);
}
