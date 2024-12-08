# Arduino Based Distance Measuring System Integrated with Temperature and Humidity Monitoring

This project involves an Arduino-based system that measures distance and monitors temperature and humidity. The system uses an ultrasonic sensor for distance measurement, a DHT22 sensor for temperature and humidity monitoring, and displays the results on an LCD screen. It also features LED indicators and an active buzzer for feedback.

## Features
- **Distance Measurement**: Uses an ultrasonic sensor (HC-SR04) to measure the distance.
- **Temperature and Humidity Monitoring**: Uses the DHT22 sensor to measure the temperature and humidity.
- **LCD Display**: Displays distance, temperature, and humidity on a 16x4 LCD.
- **LED Indicators**: Green, yellow, and red LEDs for status indication based on distance.
- **Active Buzzer**: Buzzer activated for close-range distances (danger zone).
- **I2C LCD**: 16x4 LCD with I2C communication.

## Components Used
- **Arduino Uno**
- **HC-SR04 Ultrasonic Sensor**
- **DHT22 Temperature and Humidity Sensor**
- **16x4 LCD Display with I2C Module**
- **Green, Yellow, and Red LEDs (with 220-ohm resistors)**
- **Active Buzzer**
- **Breadboard**
- **Male-to-Male and Male-to-Female Jumper Wires**

## Setup Instructions
1. **Arduino Setup**: 
   - Connect the HC-SR04 sensor to the Arduino as follows:
     - **Trig Pin** to pin 9
     - **Echo Pin** to pin 10
   - Connect the DHT22 sensor to pin 7 on the Arduino.
   - Connect the LCD display using I2C:
     - **SDA** to A4 on Arduino
     - **SCL** to A5 on Arduino
   - Connect the LED pins (green, yellow, red) to pins 13, 12, and 11, respectively.
   - Connect the buzzer to pin 8.

2. **Upload the Code**: 
   - Open the Arduino IDE, copy the provided code, and upload it to your Arduino Uno.

## How It Works
- **Distance Measurement**: The system uses the HC-SR04 ultrasonic sensor to measure distance. It sends out an ultrasonic pulse and measures the time it takes to return, calculating the distance based on the speed of sound.
- **Temperature and Humidity Monitoring**: The DHT22 sensor reads the temperature and humidity every second and displays the values on the LCD.
- **LED Indicators**: The LEDs are used to indicate different status levels based on the measured distance:
  - **Red LED**: Activated when the distance is below 10 cm (danger zone).
  - **Yellow LED**: Activated when the distance is between 10 cm and 20 cm (warning zone).
  - **Green LED**: Activated when the distance is between 20 cm and 50 cm (safe zone).
- **Buzzer**: The buzzer sounds when the distance is below 10 cm (danger zone) to alert the user.

## Code

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Initialize the LCD with I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 if needed

// Define pins for LEDs and buzzer
const int greenLED = 13;  // Green LED pin
const int yellowLED = 12; // Yellow LED pin
const int redLED = 11;    // Red LED pin
const int buzzer = 8;     // Buzzer pin

// Define pins for HC-SR04
const int trigPin = 9;    // HC-SR04 Trigger pin
const int echoPin = 10;   // HC-SR04 Echo pin

// Define pin for DHT22
const int dhtPin = 7;     // DHT22 data pin

// Create DHT object
DHT dht(dhtPin, DHT22);

// Variables for distance measurement
long duration;
int distance;
const int numReadings = 10; // Number of readings for averaging
int readings[numReadings];  // Array to store distance readings
int index = 0;              // Current index in the readings array
int total = 0;              // Sum of the readings
int averageDistance = 0;    // Average distance

// Timing variables
unsigned long previousDHTMillis = 0;
const long dhtInterval = 1000; // Read DHT every second

void setup() {
  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.backlight(); // Turn on the backlight

  // Set LED and buzzer pins as output
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Set HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize DHT sensor
  dht.begin();

  // Initialize readings array
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2;

  // Update the readings array for averaging
  total -= readings[index];
  readings[index] = distance;
  total += readings[index];
  index = (index + 1) % numReadings;
  averageDistance = total / numReadings;

  // Non-blocking timing for DHT readings
  unsigned long currentMillis = millis();
  if (currentMillis - previousDHTMillis >= dhtInterval) {
    previousDHTMillis = currentMillis;
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Display on LCD
    lcd.setCursor(0, 0); // Set cursor to the first row
    lcd.print("Dist: ");
    lcd.print(averageDistance);
    lcd.print(" cm   "); // Space to clear previous text

    lcd.setCursor(0, 1); // Set cursor to the second row
    lcd.print("T: ");
    lcd.print(temperature);
    lcd.print("C H: ");
    lcd.print(humidity);
    lcd.print("%   "); // Space to clear previous text
  }

  // Control LEDs and buzzer based on average distance
  if (averageDistance < 10) { // Danger zone
    digitalWrite(redLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, HIGH);
  } else if (averageDistance >= 10 && averageDistance < 20) { // Warning zone
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, LOW);
  } else if (averageDistance >= 20 && averageDistance <= 50) { // Safe zone
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzer, LOW);
  } else { // Distance greater than 50 cm
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, LOW);
  }

  // Small delay to avoid overwhelming the loop
  delay(5); // Reduced to 5 ms for faster updates
}
