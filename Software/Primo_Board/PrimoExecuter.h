/*
	This Class is responsible for Executing an PrimoProgram
	using some sort of State Machine.

	With this, is possible to create even more complex
	Program cases such as 'Waiting for Sensors', Delays...
*/
#define STOPPED 0
#define RUNNING 1

class PrimoExecuter: public Thread
{
protected:

	int state;
	int currentCommand;
	long nextScheduledCommand;

	PrimoCommand *lastCommand;

	PrimoProgram *program;

public:

	void (*executeCommand)(char cmd);

	PrimoExecuter(PrimoProgram *_program){
		// Save program
		program = _program;

		state = STOPPED;
		currentCommand = -1;
		nextScheduledCommand = 0;
		lastCommand = 0;

		// Runs every 10ms
		setInterval(10);
	};

	/*
		Set flag to start executing program flow.

		Pass false as parameter to continue from
		previous paused command.
	*/
	void start(bool reset = true){
		// Change State to RUNNING
		state = RUNNING;

		// Reset if needed
		if(reset)
			currentCommand = -1;

		// Schedule next command for 0ms (next time it runs)
		nextScheduledCommand = 0;

		// Reset last executed block
		lastCommand = 0;
	};

	/*
		Set flag to stop executing.

		THIS SHOULD BE CALLED ALWAYS BEFORE CHANGING
		THE PROGRAM LIST TO AVOID LED STATE PROBLEMS
	*/
	void stop(){
		// Set Normal Block state to blocks
		for(int i = 0; i < program->size(); i++){
			program->get(i)->block->setBlockState(IDLE);
			if(program->get(i)->parentBlock)
				program->get(i)->parentBlock->setBlockState(IDLE);
		}

		// Stop flag
		state = STOPPED;
	};

	/*
		Return Current Executer state (STOPPED, RUNNING...)
	*/
	int getState(){
		return state;
	};

	/*
		Schedule the timer to run after a desired period
	*/
	void scheduleRun(long delay){
		nextScheduledCommand = millis() + delay;
	};

	/*
		Check if expired time to execute command
	*/
	bool expired(){
		return millis() >= nextScheduledCommand;
	}

	/*
		Each time run is called, it will check the
		current block, and do it's own logic.
	*/
	virtual void run(){
		runned();

		// Skip if is stopped
		if(getState() == STOPPED)
			return;

		// Check if should run next command (skip if not)
		if(!expired())
			return;

		// Next command
		currentCommand++;

		// Check if it ended program list
		if(currentCommand >= program->size())
			return stop();

		// Get current Block
		PrimoCommand *cmd = program->get(currentCommand);

		// If Command is invalid, stop
		if(!cmd)
			return stop();

		// Get associated block from Command
		Block *block = cmd->block;

		// Reset previous block state
		if(lastCommand){
			lastCommand->block->setBlockState(IDLE);
			// Parent block is also idle if set
			if(lastCommand->parentBlock)
				lastCommand->parentBlock->setBlockState(IDLE);
		}
		
		// Set Block state (Both Block and Function caller)
		block->setBlockState(EXECUTING);

		// Parent block is executing
		if(cmd->parentBlock)
			cmd->parentBlock->setBlockState(EXECUTING);

		lastCommand = cmd;


		// Execute command
		Serial.print("EXEC: ");
		Serial.println(BLOCK_STATE_NAMES[cmd->command]);

		// Delegate Command execution
		if(executeCommand)
			executeCommand(cmd->command);

		// Schedule next run
		scheduleRun(cmd->delay ? cmd->delay : 1000);

	};

};