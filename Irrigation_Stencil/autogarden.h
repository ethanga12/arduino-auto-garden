#ifndef AUTO_GARDEN_H
#define AUTO_GARDEN_H

#include <sys/_stdint.h>
#include <LiquidCrystal.h>

// Declare global variables with extern
extern const int relayPin;
extern const int soilSensorPin;
extern const int waterLevelPin;
extern const int interruptPin;
extern const int rs, en, d4, d5, d6, d7;

extern LiquidCrystal lcd;

extern const int HUMIDITY_THRESHOLD;
extern const int WATER_LEVEL_THRESHOLD;
extern int pumpOpenDuration;
extern const int POST_WATERING_WAIT_DURATION;

extern volatile bool sysOn;
extern volatile unsigned long timeAtLastButtonPress;
extern long debounceDelay;

typedef enum {
  sWAITING,
  sWATERING,
  sPOST_WATER,
  sREFILL_WATER,
  sSYSTEM_OFF,
} state;

// Function declarations
void handlePowerButton();
void lcdOutput(String message);
void displayHumidityReading(int humidityReading);
bool waterLevelEmpty();
bool checkWateringWorked(int waterLevel, int humidityReading);
state updateFSM(state curState, int mils);
void initializeWDT();
void initializeTimer();

#endif

