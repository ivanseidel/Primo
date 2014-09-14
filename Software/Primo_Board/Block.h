#include <ArduinoSensors.h>
#include <AnalogIn.h>
#include <DigitalOut.h>
#include <Thread.h>

#define IDLE 0
#define EXECUTING 1

class Block: public Thread
{
protected:
	
	AnalogIn *analog;
	DigitalOut *led;

	int *states;
	int numberOfStates;

	int currentState;

	int blockState;

public:
	int ID;

	Block(){};
	Block(int _ID, int analogPin, int ledPin, int _states[], int _numberOfStates){
		Block(
			_ID,
			new AnalogIn(analogPin),
			new DigitalOut(ledPin),
			_states,
			_numberOfStates);
	};

	Block(
		int _ID,
		AnalogIn *_analog,
		DigitalOut *_led,
		int _states[],
		int _numberOfStates){

			ID = _ID;
			analog = _analog;
			led = _led;
			states = _states;
			numberOfStates = _numberOfStates;

			blockState = IDLE;
	}

	/*
		Reads block state and cache it.
		Performs Debounce and trigger changeBlock
		in boardController
	*/
	int readState(){
		// Read Analog
		analog->read();

		// Compare for each one and find out best match
		int lastError = 1024;
		int lastIndex = 0;

		int error;
		// Serial.print("Interaction Test: ");
		// Serial.println(sizeof(states));
		for(int i = 0; i < numberOfStates; i++){
			error = abs(states[i] - analog->getValue());
			// Serial.print("\tErr: ");
			// Serial.println(error);

			// Check if closer to the value; If so, save to the flag
			if(error < lastError){
				lastError = error;
				lastIndex = i;
			}
		}

		// Cache State
		currentState = lastIndex;

		// Return State
		return getState();
	};

	/*
		Get current state (Whitch kind of block inserted?)
	*/
	int getState(){
		return currentState;
	}

	/*
		Set the current Block State.
			+ IDLE:
				Usual state. Will turn on LED on block plugged

			+ EXECUTING:
				Turn OFF led.
	*/
	void setBlockState(int _blockState){
		blockState = _blockState;
	}


	/*
		Thread will:
			+ Cache Block State value (With debounce)
			+ Will correcly flash/light/turn off LED

	*/
	void run(){
		readState();

		// Turn off if Not plugged, or, EXECUTING
		if(currentState == 0 || blockState == EXECUTING)
			led->turn(LOW);
		else
			led->turn(HIGH);

		runned();
	};
};