#include <Arduino.h>
#define GCLK_GENCTRL_SRC_DPLL96M_Val 0x8ul
#define GCLK_GENCTRL_SRC_DPLL96M (GCLK_GENCTRL_SRC_DPLL96M_Val << GCLK_GENCTRL_SRC_Pos)

typedef struct { 
  unsigned int arduinoPin;
  unsigned int port; 
  unsigned int samd21Pin; 
  unsigned int countRegister;
  unsigned long int pMux;
} PinLookup;

//This is the pin table for the Arduino Nano 33 IOT
static const PinLookup pinTable[] = {{},
                                     {}, 
                                     {}, 
                                     {},
                                     { 4, PORTA,  7, 11, PORT_PMUX_PMUXO_E},
                                     { 5, PORTA,  5, 01, PORT_PMUX_PMUXO_E},
                                     { 6, PORTA,  4, 00, PORT_PMUX_PMUXE_E},
                                     { 7, PORTA,  6, 10, PORT_PMUX_PMUXE_E},
                                     { 8, PORTA, 18, 02, PORT_PMUX_PMUXE_F},
                                     {}, 
                                     {}, 
                                     {}, 
                                     {},
                                     {13, PORTA, 17, 03, PORT_PMUX_PMUXO_F}};
static const unsigned int pinTableSize = sizeof(pinTable) / sizeof(pinTable[0]);

class TurboPWM {
  public:
    void setClockDivider(unsigned int GCLKDiv, bool turbo);
    int timer(int timernumber, unsigned int TCCDiv, unsigned long int sts, bool fastPWM);
    int analogWrite(unsigned int pin, unsigned int dC);
    void enable(unsigned int timerNumber, bool enabled);
    float frequency(unsigned int timerNumber);
  private:
    unsigned int _GCLKDiv = 1;        // Main clock divider: 1 to 255 for both TCC0 and TCC1
    unsigned int _TCCDiv0 = 1;        // TTC0 clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
    unsigned int _TCCDiv1 = 1;        // TTC1 clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
    unsigned long int _sts0 = 500000; // Timer 0 PWM steps (resolution): 2 to 0xFFFFFF (24 bits)
    unsigned long int _sts1 = 500000; // Timer 1 PWM steps (resolution): 2 to 0xFFFFFF (24 bits)
    bool _fastPWM0 = false;           // False for phase-correct aka dual-slope PWM, true for fast aka normal aka single-slope PWM
    bool _fastPWM1 = false;           // False for phase-correct aka dual-slope PWM, true for fast aka normal aka single-slope PWM
    bool _enabled0 = true;            // Shows if TCC0 is enabled
    bool _enabled1 = true;            // Shows if TCC1 is enabled
    bool _turbo = false;
};
