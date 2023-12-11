void initializeWDT() {
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);

  // TODO: Configure and enable WDT GCLK:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  // set GCLK->GENCTRL.reg and GCLK->CLKCTRL.reg
  GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(5) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(3);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(3);
  while (GCLK->STATUS.bit.SYNCBUSY);


  // TODO: Configure and enable WDT:
  // use WDT->CONFIG.reg, WDT->EWCTRL.reg, WDT->CTRL.reg
  WDT->CONFIG.reg = WDT_CONFIG_PER(9);
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(8);
  WDT->CTRL.reg |= WDT_CTRL_ENABLE;
  

  // TODO: Enable early warning interrupts on WDT:
  // reference WDT registers with WDT->registername.reg
  WDT->INTENSET.reg |= WDT_INTENSET_EW;
  while (WDT->STATUS.bit.SYNCBUSY);
}

void clearWDT() {
  WDT->CLEAR.reg = 0xA5;
  while (WDT->STATUS.bit.SYNCBUSY);
}

void WDT_Handler() {
  // TODO: Clear interrupt register flag
  // (reference register with WDT->registername.reg)
  WDT->INTFLAG.reg = WDT_INTFLAG_EW;
  
  // TODO: Warn user that a watchdog reset may happen
  Serial.println("Watchdog about to trigger");
}

void initializeTimer() {
  //TODO: Use TC4 (TC3 not configured for 32-bit counters)
  // Configure and enable GCLK for TC:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(4); // do not divide gclk 4
  while(GCLK->STATUS.bit.SYNCBUSY);

  GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_ID(4) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_SRC(6);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4) | GCLK_CLKCTRL_ID_TC4_TC5;
  while(GCLK->STATUS.bit.SYNCBUSY); // write-synchronized

  Serial.println((PM->APBCMASK.reg >> 12) & 1, BIN);
  Serial.println((PM->APBCMASK.reg >> 13) & 1, BIN);
  Serial.println((PM->APBCMASK.reg >> 11) & 1, BIN);
  Serial.println((PM->APBCMASK.reg >> 15) & 1, BIN);

  //TODO: prescaler set to 0x5 (64)
  TC4->COUNT32.CTRLA.reg = TC_CTRLA_ENABLE | TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_PRESCSYNC(1);

  //Enable TC interrupt. Likely never occurs
  NVIC_SetPriority(TC4_IRQn, 0);
  NVIC_EnableIRQ(TC4_IRQn);

  //EVCTRL.OVFEO enable bit to generate event upon overflow/underflow
  //Note that timer values overflow!
  //TODO: test reading from counter register vs 

  //Can i even use interrupts? Maybe just get counter value
}

uint32_t getCurTime() {
  return REG_TC4_COUNT16_COUNT;
}