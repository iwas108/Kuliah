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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

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
    StaticJsonDocument<200> doc;
    doc["rssi"] = min(max(2 * (WiFi.RSSI() + 100), 0), 100);
    doc["ip"] = WiFi.localIP();
    doc["ssid"] = WiFi.SSID();
    doc["temperature"] = random(17, 32);
    doc["uptime"] = millis();
    //broadcast data ke semua klien
    String output;
    serializeJson(doc, output);
    if(myWs.count() > 0){
      myWs.textAll(output.c_str());
    }

    SCHEDULER_WS_ROUTINE = now;
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
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
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT){}
      else{}
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT){}
          else{}
        }
      }
    }
  }
}