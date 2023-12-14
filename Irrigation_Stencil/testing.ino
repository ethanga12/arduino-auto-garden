// #include "autogarden.h"

// // Mock functions and variables
// int mockAnalogValues[10]; // Adjust size based on number of analog pins used

// void setMockAnalogRead(int pin, int value) {
//     if(pin >= 0 && pin < 10) { // Replace 10 with the actual number of pins you are using
//         mockAnalogValues[pin] = value;
//     }
// }

// void resetMockAnalogRead() {
//     for(int i = 0; i < 10; i++) { // Again, replace 10 with the number of pins you are using
//         mockAnalogValues[i] = 0;
//     }
// }

// int mockAnalogRead(int pin) {
//     if(pin >= 0 && pin < 10) { // Replace 10 with the number of pins you are using
//         return mockAnalogValues[pin];
//     }
//     return 0; // Default value if the pin is out of range
// }

// // Override the original analogRead function with the mock
// #define analogRead(pin) mockAnalogRead(pin)

// void setup() {
//     Serial.begin(9600);
//     while (!Serial);

//     resetMockAnalogRead();

//     // Test Cases
//     testWaterLevelSensor();
//     testSoilMoistureSensor();
//     testStateTransitions();
// }

// void loop() {
//     // Optionally add continuous tests or monitoring here
// }

// void testWaterLevelSensor() {
//     Serial.println("Testing Water Level Sensor...");

//     // Low water level test
//     setMockAnalogRead(waterLevelPin, LOW_WATER_LEVEL); // Define LOW_WATER_LEVEL appropriately
//     // Call any necessary update function if needed
//     bool isLow = waterLevelEmpty();
//     Serial.println("Low Water Level Test: " + String(isLow ? "Passed" : "Failed"));

//     // High water level test
//     setMockAnalogRead(waterLevelPin, HIGH_WATER_LEVEL); // Define HIGH_WATER_LEVEL appropriately
//     // Call any necessary update function if needed
//     isLow = waterLevelEmpty();
//     Serial.println("High Water Level Test: " + String(!isLow ? "Passed" : "Failed"));

//     Serial.println("Water Level Sensor Test Completed.");
// }

// void testSoilMoistureSensor() {
//     Serial.println("Testing Soil Moisture Sensor...");

//     // Simulate dry soil condition
//     setMockAnalogRead(soilSensorPin, DRY_SOIL); // Define DRY_SOIL appropriately
//     // Call any necessary update function if needed
//     // Assert and print the result

//     // Simulate wet soil condition
//     setMockAnalogRead(soilSensorPin, WET_SOIL); // Define WET_SOIL appropriately
//     // Call any necessary update function if needed
//     // Assert and print the result

//     Serial.println("Soil Moisture Sensor Test Completed.");
// }

// void testStateTransitions() {
//     Serial.println("Testing State Transitions...");

//     // Example: Test transition from sWAITING to sWATERING
//     // Set necessary conditions for the transition
//     // Call updateFSM or other relevant functions
//     // Assert and print the result

//     Serial.println("State Transitions Test Completed.");
// }
