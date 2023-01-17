#include <Arduino.h>

extern void makeNumber( uint16_t *number, char _c, uint16_t min, uint16_t max ) ;
//extern bool makingNumber ;
extern bool makeNumberTimeout() ;
extern void abortMakeNumber() ;
extern void updateNumber() ;
extern void printNumber(uint16_t) __attribute__ ((weak)) ;
extern void notifyNumberEntered() __attribute__ ((weak)) ;