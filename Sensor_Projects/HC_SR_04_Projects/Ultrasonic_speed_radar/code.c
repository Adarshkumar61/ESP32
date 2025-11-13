#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// --- Display setup (for 0.96" I2C OLED) ---
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// --- Pins ---
#define trigPin 5
#define echoPin 18 // i used 3.3V tolerant pin
#define ledPin  2
#define buzzerPin 4

// --- Variables ---
long duration;
float distanceCm = 0, lastDistance = 0;
unsigned long lastTime = 0;
float speedKmph = 0;
float speedLimit = 10.0;

// --- Function prototypes ---
float getDistance();
void beepBuzzer(int times);

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C speed (faster)
  
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.drawStr(10, 30, "Speed Radar Ready");
  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  float currentDistance = getDistance();
  unsigned long currentTime = millis();

  // Calculate speed
  if (currentDistance > 0 && lastDistance > 0) {
    float deltaDist = fabs(currentDistance - lastDistance);
    float deltaTime = (currentTime - lastTime) / 1000.0;
    if (deltaTime > 0.05 && deltaTime < 1.0) {
      float speedCmPerSec = deltaDist / deltaTime;
      speedKmph = speedCmPerSec * 0.036;
    }
  }

  lastDistance = currentDistance;
  lastTime = currentTime;

  // --- Display update (super fast, non-blocking) ---
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.drawStr(0, 10, "Ultrasonic Speed Radar");

  char buffer[20];
  sprintf(buffer, "Speed: %.1f km/h", speedKmph);
  u8g2.setFont(u8g2_font_fur14_tf);
  u8g2.drawStr(0, 40, buffer);

  if (speedKmph > speedLimit) {
    u8g2.setFont(u8g2_font_6x13_tf);
    u8g2.drawStr(20, 60, "⚠ OVERSPEED!");
    digitalWrite(ledPin, HIGH);
    beepBuzzer(2);
  } else {
    digitalWrite(ledPin, LOW);
  }

  u8g2.sendBuffer();

  Serial.print("Speed: ");
  Serial.print(speedKmph);
  Serial.println(" km/h");

  delay(100);
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 20000); // timeout 20ms (~3.4m)
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2.0;
}

void beepBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(80);
    digitalWrite(buzzerPin, LOW);
    delay(80);
  }
}
