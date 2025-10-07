#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
unsigned char buffer_RTT[4] = {0};
uint8_t CS;
#define COM 0x55
int Distance = -1, lastStableDistance = -1;
unsigned long stableStartTime = 0;
 
SoftwareSerial mySerial(2, 3);
 
// For the SSD1306 OLED display Module
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
 
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
 
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
}
 
void loop() {
  mySerial.write(COM); // COM = 0X55
  delay(100);
 
  if (mySerial.available() > 0) {
    delay(4);
    if (mySerial.read() == 0xff) {
      buffer_RTT[0] = 0xff;
      for (int i = 1; i < 4; i++) {
        buffer_RTT[i] = mySerial.read();
      }
      CS = buffer_RTT[0] + buffer_RTT[1] + buffer_RTT[2];
 
      if (buffer_RTT[3] == CS) {
        int newDistance = (buffer_RTT[1] << 8) + buffer_RTT[2];
 
        if (Distance == -1) {  
          // First valid reading, set immediately
          Distance = newDistance;
          lastStableDistance = newDistance;
          stableStartTime = millis();
          updateDisplay(lastStableDistance);
        } else if (newDistance == Distance) {
          // Check if stable for 500ms
          if (millis() - stableStartTime >= 500 && newDistance != lastStableDistance) {
            lastStableDistance = newDistance;
            updateDisplay(lastStableDistance);
          }
        } else {
          // Reset stability timer on change
          Distance = newDistance;
          stableStartTime = millis();
        }
 
        Serial.print("Current Distance: ");
        Serial.print(Distance);
        Serial.println("mm");
      }
    }
  }
}
 
void updateDisplay(int value) {
  display.clearDisplay();
  display.setCursor(10, 10);
  display.println("Distance: ");
  display.setCursor(10, 30);
  display.print(value);
  display.print("mm");
    display.setCursor(10, 50);
  display.print(value*0.00328084); // feet
  display.print("feet");
  display.display();
}
