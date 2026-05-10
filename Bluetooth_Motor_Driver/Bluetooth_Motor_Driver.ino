#include <ArduinoBLE.h>

// Motor control pins
const int motorA1 = 4;
const int motorA2 = 5;
const int motorB1 = 6;
const int motorB2 = 7;

// Define BLE Service and Characteristic
BLEService gamePadService("19B10000-E8F2-537E-4F6C-D104768A1214");  // Random UUID for service
BLEByteCharacteristic gamePadCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Motor pins setup
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("ArduinoR4Bot");
  BLE.setAdvertisedService(gamePadService);

  gamePadService.addCharacteristic(gamePadCharacteristic);
  BLE.addService(gamePadService);

  gamePadCharacteristic.writeValue(0); // Initial state

  BLE.advertise();
  Serial.println("BLE GamePad Server Started. Waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (gamePadCharacteristic.written()) {
        uint8_t buttonState = gamePadCharacteristic.value();

        Serial.print("Received button state: ");
        Serial.println(buttonState);

        driveMotors(buttonState);
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
    stopMotors(); // Stop motors on disconnect
  }
}

void driveMotors(uint8_t buttonState) {
  // You can customize this based on your Dabble mapping
  switch (buttonState) {
    case 1: // UP pressed
      moveForward();
      break;
    case 2: // DOWN pressed
      moveBackward();
      break;
    case 3: // LEFT pressed
      turnLeft();
      break;
    case 4: // RIGHT pressed
      turnRight();
      break;
    default:
      stopMotors();
      break;
  }
}

void moveForward() {
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}

void moveBackward() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
}

void turnLeft() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}

void turnRight() {
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
}

void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}