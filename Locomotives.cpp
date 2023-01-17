#include "Locomotives.h"
// #include "src/i2cEeprom.h"


// #define SLOT_ADDRESS 7800

// EepromLoco eepromLoco ;
// static I2cEeprom i2cEeprom( 0x50 ) ;

// uint8 getIndex( uint8 func )
// {
//     for (int i = 0; i < 10; i++)
//     {
//         if( func == getFunction( i )) return i ;
//     }
//     return 15 ;                         // bit 15 is not in use, so if no match is found.. nothing happens
// }

// uint8 getFunction( uint8 func )
// {
//     return eepromLoco.functions[ func ] >> 4 ;                                  // lower nibble holds actual function number,
// }

// uint8 getSymbol( uint8 func )
// {
//     return eepromLoco.functions[ func ] & 0x0F ;                                // high nibble holds symbol
// }

// void setLoco( uint8 slot )
// {
//     uint16 eeAddress = SLOT_ADDRESS + ( sizeof( eepromLoco ) * slot ) ;
//     i2cEeprom.put( eeAddress, eepromLoco ) ;
// }

// void getLoco( uint8 slot )
// {
//     uint16 eeAddress = SLOT_ADDRESS + ( sizeof( eepromLoco ) * slot ) ;
//     i2cEeprom.get( eeAddress, eepromLoco ) ;

//     /*static */char* ptr ;
//     ptr = eepromLoco.name ;
    
//     if( *ptr == 255 )                                                           // if first char of name is 255 -> no loco in this slot TEST ME!
//     {
//         eepromLoco.name = "Slot #          " ;                                  // load default values
//         ptr += 7 ;
//         *ptr = slot += '0' ;                                                    // should set 7th char to current slot number...
//         for (int i = 0; i < 10; i++)
//         {
//             eepromLoco.functions[i] = i << 4 ;                                  // load default function numbers and let all symbols be 0
//             eepromLoco.timeDelay[i] = 0;                                        
//         }
//         eepromLoco.address = slot ;                                             // default adress will be slot number...
//         eepromLoco.speedFactor = 50 ;       
//     } 
// }

// // void listAllLoco( uint8 page )                                                  // DISCARDED
// // {
// //     for( int slot = page*8; slot < page * 8 + 8 ; slot++ )                      // gets 8 locos from i2cEeprom and print desciption
// //     {
// //         EepromLoco loco ;
// //         uint16 eeAddress = SLOT_ADDRESS + ( sizeof( loco ) * slot) ;

// //         i2cEeprom.get( eeAddress, loco ) ; 

// //         char *ptr ;
// //         ptr = loco.name ;

// //         for( int i = 0 ; i < 16 ; i ++ )
// //         {
// //             if( *ptr >= 0x80 ) *ptr = '*' ;                                     // if invalid character, replace it by a *
// //         }

// //         printDescription( loco.name, slot % 8 ) ;           // 2nd argument is y coordinate...
// //     }
// // }


/****** RAM LOCO SLOTS ******/

/* IN WORDS
    if a new address is entered, a slot must be located in RAM to load a locomotive and update all speed and functions
    if the address isn't in use yet, a new slot must be assigned

    if a value like speed or function changes, the slot must be updated. This could be done continously, that is where ram is good for...

*/

locoSlot slotLoco[nSlots] ;
bool    newSlot ;

void initSlots()
{
    for( int i = 0 ; i < nSlots ; i ++ )
    {
        slotLoco[i].address = 0xFFFF ;
        slotLoco[i].speed = 0 ;
        slotLoco[i].F0_F4 = 0 ;
        slotLoco[i].F5_F8 = 0 ;
        slotLoco[i].F9_F12 = 0 ;
    }
}


uint8 addSlot( uint16 address )
{
    for( int i = 0 ; i < nSlots ; i ++ )
    {
        if( slotLoco[i].address == 0xFFFF )     // if free address is found
        {
            slotLoco[i].address = address ;     // fill this with an address 
                                                // speed and functions are defaulted to 0
            return i ;                          // return the free index
        }
    }
}

void updateSlot( uint16 address, uint8 type, uint8 data )     // current loco contains all variables like speed n functions
{
    for( int i = 0 ; i < nSlots ; i ++ )
    {
        if( address == slotLoco[i].address )    // if address is found
        {
            switch( type )
            {
            case eSpeed :  slotLoco[i].speed  = data ; break ;
            case eFunc1 :  slotLoco[i].F0_F4  = data ; break ;
            case eFunc2 :  slotLoco[i].F5_F8  = data ; break ;
            case eFunc3 :  slotLoco[i].F9_F12 = data ; break ; 
            }
        }
    }
}

bool loadSlot( locoSlot *currentLoco, uint16 address )
{
    

    for( int i = 0 ; i < nSlots ; i ++ )
    {
        if( address == slotLoco[i].address )                                    // if address is found
        {
            *currentLoco = slotLoco[i] ;                                        // set address of pointer to slot in ram

            return true ;                                                       // return if done
        }
    }

    if( newSlot )                                                               // should prevent from adding slots from accidental selected addresses
    {
        newSlot = false ;
        return false ;
    }

    byte newSlot = addSlot( address ) ;

    *currentLoco = slotLoco[newSlot] ;

    return false ;
}
