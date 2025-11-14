#include <WiFi.h>

const char* ssid = "ESP32_LED";
const char* password = "adarsh1234";

WiFiServer server(80);
const int ledPin = 2;

void setup() {
  pinMode(ledPin, OUTPUT);
  WiFi.softAP(ssid, password);
  server.begin();
}

