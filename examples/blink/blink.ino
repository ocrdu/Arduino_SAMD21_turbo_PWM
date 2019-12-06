// Uses the built-in LED.

#include <SAMD21turboPWM.h>

TurboPWM pwm;

void setup() {
  pwm.setClockDivider(1, false);
  pwm.timer(0, 16, 6000000, true);
  pwm.analogWrite(13, 500);
}

void loop() {}
