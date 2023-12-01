const int relayPin = 2;
const int soilSensorPin = A0;
const int waterLevelPin = A1;

const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//TODO: Find actual humidity threshold
const int HUMIDITY_THRESHOLD = 500;
const int WATER_LEVEL_THRESHOLD = 100:

int initialWaterLevel;
int soilMoisture;

typedef enum {
  sWAITING,
  sWATERING,
  sPOST_WATER
} state;

void setup() {
  //begins communication with serial monitor
  Serial.begin(9600);
  while (!Serial);
  pinMode(relayPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(waterLevelPin, INPUT);
  lcd.begin(16,2); //TODO: Reconfigure to our needs
}

void loop() {
  // delay(1000);
  // //turns on relay, in turn turning on pump
  // digitalWrite(relayPin, HIGH);
  // delay(500);
  // digitalWrite(relayPin, LOW);
  // // put your main code here, to run repeatedly:

  // //prints moisture level
  // Serial.print(sensorValue);
  // if(sensorValue>750) {
  //   //if moisture level is not above 750, turns on pump
  //   digitalWrite(relayPin, HIGH);
  // }
  // else {
  //   digitalWrite(relayPin, LOW);
  //   //if moisture level is above 750, turns off pump
  // }
  // Serial.println();
  // //prints new line for spacing
  // delay(1000);
  // //waits 1 second before re-checking moisture level
  soilMoisture = analogRead(soilSensorPin);
  Serial.print("MOISTURE LEVEL: ");
  Serial.println(soilMoisture);

  //TODO: Make soil moisture display function. Numbers mean nothing to user
  lcd.clear();
  lcd.print(soilMoisture);

  updateFSM(soilMoisture);
}

state updateFSM(int sensorValue) {
  switch(curState) {
    case sWAITING:
      if soilMoisture > HUMIDITY_THRESHOLD {
        int initialWaterLevel = analogRead(waterLevelPin);
        if initialWaterLevel < WATER_LEVEL_THRESHOLD {
          lcd.clear();
          lcd.print("REFILL WATER");
          break;
        }
        return sWATERING;
      }
      break;
    case sWATERING:
      digitalWrite(relayPin, HIGH);
      delay(500);
      digitalWrite(relayPin, LOW);
      return sPOST_WATER
    case sPOST_WATER:
      int curWaterLevel = analogRead(waterLevelPin);
      if curWaterLevel >= initialWaterLevel {
        //ERROR

      }
      initialWaterLevel = curWaterLevel;


      int curSoilMoisture = analogRead(soilSensorPin);
      if curSoilMoisture >= soilMoisture {
        //ERROR or put more water or wait?
      }
      soilMoisture = curSoilMoisture;
      return sWAITING;

  }
}