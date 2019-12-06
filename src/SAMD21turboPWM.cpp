#include "SAMD21turboPWM.h"

void turboPWM::setClockDivider(unsigned int GCLKDiv) {
  //Limit GCLKDiv to 1 - 255
  if (GCLKDiv < 1) {
    GCLKDiv = 1;
  }
  if (GCLKDiv > 255) {
    GCLKDiv = 255;
  }
  _GCLKDiv = GCLKDiv;

  //Select DFLL48M as clock source and GCLK4 as GCLK and connect them with prescaler _GCLKDiv
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(_GCLKDiv) | GCLK_GENDIV_ID(4);
  while (GCLK->STATUS.bit.SYNCBUSY);
  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
  while (GCLK->STATUS.bit.SYNCBUSY);

  //Connect GCLCK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_TCC0_TCC1;
  while (GCLK->STATUS.bit.SYNCBUSY);
}

int turboPWM::timer(int timernumber, unsigned int TCCDiv, unsigned long int sts, bool fastPWM) {
  //Derive TCC prescaler from parameter TCCDiv; default to 4
  unsigned int my_TCC_CTRLA_PRESCALER_DIV;
  if (TCCDiv == 1) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV1_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 2) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV2_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 4) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV4_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 8) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV8_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 16) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV16_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 64) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV64_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 256) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV256_Val << TCC_CTRLA_PRESCALER_Pos;
  } else if (TCCDiv == 1024) {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV1024_Val << TCC_CTRLA_PRESCALER_Pos;
  } else {
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV4_Val << TCC_CTRLA_PRESCALER_Pos;
    TCCDiv = 4;
  }

  //Limit resolution to 24 bits
  if (sts < 2) {
    sts = 2;
  }
  if (sts > 0xFFFFFF) {
    sts = 0xFFFFFF;
  }
  
  //Set prescaler TCCDiv for TCCx, select single or dual slope PWM, and set the resolution
  if (timernumber == 0) {
    _TCCDiv0 = TCCDiv;
    _sts0 = sts;
    _fastPWM0 = fastPWM;
    REG_TCC0_CTRLA |= my_TCC_CTRLA_PRESCALER_DIV;
    if (_fastPWM0) {
      REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_NPWM;
    } else if (!_fastPWM0) {
      REG_TCC0_WAVE |= TCC_WAVE_POL(0xF) | TCC_WAVE_WAVEGEN_DSBOTTOM;
    }
    while (TCC0->SYNCBUSY.bit.WAVE);
    REG_TCC0_PERB = _sts0;
    while (TCC0->SYNCBUSY.bit.PERB);
    enable(timernumber, _enabled0);
    return 1;
  } else if (timernumber == 1) {
    _TCCDiv1 = TCCDiv;
    _sts1 = sts;
    _fastPWM1 = fastPWM;
    REG_TCC1_CTRLA |= my_TCC_CTRLA_PRESCALER_DIV;
    if (_fastPWM1) {
      REG_TCC1_WAVE |= TCC_WAVE_WAVEGEN_NPWM;
    } else if (!_fastPWM1) {
      REG_TCC1_WAVE |= TCC_WAVE_POL(0xF) | TCC_WAVE_WAVEGEN_DSBOTTOM;
    }
    while (TCC1->SYNCBUSY.bit.WAVE);
    REG_TCC1_PERB = _sts1;
    while (TCC1->SYNCBUSY.bit.PERB);
    enable(timernumber, _enabled1);
    return 1;
  } else {
    return 0;
  }
}

int turboPWM::analogWrite(unsigned int pin, unsigned int dC) {
  //Check if an acceptable pin is used
  if (pin != 4 && pin != 5 && pin != 6 && pin != 7 && pin != 8 && pin != 13) {
    return 0;
  }

  //limit dutycycle to 0-1000
  if (dC < 0) {
    dC = 0;
  }
  if (dC > 1000) {
    dC = 1000;
  }
  
  //Enable a SAMD21 pin as multiplexed and connect it to a pin using the port multiplexer
  unsigned int realPort = g_APinDescription[pin].ulPort;
  unsigned int realPin = g_APinDescription[pin].ulPin;
  unsigned long int my_PORT_PMUX;
  if (pin == 4) {
    my_PORT_PMUX = PORT_PMUX_PMUXO_E;
  } else if (pin == 5) {
    my_PORT_PMUX = PORT_PMUX_PMUXO_E;
  } else if (pin == 6) {
    my_PORT_PMUX = PORT_PMUX_PMUXE_E;
  } else if (pin == 7) {
    my_PORT_PMUX = PORT_PMUX_PMUXE_E;
  } else if (pin == 8) {
    my_PORT_PMUX = PORT_PMUX_PMUXE_F;
  } else if (pin == 13) {
    my_PORT_PMUX = PORT_PMUX_PMUXO_F;
  }
  PORT->Group[realPort].PINCFG[realPin].bit.PMUXEN = 1;
  PORT->Group[realPort].PMUX[realPin >> 1].reg = my_PORT_PMUX;
  
  //Set duty cycle
  if (pin == 4) {
    REG_TCC1_CCB1 = (_sts1 * dC) / 1000;
    while (TCC1->SYNCBUSY.bit.CCB1);
  } else if (pin == 5) {
    REG_TCC0_CCB1 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB1);
  } else if (pin == 6) {
    REG_TCC0_CCB0 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB0);
  } else if (pin == 7) {
    REG_TCC1_CCB0 = (_sts1 * dC) / 1000;
    while (TCC1->SYNCBUSY.bit.CCB0);
  } else if (pin == 8) {
    REG_TCC0_CCB2 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB2);
  } else if (pin == 13) {
    REG_TCC0_CCB3 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB3);
  }
  return pin;
}

void turboPWM::enable(int timernumber, bool enabled) {
  if (timernumber == 0) {
    _enabled0 = enabled;
    if (_enabled0) {
      REG_TCC0_CTRLA |= TCC_CTRLA_ENABLE;
    } else {
      REG_TCC0_CTRLA &= ~(TCC_CTRLA_ENABLE);
    }
    while (TCC0->SYNCBUSY.bit.ENABLE);
  } else if (timernumber == 1) {
    _enabled1 = enabled;
    if (_enabled1) {
      REG_TCC1_CTRLA |= TCC_CTRLA_ENABLE;
    } else {
      REG_TCC1_CTRLA &= ~(TCC_CTRLA_ENABLE);
    }
    while (TCC1->SYNCBUSY.bit.ENABLE);
  }
}

float turboPWM::frequency(int timer) {
  float fastDivider;
  if (timer == 0) {
    if (_fastPWM0) {
      fastDivider = 1.0;
    } else {
      fastDivider = 2.0;
    }
    return (static_cast<float>(VARIANT_MCK) / (fastDivider * _GCLKDiv * _TCCDiv0 * _sts0));
  } else if (timer == 1) {
    if (_fastPWM1) {
      fastDivider = 1.0;
    } else {
      fastDivider = 2.0;
    }
    return (static_cast<float>(VARIANT_MCK) / (fastDivider * _GCLKDiv * _TCCDiv1 * _sts1));
  }
}
