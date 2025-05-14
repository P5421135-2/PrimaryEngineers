/*
  radar Control

  This example scans for Bluetooth® Low Energy peripherals until one with the advertised service
  "19b10000-e8f2-537e-4f6c-d104768a1214" UUID is found. Once discovered and connected,
  it will remotely control the Bluetooth® Low Energy peripheral's radar, when the button is pressed or released.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button with pull-up resistor connected to pin 2.

  You can use it with another board that is compatible with this library and the
  Peripherals -> radar example.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <U8g2lib.h>

// variables for button
const int leftButton = 2;
const int rightButton = 3;

// OLED Display Pin setup
U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);


int leftOldButtonState = LOW;
int rightOldButtonState = LOW;
void setup() {
  u8g2.begin();
  Serial.begin(9600);
  while (!Serial);

  // configure the button pin as input
  pinMode(leftButton, INPUT);
  　pinMode(rightButton, INPUT);
  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  Serial.println("Bluetooth® Low Energy Central - radar control");

  // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != "Radar") {
      return;
    }

    // stop scanning
    BLE.stopScan();

    controlLed(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}

void controlLed(Bradarevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // retrieve the Radar characteristics
BLECharacteristic centreCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic leftCharacteristic   = peripheral.characteristic("19b10002-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic rightCharacteristic  = peripheral.characteristic("19b10003-e8f2-537e-4f6c-d104768a1214");


  if (!centreCharacteristic && !leftCharacteristic && !rightCharacteristic) {
    Serial.println("Peripheral does not have radar characteristics!");
    peripheral.disconnect();
    return;
    }

  while (peripheral.connected()) {
    // while the peripheral is connected
      int centre, left, right;
    centreCharacteristic.readValue(centre);
    leftCharacteristic.readValue(left);
    rightCharacteristic.readValue(right);

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    char buf[32];
    sprintf(buf, "Centre: %d cm", centre); u8g2.drawStr(0, 12, buf);
    sprintf(buf, "Left:   %d cm", left);   u8g2.drawStr(0, 28, buf);
    sprintf(buf, "Right:  %d cm", right);  u8g2.drawStr(0, 44, buf);
    u8g2.sendBuffer();

    delay(500);



    // read the button pin
    int leftButtonState = digitalRead(leftButton);
    int rightButtonState = digitalRead(rightButton);
    if (leftOldButtonState != leftButtonState) {
      // button changed
      leftOldButtonState = leftButtonState;


      if (leftButtonState) {
        Serial.println("left button pressed");

        // button is pressed, write 0x01 to turn the radar on
        radarCharacteristic.writeValue((byte)0x01);
      } else {
        Serial.println("button released");

        // button is released, write 0x00 to turn the radar off
        radarCharacteristic.writeValue((byte)0x00);
      }

      if (rightButtonState) {
        Serial.println("left button pressed");

        // button is pressed, write 0x01 to turn the radar on
        radarCharacteristic.writeValue((byte)0x01);
      } else {
        Serial.println("button released");

        // button is released, write 0x00 to turn the radar off
        radarCharacteristic.writeValue((byte)0x00);
      }

  }

  Serial.println("Peripheral disconnected");
}
