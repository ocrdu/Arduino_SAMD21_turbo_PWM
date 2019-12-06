#include <Arduino.h>

class turboPWM {
  public:
    void setClockDivider(unsigned int GCLKDiv);
    int timer(int timernumber, unsigned int TCCDiv, unsigned long int sts, bool fastPWM);
    int analogWrite(unsigned int pin, unsigned int dC);
    void enable(int timernumber, bool enabled);
    float frequency(int timer);
  private:
    unsigned int _GCLKDiv = 1;        // Main clock divider: 1 to 255 for both TCC0 and TCC1
    unsigned int _TCCDiv0 = 1;        // TTC0 clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
    unsigned int _TCCDiv1 = 1;        // TTC1 clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
    unsigned long int _sts0 = 500000; // Timer 0 PWM steps (resolution): 2 to 0xFFFFFF
    unsigned long int _sts1 = 500000; // Timer 1 PWM steps (resolution): 2 to 0xFFFFFF
    bool _fastPWM0 = false;           // False for phase-correct PWM, true for fast aka normal PWM
    bool _fastPWM1 = false;           // False for phase-correct PWM, true for fast aka normal PWM
    bool _enabled0 = true;            // Shows if TCC0 is enabled
    bool _enabled1 = true;            // Shows if TCC1 is enabled
};
