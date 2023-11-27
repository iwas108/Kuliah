#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ArduinoJson.h>

const char* ssid = "LAB TI";
const char* pass = "#tiundiknas";

WiFiMulti myWiFi;
AsyncWebServer myWeb(80);
AsyncWebSocket myWs("/ws");

// put function declarations here:
int myFunction(int, int);

void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void setSwitch(uint8_t switchId);
void sendData(StaticJsonDocument<512> &doc);

uint8_t pinLed[3] = {25, 33, 32}; // hijau, biru, merah
bool ledState[3] = {false, false, false};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  for(int i = 0; i < 3; i++){
    pinMode(pinLed[i], OUTPUT);
  }

  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.onEvent(onWiFiConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(onWiFiDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(onWiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  myWiFi.addAP(ssid, pass);
  
  /*myWeb.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hello, world");
  });*/

  myWeb.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");

  while (myWiFi.run() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  myWeb.begin();
  myWs.onEvent(onWsEvent);
  myWeb.addHandler(&myWs);
}

unsigned long SCHEDULER_WS_ROUTINE = 0;
void loop() {
  // put your main code here, to run repeatedly:
  myWiFi.run();
  myWs.cleanupClients();

  //Eksekusi setiap 1 detik sekali
  unsigned long now = millis();
  if( (now - SCHEDULER_WS_ROUTINE) >= 100 ){

    //Persiapakan data yang akan dibroadcast
    StaticJsonDocument<512> doc;
    doc["rssi"] = min(max(2 * (WiFi.RSSI() + 100), 0), 100);
    doc["ip"] = WiFi.localIP();
    doc["ssid"] = WiFi.SSID();
    doc["temperature"] = random(17, 32);
    doc["humidity"] = random(0, 100);
    doc["uptime"] = millis();
    //broadcast data ke semua klien
    sendData(doc);

    SCHEDULER_WS_ROUTINE = now;
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void sendData(StaticJsonDocument<512> &doc){
  String output;
  serializeJson(doc, output);
  if(myWs.count() > 0){
    myWs.textAll(output.c_str());
  }
}

void setSwitch(uint8_t switchId){
  if(switchId == 1){
    ledState[0] = !ledState[0];
    digitalWrite(pinLed[0], ledState[0]);

    StaticJsonDocument<512> doc;
    doc["switch"] = switchId;
    doc["state"] = ledState[switchId - 1];
    sendData(doc);

    Serial.printf("Switch %d is %s\n", switchId, ledState[switchId - 1] ? "ON" : "OFF");
  }
  else if(switchId == 2){
    ledState[1] = !ledState[1];
    digitalWrite(pinLed[1], ledState[1]);

    StaticJsonDocument<512> doc;
    doc["switch"] = switchId;
    doc["state"] = ledState[switchId - 1];
    sendData(doc);

    Serial.printf("Switch %d is %s\n", switchId, ledState[switchId - 1] ? "ON" : "OFF");
  }
  else if(switchId == 3){
    ledState[2] = !ledState[2];
    digitalWrite(pinLed[2], ledState[2]);

    StaticJsonDocument<512> doc;
    doc["switch"] = switchId;
    doc["state"] = ledState[switchId - 1];
    sendData(doc);

    Serial.printf("Switch %d is %s\n", switchId, ledState[switchId - 1] ? "ON" : "OFF");
  }

}

void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("WiFi connected: ");
  Serial.print(WiFi.SSID());
  Serial.print(" | Signal: ");
  Serial.print(min(max(2 * (WiFi.RSSI() + 100), 0), 100));
  Serial.println("%");
}

void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi disconnected!");
}

void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("Got IP Address: ");
  Serial.println(WiFi.localIP());
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
  } 
  else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } 
  else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } 
  else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } 
  else if(type == WS_EVT_DATA){
    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, data);
    if(err == DeserializationError::Ok){
      if(doc["switch"] != nullptr){
        if(doc["switch"].as<int>() == 1){
          setSwitch(1);
        }
        else if(doc["switch"].as<int>() == 2){
          setSwitch(2);
        }
        else if(doc["switch"].as<int>() == 3){
          setSwitch(3);
        }
      }
    }
    else{
      Serial.println(err.c_str());
    }
  }
}