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
