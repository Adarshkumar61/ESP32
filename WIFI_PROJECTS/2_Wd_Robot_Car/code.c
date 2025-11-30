// #checked:
// isme ena and enb ke jumpers hain
#include <WiFi.h>
#include <WebServer.h>

// Robot apna WiFi banayega
const char* ssid     = "MyRobot";
const char* password = "12345678";

WebServer server(80);

// L298N pins
#define ENA  14
#define IN1  26
#define IN2  27
#define IN3  12
#define IN4  13
#define ENB  33

void setup() {
  // Pins ko output banao
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopp();   // shuru mein band rakho

  // Apna WiFi chalu karo
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.begin(115200);
  Serial.println();
  Serial.println("==================================");
  Serial.print("WiFi naam: "); Serial.println(ssid);
  Serial.println("Password: 12345678");
  Serial.println("Phone se connect karo aur");
  Serial.println("Browser mein kholo → http://192.168.4.1");
  Serial.println("==================================");

  // Web pages
  server.on("/", []() {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                  "<style>body{background:#000;color:#fff;text-align:center;padding:40px;font-family:Arial;}"
                  ".btn{width:140px;height:140px;border-radius:50%;font-size:50px;margin:20px;background:#2980b9;color:white;border:none;}"
                  ".btn:active{background:#e74c3c;}</style></head><body><h1>Robot</h1>"
                  "<button class='btn' onmousedown=\"fetch('/f')\" ontouchstart=\"fetch('/f')\" "
                  "onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Forward</button><br>"
                  "<button class='btn' onmousedown=\"fetch('/l')\" ontouchstart=\"fetch('/l')\" "
                  "onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Left</button>"
                  "<button class='btn' onmousedown=\"fetch('/s')\" ontouchstart=\"fetch('/s')\">Stop</button>"
                  "<button class='btn' onmousedown=\"fetch('/r')\" ontouchstart=\"fetch('/r')\" "
                  "onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Right</button><br>"
                  "<button class='btn' onmousedown=\"fetch('/b')\" ontouchstart=\"fetch('/b')\" "
                  "onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Backward</button>"
                  "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/f", [](){ forward();  });
  server.on("/b", [](){ backward(); });
  server.on("/l", [](){ left();     });
  server.on("/r", [](){ right();    });
  server.on("/s", [](){ stopp();    });

  server.begin();
}

void loop() {
  server.handleClient();
}

// Motor functions
void forward()  { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);   analogWrite(ENA,255);
                  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);   analogWrite(ENB,255); }
void backward() { digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);  analogWrite(ENA,255);
                  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);  analogWrite(ENB,255); }
void left()     { digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);  analogWrite(ENA,220);
                  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);   analogWrite(ENB,220); }
void right()    { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);   analogWrite(ENA,220);
                  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);  analogWrite(ENB,220); }
void stopp()    { analogWrite(ENA,0); analogWrite(ENB,0); }


// # not checked:
// isme ena and enb ke jumpers nahi hain:

#include <WiFi.h>
#include <WebServer.h>

// Robot ka apna WiFi
const char* ssid     = "MyRobot";
const char* password = "12345678";

WebServer server(80);

// L298N pins (jumpers of ENA & ENB removed)
#define ENA  14
#define IN1  26
#define IN2  27
#define IN3  12
#define IN4  13
#define ENB  33

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopp();   // shuru mein band

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.begin(115200);
  Serial.println();
  Serial.println("WiFi: MyRobot");
  Serial.println("Pass: 12345678");
  Serial.println("Open → http://192.168.4.1");

  server.on("/", []() {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                  "<style>body{background:#000;color:#fff;text-align:center;padding:50px;font-family:Arial;}"
                  "h1{font-size:50px;} .btn{width:140px;height:140px;border-radius:50%;font-size:50px;margin:20px;background:#2980b9;color:white;border:none;}"
                  ".btn:active{background:#e74c3c;}</style></head><body><h1>Robot</h1>"
                  "<button class='btn' onmousedown=\"fetch('/f')\" ontouchstart=\"fetch('/f')\" onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Forward</button><br>"
                  "<button class='btn' onmousedown=\"fetch('/l')\" ontouchstart=\"fetch('/l')\" onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Left</button> "
                  "<button class='btn' onmousedown=\"fetch('/s')\" ontouchstart=\"fetch('/s')\">Stop</button> "
                  "<button class='btn' onmousedown=\"fetch('/r')\" ontouchstart=\"fetch('/r')\" onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Right</button><br>"
                  "<button class='btn' onmousedown=\"fetch('/b')\" ontouchstart=\"fetch('/b')\" onmouseup=\"fetch('/s')\" ontouchend=\"fetch('/s')\">Backward</button>"
                  "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/f", [](){ forward();  });
  server.on("/b", [](){ backward(); });
  server.on("/l", [](){ left();     });
  server.on("/r", [](){ right();    });
  server.on("/s", [](){ stopp();    });

  server.begin();
  Serial.println("Robot ready!");
}

void loop() {
  server.handleClient();
}

// Motor functions (full power + perfect stop)
void forward()  { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);   analogWrite(ENA,255);
                  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);   analogWrite(ENB,255); }
void backward() { digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);  analogWrite(ENA,255);
                  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);  analogWrite(ENB,255); }
void left()     { digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);  analogWrite(ENA,230);
                  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);   analogWrite(ENB,230); }
void right()    { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);   analogWrite(ENA,230);
                  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);  analogWrite(ENB,230); }

// Perfect STOP (ab ekdum dead rukega)
void stopp() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}