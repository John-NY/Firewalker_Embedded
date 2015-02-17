
#ifndef PARAMETERS_H
#define PARAMETERS_H

#define DEBUG 1 // Define Debugging Level
 
/** Define serial channels **/
#define cmdSerial Serial  // Interface to Embedded PC
#define cmdDebug  Serial3 // Debug Terminal
#define htrSerial Serial1 // Heater Control
#define ex1Serial Serial2 // Extra Serial
#define ex2Serial Serial3 // Extra Serial

/* Define serial control characters for cmdSerial */
#define SZ_COMMAND_BYTES 256 // max 256 bytes per command
#define FEND '|' // Assume ASCII input with no decimals

#endif
