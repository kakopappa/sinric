/*
 Example Sinric <-> Domoticz connection
 Version 0.1 - Nov 27 2018 GizMoCuz
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <esp8266httpclient.h>
#include <ArduinoJson.h>

WebSocketsClient webSocket;

#define MyApiKey "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define MySSID "YOUR_WIFI_SSID"
#define MyWifiPassword "YOUR_WIFI_PASSWORD"
#define MyHostName "ESPEcho"

#define API_ENDPOINT "http://sinric.com"
#define WEBSOCKET_ENDPOINT "iot.sinric.com"
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes (5*60*1000)
#define SERIAL_BAUDRATE 9600

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

#define DOMOTICZ_ENDPOINT "http://<IPADDRESS_OF_DOMOTICZ>:<PORT>/json.htm?" //for example http://192.168.0.123:8080/json.htm?
#define DOMOTICZ_USERNAME "" //you can whitelist the ip address of this hardware
#define DOMOTICZ_PASSWORD ""

#define DEBUG

enum eDeviceType
{
  DTYPE_UNKNOWN = 0,
  DTYPE_LIGHT,
  DTYPE_SCENE,
  DTYPE_GROUP,
  DTYPE_THERMOSTAT,
};

struct _tSinricDevices
{
  String DeviceID;
  eDeviceType DeviceType;
  String DeviceName;
  int DomoticzID;
  int DomoticzID_2; //currently used for Scene Off action
} ;

_tSinricDevices sDevices[] = {
  { "5bxxxxxxxxxxxxxxxxxxxxx1", DTYPE_SCENE, "living room", 21, 22},
  { "5bxxxxxxxxxxxxxxxxxxxxx2", DTYPE_SCENE, "lights", 21, 22},

  { "5bxxxxxxxxxxxxxxxxxxxxx3", DTYPE_SCENE, "mood living room", 25, 26},
  { "5bxxxxxxxxxxxxxxxxxxxxx4", DTYPE_SCENE, "mood val", 27, 127},

  { "5bxxxxxxxxxxxxxxxxxxxxx5", DTYPE_SCENE, "office", 31, 32},

  { "5bxxxxxxxxxxxxxxxxxxxxx6", DTYPE_SCENE, "good morning", 33, 33},
  { "5bxxxxxxxxxxxxxxxxxxxxx7", DTYPE_SCENE, "good night", 34, 34},

  { "5bxxxxxxxxxxxxxxxxxxxxx8", DTYPE_LIGHT, "kitchen", 2001, 0},

  { "5bxxxxxxxxxxxxxxxxxxxxx9", DTYPE_LIGHT, "bedroom", 2002, 0},
  { "5bxxxxxxxxxxxxxxxxxxxx10", DTYPE_LIGHT, "mood bedroom", 2003, 0},

  { "5bxxxxxxxxxxxxxxxxxxxx11", DTYPE_LIGHT, "christmas tree", 4100, 0},

  { "5bxxxxxxxxxxxxxxxxxxxx12", DTYPE_LIGHT, "outside", 4200, 0},

  { "5bxxxxxxxxxxxxxxxxxxxx13", DTYPE_THERMOSTAT, "thermostat", 118, 0},
  { "5bxxxxxxxxxxxxxxxxxxxx14", DTYPE_THERMOSTAT, "heating", 118, 0},
  
  { "", DTYPE_UNKNOWN, ""}
};

String HTTP_GET(String url)
{
  if (String(DOMOTICZ_USERNAME)!="") {
    url += "&username=" + String(DOMOTICZ_USERNAME);
    url += "&password=" + String(DOMOTICZ_PASSWORD);
  }
#ifdef DEBUG
  Serial.println("url: " + url);
#endif
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  String payload;

  if (httpCode <= 0)
   return payload;

  if (httpCode == 200)
  {
    payload = http.getString();
#ifdef DEBUG    
    Serial.println(payload);
#endif    
    return payload;
  }
  //Error
  payload = http.getString();
  Serial.println("HTTP Error: code: " + String(httpCode) + ", " + payload);
  payload="";
  return payload;
}


const _tSinricDevices* FindDevice(String deviceId)
{
  _tSinricDevices *pDevice=(_tSinricDevices*)&sDevices;
  while (pDevice->DeviceID!="")
  {
    if (pDevice->DeviceID == deviceId)
      return pDevice;
    pDevice++;
  }
  return NULL;
}

String TranslateCommand(const String cmd)
{
  if (cmd == "ON") return "On";
  if (cmd == "OFF") return "Off";
  return cmd;
}

void SendDomoticzCommand(const int dev_idx, String cmd, String param)
{
    String url = String(DOMOTICZ_ENDPOINT) + "type=command&param=" + cmd + "&idx=" + String(dev_idx) + param;
    HTTP_GET(url);
}

void CmdSwitchLight(const _tSinricDevices *pDevice, const String cmd) {
#ifdef DEBUG
    Serial.println("Turning switch: " + pDevice->DeviceName  + " " + TranslateCommand(cmd));
#endif
    SendDomoticzCommand(pDevice->DomoticzID, "switchlight", "&switchcmd=" + TranslateCommand(cmd));
}

void CmdSwitchScene(const _tSinricDevices *pDevice, const String cmd) {
#ifdef DEBUG
    Serial.println("Turning scene: " + pDevice->DeviceName  + " " + TranslateCommand(cmd));
#endif
    int idx = (TranslateCommand(cmd) == "On") ? pDevice->DomoticzID : pDevice->DomoticzID_2;
    SendDomoticzCommand(idx, "switchscene", "&switchcmd=On");
}

void CmdSwitchGroup(const _tSinricDevices *pDevice, const String cmd) {
#ifdef DEBUG
    Serial.println("Turning scene: " + pDevice->DeviceName  + " " + TranslateCommand(cmd));
#endif
    SendDomoticzCommand(pDevice->DomoticzID, "switchscene", "&switchcmd=" + TranslateCommand(cmd));
}

void CmdSetColor(const _tSinricDevices *pDevice, const String hue) {
#ifdef DEBUG
    Serial.println("SetColor light: " + pDevice->DeviceName  + " to: " + hue);
#endif
    SendDomoticzCommand(pDevice->DomoticzID, "setcolbrightnessvalue", "&hue=" + hue);
}

void CmdSetBrightness(const _tSinricDevices *pDevice, const uint16_t brightness) {
#ifdef DEBUG
    Serial.println("SetBrightness switch: " + pDevice->DeviceName  + " to: " + brightness);
#endif
}

void CmdSetThermostat(const _tSinricDevices *pDevice, const String degrees) {
#ifdef DEBUG
    Serial.println("SetThermostat: " + pDevice->DeviceName  + " to: " + degrees);
#endif    
    SendDomoticzCommand(pDevice->DomoticzID, "setsetpoint", "&setpoint=" + degrees);
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
#ifdef DEBUG
        Serial.printf("[WSc] get text: %s\n", payload);
#endif        
        // Example payloads

        // For Light device type
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html
        // {"deviceId": xxxx, "action": "AdjustBrightness", value: 3} // https://developer.amazon.com/docs/device-apis/alexa-brightnesscontroller.html
        // {"deviceId": xxxx, "action": "setBrightness", value: 42} // https://developer.amazon.com/docs/device-apis/alexa-brightnesscontroller.html
        // {"deviceId": xxxx, "action": "SetColor", value: {"hue": 350.5,  "saturation": 0.7138, "brightness": 0.6501}} // https://developer.amazon.com/docs/device-apis/alexa-colorcontroller.html
        // {"deviceId": xxxx, "action": "DecreaseColorTemperature"} // https://developer.amazon.com/docs/device-apis/alexa-colortemperaturecontroller.html
        // {"deviceId": xxxx, "action": "IncreaseColorTemperature"} // https://developer.amazon.com/docs/device-apis/alexa-colortemperaturecontroller.html
        // {"deviceId": xxxx, "action": "SetColorTemperature", value: 2200} // https://developer.amazon.com/docs/device-apis/alexa-colortemperaturecontroller.html
        
        // For Thermostat device type
        // {"deviceId":"5bfe4f7c85f6225c9115d000","action":"SetTargetTemperature","value":{"targetSetpoint":{"value":21,"scale":"CELSIUS"}}} //https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html

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

        if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
            return;
        }

        const _tSinricDevices *pDevice = FindDevice(deviceId);
        if (pDevice==NULL) {
          Serial.print("Unknown device_id received: ");
          Serial.println(deviceId);
          return;
        }
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if (pDevice->DeviceType == DTYPE_LIGHT)
              CmdSwitchLight(pDevice, value);
            else if (pDevice->DeviceType == DTYPE_SCENE)
              CmdSwitchScene(pDevice, value);
            else if (pDevice->DeviceType == DTYPE_GROUP)
              CmdSwitchGroup(pDevice, value);
        }
        else if(action == "SetColor") {
            // Alexa, set the device name to red
            // get text: {"deviceId":"xxxx","action":"SetColor","value":{"hue":0,"saturation":1,"brightness":1}}
            String hue = json ["value"]["hue"];
            String saturation = json ["value"]["saturation"];
            String brightness = json ["value"]["brightness"];

            CmdSetColor(pDevice, hue);
        }
        else if(action == "SetBrightness") {
            String brightness = json ["value"];
            CmdSetBrightness(pDevice, atoi(brightness.c_str()));
        }
        else if(action == "AdjustBrightness") {
          
        }
        else if(action == "SetTargetTemperature") {
            String target_temp = json ["value"]["targetSetpoint"]["value"];
            String temp_scale = json ["value"]["targetSetpoint"]["scale"];
            CmdSetThermostat(pDevice, target_temp);
        }
        else {
            Serial.println("[WSc] received unknown action: " + action);
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
    case WStype_ERROR:
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
      Serial.printf("[WSc] Error!!get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  delay(10);
  Serial.println(String(MyHostName)+" started...");

  WiFi.hostname(MyHostName);
  WiFi.begin(MySSID, MyWifiPassword);
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);

  // server address, port and URL
  webSocket.begin(WEBSOCKET_ENDPOINT, 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
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
