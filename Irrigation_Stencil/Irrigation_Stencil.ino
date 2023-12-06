#include "autogarden.h"

state newState = sWAITING;
int savedClock;

void setup() {
  //begins communication with serial monitor
  Serial.begin(9600);
  while (!Serial);
  pinMode(relayPin, OUTPUT);

  // digitalWrite(relayPin, LOW);
  // delay(1000);
  digitalWrite(relayPin, HIGH);
  // delay(1000);
  // digitalWrite(relayPin,LOW);


  pinMode(soilSensorPin, INPUT);
  pinMode(waterLevelPin, INPUT);
  lcd.begin(16,2); //TODO: Reconfigure to our needs
  lcdOutput("SETTING UP");
  savedClock = millis();
  delay(2000);
}

void loop() {
  newState = updateFSM(newState, millis());
  delay(500);
}

state updateFSM(state curState, int mils) {
  static int humidityReading = analogRead(humidityReading);
  displayHumidityReading(humidityReading);

  if (waterLevelEmpty()) {
    lcdOutput("REFILL WATER!");
    return sREFILL_WATER; //Likely shutdown whole system while in this state until water refilled to save energy
  }

  switch(curState) {
    case sWAITING:
    {
      lcdOutput("WAITING");
      if (humidityReading >= HUMIDITY_THRESHOLD) {
        digitalWrite(relayPin, LOW); //Not sure why this has to be low?
        lcdOutput("Watering...");
        timeAtPumpOpen = millis();
        return sWATERING;
      }
      return sWAITING;
    }

    case sWATERING:
    {
      lcdOutput("WATERING");
      if (mils - timeAtPumpOpen >= pumpOpenDuration) {
        digitalWrite(relayPin, HIGH);
        lcdOutput("Done Watering");
        timeAtPumpClosed = millis();
        return sPOST_WATER;
      }

      return sWATERING;
    }

    case sPOST_WATER:
    {
      lcdOutput("POST-WATER");
      if (mils - timeAtPumpClosed >= POST_WATERING_WAIT_DURATION) {
        if (!checkWateringWorked()) {
          //Some sort of error
          return sPOST_WATER;
        }
        lcdOutput("Waiting...");
        return sWAITING;
      }

      return sPOST_WATER;
    }
    case sREFILL_WATER:
    {
      lcdOutput("REFILL_WATER");
      if (!waterLevelEmpty()) {
        lcdOutput("Waiting...");
        return sWAITING;
      }

      lcdOutput("REFILL WATER!");
      delay(1000);
      return sREFILL_WATER;
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
  initialWaterLevel = curWaterLevel;


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
    lcd.clear();
    lcd.output(message);
  }
}

void displayHumidityReading(int humidityReading) {
  if (debugging) {
    Serial.print("Moisture reading: ");
    Serial.println(humidityReading);
  } else {
    lcd.clear();
    lcd.output(humidityReading);
  }
}