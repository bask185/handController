#include "keypad.h"
#include "oled.h"
#include "debounceClass.h"
#include "io.h"

const int marge =  3 ;
const int nCol = 3 ;
const int nRow = 6 ;

const int SHORT_PRESS = 1 ;
const int LONG_PRESS  = 2 ;

const int keyVal[nRow][nCol] =
{
    {   0, 319, 486 } ,
    {  43, 340, 499 } ,
    {  84, 360, 511 } ,
    { 121, 380, 523 } ,
    { 156, 397, 533 } ,
    { 188, 414, 544 } ,
} ;

const char key[nRow][nCol] =
{
    {  '1', '2', '3' } ,
    {  '4', '5', '6' } ,
    {  '7', '8', '9' } ,
    {  'p', '0', 'l' } ,  // point and train
    {  'P', 'x', 'D' } ,  // toggle,  progams  , dir (cabine change of driver)
    {  'Y', 'E', 'N' } ,  // 
} ;

Debounce keys(255);
char currentKey ;

void updateKeypad()
{
    REPEAT_MS( 20 )
    {
        int sample = analogRead( keyPin ) ;

        for( int col = 0 ; col < nCol ; col ++ )
        {
            for( int row = 0 ; row < nRow ; row ++ )

            if( sample >= ( keyVal[row][col] - marge )
            &&  sample <= ( keyVal[row][col] + marge ) )                        // if corresponding value is found
            {
                currentKey = key[row][col] ;                                    // store key in currentKey
                keys.debounce( 0 ) ;                                            // and run debouncer for the falling and rising flank detection
                return ;
            }
        }
        keys.debounce( 1 ) ;
    }
    END_REPEAT

    static bool longPress  = false ;
    static bool keyPressed = false ;
    static uint32 prevTime ;

    byte state = keys.getState() ;

    if( state == FALLING )                                                      // true if button is pressed
    {
        keyPressed = true ;
        prevTime = millis() ;
    }

    if( keyPressed == false ) return ; 

    if( millis() - prevTime >= 1500 )                                           // long press returns 2
    {
        keyPressed = false ;
        if( notifyKeyPressed ) notifyKeyPressed( LONG_PRESS, currentKey ) ;
    }

    if( state == RISING )                                                       // short press returns 1
    {
        keyPressed = false ;
        if( notifyKeyPressed ) notifyKeyPressed( SHORT_PRESS, currentKey ) ;
    }
}