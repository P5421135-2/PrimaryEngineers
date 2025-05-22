　/*
  The code for the Ultrasonic sensors in the Primary Engineers project. This circuit sits on the backpack and has three ultrasonic sensors to detect the distance of vehicles in three directions, the centre, left and right. The nano scans for the central device on the handlebars and sends over the data read from the ultrasonic sensors, warning statements are included to alert the cyclist of how close approaching vehicles are.

　*/
#include <ArduinoBLE.h>

// Radar Pins
const int RadarLeftTrig = 2;
const int RadarLeftEcho = 3;

const int RadarCentreTrig = 4;
const int RadarCentreEcho = 5;

const int RadarRightTrig = 6;
const int RadarRightEcho = 7;

const int LeftIndicator = 8;
const int RightIndicator = 9;

// BLE Service and Characteristics
BLEService radarService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEFloatCharacteristic centreCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic leftCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic rightCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic leftIndicatorCharacteristic("19b10004-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite);
BLEFloatCharacteristic rightIndicatorCharacteristic("19b10005-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite);

BLEFloatCharacteristic closeCharacteristic("19B10006-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic mediumCharacteristic("19B10007-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic farCharacteristic("19B10008-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);



void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  // Sensor pin modes
  pinMode(RadarLeftTrig, OUTPUT);
  pinMode(RadarLeftEcho, INPUT);

  pinMode(RadarCentreTrig, OUTPUT);
  pinMode(RadarCentreEcho, INPUT);

  pinMode(RadarRightTrig, OUTPUT);
  pinMode(RadarRightEcho, INPUT);

  pinMode(LeftIndicator, OUTPUT);
  pinMode(RightIndicator, OUTPUT);

  // BLE setup
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1)
      ;
  }

  BLE.setLocalName("Radar");
  BLE.setAdvertisedService(radarService);

  radarService.addCharacteristic(centreCharacteristic);
  radarService.addCharacteristic(leftCharacteristic);
  radarService.addCharacteristic(rightCharacteristic);
  radarService.addCharacteristic(leftIndicatorCharacteristic);
  radarService.addCharacteristic(rightIndicatorCharacteristic);
  radarService.addCharacteristic(closeCharacteristic);
  radarService.addCharacteristic(mediumCharacteristic);
  radarService.addCharacteristic(farCharacteristic);





  BLE.addService(radarService);

  centreCharacteristic.writeValue(0.0);
  leftCharacteristic.writeValue(0.0);
  rightCharacteristic.writeValue(0.0);

  closeCharacteristic.writeValue(0.0);
  mediumCharacteristic.writeValue(0.0);
  farCharacteristic.writeValue(0.0);

  BLE.advertise();
  Serial.println("BLE Radar Peripheral Started");
}


// Function to get the  distance for each sensor
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2.0;
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      　// Function is applied to get the reading for each ultrasonic sensor
      float left = getDistance(RadarLeftTrig, RadarLeftEcho);
      float centre = getDistance(RadarCentreTrig, RadarCentreEcho);
      float right = getDistance(RadarRightTrig, RadarRightEcho);

      Serial.print("Left: ");
      Serial.print(left);
      Serial.print(" cm | Centre: ");
      Serial.print(centre);
      Serial.print(" cm | Right: ");
      Serial.println(right);

      leftCharacteristic.writeValue(left);
      centreCharacteristic.writeValue(centre);
      rightCharacteristic.writeValue(right);
      // Define thresholds
      float dangerThreshold = 100.0;
      float warningThreshold = 300.0;
      float safeThreshold = 500.0;

      // Determine proximity state
      if (left < dangerThreshold || centre < dangerThreshold || right < dangerThreshold) {
        Serial.println("!!! WARNING: OBJECT VERY CLOSE !!!");
        closeCharacteristic.writeValue(dangerThreshold);
      } else if (left < warningThreshold || centre < warningThreshold || right < warningThreshold) {
        Serial.println("-- CAUTION: Object Approaching --");
        mediumCharacteristic.writeValue(warningThreshold);
      } else if (left < safeThreshold || centre < safeThreshold || right < safeThreshold) {
        Serial.println("Safe: Clear area within range.");
        farCharacteristic.writeValue(safeThreshold);
      } else {
        Serial.println("Clear: No objects detected nearby.");
      }

      delayMicroseconds(200);  // Adjust delay as needed


      // if the remote device wrote to the characteristic,
      // use the value to control the  Left indicator LED:
      if (leftIndicatorCharacteristic.written()) {
        if (leftIndicatorCharacteristic.value()) {  // any value other than 0
          Serial.println("LED on");
          digitalWrite(LeftIndicator, HIGH);  // will turn the LED on
        } else {                              // a 0 value
          Serial.println(F("LED off"));
          digitalWrite(LeftIndicator, LOW);  // will turn the LED off
        }
      }


      // if the remote device wrote to the characteristic,
      // use the value to control the  Right indicator LED:
      if (rightIndicatorCharacteristic.written()) {
        if (rightIndicatorCharacteristic.value()) {  // any value other than 0
          Serial.println("LED on");
          digitalWrite(RightIndicator, HIGH);  // will turn the LED on
        } else {                               // a 0 value
          Serial.println(F("LED off"));
          digitalWrite(RightIndicator, LOW);  // will turn the LED off
        }
      }
    }
  }

  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}
