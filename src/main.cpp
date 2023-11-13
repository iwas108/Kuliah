#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "SPIFFS.h"

const char* ssid = "LAB TI";
const char* pass = "#tiundiknas";

WiFiMulti myWiFi;
AsyncWebServer myWeb(80);

// put function declarations here:
int myFunction(int, int);

void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
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

  myWeb.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

  while (myWiFi.run() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  myWeb.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  myWiFi.run();
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.SSID());
}

void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi disconnected!");
}

void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("Got IP Address: ");
  Serial.println(WiFi.localIP());
}