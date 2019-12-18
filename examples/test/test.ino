// Uses a status LED on pin 13 and an extra LED on pin 7; may need changing for other boards.
// Make sure you don't pull more than 7mA from any pin.

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

TurboPWM pwm;
const int statusLED = 13; // Note: These pin definitions are for the Arduino Nano 33 IoT;
const int otherLED = 7;   //       change them for other boards

void setup() {
  pwm.setClockDivider(1, true);     // Input clock is divided by 1 and sent to Generic Clock, Turbo is On
  pwm.timer(2, 256, 40000, false);  // Timer 2 is set to Generic Clock divided by 256, resolution is 40000, phase-correct aka dual-slope PWM 
  pwm.timer(1, 1, 250, true);       // Timer 1 is set to Generic Clock divided by 1, resolution is 250, normal aka fast aka single-slope PWM

  Serial.begin(115200);
  while(!Serial);

 pwm.analogWrite(statusLED, 500);

 pwm.analogWrite(otherLED, 100);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 100/1000\n");
  delay(2000);
  
 pwm.analogWrite(otherLED, 200);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 200/1000\n");
  delay(2000);

 pwm.enable(1, false);
  delay(2000);

 pwm.timer(1, 1, 96, true); 
 pwm.analogWrite(otherLED, 600);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 600/1000\n");

 pwm.enable(1, true);
  delay(2000);

 pwm.analogWrite(otherLED, 1000);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 1000/1000\n");
  delay(2000);

 pwm.analogWrite(otherLED, 500);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 500/1000\n");
  delay(2000);

 pwm.analogWrite(otherLED, 50);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 50/1000\n");
  delay(2000);

  Serial.println("Pulsing in the loop ...");
}

void loop() {
  pwm.analogWrite(statusLED, 500);
  for (int i = 50; i < 980; i++) {
    pwm.analogWrite(otherLED, i);
    delay(2);
  }
  pwm.analogWrite(statusLED, 0);
  for (int i = 980; i > 50; i--) {
    pwm.analogWrite(otherLED, i);
    delay(1);
  }
}