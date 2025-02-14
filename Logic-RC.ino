#include <SoftwareSerial.h>
#include <Servo.h>

const int trigPin = 9;
const int echoPin = 10;
const int rxPin = 2;
const int txPin = 3;
const int servoPin = 8;

SoftwareSerial bluetooth(rxPin, txPin);  // RX, TX
Servo sensorServo;

long duration;
int distance, move_time;
char command;
int elapsedTime;
int currentAngle = 90;

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
        command = bluetooth.read();
        move_time = bluetooth.parseInt();
        Serial.print("Received command: ");
        Serial.println(command);
        Serial.print("Move time: ");
        Serial.println(move_time);

        distance = getDistance();
        delay(500);
        Serial.print("Distance: ");
        Serial.println(distance);

        if (command == 'F' || command == 'B'){
            int elapsedTime = 0;
            moveMotors(command);
            Serial.println("Moving...");
            while (elapsedTime < move_time*1000){
                distance = getDistance();
                Serial.print("Distance: ");
                Serial.println(distance);
                if (distance < 10){
                    stopMotors();
                    Serial.println("Obstacle detected");
                    break;
                }
                elapsedTime += 100;
                delay(100);
            }
            stopMotors();
            Serial.println("Stopped");
        }
        else if (command == 'L' || command == 'R')
        {
            moveMotors(command);
            Serial.println("Moving...");
            delay(500);
            stopMotors();
            Serial.println("Stopped");
        }
        
    }
    Serial.println("Waiting for command...");
}
int getDistance(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    return (duration * 0.034 / 2);
}

void moveMotors(char command){
	switch (command)
	{
	case 'B':
		digitalWrite(7, HIGH);
		digitalWrite(6, LOW);
		digitalWrite(5, HIGH);
		digitalWrite(4, LOW);
		break;
	case 'F':
		digitalWrite(7, LOW);
		digitalWrite(6, HIGH);
		digitalWrite(5, LOW);
		digitalWrite(4, HIGH);
		break;
	case 'L':
		digitalWrite(7, HIGH);
		digitalWrite(6, LOW);
		digitalWrite(5, LOW);
		digitalWrite(4, HIGH);
		break;
	case 'R':
		digitalWrite(7, LOW);
		digitalWrite(6, HIGH);
		digitalWrite(5, HIGH);
		digitalWrite(4, LOW);
		break;
	case 'S':
		stopMotors();
		break;
	}
}

void stopMotors(){
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
}

void turnServo(char command){
    switch(command){
        case 'L':
            currentAngle = 180;
            break;
        case 'R':
            currentAngle = 0;
            break;
        case 'F':
            currentAngle = 90;
            break;
        case 'B':
            currentAngle = 270;
            break;
        sensorServo.write(currentAngle);
        delay(200);
    }
}