#include "autogarden.h"

// Uncomment the following line to enable test mode
// #define TEST_MODE

const int relayPin = 9;
const int soilSensorPin = A0;
const int waterLevelPin = A1;
const int interruptPin = 7;

const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int HUMIDITY_THRESHOLD = 700;
const int WATER_LEVEL_THRESHOLD = 100;

int timeAtPumpOpen;
int pumpOpenDuration = 1000;
int timeAtPumpClosed;

volatile bool sysOn = true;
volatile unsigned long timeAtLastButtonPress = 0;
long debounceDelay = 15;

const int POST_WATERING_WAIT_DURATION = 5000;

const bool debugging = false;

#ifdef TEST_MODE
int mockAnalogValues[10]; // Adjust size based on number of analog pins used

void setMockAnalogRead(int pin, int value) {
    switch (pin){
      case relayPin:
        break;
      case soilSensorPin:
        break;
      case waterLevelPin:
        break;
      case interruptPin:
        break; 
      default: 
        break;
    }
}

int mockAnalogRead(int pin) {
    if(pin >= 0 && pin < 10) { // Replace 10 with the number of pins you are using
        return mockAnalogValues[pin];
    }
    return 0; // Default value if the pin is out of range
}

void resetMockAnalogRead() {
    for(int i = 0; i < 10; i++) {
        mockAnalogValues[i] = 0;
    }
}

// Override the original analogRead function with the mock
#define analogRead(pin) mockAnalogRead(pin)

void testWaterLevelSensor();
void testSoilMoistureSensor();
// Other test function declarations
#endif

void setup() {
    #ifndef TEST_MODE
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH);

    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), handlePowerButton, RISING);

    lcd.begin(16,2); //TODO: Reconfigure to our needs
    lcdOutput("SETTING UP");

    delay(1000);
    initializeWDT();
    initializeTimer();
    #else
    // Setup code for testing mode
    Serial.begin(9600);
    while (!Serial);
    resetMockAnalogRead();
    // Additional setup for testing if necessary
    #endif
}

void loop() {
    #ifndef TEST_MODE
    static state newState = sWAITING;
    //Pets WDT
    clearWDT();

    //Clear LCD each iteration
    lcd.clear();

    newState = updateFSM(newState, getCurTime());
    delay(500);
    #else
    // Testing loop
    testWaterLevelSensor();
    testSoilMoistureSensor();
    // Other tests
    while(true); // Stop the loop after running tests
    #endif
}

#ifdef TEST_MODE
// Test function implementations
void testWaterLevelSensor() {
    Serial.println("Testing Water Level Sensor...");

    // Mock low water level
    setMockAnalogRead(waterLevelPin, 50); // Assuming 50 represents a low water level
    bool isLow = waterLevelEmpty();
    Serial.println("Low Water Level Test: " + String(isLow ? "Passed" : "Failed"));

    // Mock high water level
    setMockAnalogRead(waterLevelPin, 200); // Assuming 200 represents a high water level
    isLow = waterLevelEmpty();
    Serial.println("High Water Level Test: " + String(!isLow ? "Passed" : "Failed"));

    Serial.println("Water Level Sensor Test Completed.");
}

void testSoilMoistureSensor() {
    Serial.println("Testing Soil Moisture Sensor...");

    // Mock dry soil
    setMockAnalogRead(soilSensorPin, 800); // Assuming 800 represents dry soil
    // Simulate the FSM response or check the variable directly
    // ... 
    Serial.println("Dry Soil Test: [Your Condition]");

    // Mock wet soil
    setMockAnalogRead(soilSensorPin, 300); // Assuming 300 represents wet soil
    // Simulate the FSM response or check the variable directly
    // ...
    Serial.println("Wet Soil Test: [Your Condition]");

    Serial.println("Soil Moisture Sensor Test Completed.");
}

#endif

state updateFSM(state curState, int mils) {
  int humidityReading = analogRead(soilSensorPin);
  displayHumidityReading(humidityReading);

  if (!sysOn) { //Transitions 1-5, 2-5, 3-5, 4-5
    lcdOutput("System Off");
    digitalWrite(relayPin, HIGH);
    return sSYSTEM_OFF;
  }

  if (waterLevelEmpty() && sysOn) { //Transitions 1-4, 2-4, 3-4
    lcdOutput("REFILL WATER!");
    return sREFILL_WATER; //Likely shutdown whole system while in this state until water refilled to save energy
  }

  switch(curState) {
    case sWAITING:
    {
      lcdOutput("Waiting...");
      if (humidityReading >= HUMIDITY_THRESHOLD) {
        digitalWrite(relayPin, LOW); //Not sure why this has to be low?
        lcdOutput("Watering...");
        timeAtPumpOpen = getCurTime();
        return sWATERING;
      }
      return sWAITING;
    }

    case sWATERING:
    {
      // lcdOutput("WATERING");
      lcdOutput("Watering...");
      if (mils - timeAtPumpOpen >= pumpOpenDuration) {
        digitalWrite(relayPin, HIGH);
        lcdOutput("Done Watering");
        timeAtPumpClosed = getCurTime();
        return sPOST_WATER;
      }

      return sWATERING;
    }

    case sPOST_WATER:
    {
      // lcdOutput("POST-WATER");
      lcdOutput("Done Watering");
      if (mils - timeAtPumpClosed >= POST_WATERING_WAIT_DURATION) {
        // if (!checkWateringWorked()) {
        //   //Some sort of error
        //   return sPOST_WATER;
        // }
        lcdOutput("Waiting...   ");
        return sWAITING;
      }

      return sPOST_WATER;
    }
    case sREFILL_WATER:
    {
      // lcdOutput("REFILL_WATER");
      if (!waterLevelEmpty()) {
        lcdOutput("Waiting...");
        return sWAITING;
      }

      lcdOutput("REFILL WATER!");
      delay(1000);
      return sREFILL_WATER;
    }
    case sSYSTEM_OFF:
    {
      lcdOutput("System Off");
      if (sysOn) {
        lcdOutput("Waiting...");
        return sWAITING;
      }
    }
  }
}

bool waterLevelEmpty() {
  return analogRead(waterLevelPin) < WATER_LEVEL_THRESHOLD;
}

bool checkWateringWorked(int waterLevel, int humidityReading) {
  int curWaterLevel = analogRead(waterLevelPin);
  if (curWaterLevel >= waterLevel) {
    //ERROR
    Serial.println("ERROR: Water level did not change");
    return false;
  }
  // initialWaterLevel = curWaterLevel;


  int curSoilMoisture = analogRead(soilSensorPin);
  if (curSoilMoisture >= humidityReading) {
    //ERROR or put more water or wait?
    Serial.println("ERROR: Humidity did not change");
    return false;
  }
  return true;
}

void lcdOutput(String message) {
  if (debugging) {
    Serial.println(message);
  } 
  else {
    lcd.setCursor(0,0);
    lcd.print(message);
  }
}

void displayHumidityReading(int humidityReading) {
  if (debugging) {
    Serial.print("Moisture reading: ");
    Serial.println(humidityReading);
  } else {
    lcd.setCursor(0, 1);

    int humidityPercentage = min(max((850 - humidityReading)/4,0),100);
    // String message = "Moisture: " + String(humidityReading); 
    String message = "Moisture: " + String(humidityPercentage) + "%";
    lcd.print(message);
    // lcd.print(digitalRead(interruptPin));
  }
}

void handlePowerButton() {
  // unsigned long currPressTime = millis();
  // lcd.Output("ISR ACTIVATED");

  if ((long)(millis() - timeAtLastButtonPress) >= debounceDelay * 100) {
    // Serial.println("ISR Successful");
    // sysOn = !sysOn;
    if (sysOn) {
      sysOn = false;
    } else {
      sysOn = true;
    }
    timeAtLastButtonPress = millis();
  }
}