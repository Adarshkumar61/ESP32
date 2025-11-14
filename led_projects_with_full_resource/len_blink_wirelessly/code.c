#include <WiFi.h>

const char* ssid = "ESP32_LED";
const char* password = "adarsh1234";

WiFiServer server(80);
const int ledPin = 2;

