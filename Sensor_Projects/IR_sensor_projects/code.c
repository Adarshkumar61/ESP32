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

// pins
const int irPin     = 13;
const int ledPin    = 2;
const int buzzerPin = 4;
const int buttonPin = 14;

// button debounce
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
int lastButtonReading = HIGH;
int stableButtonState = HIGH; // stable reading after debounce

// system state
bool systemOn = false;

// display state tracking to avoid unnecessary redraws
enum DisplayState { DS_OFF, DS_MONITOR, DS_OBSTACLE };
DisplayState currentDisplay = DS_OFF;

void setup() {
  Serial.begin(115200);

  // pins
  pinMode(irPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // button to GND

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // OLED init
  Wire.begin(); // SDA=21, SCL=22 default on most ESP32 boards
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed - check wiring or address (0x3C/0x3D).");
    for (;;); // stop here if OLED not found
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();
  delay(500);

  // initial screen
  updateDisplay(DS_OFF);
}

void loop() {
  // -------- Button handling with debounce (toggle systemOn) ----------
  int reading = digitalRead(buttonPin); // HIGH (not pressed) / LOW (pressed)
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // stable reading
    if (reading != stableButtonState) {
      stableButtonState = reading;
      // toggle when button is pressed (goes LOW)
      if (stableButtonState == LOW) {
        systemOn = !systemOn;
        Serial.print("System toggled: ");
        Serial.println(systemOn ? "ON" : "OFF");
        updateDisplay(systemOn ? DS_MONITOR : DS_OFF);
        // brief visual feedback (flash LED once when toggling ON)
        if (systemOn) {
          digitalWrite(ledPin, HIGH);
          delay(120);
          digitalWrite(ledPin, LOW);
        }
      }
    }
  }
  lastButtonReading = reading;

  // -------- IR sensor reading & reactions ----------
  int obstacle = digitalRead(irPin); // many modules output LOW when object detected
  bool detected = false;
  if (obstacle == LOW) detected = true;

  if (systemOn) {
    if (detected) {
      // obstacle detected: turn on LED + buzzer
      digitalWrite(ledPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      updateDisplay(DS_OBSTACLE);
      Serial.println("Obstacle detected!");
    } else {
      // monitoring state
      digitalWrite(ledPin, LOW);
      digitalWrite(buzzerPin, LOW);
      updateDisplay(DS_MONITOR);
    }
  } else {
    // system OFF => everything off
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    // display already set to OFF on toggle, but ensure it's correct here
    updateDisplay(DS_OFF);
  }

  delay(80); // small loop delay (keeps UI responsive but debounced)
}

// ----------------- Helper: update OLED display (only redraw on state change) -------------
void updateDisplay(DisplayState newState) {
  if (newState == currentDisplay) return; // nothing to do

  currentDisplay = newState;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (newState == DS_OFF) {
    display.setTextSize(2);
    display.println("System");
    display.println(" OFF");
    display.setTextSize(1);
    display.println();
    display.println("Press button to ON");
  } else if (newState == DS_MONITOR) {
    display.setTextSize(2);
    display.println("Monitoring");
    display.setTextSize(1);
    display.println();
    display.print("IR: ");
    display.println("Waiting...");
    display.println();
    display.println("Press to turn OFF");
  } else if (newState == DS_OBSTACLE) {
    display.setTextSize(2);
    display.println("! ALERT !");
    display.setTextSize(1);
    display.println();
    display.println("Obstacle detected!");
    display.println("LED+Buzzer ON");
  }

  display.display();
}
