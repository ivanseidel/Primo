/*
	Responsible for Taking an Array of Briks,
	and turning into an ArrayList of Commands
	that will be sent to Cubetto.
*/

// Class contains the Command to send, and a timeout
class PrimoCommand
{
public:
	char command;
	int delay;
	Block *block;
	Block *parentBlock;

	PrimoCommand(char _command, Block *_block, int _delay = 1000, Block *_parentBlock = 0){
		command = _command;
		delay = _delay;
		block = _block;
		parentBlock = _parentBlock;
	};
};

// Defining the Type of PrimoProgram
typedef LinkedList<PrimoCommand*> PrimoProgram;

class PrimoCompiler
{
protected:
	Block **mainBlocks;
	int numberOfMainBlocks;

	Block **functionBlocks;
	int numberOfFunctionBlocks;

	// This flag is used to know if is inside a function compilation;
	// If inside a function, this should be the pointer to the Function Blocl
	Block *_functionBlockCaller;
public:

	PrimoCompiler(
		Block *_mainBlocks[], const int _numberOfMainBlocks,
		Block *_functionBlocks[], const int _numberOfFunctionBlocks){
		
			mainBlocks = _mainBlocks;
			numberOfMainBlocks = _numberOfMainBlocks;

			functionBlocks = _functionBlocks;
			numberOfFunctionBlocks = _numberOfFunctionBlocks;

			_functionBlockCaller = 0;
	}

	/*
		Does a Full Compilation.

		Start by compiling MainBlocks (MainProgram)
	*/
	void Compile(PrimoProgram *program){
		// Serial.println("\nCOMPILING FROM MAIN CODE");
		// Serial.println("+ Clear ArrayList");

		// Clear the Program (Free Command memory)
		while(program->size() > 0)
			delete(program->shift());

		// Serial.print("+ CompileProgram |Blocks: ");
		// Serial.println(numberOfMainBlocks);
		
		// Compile Main program
		CompileProgram(program, mainBlocks, numberOfMainBlocks, 0);
	};

	/*
		This Method will do the compilation of a given
		Array of Block objects.

		It will Read each one of them, and call 
			CompileBlock(List, block, position);

			PrimoProgram *program:
				The List to save the compiled Program

			Block blocks: 
				Blocks used to compile. They will be
				compiled in the given order

			numberOfBlocks:
				The Number of blocks to compile from `blocks`

			compileAt:
				Specify an index to compile at this point in
				the list. (Usefull for Function Blocks)

		Returns: Number of blocks Compiled
	*/
	int CompileProgram(
		PrimoProgram *program,
		Block *blocks[],
		int numberOfBlocks,
		int compileAt = 0){

		int compiled = 0;

		// Serial.print("- CompileProgram at ");
		// Serial.println(compileAt);

		// Iterate through blocks compiling them
		for(int i = 0; i < numberOfBlocks; i++){
			Block *block = blocks[i];

			// Serial.print("+ Compiling block: ");
			// Serial.print(block->ID);
			// Serial.print(" at ");
			// Serial.println(i + compileAt);

			int added = PrimoCompiler::CompileBlock(
				program,
				block,
				// Add the offset given to this method
				compileAt + compiled);

			// Add offset given by CompileBlock. Also
			if(added < 0) break;
			if(added > 0) compiled += added;
		}

		Serial.print("+ Finished CompileProgram: ");
		Serial.println(compiled);
		return compiled;
	};

	/*
		This method will create PrimoCommands and insert
		at the specified index.

		If it's a function, it should add multiple Commands.

		It should return the number of blocks compiled,
		or -1 if BREAK is requested (END OF PROGRAM)
			-1: End of program;

			0: None added
			1: 1 Added
			Functions should return 0+

	*/
	int CompileBlock(
		PrimoProgram *program,
		Block *block,
		int index){

		int state = block->readState();
		
		if(state == EMPTY){
			// No Block Inserted. Break it.
			return -1;
		}else if(state == FUNCTION){

			// Skip if trying recursive (HAHA!)
			if(_functionBlockCaller)
				return 0;

			/*
				If it's a function, compile the Function
				and add it to the Program.
			*/
			_functionBlockCaller = block;
			int ret = CompileProgram(program, functionBlocks, numberOfFunctionBlocks, index);
			_functionBlockCaller = 0;

			// Minimum return is 0 (Consumed OK) when executing functions
			return (ret < 0 ? 0 : ret);
		}else{
			// Add the custom state read
			program->add(index, new PrimoCommand(state, block, BLOCK_TIME, _functionBlockCaller));
			return 1;
		}

		return -1;
	};
};