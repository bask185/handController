#include <Arduino.h>
#include "macros.h"

#ifndef oled_h
#define oled_h


extern void clearDisplay() ;
extern void printCustom( uint8 , uint8 , uint8 state, char*  ) ;
extern void clear( uint8  ) ;
extern void printAt(uint8 , uint8 , String  ) ;
extern void printCharAt(uint8 , uint8 , String  ) ;
extern void printNumberAt( uint8, uint8 , uint8 , uint16  ) ;
extern void lcdInit() ;

extern uint8 toRam[] ;

enum Symbols
{
    eNone,
    eSpeakerSymbol,
    eHorn1,
    eHorn2,
    eBell,
    eMusic,
    eSmoke,
    eLight,
    eCabineLight,
    eBrakeSqueking,
}  ;

/*
speakerSymbol
  1 2 3 4 5 6 7 8
1         X      
2       X X   X  
3 X X X X X   X  
4 X X X X X   X  
5   X   X X   X  
6   X     X      
7   X            
8                  
*/
const char speakerSymbol[] PROGMEM  =
{
    0b00001100,
    0b01111100,
    0b00001100,
    0b00011110,
    0b00111111,
    0b00000000,
    0b00011110,
    0b00000000,
} ;


/*
horn1
  1 2 3 4 5 6 7 8
1         X      
2       X X      
3     X X X      
4 X   X X X      
5     X X X      
6       X X      
7         X      
8                
*/
const char horn1[] PROGMEM  =
{
    0b00001000,
    0b00000000,
    0b00011100,
    0b00111110,
    0b01111111,
    0b00000000,
    0b00000000,
    0b00000000,
} ;



/*
horn2
  1 2 3 4 5 6 7 8
1         X   X  
2       X X   X  
3     X X X   X  
4 X   X X X   X  
5     X X X   X  
6       X X   X  
7         X   X  
8                
*/
const char horn2[] PROGMEM  =
{
    0b00001000,
    0b00000000,
    0b00011100,
    0b00111110,
    0b01111111,
    0b00000000,
    0b01111111,
    0b00000000,
} ;


/*
bell
  1 2 3 4 5 6 7 8
1       X X      
2     X X X X    
3     X X X X    
4     X X X X    
5   X X X X X X  
6 X X X X X X X X
7       X X      
8                
*/
const char bell[] PROGMEM  =
{
    0b00100000,
    0b00110000,
    0b00111110,
    0b01111111,
    0b01111111,
    0b00111110,
    0b00110000,
    0b00100000,
} ;


/*
music
  1 2 3 4 5 6 7 8
1       X X X X X 
2     X X X X X X
3     X         X
4     X     X X X
5 X X X     X X X
6 X X X     X X X
7 X X X          
8                
*/
const char music[] PROGMEM  =
{
    0b01110000,
    0b01110000,
    0b01111110,
    0b00000011,
    0b00000011,
    0b00111011,
    0b00111011,
    0b00111111,
} ;


/*
play
  1 2 3 4 5 6 7 8
1           X    
2       X X X       
3   X X X X X    
4 X X X X X X    
5   X X X X X      
6       X X X     
7           X          
8                
*/
const char play[] PROGMEM  =
{
    0b00001000,
    0b00011100,
    0b00011100,
    0b00111110,
    0b00111110,
    0b01111111,
    0b00000000,
    0b00000000,
} ;


/*
pause
  1 2 3 4 5 6 7 8
1   X X     X X  
2   X X     X X  
3   X X     X X  
4   X X     X X  
5   X X     X X  
6   X X     X X  
7   X X     X X  
8                
*/
const char pause[] PROGMEM  =
{
    0b00000000,
    0b01111111,
    0b01111111,
    0b00000000,
    0b00000000,
    0b01111111,
    0b01111111,
    0b00000000,
} ; ;

/*
stop
  1 2 3 4 5 6 7 8
1                
2   X X X X X X  
3   X X X X X X  
4   X X X X X X  
5   X X X X X X  
6   X X X X X X  
7   X X X X X X  
8                
*/
const char stop[] PROGMEM  =
{
    0b00000000,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00000000,
} ;

/*
record
  1 2 3 4 5 6 7 8
1       X X X     
2     X X X X X    
3   X X X X X X X 
4   X X X X X X X 
5   X X X X X X X 
6     X X X X X   
7       X X X     
8                  
*/
const char record[] PROGMEM  =
{
    0b00000000,
    0b00011100,
    0b00111110,
    0b01111111,
    0b01111111,
    0b01111111,
    0b00111110,
    0b00011100,
} ;

/*
up
  1 2 3 4 5 6 7 8
1       X        
2     X X X      
3   X X X X X    
4 X X X X X X X  
5                
6                
7                
8                
*/
const char up[] PROGMEM  =
{
    0b00001000, 
    0b00001100, 
    0b00001110, 
    0b00001111, 
    0b00001110, 
    0b00001100, 
    0b00001000, 
    0b00000000, 
} ;


/*
down
  1 2 3 4 5 6 7 8
1                
2                
3                
4 X X X X X X X  
5   X X X X X    
6     X X X      
7       X        
8                
*/
const char down[] PROGMEM  =
{
    0b00001000,
    0b00011000,
    0b00111000,
    0b01111000,
    0b00111000,
    0b00011000,
    0b00001000,
    0b00000000,
} ;


/*
neutral
  1 2 3 4 5 6 7 8
1                
2                
3                
4 X X X X X X X X
5 X X X X X X X X
6                
7                
8                
*/
const char neutral[] PROGMEM  =
{
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
} ;


/*
curved
  1 2 3 4 5 6 7 8
1                
2               X
3             X  
4         X X    
5 X X X X        
6                
7                
8                
*/
const char curved[] PROGMEM  =
{
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00001000,
    0b00001000,
    0b00000100,
    0b00000010,
} ;


/*
straight
  1 2 3 4 5 6 7 8
1                
2                
3                
4                
5 X X X X X X X X
6                
7                
8                
*/
const char straight[] PROGMEM  =
{
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
} ;


/*
ramp
  1 2 3 4 5 6 7 8
1               X
2             X X
3           X   X
4       X X     X
5 X X X         X
6 X X X X X X X X
7                
8                
*/
const char ramp[] PROGMEM  =
{
    0b00110000,
    0b00110000,
    0b00110000,
    0b00101000,
    0b00101000,
    0b00100100,
    0b00100010,
    0b00111111,
} ;


/*
whistle
  1 2 3 4 5 6 7 8
1 X       X X X X
2   X       X X X
3 X     X     X X
4   X       X X X
5 X       X X X X
6         X X X X
7         X X X X
8                
*/
const char whistle[] PROGMEM  =
{
    0b00010101,
    0b00001010,
    0b00000000,
    0b00000100,
    0b01110001,
    0b01111011,
    0b01111111,
    0b01111111,
} ;


/*
smoke
  1 2 3 4 5 6 7 8
1             X X 
2         X X X  
3       X X      
4     X          
5   X X          
6   X X          
7   X X          
8                
*/
const char smoke[] PROGMEM  =
{
    0b00000000,
    0b01110000,
    0b01111000,
    0b00000100,
    0b00000110,
    0b00000010,
    0b00000010,
    0b00000000,
} ;


/*
light
  1 2 3 4 5 6 7 8
1       X   X    
2     X X   X    
3   X X X   X    
4   X X X   X    
5   X X X   X    
6     X X   X    
7       X   X    
8                
*/
const char light[] PROGMEM  =
{
    0b00000000,
    0b00011100,
    0b00111110,
    0b01111111,
    0b00000000,
    0b01111111,
    0b00000000,
    0b00000000,
} ;


/*
cabineLight
  1 2 3 4 5 6 7 8
1 X X X X X X X X
2   X X X X X X  
3     X X X X    
4                
5     X   X   X  
6                
7   X     X     X
8                
*/
const char cabineLight[] PROGMEM  =
{
    0b00000001,
    0b01000011,
    0b00010111,
    0b00000111,
    0b01010111,
    0b00000111,
    0b00010011,
    0b01000001,
} ;


/*
brakeSqueking
  1 2 3 4 5 6 7 8
1     X X     X  
2   X X X X     X
3 X X     X X   X
4 X X     X X   X
5   X X X X     X
6     X X     X  
7                
8                
*/
const char brakeSqueking[] PROGMEM  =
{
    0b00001100,
    0b00011110,
    0b00110011,
    0b00110011,
    0b00011110,
    0b00001100,
    0b00100001,
    0b00011110,
} ;



/*
F0
  1 2 3 4 5 6 7 8
1 X X X          
2 X       X X X X 
3 X       X     X 
4 X X     X     X 
5 X       X     X 
6 x       X     X 
7 X       X X X X  
8                 
*/
const char F0[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01111110,
} ;



/*
F1
  1 2 3 4 5 6 7 8
1 X X X          
2 X             X 
3 X             X 
4 X X           X 
5 X             X 
6 x             X 
7 X             X  
8                 
*/
const char F1[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111110,
} ;


/*
F2
  1 2 3 4 5 6 7 8
1 X X X          
2 X         X X   
3 X       X     X 
4 X X           X 
5 X           X   
6 x         X     
7 X       X X X X      
8                 
*/
const char F2[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b01000100,
    0b01100010,
    0b01010010,
    0b01001100,
} ;


/*
F3
  1 2 3 4 5 6 7 8              
1 X X X   X X X   
2 X     X       X 
3 X             X 
4 X X       X X   
5 X             X 
6 x     X       X 
7 X       X X X   
8                 
*/
const char F3[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00100010,
    0b01000001,
    0b01001001,
    0b01001001,
    0b00110110,
} ;


/*
F4
  1 2 3 4 5 6 7 8
1 X X X       X  
2 X         X X    
3 X       X   X  
4 X X   X     X  
5 X     X X X X X 
6 x           X   
7 X           X    
8                 
*/
const char F4[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00011000,
    0b00010100,
    0b00010010,
    0b01111111,
    0b00010000,
} ;


/*
F5
  1 2 3 4 5 6 7 8
1 X X X           
2 X       X X X X       
3 X       X        
4 X X     X X X      
5 X             X          
6 x       X     X 
7 X         X X   
8                        
*/
const char F5[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b00101110,
    0b01001010,
    0b01001010,
    0b00110010,
} ;


/*
F6
  1 2 3 4 5 6 7 8
1 X X X          
2 X         X X    
3 X       X          
4 X X     X X X      
5 X       X     X  
6 x       X     X 
7 X         X X   
8                 
*/
const char F6[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b00111100,
    0b01001010,
    0b01001010,
    0b00110000,
} ;


/*
F7
  1 2 3 4 5 6 7 8
1 X X X          
2 X       X X X X     
3 X             X
4 X X         X    
5 X         X     
6 x       X        
7 X       X        
8                 
*/
const char F7[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b01100010,
    0b00010010,
    0b00001010,
    0b00000110,
} ;



/*
F8
  1 2 3 4 5 6 7 8
1 X X X     X X           
2 X       X     X 
3 X       X     X 
4 X X       X X    
5 X       X     X 
6 x       X     X 
7 X         X X    
8                 
*/
const char F8[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b00110110,
    0b01001001,
    0b01001001,
    0b00110110,
} ;


/*
F9
  1 2 3 4 5 6 7 8
1 X X X     X X          
2 X       X     X 
3 X       X     X 
4 X X       X X X  
5 X             X 
6 x       X     X 
7 X         X X    
8                 
*/
const char F9[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b00100110,
    0b01001001,
    0b01001001,
    0b00111110,
} ;


/*
F10
  1 2 3 4 5 6 7 8
1 X X X             
2 X     X   X X X
3 X     X   X   X
4 X X   X   X   X 
5 X     X   X   X
6 x     X   X   X
7 X     X   X X X  
8                
*/
const char F10[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b01111110,
    0b00000000,
    0b01111110,
    0b01000010,
    0b01111110,
} ;


/*
F11
  1 2 3 4 5 6 7 8
1 X X X             
3 X       X     X
2 X       X     X
4 X X     X     X 
5 X       X     X
6 x       X     X
7 X       X     X  
8                
*/
const char F11[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b00000000,
    0b01111110,
    0b00000000,
    0b00000000,
    0b01111110,
} ;


/*
F12
  1 2 3 4 5 6 7 8
1 X X X             
3 X     X     X  
2 X     X   X   X
4 X X   X       X 
5 X     X     X  
6 x     X   X    
7 X     X   X X X      
8                
*/
const char F12[] PROGMEM  =
{
    0b01111111,
    0b00001001,
    0b00000001,
    0b01111110,
    0b00000000,
    0b01100100,
    0b01010010,
    0b01001100,
} ;


/*
on
  1 2 3 4 5 6 7 8
1                   
3                   
2                     
4       X X          
5       X X          
6                     
7                       
8                  
*/
const char on[] PROGMEM  =
{
    0b00000000,
    0b00000000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000,
} ;


const char none[] PROGMEM  =
{
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
} ;

/*
on
  1 2 3 4 5 6 7 8
1                   
3                   
2                     
4       X X          
5       X X          
6                     
7                       
8                  
*/

#endif