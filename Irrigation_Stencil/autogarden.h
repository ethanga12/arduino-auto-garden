#include <LiquidCrystal.h>

const int relayPin = 9;
const int soilSensorPin = A0;
const int waterLevelPin = A1;

const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//TODO: Find actual humidity threshold
const int HUMIDITY_THRESHOLD = 700; //Values range from 1000 (dry) to 300 (wet)

// If water level is below this threshold, water needs to be refilled
const int WATER_LEVEL_THRESHOLD = 100; //Values range form 0 (dry) to 600 (wet)

int timeAtPumpOpen;
int pumpOpenDuration = 1000; //Configurable
int timeAtPumpClosed;

const int POST_WATERING_WAIT_DURATION = 180000;

const bool debugging = true;

typedef enum {
  sWAITING,
  sWATERING,
  sPOST_WATER,
  sREFILL_WATER,
} state;