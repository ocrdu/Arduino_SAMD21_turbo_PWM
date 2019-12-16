// Uses two orange LEDS plus resistors on pins 4 and 7 (on the Nano 33 IoT; change for other boards).
// Don't pull more than 7mA from any pin.

#include <SAMD21turboPWM.h>

TurboPWM pwm;

void setup() {
  pwm.setClockDivider(1, false);
  pwm.timer(1, 1, 1000, true);
}

void loop() {
  pwm.analogWrite(4, random(500, 1000));
  delay(25);
  pwm.analogWrite(7, random(300, 1000));
  delay(25);
}
