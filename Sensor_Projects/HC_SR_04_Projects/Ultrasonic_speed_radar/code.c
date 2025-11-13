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

