// Tested with an Arduino Nano 33 IoT with PWM signal on pin 7 and a Hitec HS-311 servo

// Program will start when Serial Monitor is opened.

#include <SAMD21turboPWM.h>

TurboPWM servo;
const int servoPin = 7;
const int servoMinTime = 600;  // Microseconds
const int servoMaxTime = 2400; // Microseconds

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
  //Set timer 1 to 50Hz single-slope PWM with a resolution of 960000
  servo.setClockDivider(1, false);  // Input clock is divided by 1 and 48MHz is sent to Generic Clock, Turbo is off
  servo.timer(1, 1, 960000, true);  // Timer 1 is set to Generic Clock divided by 1, resolution is 960000, left-aligned aka single-slope PWM

  Serial.begin(115200);
  while(!Serial);
  
  Serial.print("Timer 1 frequency is "); Serial.print(servo.frequency(1)); Serial.println("Hz\n");
  Serial.print("Driving servo on pin "); Serial.print(servoPin); Serial.println(" ...");
}

void loop() {
  setServo(servoPin, 600);
  delay(1000);
  setServo(servoPin, 2400);
  delay(1000);
  setServo(servoPin, 1500);
  delay(2000);
}
