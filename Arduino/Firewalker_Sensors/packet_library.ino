#include "parameters.h"

/* packet_library
 * 
 * This does the serial port reader.
 * 
 */
 
int readIfSerialAvailable(HardwareSerial* mySerial)
{
    bool unavailable = false;
    if( mySerial->available() )
    {
        mySerial->read();
        return 1;
    }
    return -1;
} 
