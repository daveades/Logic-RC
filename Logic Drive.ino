const int TRIG_PIN = 8;		 // Trigger pin of HC-SR04
const int ECHO_PIN = 9;		 // Echo pin of HC-SR04
const int MAX_DISTANCE = 20; // Maximum distance in cm

char command;				 // Variable to store movement command
float duration;				 // Variable to store duration in seconds
unsigned long startTime = 0; // To track when movement started
bool isMoving = false;		 // Flag to track if motors are running

void setup()
{
	Serial.begin(9600); // Start serial communication at 9600 baud rate

	pinMode(TRIG_PIN, OUTPUT); // Set trigger pin as output
	pinMode(ECHO_PIN, INPUT);  // Set echo pin as input

	// Set motor control pins as output
	pinMode(7, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
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

void loop()
{
	int distance = getDistance(); // Get distance from ultrasonic sensor
	Serial.println("B");
	Serial.println(distance);

	if (isMoving && command == 'F' && distance <= MAX_DISTANCE)
	{
		stopMotors();
		isMoving = false;
		Serial.println("O");
		return;
	}

	if (Serial.available() > 0)
	{
		Serial.println("A");
		Serial.println(distance);
		command = Serial.read(); // Read command character

		if (command == 'F')
		{
			Serial.println(distance);
			if (distance <= MAX_DISTANCE)
			{
				Serial.println("O");
				return;
			}
			duration = Serial.parseInt(); // Read duration in seconds
		}
		else if (command == 'B')
		{
			duration = Serial.parseInt();
		}
		else if (command == 'L' || command == 'R')
		{
			duration = 0.4;
		}

		startTime = millis();
		isMoving = true;
		moveMotors(command);
	}

	// Check if it's time to stop the motors
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