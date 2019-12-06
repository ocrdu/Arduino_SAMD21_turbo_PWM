// Uses the built-in LED and an extra LED on pin 7. Make sure you don't pull more than 7mA from any pin.

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

turboPWM pwm;
const int statusLED = 13;
const int otherLED = 7;

void setup() {
  pwm.setClockDivider(1, true);     // Input clock is divided by 1 and sent to Generic Clock, Turbo is On
  pwm.timer(0, 16, 750000, false);  // Timer 0 is set to Generic Clock divided by 16, resolution is 750000, phase-correct aka dual-slope PWM 
  pwm.timer(1, 1, 255, true);       // Timer 1 is set to Generic Clock divided by 1, resolution is 255, normal aka fast aka single-slope PWM

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