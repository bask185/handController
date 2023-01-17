#include <Arduino.h>
#include "src/macros.h"

enum locoMessages
{
    eSpeed ,
    eFunc1 ,
    eFunc2 ,
    eFunc3 ,
    eFunc4 ,
    eFunc5 ,
} ;

const int nSlots = 50 ;

extern uint8    getIndex( uint8 func ) ;
// extern uint8    getFunction( uint8 func ) ;
// extern void     setLoco( uint8 slot ) ;
// extern void     getLoco( uint8 slot ) ;

// extern void printDescription( char *description, uint8 y ) ;

// const int mph = 0x80 ;
// typedef struct  // for usage in eeprom storage, comes with function mapping.
// {
//     char    *name = "                " ;                                                            // 16 byte
//     uint16  address ;                                                                               //  2 byte
//     uint8   speedSteps ;                                                                            //  1 byte (can be removed?)
//     uint8   functions [10] ;        // low nibble function symbol, high nibble function number      // 10 bytes
//     uint8   timeDelay [10] ;        //                                                              // 10 bytes (can be removed if auto turn off fails)
//     uint8   speedFactor ;                                                                           //  1 byte
//                                                                                                     // ----------  +
// } EepromLoco ;                                                                                      //  40 bytes per loco... (SHORT NAMES PERHAPS?)

// extern EepromLoco eepromLoco ;
// /* to add to above struct
//  * 4 extra addresses for multiple unit operation. OPTIONAL -> LOW PRIO

// */

typedef struct
{
    uint16  address ;
    uint8   speed ;
    uint8   F0_F4 ;
    uint8   F5_F8 ;
    uint8   F9_F12 ;
    // uint8   F13_F20 ;
    // uint8   F21_F18 ;

} locoSlot ;

extern bool  newSlot ;

extern bool  loadSlot( locoSlot *currentLoco, uint16 address ) ;
extern void  updateSlot( uint16 address, uint8 type, uint8 data )  ;
extern uint8 addSlot( uint16 address ) ;
extern void  initSlots() ;
