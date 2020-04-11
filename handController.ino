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

 
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "keypad.h"

Keypad keypad(4,5,6,7,0,1,2,3);

enum commands {
	instruction = 'a',
	power = 'P',
	selectDecoder = 'b',
	setSpeed = 'e' };

#define lowerLimit 170
#define upperLimit 853

#define POT_PIN A1
#define Estop A0



#define loginCode 'g'


enum mySerialCommands {
	ADDRES = '1',
	SPEED1,
	HEADLIGHT,
	FUNCTION_BANK,
	FUNCTION,
	DECODER_SELECTED,
	TRAIN_DELETED,
	POWER,
	LCD_COMMAND1,
	LCD_COMMAND2,
	IDLE} ;

//byte keyPin[8]={0,1,2,3,4,5,6,7};


enum mcp23017 {
	addres = 0x20,
	IODIRA = 0x00,
	IODIRB = 0x01,
	portA = 12,
	portB = 13};

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// these constants won't change.	But you can change the size of
// your LCD using them:
const int numRows = 2;
const int numCols = 16;

byte timeOut;
byte pressT;
byte mode = IDLE;

union {
	byte b;
	char c;
} var;

LiquidCrystal lcd(13, 12, 8, 10, 9, 11);
SoftwareSerial mySerial(A2,A3);

void setup() {
	mySerial.begin(9600);
  delay(5000);
	mySerial.write(loginCode);

  pinMode(Estop,INPUT);
  digitalWrite(Estop,HIGH);
  
	keypad.init();

	lcd.begin(numCols, numRows);
	lcd.setCursor(0,1);
	lcd.print("SPEED="); }

void updateTimers() {		// take note, this function has deviating indentations
	static byte toggle = 0, ms, cs;
	if((millis() & 0x1) ^ toggle) {	toggle ^= 1; ms++; // every ms
		if(timeOut) timeOut--;

	if(ms == 10) {	ms = 0;	cs += 1;				// every 10ms

	if(cs == 10) {	cs = 0; 						// every 100ms
		if(pressT) pressT--;

} } } }

		

void loop() {
	updateTimers();
	readmySerialBus();								// reads stuff from DCCduino and displays it on LCD
	
	if(!timeOut) {
		timeOut = 50;
		if(readPotentiometer(&var.b))	sendSpeed(var.b);										// read the potentiometer and transmitt a byte if it has changed 
		char c = keypad.readKeyPad(); if(c) sendKeypad(c);									 // read the keypad and transmitt a byte if something is pressed
		if(readButtons())			sendButtons(); } }

byte readPotentiometer(byte *ptr) {
	static int previousSample = 0;
	
	byte sample = 56 - map(constrain(analogRead(POT_PIN),lowerLimit,upperLimit), lowerLimit, upperLimit, 0,56);		 // re-map the value to speed range

	if(sample != previousSample ) {
		previousSample = sample;
		*ptr = sample;
		return 1;}
	else {
		return 0; } }

void readmySerialBus() {
	if(mySerial.available()) {
		byte b = mySerial.read();
    static bool receiveState = false;
		switch(mode) {
			default: mode = IDLE; break;

			case IDLE:
			if(b == '$') {
			  mode = LCD_COMMAND1;	 // $$ = LCD command
        //lcd.setCursor(0,0);
        //lcd.print("X1"); 
       }
			break;

			case LCD_COMMAND1:
			if(b == '$') {
			  mode = LCD_COMMAND2;
			  //lcd.setCursor(0,0);
        //lcd.print("X2"); 
			}
			break;
			
			case LCD_COMMAND2:
			mode = b;
     // lcd.setCursor(0,0);
     // lcd.print(mode); 
			break;
			
			case ADDRES: 
			lcd.setCursor(0,0);
			lcd.print("ADDRES=");
			if(b<10)lcd.print(" ");
			lcd.print(b);
			mode = IDLE;
			break;
			
			case SPEED1:{
			signed char _speed = b - 28;
			lcd.setCursor(0,1);
			lcd.print("SPEED=");
			if(_speed >= 0) {
				lcd.print(" ");
				if(_speed < 10) lcd.print(" "); }
			else {
				if(_speed > -10) lcd.print(" "); }
			lcd.print(_speed);
			mode = IDLE;}
			break;
			
			case HEADLIGHT:
			lcd.setCursor(10,1);
			lcd.print("HL=");
			if(b == '1') lcd.print("ON ");
			else if(b == '0') lcd.print("OFF");
			mode = IDLE;
			break;
			
			case FUNCTION_BANK:
			lcd.setCursor(10,0);
			if(b == '1') lcd.print("F5-F8 ");
			else if(b == '0') lcd.print("F1-F8 ");
			mode = IDLE;
			break;
			
			case DECODER_SELECTED:
			lcd.setCursor(0,1);
			switch(b){
				case '0': lcd.print("MM2");break;
				case '1': lcd.print("DCC14");break;
				case '2': lcd.print("DCC18");break;}
			lcd.print(" SELECTED");
			delay(1500);
			lcd.clear();
			mode = IDLE; 
			break; 

			case FUNCTION:
			// static bool receiveState = false;
			if(!receiveState) {
				receiveState = true;
				lcd.setCursor(10,0);
				lcd.print("F");
				lcd.print(b-'0');
				lcd.print("="); }
			else {
				receiveState = false;
				if(b == '1') lcd.print("ON ");
				else if(b == '0') lcd.print("OFF");
				mode = IDLE; }
			break; 
			
			case POWER:
			lcd.clear();
			lcd.setCursor(3,0);
			lcd.print("POWER ");
			if(b == '1') lcd.print("ON ");
			if(b == '0') lcd.print("OFF");
      mode = IDLE;
			break; } } }

void sendKeypad(char character) {
	// lcd.setCursor(0,0);
	// lcd.print(character);
	if(character < '0' || character > '9') { // sends an instruction if no number
		mySerial.write(instruction); }
	mySerial.write(character);}

void sendSpeed(byte speed) {
	mySerial.write(setSpeed);
	mySerial.write(speed);}

byte readButtons() {
  static byte state, statePrev;
  
  state = !digitalRead(Estop);
	if(state) {
    if(state != statePrev) {
      statePrev = 1;
  		return 1; }
    else {
		if(!pressT) {
			mySerial.write('b');
      lcd.setCursor(0,0);
      lcd.print("DECODER? 1=DCC14");
      lcd.setCursor(0,1);
      lcd.print("2=DCC28 3=MM2   ");
			pressT = 200; }			
    	return 0; } }
	statePrev = 0;
	pressT = 10; // 1 second delay
	return 0; }

void sendButtons() {
	mySerial.write(instruction);
	mySerial.write(power); }




