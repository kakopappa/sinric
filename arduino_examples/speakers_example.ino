#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>

#define API_KEY "49439026-bd43-48a8-b2a6-f8500e6e891a" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define SSID_NAME "June" // TODO: Change to your Wifi network SSID
#define WIFI_PASSWORD "wifipassword" // TODO: Change to your Wifi network password
#define SERVER_URL "192.168.1.100" // TODO: Change to your server url
#define SERVER_PORT 3000 // TODO: Change to your server url

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);


void setup() {
  Serial.begin(115200);
  
  WiFiMulti.addAP(SSID_NAME, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(SSID_NAME);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin(SERVER_URL, SERVER_PORT, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", API_KEY);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here
 

void adjustVolume(int adjustVolume) {
   Serial.print("adjustVolume: ");
   Serial.println(adjustVolume);    
}

void setMute(bool mute) {
   Serial.print("mute: ");
   Serial.println(mute);    
}

void turnOn(String deviceId) {
  if (deviceId == "5axxxxxxxxxxxxxxxxxxx") // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }     
}

void turnOff(String deviceId) {
   if (deviceId == "5axxxxxxxxxxxxxxxxxxx") // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
   }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[webSocketEvent] Webservice disconnected from server!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[webSocketEvent] Service connected to server at url: %s\n", payload);
      Serial.printf("[webSocketEvent] Waiting for commands from server ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[webSocketEvent] get text: %s\n", payload);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") {
            // alexa, turn on speakers ==> {"deviceId":"xx","action":"setPowerState","value":"ON"}
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }        
        }
        else if(action == "Pause") {
            // alexa, pause speakers ==> {"deviceId":"xx","action":"Pause","value":{}}
            
        }
        else if(action == "Play") {
            // alexa, pause speakers ==> {"deviceId":"xx","action":"Play","value":{}}

        } 
        else if(action == "Stop") {
            // alexa, pause speakers ==> {"deviceId":"xxx","action":"Stop","value":{}}
        }
        else if(action == "SetVolume") {
            // alexa, set the volume of speakers to 50 ==> {"deviceId":"xxx","action":"SetVolume","value":{"volume":50}}
        }
        else if(action == "SetVolume") {
            // alexa, set the volume of speakers to 50 ==> {"deviceId":"xxx","action":"SetVolume","value":{"volume":50}}
        }
        else if(action == "AdjustVolume") {
            // alexa, turn the volume down on speakers by 20 ==> {"deviceId":"xxx","action":"AdjustVolume","value":{"volume":-20,"volumeDefault":false}}
            // alexa, lower the volume on speakers ==> {"deviceId":"xxx","action":"AdjustVolume","value":{"volume":-10,"volumeDefault":true}}
        }
        else if(action == "SetMute") {
            // alexa, mute speakers ==> {"deviceId":"xx","action":"SetMute","value":{"mute":true}}
        }
        else if(action == "ChangeChannel") {
            // alexa, change channel to 200 on speakers ==> {"deviceId":"xx","action":"ChangeChannel","value":{"channel":{},"channelMetadata":{"name":"CH9-200"}}}
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[webSocketEvent] get binary length: %u\n", length);
      break;
  }
}


// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")
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
 
