// Uses the built-in LED and an extra LED on pin 7. Make sure you don't pull more than 7mA from any pin.

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

turboPWM pwm;
const int statusLED = 13;
const int otherLED = 7;

void setup() {
  pwm.setClockDivider(1);           // 48Mhz clock is divided by 1 and sent to Generic Clock
  pwm.timer(0, 16, 750000, true);   // Timer 0 is set to Generic Clock divided by 16, resolution is 750000, fast PWM 
  pwm.timer(1, 1, 240, false);      // Timer 1 is set to Generic Clock divided by 1, resolution is 240, phase-correct aka dual-slope PWM

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

 pwm.timer(1, 1, 48, true); 
 pwm.analogWrite(otherLED, 600);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 600/1000\n");

 pwm.enable(1, true);
  delay(2000);

 pwm.analogWrite(otherLED, 1000);
  Serial.print("PWM frequency: "); Serial.print(pwm.frequency(1)); Serial.println("Hz");
  Serial.println("Duty cycle: 1000/1000\n");
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
