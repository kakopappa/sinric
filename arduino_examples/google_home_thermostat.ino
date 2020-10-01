#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h>      // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
#include <IRremoteESP8266.h>
#include <ir_Samsung.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

#define MyApiKey ""       // ApiKey Sinric
#define MySSID ""         // TODO: Change to your Wifi network SSID
#define MyWifiPassword "" // TODO: Change to your Wifi network password
#define HEARTBEAT_INTERVAL 300000
#define MyThermostatId "" // Your Thermostat ID

//---------------Thermostat-------------------------

IRSamsungAc ac(D2);

//--------------------------------------------------

void setTemp(String deviceId, int temp)
{
    if (deviceId == MyThermostatId) // Device ID of first device
    {
        Serial.print("Turn temp Thermostat id: ");
        Serial.println(deviceId);
        ac.setTemp(temp);
        ac.send();
    }
}

void turnOn(String deviceId)
{
    if (deviceId == MyThermostatId) // Device ID of first device
    {
        Serial.print("Turn on AR id: ");
        Serial.println(deviceId);
        ac.on();
        ac.send();
    }
}
void turnOff(String deviceId)
{
    if (deviceId == MyThermostatId) // Device ID of first device
    {
        Serial.print("Turn off AR id: ");
        Serial.println(deviceId);
        ac.off();
        ac.send();
    }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        isConnected = false;
        Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
        break;
    case WStype_CONNECTED:
    {
        isConnected = true;
        Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
        Serial.printf("Waiting for commands from sinric.com ...\n");
    }
    break;
    case WStype_TEXT:
    {
        Serial.printf("[WSc] get text: %s\n", payload);

#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject((char *)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char *)payload);
#endif
        String deviceId = json["deviceId"];
        String action = json["action"];

        if (action == "action.devices.commands.OnOff")
        { // Switch
            String value = json["value"]["on"];
            Serial.println(value);

            if (value == "true")
            {
                turnOn(deviceId);
            }
            else
            {
                turnOff(deviceId);
            }
        }
        else if (action == "action.devices.commands.ThermostatTemperatureSetpoint")
        {
            float value = json["value"]["thermostatTemperatureSetpoint"];
            int temp = (int)value;
            setTemp(deviceId, temp);
        }
        else if (action == "test")
        {
            Serial.println("[WSc] received test command from sinric.com");
        }
    }
    break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        break;
    default:
        break;
    }
}

void setupWifi()
{
    WiFiMulti.addAP(MySSID, MyWifiPassword);
    Serial.println();
    Serial.print("Connecting to Wifi: ");
    Serial.println(MySSID);

    // Waiting for Wifi connect
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    if (WiFiMulti.run() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.print("WiFi connected. ");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
}

void setupThermostat()
{
    ac.begin();
    ac.setPower(true);
    ac.setBeep(false);
    ac.setMode(kSamsungAcCool);
    ac.setFan(kSamsungAcFanHigh);
    ac.send();
}

void setupWebSocket()
{
    // server address, port and URL
    webSocket.begin("iot.sinric.com", 80, "/"); //"iot.sinric.com", 80

    // event handler
    webSocket.onEvent(webSocketEvent);
    webSocket.setAuthorization("apikey", MyApiKey);

    // try again every 5000ms if connection has failed
    webSocket.setReconnectInterval(5000); // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loopWebSocket()
{
    webSocket.loop();
    if (isConnected)
    {
        uint64_t now = millis();
        // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
        if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL)
        {
            heartbeatTimestamp = now;
            webSocket.sendTXT("H");
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setupThermostat();
    setupWifi();
    setupWebSocket();
}

void loop()
{
    loopWebSocket();
}