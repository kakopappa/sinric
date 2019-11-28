#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>

#define API_KEY "" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define SSID_NAME "" // TODO: Change to your Wifi network SSID
#define WIFI_PASSWORD "" // TODO: Change to your Wifi network password
#define SERVER_URL "iot.sinric.com"
#define SERVER_PORT 80

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void setPowerStateOnServer(String deviceId, String value);


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
#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") {
            // alexa, turn on tv ==> {"deviceId":"xx","action":"setPowerState","value":"ON"}
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        } else if(action == "SetMute") { 
            // alexa, mute tv ==> {"deviceId":"xxx","action":"SetMute","value":{"mute":true}}
            bool mute = json ["value"]["mute"];
            setMute(mute);
        } else if(action == "AdjustVolume") { 
            // alexa, turn the volume down on tv by 20 ==> {"deviceId":"xxx","action":"AdjustVolume","value":{"volume":-20,"volumeDefault":false}}
            // alexa, lower the volume on tv ==> {"deviceId":"xx","action":"AdjustVolume","value":{"volume":-10,"volumeDefault":true}}
            const char* kcvolume = json ["value"]["volume"];
            int volume = atoi(kcvolume);  
            adjustVolume(volume);
        }
        else if(action == "ChangeChannel") { 
          //alexa, change channel to 200 on tv ==> {"deviceId":"xx","action":"ChangeChannel","value":{"channel":{},"channelMetadata":{"name":"CH9-200"}}}
          //alexa, change channel to pbs on tv ==> {"deviceId":"xx","action":"ChangeChannel","value":{"channel":{},"channelMetadata":{"name":"pbs"}}}
          String value = json ["value"]["channelMetadata"]["name"];
          
        }
        else if(action == "SkipChannels") { 
          //Alexa, next channel on tv ==>  {"deviceId":"xx","action":"SkipChannels","value":{"channelCount":1}}
          // Alexa may say Sorry, TV is not responding. but command works          
        }
        else if(action == "Previous" || action == "Play") { 
          // alexa, previous on tv ==> {"deviceId":"xx","action":"Previous","value":{}}
          // alexa, resume tv ==> {"deviceId":"xx","action":"Play","value":{}}
          // Alexa, pause tv ==> says I dont know that one...
          // for others check https://developer.amazon.com/docs/device-apis/alexa-playbackcontroller.html
        }
         else if(action == "SelectInput") { 
          // alexa, change the input to hdmi ==> {"deviceId":"xx","action":"","value":{"input":"HDMI"}}
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[webSocketEvent] get binary length: %u\n", length);
      break;
    default: break;
  }
}


// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

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

 
