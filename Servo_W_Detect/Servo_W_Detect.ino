#include <Servo.h>

// -------------------------------------------------
// === PIN ASSIGNMENTS ===
const int trigPin = 10;
const int echoPin = 11;
const int sensorServoPin = 9;
const int releaseServoPin = 6;

// -------------------------------------------------
// === CONSTANTS ===
const int detectionDistance = 100;  // cm threshold for detection
const int delayTime = 15;           // ms between servo sweeps
const int releaseDelay = 30;        // ms wait before releasing
const int resetDelay = 1000;         // ms wait after release before reset

// -------------------------------------------------
// === OBJECTS ===
Servo sensorServo;
Servo releaseServo;

// -------------------------------------------------
// === STATE VARIABLES ===
int angle = 0;
int direction = 3;                 // 1 = forward, -1 = backward
bool scanning = true;
bool objectDetected = false;
bool releaseTriggered = false;
unsigned long detectionTime = 0;   // when object detected
unsigned long releaseTime = 0;     // when release triggered

// -------------------------------------------------
// === SETUP ===
void setup() {
  Serial.begin(9600);

  sensorServo.attach(sensorServoPin);
  releaseServo.attach(releaseServoPin);
  releaseServo.write(120); // Initial "holding" position

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

// -------------------------------------------------
// === MAIN LOOP ===
void loop() {
  
  if (scanning) {
    performScan();
  }

  if (objectDetected && !releaseTriggered) {
    checkReleaseTrigger();
  }

  if (releaseTriggered) {
    checkSystemReset();
  }
}

// -------------------------------------------------
// === FUNCTIONS ===

void performScan() {
  // Move the servo to current angle
  sensorServo.write(angle);
  delay(delayTime);

  // Take a sonar reading
  long distance = readDistance();

  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print("  Distance: ");
  Serial.println(distance);

  // Object detected within threshold?
  if (distance > 10 && distance <= detectionDistance) {
    scanning = false;
    objectDetected = true;
    detectionTime = millis();
    Serial.println("Object detected! Halting scan.");
    return;
  }

  // Sweep control
  angle += direction;
  if (angle >= 120 || angle <= 0) {
    direction = -direction; // Reverse direction
  }
}

void checkReleaseTrigger() {
  if (millis() - detectionTime >= releaseDelay) {
    releaseServo.write(0); // Release position
    releaseTime = millis(); // Mark time release happened
    releaseTriggered = true;
    Serial.println("Release servo activated.");
  }
}

void checkSystemReset() {
  if (millis() - releaseTime >= resetDelay) {
    releaseServo.write(120);      // Retract pin to starting position
    angle = 0;                  // Reset scan
    direction = 3;
    scanning = true;
    objectDetected = false;
    releaseTriggered = false;
    Serial.println("System reset. Acquiring new target...");
  }
}

//Sonar Set-up 

long readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2; // convert time to distance (cm)
}
