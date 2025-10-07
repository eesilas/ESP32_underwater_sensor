// Updated program for connecting three underwater ultrasound sensors to Arduino Mega 2560 Pro
// Each sensor uses a dedicated UART (Serial1, Serial2, Serial3)

#include <NewPing.h>

// Sensor pins
#define TRIGGER_PIN1 2
#define ECHO_PIN1 3
#define TRIGGER_PIN2 4
#define ECHO_PIN2 5
#define TRIGGER_PIN3 6
#define ECHO_PIN3 7

// Max distance (in cm) for each sensor
#define MAX_DISTANCE 200

// Create NewPing instances for each sensor
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE);
NewPing sonar3(TRIGGER_PIN3, ECHO_PIN3, MAX_DISTANCE);

void setup() {
  Serial.begin(9600); // Start serial communication
}

void loop() {
  delay(50); // Wait 50ms between measurements
  int distance1 = sonar1.ping_cm(); // Get distance from sensor 1
  int distance2 = sonar2.ping_cm(); // Get distance from sensor 2
  int distance3 = sonar3.ping_cm(); // Get distance from sensor 3

  // Print distances to Serial Monitor
  Serial.print("Distance 1: "); Serial.print(distance1); Serial.println(" cm");
  Serial.print("Distance 2: "); Serial.print(distance2); Serial.println(" cm");
  Serial.print("Distance 3: "); Serial.print(distance3); Serial.println(" cm");
}