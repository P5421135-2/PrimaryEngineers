/*
  Button LED

  This example creates a Bluetooth® Low Energy peripheral with service that contains a
  characteristic to control an LED and another characteristic that
  represents the state of the button.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button connected to pin 4

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
 
U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
const int leftIndicator = 4; // set left indicator button to digital pin 4
const int rightIndicator = 5; // Set the right indictator button to digital pin 5


BLEService IndicatorService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void setup() {
  u8g2.begin();
  Serial.begin(9600);
  while (!Serial);

  pinMode(ledPin, OUTPUT); // use the LED as an output
  pinMode(leftIndicator, INPUT); // use button pin as an input
  pinMode(rightIndicator, INPUT); // use button pin as an input


  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("ButtonLED");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(IndicatorService);

  // add the characteristics to the service
  IndicatorService.addCharacteristic(ledCharacteristic);
  IndicatorService.addCharacteristic(buttonCharacteristic);

  // add the service
  BLE.addService(IndicatorService);

  ledCharacteristic.writeValue(0);
  buttonCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void loop() {
  // poll for Bluetooth® Low Energy events
  BLE.poll();

  // read the current button pin state
  char leftIndiVal = digitalRead(leftIndicator);
  char rightIndiVal = digitalRead(rightIndicator);


  // has the value changed since the last read
  bool leftIndicatorChanged = (buttonCharacteristic.value() != leftIndiVal);
  bool rightIndicatorChanged = (buttonCharacteristic.value() != rightIndiVal);


  if (leftIndicatorChanged) {
    // button state changed, update characteristics
    ledCharacteristic.writeValue(leftIndiVal);
    buttonCharacteristic.writeValue(leftIndiVal);
  }


  if (rightIndicatorChanged) {
    // button state changed, update characteristics
    ledCharacteristic.writeValue(rightIndiVal);
    buttonCharacteristic.writeValue(rightIndiVal);
  }

  if (ledCharacteristic.written() || leftIndicatorChanged) {
    // update LED, either central has written to characteristic or button state has changed
    if (ledCharacteristic.value()) {
      Serial.println("LED on");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED off");
      digitalWrite(ledPin, LOW);
    }
  }

  
  if (ledCharacteristic.written() || rightIndicatorChanged) {
    // update LED, either central has written to characteristic or button state has changed
    if (ledCharacteristic.value()) {
      Serial.println("LED on");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED off");
      digitalWrite(ledPin, LOW);
    }
  }
}
