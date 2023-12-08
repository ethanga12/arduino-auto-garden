#include <LiquidCrystal.h>

const int relayPin = 9;
const int soilSensorPin = A0;
const int waterLevelPin = A1;

const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//TODO: Find actual humidity threshold
const int HUMIDITY_THRESHOLD = 700; //Values range from 1000 (dry) to 300 (wet)

// If water level is below this threshold, water needs to be refilled
const int WATER_LEVEL_THRESHOLD = 100; //Values range form 0 (dry) to 600 (wet)

int timeAtPumpOpen;
int pumpOpenDuration = 1000; //Configurable
int timeAtPumpClosed;

const int POST_WATERING_WAIT_DURATION = 5000;

const bool debugging = true;

typedef enum {
  sWAITING,
  sWATERING,
  sPOST_WATER,
  sREFILL_WATER,
} state;

void initiateWDT() {
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
