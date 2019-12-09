*Note: Still under development. Seems to work fine, though.*

## A first attempt at a fast PWM library for SAMD21G-based Arduinos

### Usage:

*Allowed values in square brackets.*

Include the library:

```#include <SAMD21turboPWM.h>```

Create a TurboPWM object:

```TurboPWM pwm;```

Set input clock divider and Turbo Mode (which uses a 96MHz instead of a 48Mhz input clock):

```pwm.setClockDivider([1-255], [true-false]);```

Initialise timer x, with prescaler, with steps/resolution, with fast aka single-slope PWM (or not -> double-slope PWM):

```pwm.timer([0-1], [1, 2, 4, 8, 16, 64, 256, 1024], [2-0xFFFFFF], [true-false]);```

For the Arduino Nano 33 IoT, you need to initialise timer 0 for pins 5, 6, 8, and 13, and timer 1 for pins 4 and 7.\
For the Arduino MKR series (untested), you need to initialise timer 1 for pins 2 and 3, and timer 0 for pins 4, 5, 6 and 7.

Start PWM on a pin with a duty cycle:

```pwm.analogWrite([pin number], [0-1000]);```

Also see the header file and the examples.

All help, input, and testing is welcome.