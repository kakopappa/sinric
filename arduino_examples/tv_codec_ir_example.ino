#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>

//IR includes and vars

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#define IR_LED 4 //TODO: Change to your digital pin for IR. default D2

IRsend irsend(IR_LED);

//Start of including vars
//Variables starting with 'TV' are ir commands for the TV
//Variables starting with 'TELE' are ir commands for DECODER

const unsigned long TV_POWER_H = 0x20DF10EF;
const unsigned long TV_VOLUP_H = 0x20DF40BF;
const unsigned long TV_VOLDOWN_H = 0x20DFC03F;
const unsigned long TV_OK_H = 0x20DF22DD;
const unsigned long TV_INPUT_H = 0x20DFD02F;
const unsigned long TV_UP_H = 0x20DF02FD;
const unsigned long TV_DOWN_H = 0x20DF827D;
const unsigned long TV_MUTE_H = 0x20DF906F;

const unsigned long TV_P_UP_H = 0x20DF00FF;
const unsigned long TV_P_DOWN_H = 0x20DF807F;

const unsigned long TV_1 = 0x20DF8877;
const unsigned long TV_2 = 0x220DF48B7;
const unsigned long TV_3 = 0x20DFC837;
const unsigned long TV_4 = 0x20DF28D7;
const unsigned long TV_5 = 0x20DFA857;
const unsigned long TV_6 = 0x20DF6897;
const unsigned long TV_7 = 0x20DFE817;
const unsigned long TV_8 = 0x20DF18E7;
const unsigned long TV_9 = 0x20DF9867;
const unsigned long TV_0 = 0x20DF08F7;

const unsigned long TELE_1 = 0x977B4DB2;
const unsigned long TELE_2 = 0x977B659A;
const unsigned long TELE_3 = 0x977BE718;
const unsigned long TELE_4 = 0x977BCD32;
const unsigned long TELE_5 = 0x977B15EA;
const unsigned long TELE_6 = 0x977B57A8;
const unsigned long TELE_7 = 0x977BF708;
const unsigned long TELE_8 = 0x977B1DE2;
const unsigned long TELE_9 = 0x977BCF30;
const unsigned long TELE_0 = 0x977B7788;

const unsigned long TELE_OK = 0x977BED12;

const unsigned long TELE_P_UP_H = 0x977B0DF2;
const unsigned long TELE_P_DOWN_H = 0x977B8D72;

const unsigned long DELAY_BETWEEN_COMMANDS = 100;
//end ir includes and vars

#define API_KEY "" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define SSID_NAME "" // TODO: Change to your Wifi network SSID
#define WIFI_PASSWORD "" // TODO: Change to your Wifi network password
#define DEVICE_ID "5*****************"
#define SERVER_URL "iot.sinric.com"
#define SERVER_PORT 80
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
  irsend.begin();
  WiFiMulti.addAP(SSID_NAME, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(SSID_NAME);

  // Waiting for Wifi connect
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFiMulti.run() == WL_CONNECTED) {
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

  if (isConnected) {
    uint64_t now = millis();

    // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
    if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
      heartbeatTimestamp = now;
      webSocket.sendTXT("H");
    }
  }
}

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void skipChannel(int channels) {
  if (channels < 0) {
    Serial.print("Channel skip down: ");
    Serial.println(channels);
    irsend.sendNEC(TELE_P_DOWN_H, 32);
  } else if (channels > 0) {
    Serial.print("Channel skip up: ");
    Serial.println(channels);
    irsend.sendNEC(TELE_P_UP_H, 32);
  }
}

void adjustVolume(int adjustVolume, bool kvdefault) {
  if (adjustVolume < 0) {
    Serial.print("Volume down: ");
    Serial.println(adjustVolume);
    if (kvdefault) {
      irsend.sendNEC(TV_VOLDOWN_H, 32);
    } else {
      for (int i = 0; i < -adjustVolume; i++ ) {
        irsend.sendNEC(TV_VOLDOWN_H, 32);
        delay(DELAY_BETWEEN_COMMANDS);
      }
    }
  } else if (adjustVolume > 0) {
    Serial.print("Volume up: ");
    Serial.println(adjustVolume);
    if (kvdefault) {
      irsend.sendNEC(TV_VOLUP_H, 32);
    } else {
      for (int i = 0; i < adjustVolume; i++ ) {
        irsend.sendNEC(TV_VOLUP_H, 32);
        delay(DELAY_BETWEEN_COMMANDS);
      }
    }
  }
}


void setMute(bool mute) {
  Serial.print("mute: ");
  Serial.println(mute);
  irsend.sendNEC(TV_MUTE_H, 32);
}

void setChannel(String ch) {
  Serial.print("Changing channel: ");
  Serial.println(ch);
  for (int i = 0; i < ch.length(); i++) {
    switch (ch[i]) {
      case '1':
        send_ir(TELE_1, 32);
        break;
      case '2':
        send_ir(TELE_2, 32);
        break;
      case '3':
        send_ir(TELE_3, 32);
        break;
      case '4':
        send_ir(TELE_4, 32);
        break;
      case '5':
        send_ir(TELE_5, 32);
        break;
      case '6':
        send_ir(TELE_6, 32);
        break;
      case '7':
        send_ir(TELE_7, 32);
        break;
      case '8':
        send_ir(TELE_8, 32);
        break;
      case '9':
        send_ir(TELE_9, 32);
        break;
      case '0':
        send_ir(TELE_0, 32);
        break;
    }
    delay(DELAY_BETWEEN_COMMANDS);
  }
  delay(DELAY_BETWEEN_COMMANDS);
  irsend.sendNEC(TELE_OK, 32);
}

void send_ir(unsigned long type, int len) {
  irsend.sendNEC(type, len);
}

void setInput(String value) {
  Serial.print("Next input selected: ");
  irsend.sendNEC(TV_INPUT_H, 32);
  delay(DELAY_BETWEEN_COMMANDS);
  irsend.sendNEC(TV_DOWN_H, 32);
}

void turnOn(String deviceId) {
  if (deviceId == DEVICE_ID) // Device ID of first device
  {
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    irsend.sendNEC(TV_POWER_H, 32);
  }
}

void turnOff(String deviceId) {
  if (deviceId == DEVICE_ID) // Device ID of first device
  {
    Serial.print("Turn off Device ID: ");
    Serial.println(deviceId);
    irsend.sendNEC(TV_POWER_H, 32);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
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

        if (action == "setPowerState") {
          // alexa, turn on tv ==> {"deviceId":"xx","action":"setPowerState","value":"ON"}
          String value = json ["value"];
          if (value == "ON") {
            turnOn(deviceId);
          } else {
            turnOff(deviceId);
          }
        } else if (action == "SetMute") {
          // alexa, mute tv ==> {"deviceId":"xxx","action":"SetMute","value":{"mute":true}}
          bool mute = json ["value"]["mute"];
          setMute(mute);
        } else if (action == "AdjustVolume") {
          // alexa, turn the volume down on tv by 20 ==> {"deviceId":"xxx","action":"AdjustVolume","value":{"volume":-20,"volumeDefault":false}}
          // alexa, lower the volume on tv ==> {"deviceId":"xx","action":"AdjustVolume","value":{"volume":-10,"volumeDefault":true}}
          const char* kcvolume = json ["value"]["volume"];
          bool kvdefault = json["value"]["volumeDefault"];
          int volume = atoi(kcvolume);
          adjustVolume(volume, kvdefault);
        }
        else if (action == "ChangeChannel") {
          //alexa, change channel to 200 on tv ==> {"deviceId":"xx","action":"ChangeChannel","value":{"channel":{},"channelMetadata":{"name":"CH9-200"}}}
          //alexa, change channel to pbs on tv ==> {"deviceId":"xx","action":"ChangeChannel","value":{"channel":{},"channelMetadata":{"name":"pbs"}}}
          String value = json ["value"]["channel"]["number"];
          setChannel(value);
        }
        else if (action == "SkipChannels") {
          //Alexa, next channel on tv ==>  {"deviceId":"xx","action":"SkipChannels","value":{"channelCount":1}}
          // Alexa may say Sorry, TV is not responding. but command works
          const char* kchannels = json ["value"]["channelCount"];
          int skip_ch = atoi(kchannels);
          skipChannel(skip_ch);
        }
        else if (action == "Previous" || action == "Play") {
          // alexa, previous on tv ==> {"deviceId":"xx","action":"Previous","value":{}}
          // alexa, resume tv ==> {"deviceId":"xx","action":"Play","value":{}}
          // Alexa, pause tv ==> says I dont know that one...
          // for others check https://developer.amazon.com/docs/device-apis/alexa-playbackcontroller.html
        }
        else if (action == "SelectInput") {
          // alexa, change the input to hdmi ==> {"deviceId":"xx","action":"","value":{"input":"HDMI"}}
          String value = json ["value"]["input"];
          Serial.println("ChangeChannel arrived with value:");
          Serial.println(value);
          setInput(value);
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

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server.

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