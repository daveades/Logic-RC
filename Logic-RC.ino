#include <Servo.h>

const int TRIG_PIN = 8;		 // Trigger pin of HC-SR04
const int ECHO_PIN = 9;		 // Echo pin of HC-SR04
const int SERVO_PIN = 10;	 // Servo control pin
const int MAX_DISTANCE = 20; // Maximum distance in cm

Servo ultrasonicServo; // Create a servo object to control the direction of the ultrasonic sensor
const int FRONT_ANGLE = 90;
const int LEFT_ANGLE = 180;
const int RIGHT_ANGLE = 0;
const int BACK_ANGLE = 270;

char command;				 // Variable to store movement command
float duration;				 // Variable to store duration in seconds
unsigned long startTime = 0; // To track when movement started
bool isMoving = false;		 // Flag to track if motors are running

void setup()
{
	Serial.begin(9600); // Start serial communication at 9600 baud rate

	pinMode(TRIG_PIN, OUTPUT); // Set trigger pin as output
	pinMode(ECHO_PIN, INPUT);  // Set echo pin as input

	ultrasonicServo.attach(SERVO_PIN);
	ultrasonicServo.write(FRONT_ANGLE); // Set the initial angle of the servo

	// Set motor control pins as output
	pinMode(7, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
}

void loop()
{
	if (Serial.available() > 0 && !isMoving)
	{
		command = Serial.read();
		switch (command)
		{
			case 'C':
				handleCheckCommand(Serial.read());
				break;
			case 'F':
			case 'B':
				duration = Serial.parseFloat();
				startTime = millis();
				isMoving = true;
				moveMotors(command);
				break;

			case 'L':
			case 'R':
				duration = 0.4;
				startTime = millis();
				isMoving = true;
				moveMotors(command);
				break;

			case 'S':
				stopMotors();
				isMoving = false;
				break;
		}
	}
	if (isMoving && (millis() - startTime >= duration * 1000))
	{
		stopMotors();
		isMoving = false;
	}
}

void moveMotors(char command)
{
	switch (command)
	{
	case 'B': // Move Backward
		digitalWrite(7, HIGH);
		digitalWrite(6, LOW);
		digitalWrite(5, HIGH);
		digitalWrite(4, LOW);
		break;
	case 'F': // Move Forward
		digitalWrite(7, LOW);
		digitalWrite(6, HIGH);
		digitalWrite(5, LOW);
		digitalWrite(4, HIGH);
		break;
	case 'L': // Turn Left (1 second)
		digitalWrite(7, HIGH);
		digitalWrite(6, LOW);
		digitalWrite(5, LOW);
		digitalWrite(4, HIGH);
		break;
	case 'R': // Turn Right (1 second)
		digitalWrite(7, LOW);
		digitalWrite(6, HIGH);
		digitalWrite(5, HIGH);
		digitalWrite(4, LOW);
		break;
	case 'S': // Stop
		stopMotors();
		isMoving = false;
		break;
	}
}

void stopMotors()
{
	digitalWrite(7, LOW);
	digitalWrite(6, LOW);
	digitalWrite(5, LOW);
	digitalWrite(4, LOW);
}

int getDistance()
{
	digitalWrite(TRIG_PIN, LOW);
	delayMicroseconds(2);

	digitalWrite(TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG_PIN, LOW);

	long duration = pulseIn(ECHO_PIN, HIGH);
	int distance = duration * 0.034 / 2;

	return distance;
}


void handleCheckCommand(char direction) {
	int checkAngle;

	switch(direction) {
		case 'F':
			checkAngle = FRONT_ANGLE;
			break;
		case 'L':
			checkAngle = LEFT_ANGLE;
			break;
		case 'R':
			checkAngle = RIGHT_ANGLE;
			break;
		case 'B':
			checkAngle = BACK_ANGLE;
			break;
	}
	ultrasonicServo.write(checkAngle);
	delay(500);

	int distance = getDistance();
	ultrasonicServo.write(FRONT_ANGLE); // Reset the servo to the front

	if(distance <= MAX_DISTANCE) {
		Serial.println("O");
	} else {
		Serial.println("N");
	}
} 