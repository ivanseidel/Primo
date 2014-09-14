/*
 	Controls Cubetto

	For instructions, go to https://github.com/ivanseidel/Primo

	Created by Ivan Seidel Gomes, September, 2014.
	Released into the public domain.

	Dependencies
	I used a few libraries to make the code Reliable, Generic,
	and easy to change if anyone needs it.

	It is based on my other Library called ArduinoThreads
	(http://GitHub.com/ivanseidel/ArduinoThread).

	I also used AccelStepper
	(http://www.airspayce.com/mikem/arduino/AccelStepper/)
	to control the Stepper motor.

	Make sure to have them installed.
	
	It might look like a lot of things for get this going, but the
	goal is do do even more complex things with time, including: If’s,
	Whiles, Wait blocks (Wait for Sound, Wait for touch…).

	Have fun =)
*/

#include <ArduinoThread.h>
#include <ThreadController.h>

#include <AccelStepper.h>

// Motores de Passo
AccelStepper Left(AccelStepper::HALF4WIRE, 9, 10, 11, 12);
AccelStepper Right(AccelStepper::HALF4WIRE, 4, 5, 6, 7);

// CPU
ThreadController CPU;

// Thread to Execute Motor commands
Thread MotorThread;
void thrRunMotors(){
	Left.run();
	Right.run();
}

// Power Saver Thread (disables outputs when idle)
Thread PowerSaver;
void thrPowerSaver(){
	// Release motors if idle
	if(Right.distanceToGo() == 0 && Left.distanceToGo() == 0){
		Left.disableOutputs();
		Right.disableOutputs();
	}
}

// Read command and execute (executes in parallel)
Thread CommandReader;
void thrCommandReader(){
	int cmd = 0;

	if(!Serial1.available())
		return;

	// Enable Outputs
	Left.enableOutputs();
	Right.enableOutputs();

	cmd = Serial1.read();

	// Invert led state
	digitalWrite(13, !digitalRead(13));

	if(cmd == 1){
		// FRONT
	    Right.move(5000);
	    Left.move(5000);
	}else if(cmd == 2){
		// LEFT
		Right.move(1000);
	    Left.move(-1000);
	}else if(cmd == 3){
		// RIGHT
		Right.move(-1000);
	    Left.move(1000);
	}
}

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);

	// Configure Both Motors
    Right.setMaxSpeed(2200);
    Right.setAcceleration(3500);

	Left.setMaxSpeed(2200);
	Left.setAcceleration(3500);
    
	// Setup CPU
	MotorThread.onRun(thrRunMotors);
	MotorThread.setInterval(0);

	CommandReader.onRun(thrCommandReader);
	CommandReader.setInterval(500);

	PowerSaver.onRun(thrPowerSaver);
	PowerSaver.setInterval(1000);

	CPU.add(&MotorThread);
	CPU.add(&CommandReader);
	CPU.add(&PowerSaver);

	pinMode(13, OUTPUT);
}

void loop() {
	CPU.run();
}

