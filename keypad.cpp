#include <Arduino.h>
#include "keypad.h"

Keypad::Keypad(unsigned char pin1, unsigned char pin2, unsigned char pin3, unsigned char pin4, unsigned char pin5, unsigned char pin6, unsigned char pin7, unsigned char pin8) {
        keyPin[0] = pin1;
        keyPin[1] = pin2;
        keyPin[2] = pin3;
        keyPin[3] = pin4;
        keyPin[4] = pin5;
        keyPin[5] = pin6;
        keyPin[6] = pin7;
        keyPin[7] = pin8; }
		

void Keypad::init(void) {
        for(unsigned char i = 0; i < 4; i++) {
            pinMode(keyPin[i], OUTPUT);
            pinMode(keyPin[i+4], INPUT);
            digitalWrite(keyPin[i+4], HIGH); } }

char Keypad::readKeyPad() {
    unsigned char row, column;
    static char keyPrev = 0;
    for(column=0;column<4;column++){
        for(unsigned char i=0;i<4;i++) digitalWrite(keyPin[i],HIGH); 	// set all 4 column pins HIGH
        digitalWrite(keyPin[column], LOW);						// set 1 column pin LOW

        for(row=0;row<4;row++) {				
    
            if(!digitalRead(keyPin[row+4])) {	
                char key = keys[row][column];
                if(key != keyPrev) { 
                    keyPrev = key;       
                    return key; }
                else return 0; } } }
    keyPrev = 0;
    return 0; }	
		
