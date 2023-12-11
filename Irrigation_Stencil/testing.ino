#include <AUnit.h>
#include "autogarden.h"

int mockAnalogReadVals[10];
unsigned long mockMillisVal = 0;
String lastLCDOut;

void mockAnalogRead(int pin, int val) {
  mockAnalogReadVals[pin] = val;
}

int analogRead(int pin) {
  return mockAnalogReadVals[pin]
}

void mockMillis(unsigned long val) {
  mockMillisVal = val;
}

unsigned long millis() {
  return mockMillisVal;
}

void lcdOut(string msg) {
  lastLCDOut = msg;
}

void mockReset() {
  for(int i = 0; i < 10; i++) {
        mockAnalogReadVals[i] = 0;
    }
    mockMillisVal = 0;
    lastLCDOut = "";
}

void setup() {
  Serial.begin(9600);
  while(!Serial) {}
  aunit::TestRunner::run();
}

void loop() {}


test(initialStateAfterSetup) {
  setup();
  assertEqual(sWAITING, currentState);
}
test(responseToLowWaterLevel) {
  mockAnalogRead(waterLevelPin, LOW_WATER_LEVEL); 
  state result = updateFSM(sWAITING, 0);
  assertEqual(sREFILL_WATER, result);
}
test(transitionFromWaitingToWatering) {
  mockAnalogRead(soilSensorPin, DRY_SOIL); 
  state result = updateFSM(sWAITING, 0);
  assertEqual(sWATERING, result);
}
test(wateringDuration) {
  int startTime = 0;
  mockMillis(startTime);
  updateFSM(sWATERING, startTime);
  state result = updateFSM(sWATERING, startTime + pumpOpenDuration);
  assertEqual(sPOST_WATER, result);
}
test(postWateringDuration) {
  int startTime = 0;
  mockMillis(startTime);
  updateFSM(sPOST_WATER, startTime);
  state result = updateFSM(sPOST_WATER, startTime + POST_WATERING_WAIT_DURATION);
  assertEqual(sWAITING, result);
}
test(checkWateringEffectiveness) {
  int initialHumidity = DRY_SOIL;
  int finalHumidity = WET_SOIL; 
  assertTrue(checkWateringWorked(initialWaterLevel, initialHumidity));
}
test(waitingStatePersistenceWithoutTrigger) {
  mockAnalogRead(soilSensorPin, WET_SOIL);
  state result = updateFSM(sWAITING, 0);
  assertEqual(sWAITING, result);
}
test(lcdOutputInWaitingState) {
  mockAnalogRead(soilSensorPin, WET_SOIL);
  updateFSM(sWAITING, 0);
  assertEqual("WAITING", getLastLCDOutput());
}
test(lcdOutputInWateringState) {
  mockAnalogRead(soilSensorPin, DRY_SOIL);
  updateFSM(sWATERING, 0);
  assertEqual("WATERING", getLastLCDOutput());
}
test(refillWaterStateExitCondition) {
  mockAnalogRead(waterLevelPin, ADEQUATE_WATER_LEVEL);
  state result = updateFSM(sREFILL_WATER, 0);
  assertEqual(sWAITING, result);
}

test(checkWateringWorked) {
  int initialWaterLevel = 300;
  int initialHumidity = 800;
  int mockFinalWaterLevel = 250; 
  int mockFinalHumidity = 600; 
  assertTrue(checkWateringWorked(initialWaterLevel, initialHumidity));
  mockReset();
}

test(waterLevelEmpty) {
  mockAnalogRead(waterLevelPin, 99);
  assertTrue(waterLevelEmpty());
  mockReset();
}

testFunction(lcdOutputTest){
  debugging = true;
  lcdOutput("TEST MESSAGE");
  debugging = false;
}

testFunction(displayHumidityReadingTest) {
  debugging = true;
  displayHumidityReading(111);
  debugging = false;
}
