#include <Arduino.h>

#define GCLK_GENCTRL_SRC_DPLL96M_Val 0x8ul
#define GCLK_GENCTRL_SRC_DPLL96M (GCLK_GENCTRL_SRC_DPLL96M_Val << GCLK_GENCTRL_SRC_Pos)

class TurboPWM {
  public:
    void setClockDivider(unsigned int GCLKDiv, bool turbo);
    int timer(int timernumber, unsigned int TCCDiv, unsigned long long int sts, bool fastPWM);
    int analogWrite(unsigned int pin, unsigned int dutyCycle);  //Duty cycle will be (dutyCycle / _maxDutyCycle) * 100%
    void enable(unsigned int timerNumber, bool enabled);
    float frequency(unsigned int timerNumber);
  private:
    unsigned int _GCLKDiv = 1;                // Main clock divider: 1 to 255 for both TCC0 and TCC1
    bool _turbo = false;                      // False for 48MHz clock, true for 96MHz clock
    unsigned const int _maxDutyCycle = 1000;  // The maximum duty cycle number; duty cycle will be (dutyCycle / _maxDutyCycle) * 100%
};

//Table for looking up and storing values for TCCx
typedef struct {
  const unsigned long int counterSize;  // 24 bits for TCC0 and TCC1, so 0xFFFFFF
  unsigned int TCCDiv;                  // TTCx clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
  unsigned long long int sts;           // PWM steps (resolution): 2 to counterSize
  bool fastPWM;                         // False for phase-correct aka dual-slope PWM, true for fast aka normal aka single-slope PWM
  bool enabled;                         // Shows if TCCx should be enabled
} TimerLookup;

static TimerLookup timerTable[] = {
  {0xFFFFFF, 1, 500000, false, true},
  {0xFFFFFF, 1, 500000, false, true}
};
static const unsigned int timerTableSize = sizeof(timerTable) / sizeof(timerTable[0]);

// Tables for looking up pin mappings etc. for different boards
typedef struct { 
  int arduinoPin;
  unsigned int port; 
  unsigned int samd21Pin; 
  unsigned int countRegister;
  unsigned long int pMux;
} PinLookup;

static const PinLookup pinTable[] = {
#if defined (ARDUINO_SAMD_NANO_33_IOT)
//Table begin
{-1},
{-1}, 
{-1}, 
{-1},
{ 4, PORTA,  7, 0x11, PORT_PMUX_PMUXO_E},
{ 5, PORTA,  5, 0x01, PORT_PMUX_PMUXO_E},
{ 6, PORTA,  4, 0x00, PORT_PMUX_PMUXE_E},
{ 7, PORTA,  6, 0x10, PORT_PMUX_PMUXE_E},
{ 8, PORTA, 18, 0x02, PORT_PMUX_PMUXE_F},
{-1}, 
{-1}, 
{-1}, 
{-1},
{13, PORTA, 17, 0x03, PORT_PMUX_PMUXO_F}
//Table end

#elif defined (ARDUINO_SAMD_MKRZERO) || \
      defined (ARDUINO_SAMD_MKR1000) || \
      defined (ARDUINO_SAMD_MKR1010) || \
      defined (ARDUINO_SAMD_MKRFox1200) || \
      defined (ARDUINO_SAMD_MKRWAN1300) || \
      defined (ARDUINO_SAMD_MKRWAN1310) || \
      defined (ARDUINO_SAMD_MKRGSM1400) || \
      defined (ARDUINO_SAMD_MKRNB1500) || \
      defined (ARDUINO_SAMD_MKRVIDOR4000)
//Table begin
{-1},
{-1}, 
{ 2, PORTA, 10, 0x10, PORT_PMUX_PMUXE_E}, 
{ 3, PORTA, 11, 0x11, PORT_PMUX_PMUXO_E},
{ 4, PORTB, 10, 0x00, PORT_PMUX_PMUXE_F},
{ 5, PORTB, 11, 0x01, PORT_PMUX_PMUXO_F},
{ 6, PORTA, 20, 0x02, PORT_PMUX_PMUXE_F},
{ 7, PORTA, 21, 0x03, PORT_PMUX_PMUXO_F}
//Table end

#else
  #error Board not supported by Turbo PWM Library
#endif
};
static const unsigned int pinTableSize = sizeof(pinTable) / sizeof(pinTable[0]);
