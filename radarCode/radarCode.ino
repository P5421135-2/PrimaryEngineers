#include <ArduinoBLE.h>

// Radar Pins
const int RadarLeftTrig = 2;
const int RadarLeftEcho = 3;

const int RadarCentreTrig = 4;
const int RadarCentreEcho = 5;

const int RadarRightTrig = 6;
const int RadarRightEcho = 7;

// BLE Service and Characteristics
BLEService radarService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEFloatCharacteristic centreCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic leftCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic rightCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Sensor pin modes
  pinMode(RadarLeftTrig, OUTPUT);
  pinMode(RadarLeftEcho, INPUT);

  pinMode(RadarCentreTrig, OUTPUT);
  pinMode(RadarCentreEcho, INPUT);

  pinMode(RadarRightTrig, OUTPUT);
  pinMode(RadarRightEcho, INPUT);

  // BLE setup
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Radar");
  BLE.setAdvertisedService(radarService);

  radarService.addCharacteristic(centreCharacteristic);
  radarService.addCharacteristic(leftCharacteristic);
  radarService.addCharacteristic(rightCharacteristic);

  BLE.addService(radarService);

  centreCharacteristic.writeValue(0.0);
  leftCharacteristic.writeValue(0.0);
  rightCharacteristic.writeValue(0.0);

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
      float left = getDistance(RadarLeftTrig, RadarLeftEcho);
      float centre = getDistance(RadarCentreTrig, RadarCentreEcho);
      float right = getDistance(RadarRightTrig, RadarRightEcho);

      Serial.print("Left: "); Serial.print(left);
      Serial.print(" cm | Centre: "); Serial.print(centre);
      Serial.print(" cm | Right: "); Serial.println(right);

      leftCharacteristic.writeValue(left);
      centreCharacteristic.writeValue(centre);
      rightCharacteristic.writeValue(right);

      delay(500); // Adjust delay as needed
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
