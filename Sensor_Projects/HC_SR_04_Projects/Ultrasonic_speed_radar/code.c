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
