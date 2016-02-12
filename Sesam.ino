#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <SimpleTimer.h>
#include "static.h"
#include "settings.h"

////////////////////////////////////////////////////////
// Awesome code....

ESP8266WebServer server(80);
SimpleTimer timer;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  pinMode(OPEN_PIN, OUTPUT);
  
  server.on("/", []() {
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.send_P(200, TEXT_HTML, STATIC_INDEX_HTML);
  });
  
  server.on("/open", []() {

    digitalWrite(OPEN_PIN, HIGH);
    
    timer.setTimeout(OPEN_HOLD_MS, []() {
      digitalWrite(OPEN_PIN, LOW);
    });   

    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    
    server.send_P(200, TEXT_HTML, STATIC_OPEN_HTML);
  });
    
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  timer.run();
}
