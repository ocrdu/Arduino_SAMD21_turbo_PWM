// Note: Uses pin 13 as the LED pin; may need changing for other boards

#include <SAMD21turboPWM.h>

TurboPWM pwm;

void setup() {
  pwm.setClockDivider(4, false);
  pwm.timer(0, 1, 12000000, true);
  pwm.analogWrite(13, 500);
}

void loop() {}