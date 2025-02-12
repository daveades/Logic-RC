#include <SoftwareSerial.h>

const int trigPin = 9;
const int echoPin = 10;
const int rxPin = 2;  // Bluetooth RX pin
const int txPin = 3;  // Bluetooth TX pin

SoftwareSerial bluetooth(rxPin, txPin);  // RX, TX
long duration;
int distance;

void setup() {
    Serial.begin(9600);      // Start Serial monitor
    bluetooth.begin(9600);   // Start Bluetooth communication
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
}

void loop() {
    // Handle Bluetooth data
    if (bluetooth.available()) {
        String command = bluetooth.readStringUntil('\n');
        Serial.print("Received command: ");
        Serial.println(command);
    }

    // Ultrasonic sensor code
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    
    // Send distance data to both Serial Monitor and Bluetooth
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    bluetooth.print("Distance: ");
    bluetooth.print(distance);
    bluetooth.println(" cm");
    
    delay(500);
}