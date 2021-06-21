*Note: Tested on the Arduino Nano 33 IoT and the Adafruit Trinket M0; untested on the Arduino Zero and the Arduino MKR series, but should work. Let me know about problems.*

## A fast PWM library for SAMD21G-based Arduinos

Uses the SAMD21's timers TCC0, TCC1, and TCC2 to generate PWM signals on eight pins (four on the Adafruit Trinket M0):
* Nano 33 IoT: 4, 5, 6, 7, 8, 11, 12, and 13;
* Arduino Zero (untested): 3, 4, 8, 9, 10, 11, 12, and 13; 
* MKR series (untested): 2, 3, 4, 5, 6, 7, 8, and 9;
* Adafruit Trinket M0: 0, 2, 3 and 4.

### Thanks go to ...

... Martin L. and Shawn Hymel for important parts of the code, and for explaining esoteric and dark stuff, and to infra223 for adding the Adafruit Trinket M0 support.

### Usage:

*Allowed values in square brackets.*

**Include the library:**

```#include <SAMD21turboPWM.h>```

**Create a TurboPWM object (called pwm here, but name as you wish):**

```TurboPWM pwm;```

Set input clock divider and Turbo Mode (which uses a 96MHz instead of a 48Mhz input clock):

```pwm.setClockDivider([1-255], [true, false]);```

**Initialise timer x, with prescaler, with steps (resolution), with fast aka single-slope PWM (or not -> double-slope PWM):**

```pwm.timer([0, 1, 2], [1, 2, 4, 8, 16, 64, 256, 1024], [2-MaxSteps], [true, false]);```

* For the Arduino Nano 33 IoT, you need to initialise timer 1 for pins 4 and 7, timer 0 for pins 5, 6, 8, and 12, and timer 2 for pins 11 and 13;
* For the Arduino Zero (untested), you need to initialise timer 0 for pins 3, 4, 10 and 12, timer 1 for pins 8 and 9, and timer 2 for pins 11 and 13;
* For the Arduino MKR series (untested), you need to initialise timer 1 for pins 2 and 3, timer 0 for pins 4, 5, 6 and 7, and timer 2 for pins 8 and 9;
* For the Adafruit Trinket M0, you need to initialise timer 0 for pins 0 and 2, and timer 1 for pins 3 and 4.

MaxSteps is 0xFFFFFF for (24-bits) timers 0 and 1, and 0xFFFF for (16 bits) timer 2.

**Start PWM on a pin with a duty cycle:**

```pwm.analogWrite([pin number], [0-1000]);```

**Enable/disable a timer:**

```pwm.enable([0, 1, 2], [true, false]);```

**Get a timer's current PWM frequency:**

```pwm.frequency([0, 1, 2]);```

Also see the header file and the examples.

If you feel like donating for this library, you can do so here: http://ocrdu.nl/donations .