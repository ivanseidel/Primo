/*
 	Controls Primo Board

	For instructions, go to https://github.com/ivanseidel/Primo

	Created by Ivan Seidel Gomes, September, 2014.
	Released into the public domain.

	Dependencies
	I used a few libraries to make the code Reliable, Generic,
	and easy to change if anyone needs it.

	It is based on my other Library called ArduinoThreads
	(http://GitHub.com/ivanseidel/ArduinoThread).

	Also, it makes use of ArduinoSensors
	(http://GitHub.com/ivanseidel/ArduinoSensors) to read
	from IO, and debounce sensors.

	As well as LinkedList (http://GitHub.com/ivanseidel/LinkedList),
	so that the program can be compiled into a generic PrimoProgram,
	and latter be sent over Serial. 

	Make sure to have them installed.
	
	It might look like a lot of things for get this going, but the
	goal is do do even more complex things with time, including: If’s,
	Whiles, Wait blocks (Wait for Sound, Wait for touch…).

	Have fun =)
*/

// Third-Party Libraries
#include <ArduinoThread.h>
#include <ThreadController.h>

#include <ArduinoSensors.h>
#include <DigitalOut.h>
#include <AnalogIn.h>

#include <LinkedList.h>

// Primo stuff
#include "Block.h"
#include "Button.h"
#include "PrimoConfigs.h"

#include "PrimoCompiler.h"
#include "PrimoExecuter.h"

// Function LED's (Program)
#define PR 1
#define FN 13

// CPU
ThreadController CPU;

ThreadController MainProgramController;
ThreadController FunctionProgramController;

// Initialize Primo Classes
PrimoCompiler *compiler;
PrimoProgram *rootProgram;
PrimoExecuter *primoExecuter;


/*
	This Callback Function will be called
	everytime the user Presses the button
	on the Board.

	It will Compile and Execute Primo Tasks,
	or if it's already running, will stop it first.
*/
void onUserPressedStart(){

	// If is running, force stop then re-start
	if(primoExecuter->getState() == RUNNING){
		Serial.println("Stopping...");
		primoExecuter->stop();
	}

	Serial.print("Compiling...");

	compiler->Compile(rootProgram);

	Serial.print(rootProgram->size());
	Serial.print(" [ ");
	for(int i = 0; i < rootProgram->size(); i++){
		Serial.print(rootProgram->get(i)->command);
		Serial.print(' ');
	}
	Serial.println("]");

	// Start Primo Executer
	primoExecuter->start();
}

/*
	Called when PrimoExecuter wants to send a command
*/

void sendCommandViaRadio(char command){

	Serial1.write(command);

	Serial.print("SENT COMMAND: ");	
	Serial.println(command);
}

Thread test;
void thrTest(){
	// Serial1.println("Hey!");
	// Serial.println("alive");
	while(Serial1.available())
		Serial.println(Serial1.read(), HEX);
	while(Serial.available())
		Serial1.write(Serial.read());
}

/*
	This is where mostly part of the program is
	tighten toguether.

	I'm using Calbacks, and multiple Threads to do
	the work of keeping track of each block inserted,
	When the button is pressed, and even the Program
	Execution is done in parallel with other Tasks;

	Because of that, we inject into the ThreadController
	CPU all the Threads and ThreadController groups that
	we want keep running and leave the CPU.run() in the
	main loop to do it's work
*/
void setup() {
	// Initialize Serial Ports
	Serial.begin(9600);
	Serial1.begin(9600);
	while(!Serial && !Serial1);
	Serial.println("\nPRIMO v1.0\n");

	// Initialize MainProgram Blocks
	for(int b = 0; b < MainProgram_SIZE; b++){
		MainProgram[b] = new Block(
			// ID of this Block
			b,
			// AnalogIn object
			&MainProgram_Inputs[b],
			// DigitalOut object
			&MainProgram_Leds[b],
			// Possible STATES
			BLOCK_STATES,
			// Number of states
			nBLOCK_STATES);

		// Add the Block to the Thread List
		MainProgramController.add(MainProgram[b]);
	}

	// Initialize FunctionProgram Blocks
	for(int b = 0; b < FunctionProgram_SIZE; b++){
		FunctionProgram[b] = new Block(
			b,
			&FunctionProgram_Inputs[b],
			&FunctionProgram_Leds[b],
			BLOCK_STATES,
			// Function state is the last. We remove it to avoid recursivity
			nBLOCK_STATES);

		// Add the Block to the Thread List
		FunctionProgramController.add(FunctionProgram[b]);
	}

	// Add both Main and Function Controllers to CPU
	MainProgramController.setInterval(100);
	CPU.add(&MainProgramController);

	FunctionProgramController.setInterval(100);
	CPU.add(&FunctionProgramController);

	// Initialize Start Button listner and add to CPU
	StartButton.onBump = onUserPressedStart;
	CPU.add(&StartButton);

	// This is our Program. Initializing it...
	rootProgram = new PrimoProgram();
	
	// Copiler is who does the hard working on creating our program
	compiler = new PrimoCompiler(
		MainProgram, MainProgram_SIZE,
		FunctionProgram, FunctionProgram_SIZE);

	// Executer is who executes a program
	primoExecuter = new PrimoExecuter(rootProgram);
	// Set delegator for sending messages
	primoExecuter->executeCommand = sendCommandViaRadio;
	// Add to CPU
	CPU.add(primoExecuter);


	// Test thread
	test.setInterval(500);
	test.onRun(thrTest);
	CPU.add(&test);

}

void loop() {
	CPU.run();
}

