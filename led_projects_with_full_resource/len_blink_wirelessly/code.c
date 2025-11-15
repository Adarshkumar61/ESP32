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

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();

  if (req.indexOf("/ON") != -1) digitalWrite(ledPin, HIGH);
  else if (req.indexOf("/OFF") != -1) digitalWrite(ledPin, LOW);

  // Simple webpage
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html\r\n\r\n");
  client.println("<!DOCTYPE html><html><body><center>");
  client.println("<h2>ESP32 LED Control</h2>");
  client.println("<a href='/ON'><button>ON</button></a>");
  client.println("<a href='/OFF'><button>OFF</button></a>");
  client.println("</center></body></html>");
  client.stop();
}

//  or :
//  for more good layout:
// here is the code: 
