#include <Arduino.h>
#include "src/macros.h"

const int nStreets = 64 ;
const int pointsPerStreet = 16 ;
typedef struct streets 
{
	uint16_t address[pointsPerStreet] ;
} Street ;
extern Street street;

const int leftMax	=	0 ;
const int midLeft	=	1 ;
const int midPos	= 	2 ;
const int midRight	=	3 ;
const int rightMax	=	4 ;

typedef struct settings
{
	uint16  throttleLow ;
	uint16  throttleHigh ;
	uint16  brakeLow ;
	uint16  brakeHigh ;
	uint16	dirLow ;
	uint16	dirHigh ;
	uint8   DEF ;
} Settings ;

extern Settings setting ;


// NOTE many states are not in use bcause theu are either deprecated or for future purposes
enum menuStates
{
    menuIDLE,
    Menu_,
    SSID,
    Password,
    Acceleration,
    Deceleration,
    OperationMode,
    IP,
    CentralIP,
    LayoutControl,
    // configureLoco,
    configureThrottle,
    teachInPoints,
    getStreetIndex,
    storePoints,
    setPoints,
    // for teachin in
    // getSlot,
    // adjustLoco,
    // editName,
    // getSlotIndex,
    // getAddress,
    // functionMapping,
    // getSymbol,
    // getFuncNumber,
    // setSpeedFactor,
    // getDelay,
    // resetSlot,
    // changeSlot,
} ;

//extern void adjustSlot() ; 
extern void layStreet( uint8 ) ;
extern void setMenu( uint8 newMenu) ;

extern byte Menu(void); 
extern void menuInit(void); 
extern uint8_t getMenu();
extern void loadSettings() ;
extern void storeSettings() ;


extern char key ;
extern uint8_t pressTime ;
void send( uint8_t, char ) ;

extern void setPoint( uint16_t ) __attribute__ ((weak)) ;