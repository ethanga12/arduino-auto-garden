#include "autogarden.h"

// Uncomment the following line to enable test mode
#define TEST_MODE

#ifdef TEST_MODE
int mockAnalogValues[A2];
uint32_t mockMillisVal = 0;

void setMockAnalogRead(int pin, int val) {
    switch (pin){
      case relayPin:
        mockAnalogValues[relayPin] = val;
        break;
      case soilSensorPin:
        mockAnalogValues[soilSensorPin] = val;
        break;
      case waterLevelPin:
        mockAnalogValues[waterLevelPin] = val;
        break;
      case interruptPin:
        mockAnalogValues[interruptPin] = val;
        break; 
      default:
        break;
    }
}

void setMockMillis(unsigned long val) {
  mockMillisVal = val;
}

uint32_t mockMillis() {
  return mockMillisVal;
}

int mockAnalogRead(int pin) {
    if(pin >= 0 && pin < A2) { 
        return mockAnalogValues[pin];
    }
    return 0; 
}

void resetMockAnalogRead() {
    for(int i = 0; i < A2; i++) {
        mockAnalogValues[i] = 0;
    }
}


// Override the original analogRead function with the mock
#define analogRead(pin) mockAnalogRead(pin)
#define getCurTime() mockMillis()
void testMocks();
void testWaterLevelSensor();
void testSoilMoistureSensor();
void testMotor();
void testSystemOnOff();
void testFSM();
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
    Serial.begin(9600);
    while (!Serial);
    setMockMillis(0); //'starts' mock clock
    resetMockAnalogRead();
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
    testWaterLevelSensor(); //Tests water level high/low levels
    testSoilMoistureSensor(); //Tests soil sensor high/low levels
    testMotor(); //Tests motor gets sent a high signal with our mock functions
    testSystemOnOff(); //Tests system turns on/off (without debouncing)
    testFSM(); //All transition and non-transition tests
    testMocks(); //Tests all mock functions (anything we use for our mockAnalogVals and mockMillis including their reset)
    while(true); // Stop the loop after running tests
    #endif
}

#ifdef TEST_MODE
// Test function implementations
void testMocks() {
  Serial.println("Testing Mocks: ");
  setMockAnalogRead(relayPin, HIGH);
  setMockAnalogRead(soilSensorPin, 300);
  setMockAnalogRead(waterLevelPin, 10);
  setMockAnalogRead(interruptPin, HIGH);
  int myCheck = 0;
  mockAnalogValues[relayPin] == HIGH ? myCheck++ : Serial.println("Error Setting Relay Pin"); 
  mockAnalogValues[soilSensorPin] == 300 ? myCheck++ : Serial.println("Error Setting Soil Sensor Pin"); 
  mockAnalogValues[waterLevelPin] == 10 ? myCheck++ : Serial.println("Error Setting Water Level Pin"); 
  mockAnalogValues[interruptPin] == HIGH ? myCheck++ : Serial.println("Error Setting Interrupt Pin"); 
  myCheck == 4 ? Serial.println("Set Mock Analog Read Tests: Passed!") : Serial.println("Set Mock Analog Read Tests: Failed");
  setMockMillis(1000);
  getCurTime() == 1000 ? Serial.println("Set Mock Millis and TEST_MODE Macro CurTime Redefintion Test: Passed!") : Serial.println("Set Mock Millis and TEST_MODE Macro CurTime Redefintion Test: Failed");
  setMockMillis(0);
  resetMockAnalogRead();
  for (int i = 0; 0 < i < A2; i++) {
    myCheck += mockAnalogRead(i);
  }
  (!(myCheck > 4) && getCurTime() == 0) ? Serial.println("Reset Mocks: Passed!") : Serial.println("Reset Mocks: Failed");
}

void testWaterLevelSensor() {
    Serial.println("Testing Water Level Sensor High/Low Levels: ");
    bool myCheckHigh;
    bool myCheckLow;
    // Mock low water level
    setMockAnalogRead(waterLevelPin, 50); // Assuming 50 represents a low water level
    myCheckLow = waterLevelEmpty();
    // Mock high water level
    setMockAnalogRead(waterLevelPin, 200); // Assuming 200 represents a high water level
    myCheckHigh = !waterLevelEmpty();
    myCheckHigh && myCheckLow ? Serial.println("Passed!") : Serial.println("Failed");
}

void testSoilMoistureSensor() {
    Serial.println("Testing Soil Moisture Sensor Wet/Dry Levels:");
    bool myCheckDry;
    bool myCheckWet; 
    // Mock dry soil
    setMockAnalogRead(soilSensorPin, 800); // 800 represents dry soil

    myCheckDry = mockAnalogValues[soilSensorPin] >= HUMIDITY_THRESHOLD;
    // Mock wet soil
    setMockAnalogRead(soilSensorPin, 90);
    myCheckWet = !(mockAnalogValues[soilSensorPin] >= HUMIDITY_THRESHOLD);
    myCheckDry && myCheckWet ? Serial.println("Passed!") : Serial.println("Failed");

}

void testMotor() { //this also just checks if the mockAnalog settings work. 
  Serial.println("Testing Motor Signal Send: ");
  bool myCheck; 
  setMockAnalogRead(relayPin, HIGH);
  if (mockAnalogValues[relayPin] == HIGH) {
    myCheck = true; 
  } 
  else {
    myCheck = false; 
    Serial.println("Error sending signals to motor");
    return;
  }
  setMockAnalogRead(relayPin, LOW);
  if (mockAnalogValues[relayPin] == LOW) {
    myCheck = true;
  } else {
    myCheck = false;
  }
  myCheck ? Serial.println("Passed!") : Serial.println("Error sending signals to motor");
}

void testSystemOnOff() {
  Serial.println("Testing On/Off Functionality (no debouncing): ");
  bool myCheck;
  sysOn = true;
  handlePowerButton();
  if (!sysOn) {
    myCheck = true;
  } else {
    myCheck = false; 
    Serial.println("Error turning System off");
    return; 
  }

  handlePowerButton();
  if (sysOn) {
    myCheck = true;
  } else {
    Serial.println("Error turning System on");
    return; 
  }
  Serial.println("Passed!");
}

void testFSM () {
  //Transition 1-1
  Serial.println("Checking Transition 1-1:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  setMockAnalogRead(soilSensorPin, 600);
  sWAITING == updateFSM(sWAITING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 1-2
  Serial.println("Checking Transition 1-2:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  setMockAnalogRead(soilSensorPin, 800);
  sWATERING == updateFSM(sWAITING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 1-4
  Serial.println("Checking Transition 1-4:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 90);
  setMockAnalogRead(soilSensorPin, 800);
  sREFILL_WATER == updateFSM(sWAITING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 1-5
  Serial.println("Checking Transition 1-4:");
  sysOn = false; 
  sSYSTEM_OFF == updateFSM(sWAITING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 2-2
  Serial.println("Checking Transition 2-2:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  timeAtPumpOpen = 10;
  setMockMillis(900);
  sWATERING == updateFSM(sWATERING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 2-3
  Serial.println("Checking Transition 2-3:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  timeAtPumpOpen = 10;
  setMockMillis(10000);
  sPOST_WATER == updateFSM(sWATERING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 2-4
  Serial.println("Checking Transition 2-4:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 90);
  sREFILL_WATER == updateFSM(sWATERING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 2-5
  Serial.println("Checking Transition 2-5:");
  sysOn = false; 
  sSYSTEM_OFF == updateFSM(sWATERING, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 3-1
  Serial.println("Checking Transition 3-1:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  timeAtPumpClosed = 10;
  setMockMillis(10000);
  sWAITING == updateFSM(sPOST_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 3-3
  Serial.println("Checking Transition 3-3:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  timeAtPumpOpen = 10;
  setMockMillis(100); 
  sPOST_WATER == updateFSM(sPOST_WATER, getCurTime  ()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 3-4
  Serial.println("Checking Transition 3-4:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 10);
  sREFILL_WATER == updateFSM(sPOST_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 3-5
  Serial.println("Checking Transition 3-5:");
  sysOn = false; 
  sSYSTEM_OFF == updateFSM(sPOST_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 4-1
  Serial.println("Checking Transition 4-1:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  sWAITING == updateFSM(sREFILL_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 4-4
  Serial.println("Checking Transition 4-4:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 15);
  sREFILL_WATER == updateFSM(sREFILL_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 4-5
  Serial.println("Checking Transition 4-5:");
  sysOn = false; 
  sSYSTEM_OFF == updateFSM(sREFILL_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 5-1
  Serial.println("Checking Transition 5-1:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 300);
  sWAITING == updateFSM(sSYSTEM_OFF, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 5-4
  Serial.println("Checking Transition 5-4:");
  sysOn = true; 
  setMockAnalogRead(waterLevelPin, 90);
  sREFILL_WATER == updateFSM(sSYSTEM_OFF, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
  //Transition 5-5
  Serial.println("Checking Transition 5-5:");
  sysOn = false; 
  sSYSTEM_OFF == updateFSM(sPOST_WATER, getCurTime()) ? Serial.println("Passed!") : Serial.println("Failed");
}

#endif

state updateFSM(state curState, int mils) {
  
  if (!sysOn) { //Transitions 1-5, 2-5, 3-5, 4-5
    lcdOutput("System Off");
    digitalWrite(relayPin, HIGH);
    return sSYSTEM_OFF;
  }

  if (waterLevelEmpty() && sysOn) { //Transitions 1-4, 2-4, 3-4
    lcdOutput("REFILL WATER!");
    return sREFILL_WATER; //Likely shutdown whole system while in this state until water refilled to save energy
  }

  int humidityReading = analogRead(soilSensorPin);
  displayHumidityReading(humidityReading);
  // Serial.println(); 

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
  #ifdef TEST_MODE
  sysOn = !sysOn;
    #else
    if ((long)(millis() - timeAtLastButtonPress) >= debounceDelay * 100) {
      // Serial.println("ISR Successful");
      sysOn = !sysOn;
      // if (sysOn) {
      //   sysOn = false;
      // } else if (sysOn == false) {
      //   sysOn = true;
      // }
      timeAtLastButtonPress = millis();
    }

  
  #endif

}