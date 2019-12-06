## A PWM library for the Arduino Nano 33 IoT

A basic PWM library implementing two independent PWM outputs; provides a simple interface for setting PWM frequency, resolution, and duty cycle on pin 10, 12, 13, or 16, and pin 7 or 17.

### Kudos

This library is mainly a configuration wrapper around code Martin L. came up with and published on the Arduino forum.

### Documentation

This library implements two independent PWM outputs, using TCC0 and TCC1.

TCC0's output can be sent to pins 10, 12, 13, or 16 (16 = A2), and TCC1's output can be sent to pins 7 or 17 (17 = A3). The two PWM outputs are only independent if you pick one from the TCC0 pins, and the other from the TCC1 pins.

Syntax:

**PWM your_pwm_name = PWM(Pin, Clockdivider, Resolution, FastPWM)**

If "FastPWM" is false, this creates a phase-correct PWM output with a frequency of 24Mhz / (Resolution * Clockdivider), so a resolution of 500 steps will give a PWM frequency of 48kHz if the clockdivider is set to 1.

If "FastPWM" is true, you get a normal aka fast PWM output with a frequency of 48Mhz / (Resolution * Clockdivider); twice as fast, but not as pretty.

* "Pin" can be 10, 12, 13, 16, 7, or 17. The first four will automatically use TCC0, the other two TCC1;
* "Clockdivider" (prescaler) can be 1, 2, 4, 8, 16, 64, 256, or 1024;
* "Resolution" can be 4 to 10000000.

**your_pwm_name.steps(Resolution)**

Sets the resolution; the higher the resolution, the lower the PWM frequency.

**your_pwm_name.(Dutycycle)**

Sets the duty cycle. Range: 0-1000. Note that this number is independent of resolution; the function sets the duty cycle to 0‰-1000‰ of the resolution.

**your_pwm_name.enable(true/false)**

Enables/disables the PWM output.

**your_pwm_name.frequency()**

Returns the current PWM frequency.

Also see the header file and example programs for further enlightenment.

### Notes:

* Made for, and only tested on, the Arduino Nano 33 IoT, so if you have another SAMD21 board YMMV, and your pin mappings may too;

* If you create two PWM outputs with pins that go with TCC0/TCC1, like 12 and 13, then these pins will have the same settings and will show the same output, whichever you set last;

* It would be greatly appreciated if you could test this (and/or make this work) on other boards;

* All suggestions for added functionality are welcome, as long as they don't complicate the interface too much.

If you feel like donating for this, you can do so here: http://ocrdu.nl/donations .