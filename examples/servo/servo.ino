// Untested with a real servo, use at your own risk.

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

TurboPWM servo;
const int servoPin = 7;
const int servoMinTime = 1000; // Microseconds
const int servoMaxTime = 2000; // Microseconds

void setServo(const int pin, int microSeconds) {
  if (microSeconds < servoMinTime) {
    microSeconds = servoMinTime;
  }
  if (microSeconds > servoMaxTime) {
    microSeconds = servoMaxTime;
  }
  servo.analogWrite(pin, microSeconds / 20);
}

void setup() {
  //Set timers 1 and 2 to 50Hz PWM with a resolution of 480000 
  servo.setClockDivider(1, false);   // Input clock is divided by 1 and 48MHz is sent to Generic Clock, Turbo is off
  servo.timer(0, 1, 480000, false);  // Timer 0 is set to Generic Clock divided by 1, resolution is 480000, phase-correct aka dual-slope PWM
  servo.timer(1, 1, 480000, false);  // Timer 1 is set to Generic Clock divided by 1, resolution is 480000, phase-correct aka dual-slope PWM

  Serial.begin(115200);
  while(!Serial);
  
  Serial.print("Timer 0 frequency is "); Serial.print(servo.frequency(0)); Serial.println("Hz");
  Serial.print("Timer 1 frequency is "); Serial.print(servo.frequency(1)); Serial.println("Hz\n");
  Serial.print("Driving servo on pin "); Serial.print(servoPin); Serial.print(" ...");
}

void loop() {
  setServo(servoPin, 1000);
  delay(2000);
  setServo(servoPin, 1500);
  delay(2000);
  setServo(servoPin, 2000);
  delay(2000);
  setServo(servoPin, 1500);
  delay(2000);
}