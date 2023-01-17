#include "src/io.h"
#include "src/macros.h"
#include "src/keypad.h"
#include "src/oled.h"
#include "src/XpressNetMaster.h" 
#include "src/makeNumber.h" 
#include "menu.h"
#include "src/event.h"
#include "Locomotives.h"
#include "Wire.h"

/* things to do

// GENERAL 
V prevent EEPROM from being erased after flashing, PITA. Try new arduino-cli version with new control parameters () POINTS MOVED TO I2C EEPROM, SOME SETTINGS REMAIN
V determen max amount of point streets and amount per point. --> 64 streets, 16 points per street/ 2048 bytes in eeprom used. leaving 2048 (minus settings) for 50 slots
V move settings to i2c eeprom
DISCARDED V move loco slots to i2c eeprom  TEST ME ALL PLZ
* test with DR5000 and feedback // should work tho
* make decoupler symbol

EXTRA
update arduino toolchain and copy paste new eventhandler to other projects.
design mini stick for multimaus enhancer, just to get the point streets working
re design pcb design and relocate buttons, perhaps make loconet interface as well and do something damned clever with solder jumpers?
do something with bootloader so I can flash via RS485, goddman icsp takes for fckn ever...


// CONTROL
 * direction control with cab change somewhat works, but must be improved. It does not work with an actual Xnet central attached
 * fix that bug where a loco wont stop after reaching speed 0...
 * improve throttle operation where a mininum throttle level is needed to maintain a certain speed... (PERHAPS NOT HANDY FOR RECORDING A PROGRAM ) (SPECIAL MODE?)
 * improve slot system so that a new slot is added, after a new address is controlled instead of selected. Now if you accidently select a wrong address, a slot is reserved.
   also need a system that an old slot can be overwritten if maximum amount is exceeded.
 * accessory decoders are now played back using the togglePoint function. This causes the display to show every switched turnout while in point mode. Better go back to direct Xnet Control

// EVENT HANDLER
 
 DISCARDED  // EEPROM SLOT, BIGGER PROCESSOR W. MORE MEMORY IS NEEDED.
 DISCARDED  V new loco loaded -> default settings must be loaded.  TESTME, SHOULD BE OK NOW
 DISCARDED  * display speed differently using the factor  ! km/u and m/h is built in and must be tested. Not sure at this time if I can adjust the factor..
 DISCARDED  * need to make a swap function to switch locations KILLER IDEA! commit a byte per loco which corresponds with an index. The slots dont have to be swapped in eeprom
 DISCARDED  * need to be able to clear an eeprom (or atleast set the first byte to 255)

 DISCARDED  automatic turn off function if a function with time delay is set
 DISCARDED  incorporate method to use momentory functions <-- big impact to keymatrix..  may not be possible..

// BACKLOG
DISCARDED  speed calibration test            
DISCARDED  multiple units up to 5 loco's     
* add english and 3-way points      
* add control panel                 
* request status of newly picked address  // seems not to be present in Xnet library

*/

/* OBJECTS */

#define POINT_DELAY( interval ) uint32_t prevTime = millis() ; \
                                while( millis() - prevTime <= interval ) { XpressNet.update(); }

I2cEeprom i2cEeprom( 0x50 ) ;

// XpressNet 
XpressNetMasterClass XpressNet ;

// THE 24LC256 has 0x8000 bytes of memory
// I devided this into 4 programs of each 0x2000 or 8192 bytes                  // future versions should get more
EventHandler program[] =
{
    EventHandler( 0x0000, 0x1000, 0x50 ),
    EventHandler( 0x1000, 0x1000, 0x50 ),
    EventHandler( 0x2000, 0x1000, 0x50 ),
    EventHandler( 0x3000, 0x1000, 0x50 ),
    EventHandler( 0x4000, 0x1000, 0x50 ),
    EventHandler( 0x5000, 0x1000, 0x50 ),
    EventHandler( 0x6000, 0x1000, 0x50 ),
    // 0x7000 <-> 0x7FFF reserved for other
} ;
const int nPrograms = sizeof(program) / sizeof(program[0]) ;                    // test me

void togglePoint( int8_t ) ;
extern void listAllLoco() ;  

enum modes 
{
    powerOff,
    programs,
    points,
    pointStreets,
    gettingAddress,
    //gettingSlot,
    locos,
    // locoSlots,
} ;

enum eventTypes
{
    speedEvent = 3,
    F0_F4Event,
    F5_F8Event,
    F9_F12Event,
    F13_F20Event,
    accessoryEvent,
} ;

enum drivingDirections
{
    FORWARD,
    NEUTRAL,
    REVERSE,
} ;

    
/* VARIABLES & CONSTANTS */
uint8   locoMode  = locos ;
uint8   pointMode = points ;
uint16  receivedAddress ;
uint16  selectedStreet  ;
uint16  pointAddress,   pointAddressNew ;
uint16  currentAddress, currentAddressNew ;
uint16  /*slot,*/       slotNew ;
extern  uint16 slot ;                           // relocated to menu.cpp
uint16  functionNumber, functionNumberNew ;
uint16  channel,        channelNew ;
uint16  functionMap ;
uint8   mode = locos ;
uint8   Estop ;
uint8   speedSteps = Loco128 ;
uint8   direction ;
uint8   speed = 0 ;
uint8   powerState ;
uint8   speedSetpoint = 0 ;
uint16  accelerationFactor ;
bool    togglingPoint ;
uint8   cabChange = 0 ;     // must be a locomotive setting
uint8   connected = 0 ;
uint8   invertedPosition = 0 ;
uint8   addressOffset = 0 ;
uint8   settingPoints = 0 ;
uint32  currentFunctions ;
uint8   F0to4 ;
uint8   F5to8 ;
uint8   F9to12 ;
uint8   F13to20 ;
uint8   F21to28 ;
uint8   pointStates[32] ; 
uint8   prevNibble[256] ;       // store state 1024 feedback contacts // MAY BE DELETED IF XNET REQUEST WORKS
uint8   lastDirection ;
// to set loco slot function back
uint16  funcDelay ;
uint32  lastTime ;
uint16  dir ;

locoSlot currentLoco ;

void updateLoco()
{
    speed  = currentLoco.speed  ;
    F0to4  = currentLoco.F0_F4  ;
    F5to8  = currentLoco.F5_F8  ;
    F9to12 = currentLoco.F9_F12 ;
    cabChange = F0to4 & 0x80 ;
}

void fillFunctionMap()
{
    functionMap = 0 ;   // reset to 0.

    if( mode == locos )
    {
        if( F0to4 & 0b10000 )       functionMap |= 1;
        for (int i = 0; i < 4; i++)
        {
            if( F0to4  & (1 << i )) functionMap |= ( 1 << ( i + 1 ) ) ;
            if( F5to8  & (1 << i )) functionMap |= ( 1 << ( i + 5 ) ) ;
            if( F9to12 & (1 << i )) functionMap |= ( 1 << ( i + 9 ) ) ;
        }
    }
    // else
    // {
    //     if( F0to4 & 0b10000 )       functionMap |= ( 1 << getIndex( 0 ) ) ;
    //     for (int i = 0; i < 4; i++)
    //     {
    //         if( F0to4  & (1 << i )) functionMap |= ( 1 << getIndex( i + 1 ) ) ;
    //         if( F5to8  & (1 << i )) functionMap |= ( 1 << getIndex( i + 5 ) ) ;
    //         if( F9to12 & (1 << i )) functionMap |= ( 1 << getIndex( i + 9 ) ) ;
    //     }
    // }
}


/*****************  DRAW FUNCIONS  *********************/
void drawFunctions()
{
    if( mode != locos/* && mode != locoSlots*/ ) return ;
    
    fillFunctionMap() ;

    const char *texts[] PROGMEM = { F0, F1, F2, F3, F4, F5, F6, F7, F8, F9 } ;  // I gave this PROGMEM as well, data ramain scrambled
    const char *symbols[] PROGMEM = { none, speakerSymbol, horn1, horn2, bell, 
        music, smoke, light, cabineLight, brakeSqueking, } ;
    
    uint8 x = 0 , x2 = 1, y = 2 ;                                               // set coordinates to the left side


    if( cabChange ) { printAt(5,7,F("cab A") ) ; }
    else            { printAt(5,7,F("cab B") ) ; }

    //delay(1000);

    //memcpy_P(toRam, &play, 8) ;     // <-- this one works fine
    //printCustom(10, 5, 0, toRam ) ;

    for( int func = 0 ; func < 10 ; func ++ )
    {
        y ++ ;
        if( func == 5 ) { x = 15 ; x2 = 14 ; y = 3 ; }                          // set coordinates to the right side

        if( mode == locos ) { memcpy_P(toRam, texts[func], 8); }                                   // I am so doing something wrong... but what?
        //else                { memcpy_P(toRam, symbols[eepromLoco.functions[func] & 0x0F], 8); }    // I am so doing something wrong... but what?
        
        printCustom(x, y, 0, toRam ) ;
        // if( mode == locos ) { printCustom( x, y, 0, texts[func] ) ; }                                  // this used to work fine without PROGMEM
        // else                { printCustom( x, y, 0, symbols[eepromLoco.functions[func] & 0x0F] ) ; }   // this used to work fine without PROGMEM
        
        memcpy_P(toRam, &on, 8) ;
        //printCustom( x2, y, 0, toRam ) ; }
        if( functionMap & (1 << func)) { printCharAt( x2, y, F("x")) ; }
        else                           { printCharAt( x2, y, F(" ")) ; }
    }
}
/*
void drawConfigFunctions()
{
    uint8 x1 = 0 , x2 = 2, x3 = 5, y = 2 ; 

    const char *symbols[] PROGMEM = { none, speakerSymbol, horn1, horn2, bell, 
        music, smoke, light, cabineLight, brakeSqueking, } ;

    for( int func = 0 ; func < 10 ; func ++ )
    {
        y ++ ;
        if( func == 5 ) { x1 = 15 ; x2 = 13 ; x3 = 10, y = 3 ; }                // set coordinates to the right side

        memcpy_P(toRam, symbols[eepromLoco.functions[func] & 0x0F], 8);
        printNumberAt( x1, y, 0, func  ) ;
        printCustom(   x2, y, 0, toRam ) ;
        printNumberAt( x3, y, 0, eepromLoco.functions[func] >> 4 ) ;
        printCharAt(   4, y, F("F") ) ;
        printCharAt(   9, y, F("F") ) ;
    }
}
*/
void drawSpeed( uint8 speed )
{
    if( mode != locos/* && mode != locoSlots*/ ) return ;                           // TODO: add check for current address

    if( mode == locos ) printNumberAt(7, 3, 3, speed & 0x7F ) ;
    // else
    // {
    //     uint16 temp ;
    //     temp = ( speed & 0x7F ) * (uint16)eepromLoco.speedFactor / 50 ;        // max speed is 126 * 127 / 50 = 320km/h
    //     if( eepromLoco.speedFactor & mph )
    //     {
    //         temp *= 5 ; 
    //         temp /= 8 ;                                    // aproximation to mph
    //         printNumberAt(7, 3, 3, temp ) ;
    //     }
    // }

    if( speed & 0x80 )   memcpy_P( toRam,    &up, 8 ) ; 
    else                 memcpy_P( toRam,  &down, 8 ) ; 
    if( dir == NEUTRAL ) memcpy_P( toRam, &pause, 8 ) ; 

    printCustom( 6, 3, 0, toRam ) ;
}


void updateLCD() 
{
    clearDisplay() ;

    switch( mode )
    {  
    case pointStreets :
        printAt(0, 0, F("Routes" ) ) ;
        printAt(0, 1, F("enter number" ) ) ;
        break ;

   
    case points :
        {
            printAt(0, 0, F("POINT #") ) ;
            printNumberAt( 7, 0, 3, pointAddress ) ;
            
            uint8_t bit_      = pointAddress % 8 ;
            uint8_t group    = pointAddress / 8 ;

            bool state = !bitRead( pointStates[ group ], bit_ ) ;
            if( !state ) memcpy_P(toRam,   &straight, 8 ) ;
            else         memcpy_P(toRam,   &curved,   8 ) ;
            printCustom( 12, 0, 0, toRam ) ;
        } 
        break ;   

    case gettingAddress:
        printAt(0, 0, F("ENTER ADDRESS") ) ;
        break ;

    case locos :
        drawSpeed( speed ) ;
        drawFunctions() ;
        printAt(0,0, F("Loco:")) ; printNumberAt( 7, 0, 3, currentAddress ) ;
        break ;


    // case gettingSlot:
    //     printAt(0, 0, F("ENTER SLOT") ) ;
    //     break ;
    
    // case locoSlots:
    //     drawFunctions() ;
    //     drawSpeed( speed) ;
    //     printAt(0,0, F("Loco Slot")) ; printNumberAt( 9, 0, 3, slot ) ;
    //     printDescription( eepromLoco.name, 1 );
    //     break ;

    case programs:
        {
            printAt(0, 0, F("PROGRAM MODE") ) ;
            printAt(0, 1, F("CHANNEL #") ) ;
            printNumberAt(7, 1, 4, channel+1 ) ;

            uint8 state = program[ channel ].getState() ;
            if( state == recording ) 
            {
                printAt(0, 3, F("recording") ) ;
                memcpy_P(toRam,   &stop,  8 ) ; printCustom(  7, 7, 0, toRam ) ;
            }             
            if( state == playing )   
            {
                printAt(0, 3, F("playing") ) ;
                memcpy_P(toRam,  &pause, 8 ) ; printCustom(  7, 7, 0, toRam ) ;
            }
            if( state == finishing ) 
            {
                printAt(0, 3, F("finishing") ) ;
                memcpy_P( toRam, &play, 8 ) ; printCustom( 11, 7, 0, toRam ) ;           // if you hit this before program stops, you go back to playing
                memcpy_P( toRam, &stop, 8 ) ; printCustom(  7, 7, 0, toRam ) ;
            }
            if( state == idle )      
            {
                printAt(0, 3, F("idle") )     ;
                memcpy_P( toRam, &play,   8 ) ; printCustom( 11, 7, 0, toRam ) ;
                memcpy_P( toRam, &stop,   8 ) ; printCustom(  7, 7, 0, toRam ) ;
                memcpy_P( toRam, &record, 8 ) ; printCustom(  3, 7, 0, toRam ) ;
            }
        }
        break; 
    }
}

/*****************  CALLBACK FUNCIONS  *********************
 notifyXNetPower
 notifyXNetTrnt
 notifyXNetFeedback
 notifyXNetLocoDrive28  // TODO
 notifyXNetLocoDrive128  // TODO
 notifyXNetLocoFunc1  // TODO
 notifyXNetLocoFunc2  // TODO
 notifyXNetLocoFunc3  // TODO
 notifyXNetLocoFunc4  // TODO
 notifyXNetLocoFunc5  // TODO

 notifyKeyPressed
 notifyNumberEntered
 notifyEvent
 */

// void notifyI2cErrror( uint8 error)
// {
//     clearDisplay();
//     printAt(1,1,F("I2C error"));
//     printNumberAt(1,2,error,4 ) ;
//     delay(5000);
// }

void notifyLoco( uint16 Address, uint8 Data, uint8 messageType )
{
    if(       Address == currentAddress 
    &&( mode == locos /*|| mode == locoSlots*/ ) )
    {
        switch( messageType )
        {
        case eSpeed : speed = Data ; drawSpeed( speed ) ;   return ;                            // do something to handle eepromLoco
        case eFunc1 : F0to4   = Data | cabChange ; break ;
        case eFunc2 : F5to8   = Data ; break ;
        case eFunc3 : F9to12  = Data ; break ;
        case eFunc4 : F13to20 = Data ; break ;
        case eFunc5 : F21to28 = Data ; break ;
        }

        if( messageType == eFunc1 ) updateSlot( Address, messageType, Data | cabChange ) ;
        else                        updateSlot( Address, messageType, Data ) ;
        drawFunctions() ;
    } 
}

void notifyXNetLocoDrive28(  uint16 Address, uint8 Speed ) { notifyLoco( Address, Speed, eSpeed ) ; }
void notifyXNetLocoDrive128( uint16 Address, uint8 Speed ) { notifyLoco( Address, Speed, eSpeed ) ; }
void notifyXNetLocoFunc1(    uint16 Address, uint8 Func1 ) { notifyLoco( Address, Func1, eFunc1 ) ; }
void notifyXNetLocoFunc2(    uint16 Address, uint8 Func2 ) { notifyLoco( Address, Func2, eFunc2 ) ; }
void notifyXNetLocoFunc3(    uint16 Address, uint8 Func3 ) { notifyLoco( Address, Func3, eFunc3 ) ; }
void notifyXNetLocoFunc4(    uint16 Address, uint8 Func4 ) { notifyLoco( Address, Func4, eFunc4 ) ; }
void notifyXNetLocoFunc5(    uint16 Address, uint8 Func5 ) { notifyLoco( Address, Func5, eFunc5 ) ; }

void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address )
{
    printAt(3,5, F("giveLocoFunc")) ;
    printNumberAt(3,6, 2, Address ) ;
    printNumberAt(3,7, 2, UserOps ) ;
}
void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t Address)
{
    printAt(3,5, F("giveLocoInfo")) ;
    printNumberAt(7,6, 2, Address ) ;
    printNumberAt(7,7, 2, UserOps ) ;
}

void notifyXNetPower(uint8_t State )
{
    powerState = State ;

    if( powerState != csNormal )
    {
        mode = powerOff ;
        //clearDisplay() ;
        printAt(0,0,F("POWER ")); 
        switch( powerState )
        {
        case csTrackVoltageOff : printAt(7,0, F("OFF"));           break ;
        case csShortCircuit :    printAt(7,0, F("SHORT"));         break ;
        case csServiceMode:      printAt(7,0, F("SERVICE"));       break ;
        }
    }
    else
    {
        mode = locoMode ;
        updateLCD() ;
    }    
}

void notifyXNetTrnt( uint16_t address, uint8_t Pos )                // ONLY WORKS IF OTHER DEVICES DOES SOMETHIMG
{
    if( bitRead(Pos,3) == 1 )
    {
        address ++ ;
        Pos &= 0b11 ;
        if( Pos >= 2) Pos -= 2 ; // convers 1-2 into 0-1

        uint8_t bit_     = address % 8 ;
        uint8_t group    = address / 8 ;
        bitWrite(pointStates[ group ], bit_, Pos) ;                             // updates points if other devices set it.

        if( address == pointAddressNew && mode == points )                      // if received address is the same as our address, update LCD
        {
            updateLCD() ;
        }

        if( address > 900 && address <= (900 + nStreets) )                                 // 900 <-> 924 are reserved for other devices to set point streets
        {
            layStreet( address - 900 ) ;
        }
    }
}

void notifyXNetFeedback( uint16_t address, uint8_t state )                      // Xnet knows 127 feedback module addresses with 8 bits per address
{    
    if( state & 0b01000000 )                                                    // ITT = 010 for feedback modules
    {
        address >>= 2 ;
        address &= 0x0007 ;
        address <<= 3 ;

        uint8 newNibble = state & 0x0F ;

        uint8 index = address * 2 ;                                             // 2 nibbles per address
        if (state & 0b10000)
        {
            index ++ ;                                                          // if bit N is set, correct index to upper nibble Note index should be fine
            address += 4 ;
        }
        

        for( uint8 bitMask = 0x01 ; bitMask < 0x40 ; bitMask <<= 1 )            // check 4 bits
        {
            address ++ ;
            if( (newNibble & bitMask) != (prevNibble[index] & bitMask ) )
            {
                if( newNibble & bitMask )
                {
                    prevNibble[index] |=  bitMask ;
                    program[channel].storeEvent( FEEDBACK, address, 1  ) ;        // for recording

                    for( int i = 0 ; i < nPrograms ; i ++ )
                    {
                        program[i].sendFeedbackEvent( address ) ;                 // during playing. A feedback event is sent to all programs
                    }
                }
                else
                {
                    prevNibble[index] &= ~bitMask ;
                }
            }
        }
    }
}

/*    button layout
    {  '1', '2', '3' } ,
    {  '4', '5', '6' } ,
    {  '7', '8', '9' } ,
    {  'p', '0', 'l' } ,  // point      0           train
    {  'P', 'x', 'D' } ,  // progams    undefined   dir (cabine change of driver)
    {  'Y', MENU, 'N' } ,  // Y  <menu>  E stop      N  <-- for menu mostly
	*/
void notifyKeyPressed( uint8 pressTime, char key )
{    
    send( pressTime, key ) ;

    if( getMenu() != LayoutControl ) return ;
    

    switch( key )
    {
    case 'l': 
        /*if( pressTime == LONG )                                                 // holding toggle down, toggles between locos <> locoslot
        {
            if( mode == locos ) locoMode = locoSlots ;
            else                locoMode = locos ;
            mode = locoMode ;
        }
        else*/ if( pressTime == SHORT )                                           // short press -> pick new address or abort picking new address
        {
            if( mode == locos )
            {
                mode = gettingAddress ;                                         // if address mode               -> get new loco address
            }
            // else if( mode == locoSlots )
            // {
            //     mode = gettingSlot ;                                            // if slot mode                  -> get new slot ID
            // }
            else if( mode == gettingAddress /*|| mode == gettingSlot*/ )            // if we are awaiting new number -> go back to previous mode... TEST ME
            {
                abortMakeNumber() ;                                             // if we were entering a number, stop it
                mode = locoMode ;
            }
            else                                                                // if any other mode -> locomode
            {
                mode = locoMode ;
            }
        }
        updateLCD() ;
        break ;

    case 'p':
        abortMakeNumber() ;                                                     // if we were entering a number, stop it
        if( pressTime == LONG )                                                 // holding toggle down, toggles between points<>pointStreets
        {
            if( mode == points ) pointMode = pointStreets ;
            else                 pointMode = points ;
        }
        else if( pressTime == SHORT )
        {
            if( mode == points )
            {
                togglePoint( -1 ) ;                                             // a short press on the point button whilst in point mode will toggle last set point
            }
        }
        mode = pointMode ;                                                      // always set mode accordingly
        updateLCD() ; 
        break ;
        

    case 'P':
        mode = programs ;
        updateLCD() ; 
        break ;

    case 'D': // cab change
        cabChange ^= 0x80 ;
        if(cabChange) F0to4 |=  cabChange ;
        else          F0to4 &= ~cabChange ;    // must be stored for every used loco address, up to 25? perhaps 'repurpose' loco func 28 or 32 for this purpose?
        updateLCD() ; 
        break ;

    case 'x' : // not yet defined button, maybe fun idea to use as an occupance detector? as a sort step mode? for your programs
        program[ channel ].storeEvent( FEEDBACK, 65500 + channel, 1  ) ;        // for recording
        program[ channel ].sendFeedbackEvent( 65500 + channel ) ;   
        break ;

    case GREEN: // no / red
        if( mode == programs )
        {
            program[ channel ].startPlaying() ;
            updateLCD() ;
            break ;
        }
        else if( pressTime == SHORT && powerState != csNormal )                 // short press, ENABLE POWER
        {
            mode = locoMode ; 
            powerState = csNormal ;
            goto setPower ;
        }
        break ;

    case MENU: 
        if( mode == programs )
        {
            program[ channel ].stopRecording() ;
            program[ channel ].stopPlaying() ;
            updateLCD() ;
            break ;
        }
        //if( mode == locoSlots ) {    setMenu( configureLoco ) ;      }
        if( mode == locos ) {        setMenu( configureThrottle ) ;  }
        if( mode == pointStreets ) { setMenu( getStreetIndex ) ;     }
        return ;

    case RED:
        if( mode == programs )
        {
            if( pressTime == LONG )
            {
                program[ channel ].startRecording() ;
                updateLCD() ;
            }
            break ;
        }
        else if( pressTime == SHORT && powerState == csNormal )         // KILL POWER
        {
            mode = powerOff ; 
            powerState = csTrackVoltageOff ; 
            goto setPower ;
        }
        break ; 

    setPower:
        XpressNet.setPower( powerState ) ;
        updateLCD() ; 
        break ;


    default:
        if( pressTime == SHORT ) switch( mode )     // '0' - '9' handled here
        {
            case programs:          
                if( program[ channel ].getState() != recording )                // interlocking, if current channel is recording, channel must not be changed
                {
                    makeNumber( &channelNew, key, 1, nPrograms ) ;         
                }
                break ;
            case locos :            makeNumber( &functionNumberNew, key, 0, 32 ) ;         break ;
            //case locoSlots :        makeNumber( &functionNumberNew, key, 0, 32 ) ;         break ;
            //case gettingSlot :      makeNumber( &slotNew,           key, 1, 30 ) ;         break ;
            case gettingAddress :   makeNumber( &currentAddressNew, key, 1, 9999 ) ;         break ;
            case pointStreets :     makeNumber( &selectedStreet,    key, 1, nStreets ) ;     break ;
            case points :           makeNumber( &pointAddressNew,   key, 1, 1024 ) ;         break ;
        }
        break ;
    }

}

void notifyNumberEntered()
{
    bool state ;

    switch( mode )
    {
        
        // case locoSlots:         functionNumber = getFunction( functionNumberNew ) ;
        //                         goto tglFunc ;

        case locos:             functionNumber = functionNumberNew ;    
        tglFunc:                toggleFunction( functionNumber ) ;
                                drawFunctions() ;
                                return ;                                        // toggle function

        case points:            pointAddress = pointAddressNew ;    
                                togglePoint( -1 ) ;        
                                updateLCD() ;            
                                break ;

        case pointStreets:      layStreet( selectedStreet ) ;
                                break ;
        

        case gettingAddress:    currentAddress = currentAddressNew ;                                                      // new loco is picked                                     
                                mode = locos ;
                                newSlot = true ;
                                if( loadSlot( &currentLoco, currentAddress ) )      // if slot has found, update variables
                                {
                                    updateLoco() ;
                                }
                                
                                updateLCD() ;
                                //XpressNet.getLocoInfo( currentAddress ) ;                                                 // does not work, atleast the respons does not...                                       
                                break ;

        // case gettingSlot:       slot = slotNew ;
        //                         getLoco( slot ) ;
        //                         mode = locoSlots ;
        //                         currentAddress = eepromLoco.address ;             // copy address from EEPROM loco to active address
        // reqLoco:                //XpressNet.getLocoStateFull( hb(currentAddress), lb(currentAddress), 0 );           // retreive information       
        //                         //XpressNet.getLocoInfo( currentAddress ) ;
        //                         updateLCD() ;  
        //                         break ;

        case programs:          channel = channelNew - 1 ;
                                updateLCD() ;
                                break ;
    }
}
// void displayFreeMemory( uint16 freeMem ) 
// {
//     if( mode == programs )
//     {
//         printAt(0,4,F("free mem"));
//         printNumberAt( 10, 4, 4, freeMem  ) ;
//     }
// }
// void displayStoreMemory( uint16 freeMem ) 
// {
//     if( mode == programs )
//     {
//         printAt(0,5,F("store mem"));
//         printNumberAt( 10, 5, 4, freeMem  ) ;
//     }
// }
// void displayGetMemory( uint16 freeMem ) 
// {
//     if( mode == programs )
//     {
//         printAt(0,6,F("get mem"));
//         printNumberAt( 10, 6, 4, freeMem  ) ;
//     }
// }
void notifyEvent( uint8 type, uint16 address, uint8 data )                      // CALL BACK FUNCTION FROM EVENT.CPP
{
    switch( type )
    {
    //case START:      /*updateLCD() ;*/ printAt(3,4,F("PLAYING PROG"));   break ; // no action needed for the start event
    case STOP:          updateLCD() ;  break ;
    case speedEvent:    setSpeed( address, data ) ; if( address == currentAddress ) drawSpeed( data ) ; break ;
    case F0_F4Event:    XpressNet.setFunc0to4(   address,data ) ; break ;
    case F5_F8Event:    XpressNet.setFunc5to8(   address,data ) ; break ;
    case F9_F12Event:   XpressNet.setFunc9to12(  address,data ) ; break ;
    case F13_F20Event:  XpressNet.setFunc13to20( address,data ) ; break ;
    case accessoryEvent://pointAddress = address ;
                        //togglePoint( data ) ;
                        XpressNet.SetTrntPos( address , data, 1 ) ;               
                        POINT_DELAY( 20 ) ;
                        XpressNet.SetTrntPos( address  , data, 1 ) ;    
                        notifyXNetTrnt( address, data ) ;
                        break ;
    //case FEEDBACK:      printAt(3,3,F("FEEDBACK"));                        break ;
    }
}


/**********************  CONTROL FUNCTIONS  *****************************
 * setPoint
 * setStreet
 * updateLCD
 * togglePoint
 * setSpeed
 * throttleControl
 * */

void setPoint( uint16_t address ) 
{
    uint8 pos = address >> 15 ;
    pointAddress = address & 0x03FF ;
    //togglePoint( pos ) ;
    XpressNet.SetTrntPos( address & 0x03FF, pos, 1) ;
    POINT_DELAY( 20 ) ;
    XpressNet.SetTrntPos( address & 0x03FF, pos, 0) ;
}



void togglePoint( int8_t toggleMode  )
{
    uint8_t bit_     = pointAddress % 8 ;
    uint8_t group   = pointAddress / 8 ;
    uint8_t state ;

    
    if( toggleMode == -1 ) state = !bitRead( pointStates[ group ], bit_) ;       // -1 -> get current state and flip it
    else state = toggleMode ;


    program[ channel ].storeEvent( accessoryEvent, pointAddress, state ) ;

    XpressNet.SetTrntPos( pointAddress - 1 , state, 1 ) ;               
    POINT_DELAY( 20 ) ;
    XpressNet.SetTrntPos( pointAddress - 1 , state, 1 ) ;         
    
    bitWrite(pointStates[ group ], bit_, state) ;    

    if( mode != pointStreets )
    {
        updateLCD() ;
    }
}


void toggleFunction( uint8_t func )
{    
    if( newSlot ) if( loadSlot( &currentLoco, currentAddress ) ) 
    {
        updateLoco() ;
    }

    if( func <=  4 )      // 0-4 
    {
        if( func == 0 ) func = 4 ;
        else func -- ;
        F0to4 ^= (1 << func) ;
        program[ channel ].storeEvent( F0_F4Event, currentAddress, F0to4 ) ;
        XpressNet.setFunc0to4(   currentAddress,   F0to4 ) ; 
    }
    else if( func <=  8 )  // 5-8 
    {
        func -= 5 ;
        F5to8 ^= (1 << func) ;
        program[ channel ].storeEvent( F5_F8Event, currentAddress, F5to8 ) ;
        XpressNet.setFunc5to8(   currentAddress,   F5to8 ) ;
    }
    else if( func <= 12 )  // 9-12
    {
        func -= 9 ;
        F9to12 ^= (1 << func) ;
        program[ channel ].storeEvent( F9_F12Event, currentAddress, F9to12 ) ;
        XpressNet.setFunc9to12(  currentAddress,  F9to12 ) ;
    }
    else if( func <= 20 )  // 13-20    
    {
        func -= 13 ;
        F13to20 ^= (1 << func) ;
        program[ channel ].storeEvent( F13_F20Event, currentAddress, F13to20 ) ;
        XpressNet.setFunc13to20( currentAddress, F13to20 ) ;
    }
    else if( func <= 28 )  // 21-28  
    {
        func =- 21 ;
        F21to28 ^= (1 << func) ;
        XpressNet.setFunc21to28( currentAddress, F21to28 ) ;
	}
}

const uint8_t speedTable[32] = // deprecated in favor of 128 steps. Not in use atm. Do keep for future
{ 
    0, 0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 0, 0, 2, 4, 6, 8,
    10, 12, 14, 16, 18, 20, 22, 24, 26, 28 
};


void setSpeed(uint16_t address, uint16_t speed  )
{
    XpressNet.setSpeed( address, speedSteps, speed ) ;
    if( newSlot ) if( loadSlot( &currentLoco, currentAddress ) )
    {
        updateLoco() ;
    }
}


static void throttleControl()
{
    uint16  throttleLevel ;
    uint16  brakeLevel ;
    uint8   directionChange = 0;

    REPEAT_MS( 100 )                                                            // calculate average brake and throttle lever positions
    {
        const int nSamples = 4;
        
        static  uint8_t  index  = 0 ;

        static uint16_t sample1[nSamples];
        static uint16_t sample2[nSamples];

        sample1[index] = analogRead( throttlePin ) ;
        sample2[index] = analogRead( brakePin ) ;

        if( ++ index == nSamples ) index = 0 ;

        uint16_t average = 0 ;
        for( byte j = 0 ; j < nSamples ; j++ )  average += sample1[ j ] ;       // calculate throttle level
        average /= nSamples ;
        throttleLevel = map( average, setting.throttleLow, setting.throttleHigh, 0, 8 ) ;

        average = 0 ;
        for( byte j = 0 ; j < nSamples ; j++ )  average += sample2[ j ] ;       // calculate brake level
        average /= nSamples ;
        brakeLevel = map( average, setting.brakeLow, setting.brakeHigh, 0, 10 ) ;

        uint16 dir = analogRead( directionPin )  ;                              // 3 steps // MIDDLE = ~133!!!
        uint16 middle = (setting.dirHigh - setting.dirLow) / 2 ;
             if( dir < (middle - 150) ) dir = FORWARD ;                         // < --____
        else if( dir > (middle + 150) ) dir = REVERSE ;                         // > ____--        
        else                            dir = NEUTRAL ;                         // = __--__

        static uint8 prevDir ;
        if( dir != prevDir )                                                    // only react to a change to prevent problems with driving locomotives
        {   prevDir = dir ;

            directionChange = 1 ;
            if( dir == FORWARD ) direction = 0x80 ^ cabChange ;
            if( dir == REVERSE ) direction = 0x00 ^ cabChange ;
        }

    } END_REPEAT

    REPEAT_MS( 333 )                                                            // substract braking and add throttle to speed 3x per second.
    {
        if( brakeLevel > 0 )                                                    // if braking, throttle is discarded..
        {
            if( speed >= brakeLevel ) { speed -= brakeLevel ; }
            else                      { speed = 0 ;           }
        }
        else if( throttleLevel > 0 )
        {
            speed += throttleLevel ;
            if( speed > 126 ) speed = 126 ;                                     // constrain speed to 128
        }
        
        // printNumberAt( 3, 4, 3, throttleLevel  );
        // printNumberAt( 3, 5, 3,   brakeLevel  );
        // printNumberAt( 3, 6, 3, speed ) ;
    } END_REPEAT
    
    static uint8_t lastSpeed = 255 ;
    if( lastSpeed != speed || directionChange == 1  )
    {   lastSpeed  = speed ;

        uint8_t speedLocal ;
        if( speed == 0 ) speedLocal = 0;
        else             speedLocal = speed + 1 ;
        if( dir == NEUTRAL ) 
        {
            speed = 0 ;
            speedLocal = 1 ;                                                    // E-stop if direction became neutral
        }
        setSpeed( currentAddress, speedLocal|direction ) ;
        program[ channel ].storeEvent( speedEvent, currentAddress, speedLocal|direction ) ;

        drawSpeed( speedLocal|direction ) ;
    }
}

// void turnOffFunction()
// {
//     if( millis() - lastTime >= funcDelay )
//     {

//     }
// }

/*************************** SETUP *******************************/
void setup() 
{
    Wire.begin() ;
    lcdInit();
   // initSlots() ;
    menuInit() ;    

    for( int i = 0 ; i < nPrograms ; i ++ )
    {   
        program[ i ].begin() ;
    }

    XpressNet.setup( Loco128, 2 ) ; 
    clearDisplay() ;

    updateKeypad () ;
    delay( 20 ) ;

    drawFunctions() ;

    // eepromLoco.name = "#1" ;
    // eepromLoco.address = 1 ;
    // eepromLoco.functions[0] = eLight | 0x01 ;
    // eepromLoco.functions[1] = eNone  | 0x12 ;
    // eepromLoco.functions[2] = eNone  | 0x23 ;
    // eepromLoco.functions[3] = eNone  | 0x34 ;
    // eepromLoco.functions[4] = eNone  | 0x45 ;
    // eepromLoco.functions[5] = eNone  | 0x56 ;
    // eepromLoco.functions[6] = eNone  | 0x67 ;
    // eepromLoco.functions[7] = eNone  | 0x78 ;
    // eepromLoco.functions[8] = eNone  | 0x89 ;
    // eepromLoco.functions[9] = eNone  | 0x9a ;   
    // setLoco( 0 ) ;

    // eepromLoco.name = "#2" ;
    // eepromLoco.address = 2 ;
    // eepromLoco.functions[0] = eLight | 0x95 ;
    // eepromLoco.functions[1] = eNone  | 0x86 ;
    // eepromLoco.functions[2] = eNone  | 0x77 ;
    // eepromLoco.functions[3] = eNone  | 0x68 ;
    // eepromLoco.functions[4] = eNone  | 0x59 ;
    // eepromLoco.functions[5] = eNone  | 0x4a ;
    // eepromLoco.functions[6] = eNone  | 0x3b ;
    // eepromLoco.functions[7] = eNone  | 0x2c ;
    // eepromLoco.functions[8] = eNone  | 0x1d ;
    // eepromLoco.functions[9] = eNone  | 0x0e ;   
    // setLoco( 1 ) ;

    // getLoco( 0 ) ;

    loadSettings();
}


/***************************  MAIN LOOP  *******************************/
void loop() 
{
    XpressNet.update() ;
    updateKeypad() ; 
    Menu() ; 

    for( int i = 0 ; i < nPrograms ; i ++ )
    {     
        program[i].update() ;
    }

    if( getMenu() == LayoutControl )
    {
        updateNumber() ;
        throttleControl() ;
    }
}
