#include <SoftwareSerial.h>

const int trigPin = 9;
const int echoPin = 10;
const int rxPin = 2;
const int txPin = 3;

SoftwareSerial bluetooth(rxPin, txPin);  // RX, TX
long duration;
int distance;

void setup() {
    Serial.begin(9600);
    bluetooth.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
}

void loop() {
    if (bluetooth.available()) {
        String command = bluetooth.readStringUntil('\n');
        Serial.print("Received command: ");
        Serial.println(command);
    }

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    bluetooth.print("Distance: ");
    bluetooth.print(distance);
    bluetooth.println(" cm");
    
    delay(500);
}