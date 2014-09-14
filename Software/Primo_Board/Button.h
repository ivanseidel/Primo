class Button: public Thread
{
protected:
	int pin;
	int debounce;
	bool invert;

	bool lastState;
	long lastStartPressingTime;


public:
	// Callback for Bumped button (Press+Release with minimum DEBOUNCE time)
	void (*onBump)(void);

	Button(int _pin, int _debounce = 50, bool _invert = false){
		pin = _pin;
		debounce = _debounce;
		invert = _invert;

		pinMode(pin, INPUT_PULLUP);
		setInterval(25);
	};

	bool read(){
		return digitalRead(pin) ^ invert;
	};

	virtual void run(){
		bool now = read();

		// If PRESSED and WAS NOT PRESSED (Started Pushing)
		if(now && !lastState){
			// Save current Time
			lastStartPressingTime = millis();
		}

		// Was Released!
		else if(!now && lastState){
			// Check Passed Time egainst debounce time
			if(millis() - lastStartPressingTime >= debounce){
				// That's it! Bumped. Callback (if any)
				if(onBump)
					onBump();
			}
		}

		lastState = now;
	};



};