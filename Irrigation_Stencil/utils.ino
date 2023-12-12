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
  lcd.clear();
  lcd.print("WDT");
}

uint32_t curTime;

void initializeTimer() {
  // Configure and enable GCLK for TC:
  //Note that we chose to use GCLK0 due to the more accurate frequency
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TC4_TC5;
  while(GCLK->STATUS.bit.SYNCBUSY); // write-synchronized

  //Checks that clocks 4/5 properly set
  Serial.println((PM->APBCMASK.reg >> 12) & 1, BIN);
  Serial.println((PM->APBCMASK.reg >> 13) & 1, BIN);

  //Set to match frequency with 32-bit mode
  TC4->COUNT32.CTRLA.reg |= TC_CTRLA_MODE_COUNT32 | TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_PRESCSYNC_PRESC | TC_CTRLA_WAVEGEN_MFRQ;
  TC4->COUNT32.CTRLA.bit.ENABLE = 1;

  //Configure interrupt frequency
  const int CLOCK_FREQ = SystemCoreClock;
  TC4->COUNT32.CC[0].reg = CLOCK_FREQ / (1024 * 100); //1024 to get into seconds and 100 to get into every 10ms
  while(TC4->COUNT32.STATUS.bit.SYNCBUSY);

  //Enable TC interrupt
  NVIC_SetPriority(TC4_IRQn, 0);
  NVIC_EnableIRQ(TC4_IRQn);

  TC4->COUNT32.INTENSET.reg |= TC_INTENSET_MC0;
  while(TC4->COUNT32.STATUS.bit.SYNCBUSY);
}

//Returns current time safely
uint32_t getCurTime() {
  return curTime;
  // return millis();
}

void TC4_Handler() {
  // Serial.print("Cur time: ");
  // Serial.println(curTime);
  curTime += 10; //Increments curTime by 10ms intervals
  TC4->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0;
}