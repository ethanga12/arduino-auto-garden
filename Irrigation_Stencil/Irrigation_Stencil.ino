#include "autogarden.h"


void setup() {
  //begins communication with serial monitor
  // Serial.begin(9600);
  // while (!Serial);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handlePowerButton, RISING);

  lcd.begin(16,2); //TODO: Reconfigure to our needs
  lcdOutput("SETTING UP");

  delay(1000);
  initializeWDT();
  initializeTimer();
}

void loop() {
  static state newState = sWAITING;
  //Pets WDT
  clearWDT();

  //Clear LCD each iteration
  lcd.clear();

  newState = updateFSM(newState, getCurTime());
  delay(500);
}

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
        if (!checkWateringWorked()) {
          return sPOST_WATER;
        }
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
  int curSoilMoisture = analogRead(soilSensorPin);
  if (curSoilMoisture >= humidityReading) {
    //ERROR or put more water or wait?
    lcdOutput("Watering failed!");
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