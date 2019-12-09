#include <Arduino.h>
#define GCLK_GENCTRL_SRC_DPLL96M_Val 0x8ul
#define GCLK_GENCTRL_SRC_DPLL96M (GCLK_GENCTRL_SRC_DPLL96M_Val << GCLK_GENCTRL_SRC_Pos)

class TurboPWM {
  public:
    void setClockDivider(unsigned int GCLKDiv, bool turbo);
    int timer(int timernumber, unsigned int TCCDiv, unsigned long int sts, bool fastPWM);
    int analogWrite(unsigned int pin, unsigned int dC);
    void enable(unsigned int timerNumber, bool enabled);
    float frequency(unsigned int timerNumber);
  private:
    unsigned int _GCLKDiv = 1;         // Main clock divider: 1 to 255 for both TCC0 and TCC1
    unsigned int _TCCDiv0 = 1;         // TTC0 clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
    unsigned int _TCCDiv1 = 1;         // TTC1 clock divider: 1, 2, 4, 8, 16, 64, 256, or 1024
    unsigned long int _sts0 = 500000;  // Timer 0 PWM steps (resolution): 2 to 0xFFFFFF (24 bits)
    unsigned long int _sts1 = 500000;  // Timer 1 PWM steps (resolution): 2 to 0xFFFFFF (24 bits)
    bool _fastPWM0 = false;            // False for phase-correct aka dual-slope PWM, true for fast aka normal aka single-slope PWM
    bool _fastPWM1 = false;            // False for phase-correct aka dual-slope PWM, true for fast aka normal aka single-slope PWM
    bool _enabled0 = true;             // Shows if TCC0 is enabled
    bool _enabled1 = true;             // Shows if TCC1 is enabled
    bool _turbo = false;               // False for 48MHz clock, true for 96MHz clock
};

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

#elif defined (ARDUINO_SAMD_ZERO) || \
      defined (ARDUINO_SAMD_MKRZERO) || \
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
