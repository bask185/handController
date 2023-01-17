// HEADER FILES
#include "menu.h"
#include "src/macros.h"
#include "src/io.h"
#include "src/stateMachineClass.h"
#include "src/makeNumber.h"
#include "src/keypad.h"
#include "src/oled.h"
//#include "Locomotives.h"
#include "src/i2cEeprom.h"

static StateMachine menu ;
static I2cEeprom i2cEeprom(0x50) ;
Settings setting ;

static uint16   pointIndex ;
static uint16   streetIndex ;
static uint16   slotIndex ;
static uint16   address[pointsPerStreet] ;
static uint8    pointState[pointsPerStreet] ;
static uint8_t  pressTime ;
static char     key ;
static uint8    indexFunction ;
static uint16   functionNumber ;
static uint16   timeDelay ;
uint16          slot ;
static uint16   speedFactor ;

const int streetAddress  = 0x7000 ;

extern uint16   currentAddress ;

extern void drawFunctions() ;
extern void drawConfigFunctions() ;

void send( uint8_t _pressTime, char _key )
{
    pressTime = _pressTime ;
    key = _key ;
}

void layStreet( uint8 index )
{
    streetIndex = index - 1 ;
    menu.setState( setPoints ) ;
}

void setMenu( uint8 newMenu)
{
    menu.setState( newMenu ) ;
}


void loadSettings()
{
    i2cEeprom.get( 0x7FFF - sizeof(setting) , setting ) ;                       // use the last part of storage for settings
    if( setting.DEF != 0xCC )
    {
        setting.DEF          = 0xCC ;
        setting.throttleLow  = 992 ;
        setting.throttleHigh = 500 ;
        setting.brakeLow     = 998 ;
        setting.brakeHigh    = 500 ;
        setting.dirLow       = 100 ;                                            // FIXME
        setting.dirHigh      = 900 ;

        storeSettings() ;
    }
}

void storeSettings()                    
{
    i2cEeprom.put( 0x7FFF - sizeof(setting) , setting ) ;                       // use the last part of storage for settings
}

uint8_t currentMenu ;
const int nMenus = 8 ;

struct {
    uint8_t subnet ;
    uint8_t address ;
} controller, central ;


enum modes 
{
    addressMode ,
    functionMode ,
    pointMode ,
} ;
uint8_t controlMode = addressMode ;   // keeps track if we are settings addresses, funcs or points

// FUNCTIONS
extern void updateLCD() ;

void menuInit(void)
{
    menu.nextState( LayoutControl, 0 ) ;
}

uint8_t getMenu() { return menu.getState() ; }







// STATE FUNCTIONS


StateFunction( LayoutControl )
{
    if( menu.entryState() )
    {
        updateLCD() ;
    }

    return 0 ;
}

// select loco from list..
// change speed factor
// calibrarion test? 
// function mapping     ->  select number -> select sumbol -> enter timedelay
// max speed ??
// change name

// StateFunction( configureLoco )          
// {
//     if( menu.entryState() )
//     {
//         clearDisplay() ;
//         printAt(0, 0, F("current slot"    ) ); printNumberAt( 13, 0, 0, slot ) ;
//         printAt(0, 1, F("1= Edit name"    ) );
//         printAt(0, 2, F("2= Set address"  ) );
//         printAt(0, 3, F("3= Func mapping" ) );
//         printAt(0, 4, F("4= change slot"  ) );
//         printAt(0, 5, F("5= speed factor" ) );
//         printAt(0, 6, F("6= load default" ) );
//         printAt(0, 7, F("red=back"        ) );
//     }
//     if( menu.onState() )
//     {
//         if( (key >= '1' && key <= '6') 
//         ||   key == RED )
//         {
//             menu.exit() ;
//         }
//     }
//     if( menu.exitState() )
//     {
//         if( key == RED )
//         {
//             setLoco( slot ) ;
//         }
//         clearDisplay() ;
//     }
//     return menu.endState() ;
// }

// #define lowerAscii ('0'- 1)  // the value before '0' will act as a space
// #define upperAscii 'Z'
// StateFunction( editName )
// {
//     static uint8 xCur, ascii ;
//     static char* ptr ;
//     if( menu.entryState() )
//     {
//         xCur = 0;
//         ptr = eepromLoco.name ;
//         ascii = upperAscii ;
//         key = GREEN ;

//         printAt( 7,6,F(  "^")) ;
//         printAt( 5,7,F("< v >")) ;
//     }
//     if( menu.onState() )
//     {
//         if( key )
//         {
//             if( key == GREEN && xCur < 15 )  // cursor right
//             {
//                 printCharAt( xCur, 1, F(" ")) ;
//                 xCur ++ ; ptr ++ ;
//                 printCharAt( xCur, 1, F("^")) ;
//                 ascii = *ptr ;
//             }
//             if( key == RED && xCur > 0 )  // cursor left
//             {
//                 printCharAt( xCur, 1, F(" ")) ;
//                 xCur -- ; ptr -- ;
//                 printCharAt( xCur, 1, F("^")) ;
//                 ascii = *ptr ;
//             }
//             if( key == 'x' )  // inc char
//             {
//                 if( ++ ascii > upperAscii ) ascii = lowerAscii ;
//             }
//             if( key == MENU )  // dec char
//             {
//                if( -- ascii < lowerAscii ) ascii = upperAscii ;
//             }
            
//             if( ascii == lowerAscii ) *ptr = ' ' ;                  // not using all symbols, therefor I use the char before '0' as space..
//             else                      *ptr = ascii ;

//             printDescription( eepromLoco.name, 0 ) ;

//             if( key == POINTS ) menu.exit() ; // return <-- exit button
//         }
//     }
//     if( menu.exitState() )
//     {
        
//     }
//     return menu.endState() ;
// }

// StateFunction( getAddress )
// {
//     if( menu.entryState() )
//     {
//         clearDisplay() ;
//         printAt(0, 0, F("Enter Address") );
//         printAt(0, 1, F("1 - 9999"     ) );
//         printAt(0, 7, F("green=select" ) );
//         key = 1 ;
//     }
//     if( menu.onState() )
//     {
//         if( key )
//         {
//             if( key == GREEN ) menu.exit() ;
//             else makeNumber( &eepromLoco.address, key, 1, 9999 ) ;   // index is used to calculate memory in EEPROM
//             printNumberAt( 6, 3, 2, eepromLoco.address ) ;
//         }
//     }
//     if( menu.exitState() )
//     {
//         currentAddress = eepromLoco.address ; 
//     }
//     return menu.endState() ;
// }

// StateFunction( setSpeedFactor )
// {
//     if( menu.entryState() )
//     {
//         printAt(0, 0, F( "set speed factor" ) );
//         printAt(0, 1, F( "default = 50" ) );
//         printAt(0, 2, F( "green=select" ) );
//         key = 1 ;
//     }
//     if( menu.onState() )
//     {
//         if( key )
//         {
//             makeNumber( &speedFactor, key, 1, 100 ) ;
//             printNumberAt(5,5,2, speedFactor ) ;

//             if( key == GREEN ) menu.exit() ;
//         }
//     }
//     if( menu.exitState() )
//     {
//         eepromLoco.speedFactor = speedFactor ;
//     }
//     return menu.endState() ;
// }

// StateFunction( changeSlot )
// {
//     if( menu.entryState() )
//     {
//         printAt(0, 0, F( "pick new slot" ) );
//         printAt(0, 1, F( "green=select"  ) );
//         printAt(0, 2, F( "red=back"  ) );
//         key = 1 ;
//     }
//     if( menu.onState() )
//     {
//         if( key )
//         {
//             if( key >= '0' && key <= '9' ) { makeNumber( &slot, key, 1, nSlots ) ; } // NEED FINISHING
//             printNumberAt( 5, 5, 0, slot ) ;
//             if( key == GREEN || key == RED ) { menu.exit() ; }
//         }
//     }
//     if( menu.exitState() )
//     {
//         if( key == GREEN )
//         {   // swap
//             //EepromLoco temp1 ;

//             //temp = eepromLoco ; // store current loco MUST THINK ABOUT THISGIT
 
//             //getLoco( slot ) ;
//             //setLoco( newSlot ) ;
//             menu.exit() ;
//         }
//     }
//     return menu.endState() ;
// }


// StateFunction( resetSlot )
// {
//     if( menu.entryState() )
//     {
//         printAt(0, 0, F( "reset slot?" ) );
//         printAt(0, 1, F( "green=yes"  ) );
//         printAt(0, 2, F( "red=no"  ) );
//     }
//     if( menu.onState() )
//     {
//         if( key == GREEN ) 
//         {
//             // eepromLoco.name = "#1" ;
//             // eepromLoco.address = 3 ;
            
//             // eepromLoco.functions[0] = eLight | 0x00 ;
//             // eepromLoco.functions[1] = eNone  | 0x10 ;
//             // eepromLoco.functions[2] = eNone  | 0x20 ;
//             // eepromLoco.functions[3] = eNone  | 0x30 ;
//             // eepromLoco.functions[4] = eNone  | 0x40 ;
            
//             // eepromLoco.functions[5] = eNone  | 0x50 ;
//             // eepromLoco.functions[6] = eNone  | 0x60 ;
//             // eepromLoco.functions[7] = eNone  | 0x70 ;
//             // eepromLoco.functions[8] = eNone  | 0x80 ;
//             // eepromLoco.functions[9] = eNone  | 0x90 ;
//             //static char* ptr ;
//             //ptr = eepromLoco.name ;
//             //*ptr = 255 ;                                                        // I set first byte @ 255 to flag this slot as free or empty
//             //getLoco( slot ) ;
//             //setLoco( slot ) ;                                                   // store the change to eeprom
//             //getLoco( slot ) ;                                                   // this will load the slot and if the first byte is 255, default values will be loaded in the variables... it may work..

//             printAt(0, 4, F( "SLOT RESET" ) );
//             menu.exit() ;
//         }
//         if( key == RED )
//         {
//             printAt(0, 4, F( "SLOT NOT RESET" ) );
//             menu.exit() ;
//         }
//     }
//     if( menu.exitState() )
//     {
        
//     }
//     return menu.endState() ;
// }



// StateFunction( functionMapping )
// {
//     if( menu.entryState() )
//     {
//         clearDisplay() ;
//         printAt(0,0, F("Function mapping" ) );
//         printAt(0,1, F("pick position"    ) );
//         printAt(0,2, F("red=back"         ) );
//         //mode = locoSlots ;
//         drawConfigFunctions() ;
//     }
//     if( menu.onState() )
//     {
//         if(( key >= '0' && key <= '9' )
//         ||   key == RED ) 
//         {
//             menu.exit() ;
//         }
//     }
//     if( menu.exitState() )
//     {
//         indexFunction = key - '0' ;
//     }
//     return menu.endState() ;
// }

// /*
// const char *symbols[] = { none, speakerSymbol, horn1, horn2, bell, 
//             music, smoke, light, cabineLight, brakeSqueking, } ;
// */

// StateFunction( getSymbol )
// {
//     if( menu.entryState() )
//     {
//         clearDisplay() ;
//         printAt(0,0, F("Function mapping" ) );
//         printAt(0,1, F("pick symbol"    ) );
//         printAt(0,2, F("red=back"         ) );
        
//         const char *symbols[] PROGMEM = { none, speakerSymbol, horn1, horn2, bell, 
//             music, smoke, light, cabineLight, brakeSqueking, } ;

//         for( int i = 0 ; i < 5 ; i ++ )
//         {
//             clear( i + 3 ) ;
//             memcpy_P(toRam, symbols[i],  8) ;
//             printCustom(   2, i+3, 0, toRam ) ;
//             printNumberAt( 0, i+3, 0, i) ;

//             memcpy_P(toRam, symbols[i+5], 8) ;
//             printCustom(   13, i+3, 0, toRam ) ; 
//             printNumberAt( 15, i+3, 0, i+5) ;
//         }
//     }
//     if( menu.onState() )
//     {
//         if( (key >= '0' && key <= '9') ) { menu.exit() ; }
//     }
//     if( menu.exitState() )
//     {
//         eepromLoco.functions[ indexFunction ] &= 0xF0 ;
//         eepromLoco.functions[ indexFunction ] |= (key - '0') ;
//     }
//     return menu.endState() ;
// }

// StateFunction( getFuncNumber )
// {
//     if( menu.entryState() )
//     {
//         clearDisplay() ;
//         printAt(0,0, F("Function mapping"   ) );
//         printAt(0,1, F("enter number"       ) );
//         printAt(0,2, F("F0 - F28"           ) );
//         printAt(0,3, F("green=select"       ) );
//         functionNumber = eepromLoco.functions[ indexFunction ] >> 4 ;
//         key = 1 ;
//     }
//     if( menu.onState() )
//     {
//         if( key )
//         {
//             if( key == GREEN ) { menu.exit() ; }
//             else               { makeNumber( &functionNumber, key, 0, 28 ) ; }

//             printNumberAt( 5, 5, 3, functionNumber ) ;
//         }
//     }
//     if( menu.exitState() )
//     {
//         eepromLoco.functions[ indexFunction ] &= 0x0F ;                         // clear bit 7-6-5-4
//         eepromLoco.functions[ indexFunction ] |= (functionNumber << 4) ;        // set bits with number
//     }
//     return menu.endState() ;
// }

// StateFunction( getDelay )
// {
//     if( menu.entryState() )
//     {
//         clearDisplay() ;
//         printAt(0,0, F("Function mapping"   ) );
//         printAt(0,1, F("enter duration"     ) ) ;
//         printAt(0,2, F("0 = toggle"         ) ) ;
//         printAt(0,3, F("1 = momentary"      ) ); // if possible with my current keypad system....
//         printAt(0,4, F("green=select"       ) );
//         printAt(9,6, F("(x 0.1s)"           ) ); 
//         key = 1 ;
//     }
//     if( menu.onState() )
//     {
//         if( key )
//         {
//             if( key == GREEN )  menu.exit() ;
//             else                makeNumber( &timeDelay, key, 0, 255 ) ;

//             printNumberAt( 3, 6, 3, timeDelay ) ;
//         }
//     }
//     if( menu.exitState() )
//     {
//         eepromLoco.timeDelay[ indexFunction ] = timeDelay ;
//     }
//     return menu.endState() ;
// }
/*
uint8   functions [10] ;        // low nibble function symbol, high nibble function number
*/

StateFunction( configureThrottle )      // FIXME
{
    // uint8 throttleLevel ;
    // uint8 brakeLevel ;
    // uint8 dirLevel ;
    static uint8_t lowerSet ;

    if( menu.entryState() )
    {
       clearDisplay() ;
       printAt(0,0, F("configure levers" ) );
       printAt(0,1, F("set levers in" ) );
       printAt(0,2, F("LOWER POSITION " ) );
       printAt(0,3, F("green=store" ) );
       printAt(0,4, F("red=back" ) );
       lowerSet = false ;
    }
    if( menu.onState() )
    {
        if( key == RED )
        {
            menu.exit() ;
        }

        if( key == GREEN )
        {
            if( lowerSet == false )
            {   lowerSet  = true ;
                printAt(0,2, F("UPPER POSITION " ) ) ;

                setting.throttleLow = analogRead( throttlePin ) ;
                setting.brakeLow    = analogRead( brakePin ) ;
                setting.dirLow      = analogRead( directionPin ) ;
            } else
            {
                setting.throttleHigh = analogRead( throttlePin ) ;
                setting.brakeHigh    = analogRead( brakePin ) ;
                setting.dirHigh      = analogRead( directionPin ) ;
            }
            storeSettings() ;
        }

        if( menu.repeat( 150 ) )
        {
            uint16 throttleLevel   = map( analogRead( throttlePin),   setting.throttleLow, setting.throttleHigh,  0,  8 ) ;
            uint16 brakeLevel      = map( analogRead( brakePin),      setting.brakeLow,    setting.brakeHigh,     0, 10 ) ;
            uint16 dirLevel        = map( analogRead( directionPin),  setting.dirLow,      setting.dirHigh,       0, 3  ) ;

            throttleLevel   = constrain( throttleLevel, 0, 50 ) ;
            brakeLevel      = constrain( brakeLevel, 0, 50 ) ;
            dirLevel        = constrain( dirLevel, 0, 50 ) ;
            
            printNumberAt( 0,  6, 3, dirLevel      ) ;
            printNumberAt( 6,  6, 3, brakeLevel    ) ;
            printNumberAt( 12, 6, 3, throttleLevel ) ;

            printNumberAt( 0,  7, 3, analogRead( directionPin   )  ) ;
            printNumberAt( 6,  7, 3, analogRead( brakePin       )  ) ;
            printNumberAt( 12, 7, 3, analogRead( throttlePin    )  ) ;

        }
    }
    if( menu.exitState() )
    {
        clearDisplay() ;
    }

    return menu.endState() ;
}




StateFunction( getStreetIndex )
{
    if( menu.entryState() )
    {
        clearDisplay() ;
        printAt( 0, 0, F( "Enter street num" ) ) ;
        printAt( 0, 2, F( "street index:" ) ) ;
        printAt( 0, 6, F( "<Y>  select  <N>"    ) ) ;
        printAt( 0, 7, F( "Point btn: back"    ) ) ;

        streetIndex = 1 ;
        key = 1 ;   // forces printing numbers on first pass..
    }
    if( menu.onState() )
    {
        if( key > 0 ) 
        {
            if( key == MENU || key == 'p')
            {
                menu.exit() ; 
            }            
            else if( key == 'Y')
            {
                if( streetIndex > 1 ) streetIndex -- ;
            }
            else if( key == 'N' )
            {
                if( streetIndex < nStreets-1 ) streetIndex ++ ;
            }
            else if( key >= '0' && key <= '9' )
            {
                makeNumber( &streetIndex, key, 1, nStreets ) ;   // index is used to calculate memory in EEPROM
            }
            printNumberAt( 9, 2, 3, streetIndex  ) ;
        }
    }
    if( menu.exitState() )
    {
        streetIndex --  ;
        printAt( 0, 3, F( "Selected" ) ) ;
    }
    return menu.endState() ;
}

StateFunction( setPoints )
{
    if( menu.entryState() )
    {
        clearDisplay() ;      
        printAt(0,0,F("Laying route"));
        printNumberAt(14,0,1,streetIndex ) ;

        uint16_t eeAddress =  streetAddress + (streetIndex * pointsPerStreet * 2) ;
        i2cEeprom.get( eeAddress, address ) ; 

        // getStreet( address, streetIndex ) ;
        for( uint8 i = 0 ; i < pointsPerStreet ; i ++ )
        {
            pointState[i] = address[i] >> 15 ; 
            address[i] &= ~0x8000 ;                 // clear first bit
        } 
        pointIndex = 0 ;
    }
    if( menu.onState() )
    {
        if( menu.repeat( 2500 ) )
        {
            setPoint( (address[pointIndex] -1 ) | (pointState[pointIndex] << 15) ) ;

            printAt(0,2,F("point"));
            printNumberAt(6,2,4,address[pointIndex]) ;
            if(  pointState[ pointIndex ] ) { memcpy_P( toRam, straight,  8 ) ; } // may be needed to flip..
            else                            { memcpy_P( toRam, curved,    8 ) ; }

            printCustom( 13, 2, 0, toRam ) ;

            pointIndex ++ ;

            if( (address[pointIndex] & 0x4000) || pointIndex == pointsPerStreet ) menu.exit() ;
        }   
    }
    if( menu.exitState() )
    {
        
    }
    return menu.endState() ;
}

StateFunction( storePoints )
{
    if( menu.entryState() )
    {
        pointIndex = 0 ;
        clearDisplay() ;
        printAt( 0,  0, F( "Add points "        ) ) ;
        printAt( 0,  1, F( "point index:"        ) ) ;  
        
        printAt( 0,  4, F( "Address"            ) ) ;  

        printAt( 0,  6, F( "<Y>  toggle  <N>"   ) ) ;
        printAt( 0,  7, F( "Point btn: back"    ) ) ;

        //getStreet( address, streetIndex ) ;
        uint16_t eeAddress =  streetAddress + (streetIndex * pointsPerStreet * 2) ;
        i2cEeprom.get( eeAddress, address ) ; 

        for( int i = 0 ; i < pointsPerStreet ; i ++ )
        {
            pointState[i] = address[i] >> 15 ; 
            address[i] &= ~0x8000 ;                 // clear first bit
        } 
        key = 1 ;   // force printing numbers on first pass..
    }

    if( menu.onState() )
    {
        if( key > 0 ) 
        {
            if( key == 'p'   ) {  menu.exit() ;  }
            if( key == RED   ) {  if( --pointIndex == 0xFFFF )          pointIndex = pointsPerStreet-1  ;  }
            if( key == GREEN ) {  if( ++pointIndex == pointsPerStreet ) pointIndex = 0 ;                    }
            if( key == MENU  )
            { 
                pointState[ pointIndex ] ^= 1 ; 
                setPoint( (address[pointIndex] -1 ) | (pointState[pointIndex] << 15) ) ;
            }
            else if( key >= '0' && key <= '9' )
            {
                makeNumber( &address[pointIndex], key, 1, 1024 ) ;                      // index is used to calculate memory in EEPROM
            }

            printNumberAt( 10, 1, 3, pointIndex+1 ) ;
            printNumberAt( 7, 4, 5, address[pointIndex] ) ;
            if(  pointState[ pointIndex ] ) { memcpy_P( toRam, straight,  8 ) ; }       // may be needed to flip..
            else                            { memcpy_P( toRam, curved,    8 ) ; }

            printCustom( 15, 4, 0, toRam ) ;
        }
    }
    
    if( menu.exitState() )
    {
        for( int i = 0 ; i < pointsPerStreet ; i ++ ) 
        {
            uint16 newState = pointState[i] ;
            newState <<= 15 ;
            address[i] |= newState ;
        }
        
        uint16_t eeAddress =  streetAddress + (streetIndex * pointsPerStreet * 2) ;
        i2cEeprom.put( eeAddress, address ) ; // TEST ME

        // storeStreet( address, streetIndex ) ; WORKS FINE BUT OBSOLETE IF ABOVE TESTS WELL
        clearDisplay() ;
        printAt( 0, 0, F( "Street stored!" ) ) ;
    }

    return menu.endState() ;
}

// STATE MACHINE
extern byte Menu()
{
    if( menu.run() ) switch( menu.getState() )
    {
        State( setPoints ) {
            menu.nextState( LayoutControl, 1500 ) ; }

        State(LayoutControl) { ; }

        // State( configureLoco )
        // {
        //     if( key == '1' ) menu.nextState( editName,   0 ) ;        
        //     if( key == '2' ) menu.nextState( getAddress, 0 ) ;         
        //     if( key == '3' ) menu.nextState( functionMapping, 0 ) ;
        //     if( key == '4' ) menu.nextState( changeSlot,  0 ) ;
        //     if( key == '5' ) menu.nextState( setSpeedFactor, 0 ) ;
        //     if( key == '6' ) menu.nextState( resetSlot, 0 ) ;
        //     if( key == RED ) menu.nextState( LayoutControl, 0 ) ;         
        // }

            // State( editName )        {  menu.nextState( configureLoco, 0 ) ; }
            // State( getAddress )      {  menu.nextState( configureLoco, 0 ) ; }  
            // State( changeSlot )      {  menu.nextState( configureLoco, 0 ) ; }  
            // State( resetSlot )       {  menu.nextState( configureLoco, 2000 ) ; }  
            // State( functionMapping ) {  if( key == RED )    menu.nextState( configureLoco,  0 ) ;
            //                             else                menu.nextState( getSymbol,      0 ) ; }
                                        
            //     State( getSymbol )      { menu.nextState( getFuncNumber,    0 ) ; }
            //     State( getFuncNumber )  { menu.nextState( getDelay,         0 ) ; }
            //     State( getDelay )       { menu.nextState( functionMapping,  0 ) ; }

            
        State(configureThrottle)
        {
            menu.nextState( LayoutControl, 0 ) ; }

        State( getStreetIndex )
        {
            if( key == 'p' ) menu.nextState( LayoutControl, 0 ) ;
            else             menu.nextState( storePoints, 2000 ) ;
        }

        State( storePoints )
        {
            menu.nextState( getStreetIndex, 0 ) ;        // either do another street
        }

        break ;
    }

    pressTime = 0 ;
    key = 0 ;
}

/*
remove get slot
configure loco accessed directly
add ability to 
removed back to main
alter main to exclude slot
*/