// Note: Uses pin 13 as the LED pin; may need changing for other boards

#include <SAMD21turboPWM.h>

TurboPWM pwm;

void setup() {
  pwm.setClockDivider(200, false); // Main clock divided by 200 => 240KHz
  pwm.timer(2, 4, 60000, false);   // Use timer 2 for pin 13, divide clock by 4, resolution 60000, dual-slope PWM
  pwm.analogWrite(13, 500);        // PWM frequency is now 0.5Hz, dutycycle is 500 / 1000 * 100% = 50%
}

void loop() {}