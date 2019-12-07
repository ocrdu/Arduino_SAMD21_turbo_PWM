#include "SAMD21turboPWM.h"

void TurboPWM::setClockDivider(unsigned int GCLKDiv, bool turbo) {
  // Limit GCLKDiv to 1 - 255
  if (GCLKDiv < 1) {
    GCLKDiv = 1;
  }
  if (GCLKDiv > 255) {
    GCLKDiv = 255;
  }
  _GCLKDiv = GCLKDiv;
  
  _turbo = turbo;
  
  if (_turbo) {
    // Configure generic clock generator 5
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(5);
    while (GCLK->STATUS.bit.SYNCBUSY);
 
    // Set clock divider of 48 to generic clock generator 5 (1 MHz)
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(48) | GCLK_GENDIV_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);
 
    // Enable GCLK5 and connect it to GCLK_DPLL
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID(1);
    while (GCLK->STATUS.bit.SYNCBUSY);
 
    // Set DPLL ratio to 1 MHz * (95 + 1) = 96 MHz
    SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDRFRAC(0) | SYSCTRL_DPLLRATIO_LDR(95);
 
    // Configure DPLL to disregard phase lock and select GCLK as source
    SYSCTRL->DPLLCTRLB.reg = SYSCTRL_DPLLCTRLB_LBYPASS | SYSCTRL_DPLLCTRLB_WUF | SYSCTRL_DPLLCTRLB_REFCLK(SYSCTRL_DPLLCTRLB_REFCLK_GCLK_Val);
  
    // Enable DPLL
    SYSCTRL->DPLLCTRLA.reg |= SYSCTRL_DPLLCTRLA_ENABLE;
  
    // Configure generic clock generator 4
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);
    _turbo = true;
 
  } else {
    REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);
    _turbo = false;
  }
  
  // Set GCLK4's prescaler
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(_GCLKDiv) | GCLK_GENDIV_ID(4);
  while (GCLK->STATUS.bit.SYNCBUSY);

  // Connect GCLCK4 to TCC0 and TCC1
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | GCLK_CLKCTRL_ID_TCC0_TCC1;
  while (GCLK->STATUS.bit.SYNCBUSY);
}

int TurboPWM::timer(int timernumber, unsigned int TCCDiv, unsigned long int sts, bool fastPWM) {
  // Derive TCC prescaler from parameter TCCDiv; default to 4
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

  // Limit resolution to 24 bits
  if (sts < 2) {
    sts = 2;
  }
  if (sts > 0xFFFFFF) {
    sts = 0xFFFFFF;
  }
  
  // Set prescaler TCCDiv for TCCx, select single or dual slope PWM, and set the resolution
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

int TurboPWM::analogWrite(unsigned int pin, unsigned int dC) {
  // Check if an acceptable pin is used
  int i;
  for (i = 0; i < pinTableSize; i++) {
    if (pinTable[i].arduinoPin == pin) {
      break;
    }
  }
  if (i >= pinTableSize) {
    return 0;
  }

  // limit dutycycle to 0-1000
  if (dC < 0) {
    dC = 0;
  }
  if (dC > 1000) {
    dC = 1000;
  }

  // Enable a SAMD21 pin as multiplexed and connect it to a pin using the port multiplexer
  PORT->Group[pinTable[pin].port].PINCFG[pinTable[pin].samd21Pin].bit.PMUXEN = 1;
  PORT->Group[pinTable[pin].port].PMUX[pinTable[pin].samd21Pin >> 1].reg = pinTable[pin].pMux;

  // Set duty cycle
  if (pinTable[pin].countRegister == 11) {
    REG_TCC1_CCB1 = (_sts1 * dC) / 1000;
    while (TCC1->SYNCBUSY.bit.CCB1);
  } else if (pinTable[pin].countRegister == 01) {
    REG_TCC0_CCB1 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB1);
  } else if (pinTable[pin].countRegister == 00) {
    REG_TCC0_CCB0 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB0);
  } else if (pinTable[pin].countRegister == 10) {
    REG_TCC1_CCB0 = (_sts1 * dC) / 1000;
    while (TCC1->SYNCBUSY.bit.CCB0);
  } else if (pinTable[pin].countRegister == 02) {
    REG_TCC0_CCB2 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB2);
  } else if (pinTable[pin].countRegister == 03) {
    REG_TCC0_CCB3 = (_sts0 * dC) / 1000;
    while (TCC0->SYNCBUSY.bit.CCB3);
  }
  return pin;
}

void TurboPWM::enable(unsigned int timerNumber, bool enabled) {
  if (timerNumber == 0) {
    _enabled0 = enabled;
    if (_enabled0) {
      REG_TCC0_CTRLA |= TCC_CTRLA_ENABLE;
    } else {
      REG_TCC0_CTRLA &= ~(TCC_CTRLA_ENABLE);
    }
    while (TCC0->SYNCBUSY.bit.ENABLE);
  } else if (timerNumber == 1) {
    _enabled1 = enabled;
    if (_enabled1) {
      REG_TCC1_CTRLA |= TCC_CTRLA_ENABLE;
    } else {
      REG_TCC1_CTRLA &= ~(TCC_CTRLA_ENABLE);
    }
    while (TCC1->SYNCBUSY.bit.ENABLE);
  }
}

float TurboPWM::frequency(unsigned int timerNumber) {
  float fastDivider;
  float PLL96M;
  if (_turbo) {
    PLL96M = 0.5;
  } else {
    PLL96M = 1.0;
  }
  if (timerNumber == 0) {
    if (_fastPWM0) {
      fastDivider = 1.0;
    } else {
      fastDivider = 2.0;
    }
    return (static_cast<float>(VARIANT_MCK) / (fastDivider * _GCLKDiv * _TCCDiv0 * _sts0 * PLL96M));
  } else if (timerNumber == 1) {
    if (_fastPWM1) {
      fastDivider = 1.0;
    } else {
      fastDivider = 2.0;
    }
    return (static_cast<float>(VARIANT_MCK) / (fastDivider * _GCLKDiv * _TCCDiv1 * _sts1 * PLL96M));
  }
}
