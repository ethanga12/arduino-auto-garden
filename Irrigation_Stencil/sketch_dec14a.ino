// #include "autogarden.h"

// int mockAnalogReadVals[10];
// unsigned long mockMillisVal = 0;
// String lastLCDOut;

// // void setup() {
// //   Serial.begin(9600);
// //   while(!Serial) {}
// //   // Initialize any other required components or variables
// // }

// // void loop() {
// //   // Main loop code
// // }

// void mockAnalogRead(int pin, int val) {
//   mockAnalogReadVals[pin] = val;
// }

// int analogRead(int pin) {
//   return mockAnalogReadVals[pin];
// }

// void mockMillis(unsigned long val) {
//   mockMillisVal = val;
// }

// unsigned long millis() {
//   return mockMillisVal;
// }

// void lcdOut(String msg) {
//   lastLCDOut = msg;
// }

// String getLastLCDOutput() {
//   return lastLCDOut;
// }

// void mockReset() {
//   for(int i = 0; i < 10; i++) {
//     mockAnalogReadVals[i] = 0;
//   }
//   mockMillisVal = 0;
//   lastLCDOut = "";
// }
