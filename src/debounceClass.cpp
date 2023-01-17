#include "debounceClass.h"
Debounce::Debounce(signed char _pin) {
	state = HIGH ;

	if( pin != -1 )
	{
		pinMode(_pin, INPUT_PULLUP); // take note I use a pull-up resistor by default
		pin = _pin;
	}
}

unsigned char Debounce::getState() {
	byte pressTimeue = state;

	if(state == RISING)  state = HIGH; // take note I use a pull-up resistor
	if(state == FALLING) state = LOW;  // rising or falling may be returned only once

	return pressTimeue; }

void Debounce::debounce() {
	bool newSample = digitalRead(pin);

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample ;

			if(newSample)	state = RISING; 
			else			state = FALLING;
		}

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	state = HIGH; 
			else			state = LOW;
		}
	}

	oldSample = newSample;
}

void Debounce::debounce( bool newSample ) {

	if(newSample == oldSample) {	// if the same state is detected atleast twice in 20ms...
	
		if(newSample != statePrev) { // if a flank change occured return RISING or FALLING
			statePrev = newSample ;

			if(newSample)	state = RISING; 
			else			state = FALLING;
		}

		else {						// or if there is no flank change return PRESSED or RELEASED
			if(newSample)	state = HIGH; 
			else			state = LOW;
		}
	}

	oldSample = newSample;
}




