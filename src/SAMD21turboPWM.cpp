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
    // Configure generic clock generator 5 to use DFLL48M
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(5);
    while (GCLK->STATUS.bit.SYNCBUSY);
 
    // Set GCLK5's prescaler to 48 (1 MHz)
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(48) | GCLK_GENDIV_ID(5);
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
  
    // Configure generic clock generator 4 to use DPLL96M
    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);
    _turbo = true;
 
  } else {
    // Configure generic clock generator 4 to use DFLL48M
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

int TurboPWM::timer(int timerNumber, unsigned int TCCDiv, unsigned long long int sts, bool fastPWM) {
  // Check for available timer numbers
  if (timerNumber >= timerTableSize) {
    return 0;
  }
  
  // Derive TCC prescaler from parameter TCCDiv; default to last known setting
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
    my_TCC_CTRLA_PRESCALER_DIV = TCC_CTRLA_PRESCALER_DIV1_Val << TCC_CTRLA_PRESCALER_Pos;
    TCCDiv = timerTable[timerNumber].TCCDiv;
  }

  // Limit resolution to TCCx's counter size
  if (sts < 2) {
    sts = 2;
  }
  if (sts > timerTable[timerNumber].counterSize) {
    sts = timerTable[timerNumber].counterSize;
  }
  
  // Set prescaler TCCDiv for TCCx, select single or dual slope PWM, and set the resolution
  timerTable[timerNumber].TCCDiv = TCCDiv;
  timerTable[timerNumber].sts = sts;
  timerTable[timerNumber].fastPWM = fastPWM;
  if (timerNumber == 0) {
    REG_TCC0_CTRLA |= my_TCC_CTRLA_PRESCALER_DIV;
    if (timerTable[timerNumber].fastPWM) {
      REG_TCC0_WAVE |= TCC_WAVE_WAVEGEN_NPWM;
    } else if (!timerTable[timerNumber].fastPWM) {
      REG_TCC0_WAVE |= TCC_WAVE_POL(0xF) | TCC_WAVE_WAVEGEN_DSBOTTOM;
    }
    while (TCC0->SYNCBUSY.bit.WAVE);
    REG_TCC0_PERB = timerTable[timerNumber].sts;
    while (TCC0->SYNCBUSY.bit.PERB);
  } else if (timerNumber == 1) {
    REG_TCC1_CTRLA |= my_TCC_CTRLA_PRESCALER_DIV;
    if (timerTable[timerNumber].fastPWM) {
      REG_TCC1_WAVE |= TCC_WAVE_WAVEGEN_NPWM;
    } else if (!timerTable[timerNumber].fastPWM) {
      REG_TCC1_WAVE |= TCC_WAVE_POL(0xF) | TCC_WAVE_WAVEGEN_DSBOTTOM;
    }
    while (TCC1->SYNCBUSY.bit.WAVE);
    REG_TCC1_PERB = timerTable[timerNumber].sts;
    while (TCC1->SYNCBUSY.bit.PERB);
  } else {
    return 0;
  }
  enable(timerNumber, timerTable[timerNumber].enabled);
  return 1;
}

int TurboPWM::analogWrite(unsigned int pin, unsigned int dutyCycle) {
  // Check if an acceptable pin is used
  unsigned int i;
  for (i = 0; i < pinTableSize; i++) {
    if (pinTable[i].arduinoPin == pin) {
      break;
    }
  }
  if (i >= pinTableSize) {
    return 0;
  }

  // limit dutycycle to the maximum duty cycle set in the header file; duty cycle will be (dutyCycle / _maxDutyCycle) * 100%
  if (dutyCycle < 0) {
    dutyCycle = 0;
  }
  if (dutyCycle > _maxDutyCycle) {
    dutyCycle = _maxDutyCycle;
  }

  // Enable a SAMD21 pin as multiplexed and connect it to a pin using the port multiplexer
  PORT->Group[pinTable[pin].port].PINCFG[pinTable[pin].samd21Pin].bit.PMUXEN = 1;
  PORT->Group[pinTable[pin].port].PMUX[pinTable[pin].samd21Pin >> 1].reg |= pinTable[pin].pMux;
  
  // Set duty cycle
  if (pinTable[pin].countRegister == 0x00) {
    REG_TCC0_CCB0 = (timerTable[0].sts * dutyCycle) / _maxDutyCycle;
    while (TCC0->SYNCBUSY.bit.CCB0);
  } else if (pinTable[pin].countRegister == 0x01) {
    REG_TCC0_CCB1 = (timerTable[0].sts * dutyCycle) / _maxDutyCycle;
    while (TCC0->SYNCBUSY.bit.CCB1);
  } else if (pinTable[pin].countRegister == 0x02) {
    REG_TCC0_CCB2 = (timerTable[0].sts * dutyCycle) / _maxDutyCycle;
    while (TCC0->SYNCBUSY.bit.CCB2);
  } else if (pinTable[pin].countRegister == 0x03) {
    REG_TCC0_CCB3 = (timerTable[0].sts * dutyCycle) / _maxDutyCycle;
    while (TCC0->SYNCBUSY.bit.CCB3);
  } else if (pinTable[pin].countRegister == 0x10) {
    REG_TCC1_CCB0 = (timerTable[1].sts * dutyCycle) / _maxDutyCycle;
    while (TCC1->SYNCBUSY.bit.CCB0);
  } else  if (pinTable[pin].countRegister == 0x11) {
    REG_TCC1_CCB1 = (timerTable[1].sts * dutyCycle) / _maxDutyCycle;
    while (TCC1->SYNCBUSY.bit.CCB1);
  } else {
    return 0;
  }
  return 1;
}

void TurboPWM::enable(unsigned int timerNumber, bool enabled) {
  timerTable[timerNumber].enabled = enabled;
  if (timerNumber == 0) {
    if (timerTable[timerNumber].enabled) {
      REG_TCC0_CTRLA |= TCC_CTRLA_ENABLE;
    } else {
      REG_TCC0_CTRLA &= ~(TCC_CTRLA_ENABLE);
    }
    while (TCC0->SYNCBUSY.bit.ENABLE);
  } else if (timerNumber == 1) {
    if (timerTable[timerNumber].enabled) {
      REG_TCC1_CTRLA |= TCC_CTRLA_ENABLE;
    } else {
      REG_TCC1_CTRLA &= ~(TCC_CTRLA_ENABLE);
    }
    while (TCC1->SYNCBUSY.bit.ENABLE);
  }
}

float TurboPWM::frequency(unsigned int timerNumber) {
  if (timerNumber >= timerTableSize) {
    return 0;
  }
  unsigned int fastDivider;
  unsigned int PLL96M;
  if (_turbo) {
    PLL96M = 2;
  } else {
    PLL96M = 1;
  }
  if (timerTable[timerNumber].fastPWM) {
    fastDivider = 1;
  } else {
    fastDivider = 2;
  }
  return (static_cast<float>(VARIANT_MCK) * PLL96M) / (fastDivider * _GCLKDiv * timerTable[timerNumber].TCCDiv * timerTable[timerNumber].sts);
}
