#include<ESP32Servo.h>

servo s;

void setup() {
    s.attach(13);
    s.write(90);

}
void loop() {}