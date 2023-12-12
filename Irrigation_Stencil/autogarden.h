#include <sys/_stdint.h>
#include <LiquidCrystal.h>

const int relayPin = 9;
const int soilSensorPin = A0;
const int waterLevelPin = A1;
const int interruptPin = 7;

const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//TODO: Find actual humidity threshold
const int HUMIDITY_THRESHOLD = 700; //Values range from 1000 (dry) to 300 (wet)

// If water level is below this threshold, water needs to be refilled
const int WATER_LEVEL_THRESHOLD = 100; //Values range from 0 (dry) to 600 (wet)

int timeAtPumpOpen;
int pumpOpenDuration = 1000; //TODO: Configurable?
int timeAtPumpClosed;

volatile bool sysOn = true;
volatile unsigned long timeAtLastButtonPress = 0;
long debounceDelay = 15;

const int POST_WATERING_WAIT_DURATION = 5000;

const bool debugging = false;

typedef enum {
  sWAITING,
  sWATERING,
  sPOST_WATER,
  sREFILL_WATER,
  sSYSTEM_OFF,
} state;
