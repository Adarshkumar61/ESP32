// Running lights (4 LEDs)
const int leds[] = {16, 17, 18, 19}; // GPIO pins where the LEDs are connected
const int n = sizeof(leds)/sizeof(leds[0]);
int delayTime = 150; // milliseconds

void setup() {
  for (int i = 0; i < n; ++i) {  
    pinMode(leds[i], OUTPUT); // Set each LED pin as an output
    digitalWrite(leds[i], LOW); // Ensure all LEDs are off initially
  }
  Serial.begin(115200);
  Serial.println("Running lights start");
}

void loop() {
  // forward
  for (int i = 0; i < n; ++i) {
    digitalWrite(leds[i], HIGH);
    delay(delayTime);
    digitalWrite(leds[i], LOW);
  }
  // backward
  for (int i = n - 2; i >= 1; --i) {
    digitalWrite(leds[i], HIGH);
    delay(delayTime);
    digitalWrite(leds[i], LOW);
  }
}
