/*

  The handlebar circuit for the Primary Engineers Project

  The circuit scans for the nano that houses the radar circuit . Once discovered and connected, it will read the data from the ultrasonic sensors and display them on the grove OLED to alert the user how close each car is to them. The two buttons on the circuit control the indicators on the backpack.


  The circuit:

  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,

    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  - Two buttons with pull-down resistors connected to pins 11 and 12.

*/


#include <ArduinoBLE.h>

#include <U8g2lib.h>



// variables for button

const int leftButton = 11;

const int rightButton = 12;



// Constructor setup for the grove OLED

U8G2_SH1107_SEEED_128X128_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);



int leftOldButtonState = LOW;

int rightOldButtonState = LOW;

void setup() {

  u8g2.begin();

u8g2.setCursor(0, 16);

  Serial.begin(9600);

  while (!Serial)

    ;



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



void controlLed(BLEDevice peripheral) {

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

  BLECharacteristic leftCharacteristic = peripheral.characteristic("19b10002-e8f2-537e-4f6c-d104768a1214");

  BLECharacteristic rightCharacteristic = peripheral.characteristic("19b10003-e8f2-537e-4f6c-d104768a1214");

  BLECharacteristic leftIndicatorCharacteristic = peripheral.characteristic("19b10004-e8f2-537e-4f6c-d104768a1214");

  BLECharacteristic rightIndicatorCharacteristic = peripheral.characteristic("19b10005-e8f2-537e-4f6c-d104768a1214");





  if (!centreCharacteristic || !leftCharacteristic || !rightCharacteristic) {

    Serial.println("Radar distance characteristics missing!");

    peripheral.disconnect();

    return;

  }



  if (!leftIndicatorCharacteristic || !rightIndicatorCharacteristic) {

    Serial.println("Indicator characteristics missing!");

    peripheral.disconnect();

    return;

  }





  while (peripheral.connected()) {

    // while the peripheral is connected

    // while the peripheral is connected

float centre, left, right;

centreCharacteristic.readValue((byte*)&centre, sizeof(centre));

leftCharacteristic.readValue((byte*)&left, sizeof(left));

rightCharacteristic.readValue((byte*)&right, sizeof(right));



// Print the values to debug

Serial.print("Centre distance: ");

Serial.println(centre);

Serial.print("Left distance: ");

Serial.println(left);

Serial.print("Right distance: ");

Serial.println(right);


//   Print all values to the OLED
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_ncenB08_tr);





    char buf[32];

    sprintf(buf, "Centre: %.2f cm", centre);

    u8g2.drawStr(0, 12, buf);

    sprintf(buf, "Left:   %.2f cm", left);

    u8g2.drawStr(0, 28, buf);

    sprintf(buf, "Right:  %.2f cm", right);

    u8g2.drawStr(0, 44, buf);

    u8g2.sendBuffer();



    delay(50);







    // read the button pins

    int leftButtonState = digitalRead(leftButton);

    int rightButtonState = digitalRead(rightButton);


// Control the indicators when pressed
    if (leftOldButtonState != leftButtonState) {

      leftOldButtonState = leftButtonState;

      if (leftButtonState == HIGH) {

        leftIndicatorCharacteristic.writeValue((byte)0x01);

      } else {

        leftIndicatorCharacteristic.writeValue((byte)0x00);

      }

    }



    if (rightOldButtonState != rightButtonState) {

      rightOldButtonState = rightButtonState;

      if (rightButtonState == HIGH) {

        rightIndicatorCharacteristic.writeValue((byte)0x01);

      } else {

        rightIndicatorCharacteristic.writeValue((byte)0x00);

      }

    }

  }

  Serial.println("Peripheral disconnected");

}
