#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <SimpleTimer.h>
#include "static.h"
#include "settings.h"

const PROGMEM char text_html[] = "text/html";
const PROGMEM char text_css[] = "text/css";

////////////////////////////////////////////////////////
// Awesome code....

ESP8266WebServer server(80);
SimpleTimer timer;

typedef struct {
  const char* name;
  const char* route;
  uint8_t pin;
  uint16_t holdMs;
} t_endpoint;

t_endpoint endpoints[] = {
  { "Garagentor", "/relais/garageDoor", D5, 1000 },
  { "Haustür", "/relais/frontDoor",  D6, 1000 }
};

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

  
  server.on("/", []() {
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    
    server.send_P(200, text_html, static_index_html);
  });


  server.on("/relais", []() {
    DynamicJsonBuffer jsonBuffer;
    
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& jsonEndpoints = root.createNestedArray("relais");
    
    for(t_endpoint endpoint : endpoints) {
      JsonObject& entry = jsonBuffer.createObject();
      entry["name"] = endpoint.name;
      entry["url"] = endpoint.route;
    
      jsonEndpoints.add(entry);
    }

    String response = "";
    root.printTo(response);

    server.send(200, "application/json", response);
  });

  for(t_endpoint endpoint : endpoints) {

    pinMode(endpoint.pin, OUTPUT);
    
    server.on(endpoint.route, [endpoint]() {
      digitalWrite(endpoint.pin, HIGH);
      
      timer.setTimeout(endpoint.holdMs, [endpoint]() {
        digitalWrite(endpoint.pin, LOW);
      });   
  
      server.sendHeader("Pragma", "no-cache");
      server.sendHeader("Expires", "0");
      server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");

      server.send(200, "text/plain", "OK");
    });
  }
    
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  timer.run();
}
