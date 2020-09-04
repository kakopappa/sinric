#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>

#define MyApiKey ""
#define MyWifiSSID ""
#define MyWifiPassword ""
#define MyLedStripId ""
#define HEARTBEAT_INTERVAL 300000

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;


void onOff(String deviceId, bool statusLed) {
  if (deviceId == MyLedStripId){
    Serial.printf("OnOff: %B\n", statusLed);
  }   
}

void colorAbsolute(String deviceId, int decimalColor){
  if (deviceId == MyLedStripId){  
    String hexstring =  String(decimalColor, HEX);
    hexstring = "#" + hexstring;
    int number = (int) strtol( &hexstring[1], NULL, 16);
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;
    Serial.println((String)"Color :\nRed:"+r+"\nGreen:"+g+"\nBlue:"+b+"\n");
  }
}

void brightnessAbsolute(String deviceId, int bright){
  if (deviceId == MyLedStripId){
    bright = map(bright, 0, 100, 0, 255);
    Serial.printf("Bright : %d\n", bright);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);

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
        
        if(action == "action.devices.commands.OnOff") { // Switch 
          String value = json ["value"]["on"];
          onOff(deviceId, value == "true");
        }
        else if (action  == "action.devices.commands.ColorAbsolute") {
          String value = json ["value"]["color"]["spectrumRGB"];
          colorAbsolute(deviceId, value.toInt());
        }
        else if (action  == "action.devices.commands.BrightnessAbsolute") {
          String value = json ["value"]["brightness"];
          brightnessAbsolute(deviceId, value.toInt());
        }
        else if (action == "test") {
          Serial.println("Recebendo o comando de teste da sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
    default: break;
  }
}

void setupWifi(){
  WiFiMulti.addAP(MyWifiSSID, MyWifiPassword);
  Serial.println();
  Serial.print("Conectando ao Wifi: ");
  Serial.println(MyWifiSSID);  

  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi conectado. ");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  }
}

void setupWebSocket(){
  webSocket.begin("iot.sinric.com", 80, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  webSocket.setReconnectInterval(5000);
}

void setup() {
  Serial.begin(9600);
  setupWifi();
  setupWebSocket();
}

void loop() {
  webSocket.loop();

  if(isConnected) {
      uint64_t now = millis();
     if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }
}
