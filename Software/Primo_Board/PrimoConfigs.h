/*
	===============================
	STATE CONFIGURATION
	===============================
*/ 

// Time in ms for each Block execution
#define BLOCK_TIME 4000

// Debounce time used in Button (ms)
#define DEBOUNCE_TIME 100

// Possible block states
const int EMPTY = 0;
const int FRONT = 1;
const int LEFT  = 2;
const int RIGHT = 3;
const int FUNCTION = 4;

/*
	This is where you configure the approximate
	value for each of the possible states.

	Each line corresponds to a state (0, 1, 2...)
	Put the desired ADC value on the line.

	If you perhaps, changed the resistors for each
	block, calculate the value as the folowing example:

		Used Pull-Down Resistor: 10.000Ohm (R2)
		Voltage: 5V (V)
		ADC = (V*R2 / (R2 + YourResistance)) / V * 1023
*/
int BLOCK_STATES[] = {
	0,    // State 0: EMPTY
	839,  // State 1: FRONT
	320,  // State 2: LEFT
	1023, // State 3: RIGHT
	512,  // State 4: FUNCTION
};

// Usefull for Debugging
const char BLOCK_STATE_NAMES[][16] = {
	"EMPTY\0",
	"FRONT\0",
	"LEFT\0",
	"RIGHT\0",
	"FUNCTION\0"
};

// Cache BLOCK STATES SIZE
const int nBLOCK_STATES = sizeof(BLOCK_STATES)/sizeof(BLOCK_STATES[0]);




/*
	===============================
	I/O CONFIGURATION
	===============================

	I divided the Primo Board into:
		+ Board (Board IO, such as Button, Beeps...) 
		+ MainProgram (Usual Program Flow)
		+ FunctionProgram (The Function flow)

	HOW TO change stuff:
	
	If you want to change an LED pin:
		
		If it is from the MainPgoram, alter
		the MainProgram_Leds.
		Or change FunctionProgram_Leds.


	Fast Documentation:
		DigitalOut(PIN, IS_INVERTED);
		AnalogIn(ANALOG_PIN);
		Button(PIN, DESIRED_DEBOUNCE, INVERT);
*/

// Board Config (Inverted mode)
Button StartButton(53, DEBOUNCE_TIME, true);

// Main Program Config
DigitalOut MainProgram_Leds[] = {
	DigitalOut(28, true),	// 1
	DigitalOut(29, true),	// 2
	DigitalOut(30, true),	// 3
	DigitalOut(31, true),	// 4

	DigitalOut(34, true),	// 5
	DigitalOut(22, true),	// 6
	DigitalOut(33, true),	// 7
	DigitalOut(32, true),	// 8

	DigitalOut(23, true),	// 9
	DigitalOut(24, true),	// 10
	DigitalOut(35, true),	// 11
	DigitalOut(36, true),	// 12
};

AnalogIn MainProgram_Inputs[] = {
	AnalogIn(A12),	// 1
	AnalogIn(A14),	// 2
	AnalogIn(A13),	// 3
	AnalogIn(A15),	// 4

	AnalogIn(A4),	// 5
	AnalogIn(A5),	// 6
	AnalogIn(A6),	// 7
	AnalogIn(A7),	// 8

	AnalogIn(A8),	// 9
	AnalogIn(A9),	// 10
	AnalogIn(A10),	// 11
	AnalogIn(A11),	// 12
};

const int MainProgram_SIZE = sizeof(MainProgram_Inputs)/sizeof(MainProgram_Inputs[0]);

// Function Program Config
DigitalOut FunctionProgram_Leds[] = {
	DigitalOut(41, true),	// 13
	DigitalOut(43, true),	// 14
	DigitalOut(25, true),	// 15
	DigitalOut(45, true),	// 16
};

AnalogIn FunctionProgram_Inputs[] = {
	AnalogIn(A0),	// 13
	AnalogIn(A1),	// 14
	AnalogIn(A2),	// 15
	AnalogIn(A3),	// 16
};

const int FunctionProgram_SIZE = sizeof(FunctionProgram_Inputs)/sizeof(FunctionProgram_Inputs[0]);

/*
	===============================
	BLOCK PROGRAM CONFIGURATION
	===============================

	This is where all Blocks will be initialized and saved
*/
Block *MainProgram[MainProgram_SIZE] = {0};
Block *FunctionProgram[FunctionProgram_SIZE] = {0};