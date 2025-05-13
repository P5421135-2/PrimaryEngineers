//bluetooth indicator circuit:
 
#include <ArduinoBLE.h> // Include the BLE library
 
// Define the button pin
const int leftButton = 11; // Connect your button to pin 2
const int rightButton = 12;
int leftButtonState ;
int rightButtonState ;
 
 
// Create a BLE service and characteristic
BLEService buttonService("180F"); // Custom service UUID
BLEByteCharacteristic buttonCharacteristic("2A19", BLERead | BLENotify); // Custom characteristic UUID
 
void setup() {
  pinMode(leftButton,INPUT); // Set button pin as input with pull-up resistor
  pinMode(rightButton,INPUT);
  Serial.begin(9600);
 
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }
 
  // Set up BLE service and characteristic
  BLE.setLocalName("ButtonNano");
  BLE.setAdvertisedService(buttonService);
  buttonService.addCharacteristic(buttonCharacteristic);
  BLE.addService(buttonService);
 
  // Start advertising
  BLE.advertise();
  Serial.println("Bluetooth device is now advertising...");
}
 
void loop() {
  // Listen for BLE connections
  BLEDevice central = BLE.central();
 
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
 
    while (central.connected()) {
      // Read button states
      leftButtonState = digitalRead(leftButton);
      rightButtonState = digitalRead(rightButton);
 
      // Send button state over BLE
      buttonCharacteristic.writeValue(leftButtonState);
      buttonCharacteristic.writeValue(rightButtonState);
 
 
      // Optional: Print button state to Serial Monitor
      Serial.print("The left Button State: ");
      Serial.println(leftButtonState);
 
       Serial.print("The right Button State: ");
      Serial.println(rightButtonState);
 
      delay(100); // Small delay to avoid spamming
    }
 
    Serial.println("Disconnected from central.");
  }
}
 
