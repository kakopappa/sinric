#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <ArduinoJson.h> // get it from https://arduinojson.org/

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

void turnOn() {
  Serial.println("Turn on ...");
}

void turnOff() {
  Serial.println("Turn off ...");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
  
        // Payload for ON :
        //{"deviceId":"5a2b908a74a6703928abf38b","powerState":"ON"}
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
  
        String deviceId = json ["deviceId"];
        String action = json ["action"];

        if(action == "setPowerState") {
            String value = json ["value"];

            if(value == "ON") {
                turnOn();
            } else {
                turnOff();
            }
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }

}

void setup() {
  Serial.begin(115200);
  
  // TODO: Change your WiFI settings
  WiFiMulti.addAP("<your wifi ssid>", "<your wifi password>");

  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // TODO: Change your API Key. Your API Key is displayed on sinric.com dashboard
  webSocket.setAuthorization("apikey", "<Place your API key here>");
  
 
}

void loop() {
  webSocket.loop();
}
