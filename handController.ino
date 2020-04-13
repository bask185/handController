/* NOTE SELECTED DECODER TXT DOES NOT GIVE PROPER FEEDBACK ABOUT WHICH DECODER IS ACTUALLY SELECTED
 *  ALSO TEXT ON TOP LINE IS CRAPPY, A PART OF POWER ON/OFF STAYS ON SCREEN FOR EVER
 * LCD instructions: 
 * 1 = addres										 1 follow bytes
 * 2 = speed											1 follow bytes
 * 3 = headlight ON							 1 follow bytes	
 * 4 = F1-F4/F5-F8 selected			 1 follow bytes
 * X = prompt decoder						 0 follow bytes
 * 5 = decoder selected					 1 follow bytes
 */

/******		16 x 2 LCD LAYOUT		*****/
/*	0	1	2	3	4	5	6	7	8	9	10 	11 	12 	13 	14 	15

 *	A	D	D	R	E	S	S	=	X	X	X	_	_	_	_	_    PERMANENT
 *	S	P	E	E	D	=	X	X	_	_	F	X	=	O	F	F
 *	
 *	
 *	
 *	D	E	C	O	D	E	R	?	_	1	=	D	C	C	1	4 	volatile
 *	2	=	D	C	C	2	8	_	_	3	=	M	M	2	_	_ 
 *	
 *	D	C	C	1	4	_	S	E	L	E	C	T	E	D	_	_ or dcc28 of MM2 
 *	_	_	_	_	_	_	_	_	_	_	_	_	_	_	_	_	
 
 */	

 
#include "keypad.h"
#include "src/basics/timers.h"
#include "roundRobinTasks.h"
#include "serial.h"
#include "lcd.h"


void setup() {
	pinMode(Estop,INPUT);
	digitalWrite(Estop,HIGH);
	//Serial.begin(115200); //interferes with the keypad
	initSerial();
	initRoundRobin();
	lcdInit();
	initTimers();

}	


void loop() {
	processRoundRobinTasks();
}










