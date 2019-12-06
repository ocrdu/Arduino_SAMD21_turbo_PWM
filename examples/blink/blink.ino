// Uses the built-in LED.

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

turboPWM pwm;

void setup() {
  pwm.setClockDivider(1);
  pwm.timer(0, 16, 6000000, true);
  pwm.analogWrite(13, 1000);
}

void loop() {}
