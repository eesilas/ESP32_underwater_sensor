<img width="901" height="500" alt="image" src="https://github.com/user-attachments/assets/a278ec03-26aa-47df-95e0-49295fbf9cda" />

Compare Software Serial and Hardware Serial
Arduino has only on 1 serial port, use Software Serial, and
ESP32 has 3 serial ports, use Hardware Serial.
https://www.electroniclinic.com/underwater-ultrasonic-sensor-with-arduino/

Measurement Range: 5-600cm
Measurement Accuracy: ±(0.5+S* 0.5%)cm@＜2m, ±(1+S* 1%)cm@＞2m
Operating Voltage: 5-24V
Standby Current: ≤1mA
Operating Current: ≤10mA
Output Mode: UART controlled
Power-on Startup Time: ≤500ms
Output Response Time: 14ms
Angle: 8-16°
Operating Temperature: -15~50°C
Storage Temperature: -25~80°C
Storage Humidity: 65%~90% RH (non-condensing)

<img width="493" height="368" alt="image" src="https://github.com/user-attachments/assets/2bb62c71-f4ad-4b43-b35b-18bad8d99496" />

The Underwater Ultrasonic Sensor comes with impressive specifications
 It has a measurement range of 5 to 600 cm with an accuracy of ±(0.5 + S*0.5%) cm for distances under 2m and ±(1 + S*1%) cm for distances over 2m, ensuring precise readings.
In the formula ±(0.5 + S * 0.5%) cm @ <2m and ±(1 + S * 1%) cm @ >2m, the “S” represents the measured distance (in cm).
Let's explain how it works.
The error increases as the distance (S) increases.
If the measured distance S = 100 cm (<2m):
Accuracy = ±(0.5 + 100 * 0.5%) cm                              
= ±(0.5 + 0.5) cm
= ±1 cm
The accuracy is ±1 cm
If the measured distance S = 300 cm (>2m):
Accuracy = ±(1 + 300 * 1%) cm
= ±(1 + 3) cm
= ±4 cm
The accuracy is ±4 cm
This means the sensor’s error margin increases slightly with distance, but it remains within a predictable range.
The sensor operates on a voltage range of 5-24V, making it versatile for different power setups.
With a standby current of ≤1mA and an operating current of ≤10mA, it is energy-efficient.
The UART-controlled output mode ensures smooth communication with microcontrollers or other systems.
Additionally, the sensor has a quick response time of just 14ms and a power-on startup time of ≤500ms, allowing real-time data collection.

Its narrow detection angle of 8-16° helps in focused and accurate measurements.

Designed for tough conditions, it works within an operating temperature of -15 to 50°C and can be stored in temperatures ranging from -25 to 80°C, with a humidity tolerance of 65% to 90% RH (non-condensing).

These features make it an excellent choice for underwater applications where traditional sensors fail to work.

Underwater Ultrasonic Sensor Data Format:
Before starting the programming, you need to thoroughly understand the UART output format of the underwater ultrasonic sensor, as only then will you be able to successfully read the distance value. You can spend some time over here to study how these calculations are performed.

UART output format

Frame data	Output data	Description	Bytes
Frame header	0XFF	Frame header, fixed as 0XFF	1 byte
H_DATA	0X07	Higher 8 bits of distance data	1 byte
L_DATA	0XA1	Lower 8 bits of distance data	1 byte
SUM	0XA7	data checksum	1 byte
SUM= (header+Data_H+ Data_L)&0x00FF
=(0XFF + 0X07 + 0XA1)&0x00FF
=0XA7；
Distance value = DATA_ H * 256+DATA_L = 0x07A1;
Decimal conversion is equal to 1953;
Let’s first start with the most basic program. It’s purpose is to measure the distance and send the value to the Serial Monitor.

SoftwareSerial.h enables serial communication on digital pins for sensors that need software-based serial communication.
Adafruit_GFX.h and Adafruit_SSD1306.h are used to control the SSD1306 OLED display.

unsigned char buffer_RTT[4] = {0}; 
uint8_t CS;
#define COM 0x55
int Distance = -1, lastStableDistance = -1;
unsigned long stableStartTime = 0;

buffer_RTT[4] stores received data from the sensor.
CS is used for the checksum validation.
COM = 0x55 is a Command sent to the sensor to request distance data.
The variable Distance stores the most recent distance reading.
lastStableDistance stores the last stable distance reading. and
the stableStartTime variable is used to track how long a value remains unchanged.
SoftwareSerial mySerial(2, 3);  // RX on pin 2, TX on pin 3
mySerial creates a software serial connection on pins 2 (RX) and 3 (TX) for communication with the sensor.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

This set of code defines the OLED display.

Inside the setup() function.

  void setup() {
 
  Serial.begin(115200);
 
  mySerial.begin(115200);
Initializes both the hardware serial (for debugging) and software serial (for sensor communication).

if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
 
  }
Initializes the OLED display at I2C address 0x3C. If initialization fails, it enters an infinite loop.

  delay(2000);
 
  display.clearDisplay();
 
  display.setTextColor(WHITE);
 
  display.setTextSize(2);
}
Waits 2 seconds, clears the display, and sets text color and size.
Inside the loop() function.
mySerial.write(COM);
delay(100);
First, we send 0x55 “stored in COM” to request distance data from the sensor.

And then wait 100ms for the response.

if (mySerial.available() > 0) {
 
  delay(4);
Checks if data is available to read.

Waits 4ms to ensure all bytes are received.

  if (mySerial.read() == 0xff) {
 
    buffer_RTT[0] = 0xff;
 
    for (int i = 1; i < 4; i++) {
 
      buffer_RTT[i] = mySerial.read();
 
    }
Reads the first byte; if it’s 0xFF, it means valid data is coming.

Reads the next three bytes into buffer_RTT[].

CS = buffer_RTT[0] + buffer_RTT[1] + buffer_RTT[2];
Calculates the checksum to verify data integrity.
    if (buffer_RTT[3] == CS) {
      int newDistance = (buffer_RTT[1] << 8) + buffer_RTT[2];
If checksum matches, it extracts the distance value from the received data.

if (Distance == -1) { 
        Distance = newDistance;
        lastStableDistance = newDistance;
        stableStartTime = millis();
        updateDisplay(lastStableDistance);
      }
If it’s the first valid reading, update immediately and set it as stable.
    else if (newDistance == Distance) {
        if (millis() - stableStartTime >= 500 && newDistance != lastStableDistance) {
          lastStableDistance = newDistance;
          updateDisplay(lastStableDistance);
        }
      }
If the value remains the same for 500ms, update the OLED display.

This prevents rapid fluctuations from being displayed.
else {
        Distance = newDistance;
        stableStartTime = millis();
      }
If the value changes, reset the stability timer and wait for another 500ms before updating.

Serial.print("Current Distance: ");
      Serial.print(Distance);
      Serial.println("mm");
    }
 
  }
 
}
Prints the current distance reading to the Serial Monitor for debugging.

void updateDisplay(int value) {
 
  display.clearDisplay();
 
  display.setCursor(10, 10);
 
  display.println("Distance: ");
 
  display.setCursor(10, 40);
 
  display.print(value);
 
  display.print("mm");
 
  display.display();
 
}
The updateDisplay() is a user-defined function and takes one argument as the input.

Clears the OLED display, set the cursor position, prints the stable distance value and then calls

display.display() to show the updated value.
