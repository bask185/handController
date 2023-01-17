
#include <Wire.h>
#include "oled.h"

#include <U8x8lib.h>

// https://github.com/olikraus/u8g2/wiki/fntgrp
// https://github.com/olikraus/u8g2/wiki/u8x8reference

// font u8x8_font_amstrad_cpc_extended_r
/*
uint8_t tiles[16] = { 0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0, 1, 3, 7, 15, 31, 63, 127, 255};
u8x8.drawTile(1, 3, 2, tiles);


THINGS TO THINK ABOUT?

CONTROL PANEL? HOW SELECT POINTS? NUMBERS..
SWITCH THROUGH PANELS? HOW MANY?
HOW MUCH STORAGE DOES IT TAKE? 
6 DIFFERENT ITEMS, EACH HAS ADDRESS (7 BIT), STATE, X POS (4 BITS), Y POS,(3 BITS)
// THREE WAY + SLIPSWITCH, MORE PER SQUARE/ HOW?
*/

uint8 toRam[8] ;

U8X8_SH1106_128X64_NONAME_HW_I2C display(/* reset=*/ U8X8_PIN_NONE);

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

void printCustom( uint8 x, uint8 y, uint8 state, char* custom )                 // NOTE BENE, inverting symbol may be undesirable. May as well print an asterix to indiciate that a function is on
{
    if( state > 0 )
    {
        for(int i = 0 ; i < 8 ; i ++ )  // PRINTS INVERTED FOR WHEN A FUNCTION IS ACTIVATED, NEED AN ARGUMENT THOUGH
        {
        *custom ^= 0xFF ;
        custom ++ ; 
        }
        custom -= 8 ;
    }

    display.drawTile( x, y, 1, custom ) ;
}

void clear( uint8 _line )
{
    display.clearLine(_line) ;
}

void clearDisplay()
{
    display.clearDisplay();
}

void printAt(uint8 x, uint8 y, String text )
{
    // display.clearLine(y);               // NOTE BENE, MAY BE VERY UNDESIRABLE, DON'T FORGET ABOUT THIS
    const char * c = text.c_str() ;
    display.drawString( x, y, c ) ;
}

void printCharAt(uint8 x, uint8 y, String text )
{
    const char * c = text.c_str() ;
    display.drawString( x, y, c ) ;
}


void printNumberAt( uint8 x, uint8 y, uint8 outline, uint16 number )
{
    //if( number == 0xFFFF ) printCharAt( x, y,F("_")) ; return ; // prevent displaying 35535...

    if( number <     10 && outline > 0 ) x ++ ; { display.drawString( x, y, " " ) ; } // this could use some optimalisation...
    if( number <    100 && outline > 1 ) x ++ ; { display.drawString( x, y, " " ) ; }
    if( number <   1000 && outline > 2 ) x ++ ; { display.drawString( x, y, " " ) ; }
    if( number <  10000 && outline > 3 ) x ++ ; { display.drawString( x, y, " " ) ; }
    if( number < 100000 && outline > 4 ) x ++ ; { display.drawString( x, y, " " ) ; }

    String text = String( number ) ;
    const char * c = text.c_str();
    display.drawString( x, y, c ) ;
}

void printDescription( char *description, uint8 y )
{
    printAt( 0, y, description ) ;
}


void lcdInit()
{
    display.setI2CAddress( 0x3C << 1 ) ;
    display.begin();
    display.setFont( u8x8_font_chroma48medium8_r ) ;
    display.clearDisplay();

}
