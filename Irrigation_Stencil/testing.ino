#include "autogarden.h"

int mockAnalogReadVals[10];
unsigned long mockMillisVal = 0;
String lastLCDOut;

// void setup() {
//   Serial.begin(9600);
//   while(!Serial) {}
//   // Initialize any other required components or variables
// }

// void loop() {
//   // Main loop code
// }

void mockAnalogRead(int pin, int val) {
  mockAnalogReadVals[pin] = val;
}

int analogRead(int pin) {
  return mockAnalogReadVals[pin];
}

void mockMillis(unsigned long val) {
  mockMillisVal = val;
}

unsigned long millis() {
  return mockMillisVal;
}

void lcdOut(String msg) {
  lastLCDOut = msg;
}

String getLastLCDOutput() {
  return lastLCDOut;
}

void mockReset() {
  for(int i = 0; i < 10; i++) {
    mockAnalogReadVals[i] = 0;
  }
  mockMillisVal = 0;
  lastLCDOut = "";
}

// // void testInitialStateAfterSetup() {
// //   setup();
// //   assertEqual(sWAITING, currentState);
// // }
// // void testResponseToLowWaterLevel() {
// //   mockAnalogRead(waterLevelPin, LOW_WATER_LEVEL); 
// //   state result = updateFSM(sWAITING, 0);
// //   assertEqual(sREFILL_WATER, result);
// // } 
// // void testTransitionFromWaitingToWatering() {
// //   mockAnalogRead(soilSensorPin, DRY_SOIL); 
// //   state result = updateFSM(sWAITING, 0);
// //   assertEqual(sWATERING, result);
// // }
// // void testWateringDuration() {
// //   int startTime = 0;
// //   mockMillis(startTime);
// //   updateFSM(sWATERING, startTime);
// //   state result = updateFSM(sWATERING, startTime + pumpOpenDuration);
// //   assertEqual(sPOST_WATER, result);
// // }
// // void testPostWateringDuration() {
// //   int startTime = 0;
// //   mockMillis(startTime);
// //   updateFSM(sPOST_WATER, startTime);
// //   state result = updateFSM(sPOST_WATER, startTime + POST_WATERING_WAIT_DURATION);
// //   assertEqual(sWAITING, result);
// // }
// // void testCheckWateringEffectiveness() {
// //   int initialHumidity = DRY_SOIL;
// //   int finalHumidity = WET_SOIL; 
// //   assertTrue(checkWateringWorked(initialWaterLevel, initialHumidity));
// // }
// // void testWaitingStatePersistenceWithoutTrigger() {
// //   mockAnalogRead(soilSensorPin, WET_SOIL);
// //   state result = updateFSM(sWAITING, 0);
// //   assertEqual(sWAITING, result);
// // }
// // void testLcdOutputInWaitingState() {
// //   mockAnalogRead(soilSensorPin, WET_SOIL);
// //   updateFSM(sWAITING, 0);
// //   assertEqual("WAITING", getLastLCDOutput());
// // }
// // void testLcdOutputInWateringState() {
// //   mockAnalogRead(soilSensorPin, DRY_SOIL);
// //   updateFSM(sWATERING, 0);
// //   assertEqual("WATERING", getLastLCDOutput());
// // }
// // void testRefillWaterStateExitCondition() {
// //   mockAnalogRead(waterLevelPin, ADEQUATE_WATER_LEVEL);
// //   state result = updateFSM(sREFILL_WATER, 0);
// //   // assertEqual(sWAITING, result);
// // }

// // void testCheckWateringWorked() {
// //   int initialWaterLevel = 300;
// //   int initialHumidity = 800;
// //   int mockFinalWaterLevel = 250; 
// //   int mockFinalHumidity = 600; 
// //   // assertTrue(checkWateringWorked(initialWaterLevel, initialHumidity));
// //   mockReset();
// // }

// // void testWaterLevelEmpty() {
// //   mockAnalogRead(waterLevelPin, 99);
// //   assertTrue(waterLevelEmpty());
// //   // mockReset();
// // }

// // void testFunction(lcdOutputTest){
// //   debugging = true;
// //   lcdOutput("TEST MESSAGE");
// //   debugging = false;
// // }

// // void testFunction(displayHumidityReadingTest) {
// //   debugging = true;
// //   displayHumidityReading(111);
// //   debugging = false;
// // }
