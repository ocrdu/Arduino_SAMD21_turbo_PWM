// Sends a PWM signal to timer 0 pins 5, 6, 8, and 12 (for the Arduino Nano 33 IoT)

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

TurboPWM pwm;

void setup() {
  pwm.setClockDivider(1, false); // Input clock is divided by 1 and sent to Generic Clock, Turbo is Off
  pwm.timer(0, 1, 600, false);   // Timer 2 is set to Generic Clock divided by 1, resolution is 600, phase-correct aka dual-slope PWM 

  Serial.begin(115200);
  while(!Serial);

  pwm.analogWrite(5, 200);
  pwm.analogWrite(6, 400);
  pwm.analogWrite(8, 600);
  pwm.analogWrite(12, 800);
  
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(0), 0); Serial.println("Hz");

}

void loop() {}