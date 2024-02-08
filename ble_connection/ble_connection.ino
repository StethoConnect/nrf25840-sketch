#include <ArduinoBLE.h>

#define SERVICE_UUID "12345638-1984-1234-1234-123766789012"
#define CHARACTERISTIC_UUID "87623331-4321-4387-4321-210987654321"

BLEService myService(SERVICE_UUID);
BLECharacteristic myCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite, 20);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(3000);
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    digitalWrite(LED_RED, LOW); 
    while (1);
  }

  myService.addCharacteristic(myCharacteristic);
  BLE.addService(myService);
  BLE.setLocalName("SCT");
  BLE.advertise();

  Serial.println("BLE Peripheral is now advertising");
digitalWrite(LED_BLUE, LOW); 
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.println("Connected to central device");
    digitalWrite(LED_BLUE, HIGH); 
    digitalWrite(LED_GREEN, LOW); 
    while (central.connected()) {
      if (myCharacteristic.written()) {
        String value = "";
        for (int i = 0; i < myCharacteristic.valueLength(); i++) {
          value += (char)myCharacteristic.value()[i];
        }
        Serial.print("Received: ");
        Serial.println(value);
      }
    }
    Serial.println("Disconnected from central device");
    digitalWrite(LED_GREEN, HIGH); 
    digitalWrite(LED_BLUE, LOW); 

  }
}
