// Complete integrated code for ESP32:
// IR sensor + Button toggle + LED + Buzzer + OLED (SSD1306 I2C)
// Pins used (match your wiring):
// IR OUT  -> GPIO 13
// LED     -> GPIO 2
// Buzzer  -> GPIO 4
// Button  -> GPIO 14  (use INPUT_PULLUP - button connects to GND)
// OLED I2C: SDA -> GPIO21, SCL -> GPIO22 (address 0x3C typical)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

