#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

unsigned char buffer_RTT[4] = {0};
uint8_t CS;
#define COM 0x55
int Distance = -1, lastStableDistance = -1;
unsigned long stableStartTime = 0;

// Using HardwareSerial (UART2) for the sensor
#define RXD2 16  // GPIO16 (RX2)
#define TXD2 17  // GPIO17 (TX2)
HardwareSerial SerialSensor(2);  // UART2

// For the SSD1306 OLED display Module
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  SerialSensor.begin(115200, SERIAL_8N1, RXD2, TXD2);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Initializing..."));
  display.display();
  delay(1000);
}

void loop() {
  SerialSensor.write(COM); // Request distance measurement
  delay(100); // Give sensor time to respond

  if (SerialSensor.available() >= 4) { // Wait until we have all 4 bytes
    if (SerialSensor.read() == 0xff) {
      buffer_RTT[0] = 0xff;
      for (int i = 1; i < 4; i++) {
        buffer_RTT[i] = SerialSensor.read();
      }
      
      CS = buffer_RTT[0] + buffer_RTT[1] + buffer_RTT[2];

      if (buffer_RTT[3] == CS) {
        int newDistance = (buffer_RTT[1] << 8) + buffer_RTT[2];
        
        if (Distance == -1) {  
          // First valid reading
          Distance = newDistance;
          lastStableDistance = newDistance;
          stableStartTime = millis();
          updateDisplay(lastStableDistance);
        } 
        else if (newDistance == Distance) {
          // Check if stable for 500ms
          if (millis() - stableStartTime >= 500 && newDistance != lastStableDistance) {
            lastStableDistance = newDistance;
            updateDisplay(lastStableDistance);
          }
        } 
        else {
          // Reset stability timer on change
          Distance = newDistance;
          stableStartTime = millis();
        }

        Serial.print("Distance: ");
        Serial.print(Distance);
        Serial.println(" mm");
      }
      else {
        Serial.println("Checksum error");
      }
    }
  }
  else {
    Serial.println("No data or incomplete data received");
    delay(1000);
  }
}

void updateDisplay(int value) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Distance:");
  
  display.setCursor(0, 30);
  display.setTextSize(1);
  display.print(value);
  display.setTextSize(1);
  display.print(" mm");
  
  display.display();
}