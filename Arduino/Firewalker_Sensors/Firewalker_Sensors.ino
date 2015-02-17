#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_TMP007.h"
#include "parameters.h"

#define SIGN(x) ((x)<0.0)?("-"):("")

// http://www.instructables.com/id/two-ways-to-reset-arduino-in-software/step2/using-just-software/
void(* resetFunc) (void) = 0; //declare reset function at address 0

/** Set up the Serial Buffers **/
char cmdInFrameBuffer[SZ_COMMAND_BYTES];
char cmdOutFrameBuffer[SZ_COMMAND_BYTES];
int cmdPtr = 0;

/** About TMP007 Sensor **/
// you can also use tmp007.begin(TMP007_CFG_1SAMPLE) or 2SAMPLE/4SAMPLE/8SAMPLE to have
// lower precision, higher rate sampling. default is TMP007_CFG_16SAMPLE which takes
// 4 seconds per reading (16 samples)

/** Set up the IR Temp Sensors (a.k.a. Lava Sensors) **/
#define N_LAVA 7
#define PERIOD_LAVA_SENSE_MS 4000
Adafruit_TMP007 lavaSense[N_LAVA];  // set up an array to scan through
bool lavaSensorOk[N_LAVA];
float lavaFovT[N_LAVA]; // This holds the temp sense from FOV of the IR Temp
float lavaDieT[N_LAVA]; // THis holds the die temp from the IR Temp Sensors
unsigned long lastLavaSense;

Adafruit_TMP007 lavaSense0(0x40);  // start with a diferent i2c address
Adafruit_TMP007 lavaSense1(0x41);  // start with a diferent i2c address!
Adafruit_TMP007 lavaSense2(0x42);  // start with a diferent i2c address!
Adafruit_TMP007 lavaSense3(0x43);  // start with a diferent i2c address!
Adafruit_TMP007 lavaSense4(0x44);  // start with a diferent i2c address!
Adafruit_TMP007 lavaSense5(0x45);  // start with a diferent i2c address!
Adafruit_TMP007 lavaSense6(0x46);  // start with a diferent i2c address!

void setup()
{
    cmdSerial.begin(57600);
    InitializeDevices();
}

void InitializeDevices()
{
    InitializeLavaSensors();
}

void InitializeLavaSensors()
{
    /** Set Up the IR Temp Sensors **/
    lastLavaSense = 0;

    /* Map the IR Temp Sensors to the array */
    lavaSense[0] = lavaSense0;
    lavaSense[1] = lavaSense1;
    lavaSense[2] = lavaSense2;
    lavaSense[3] = lavaSense3;
    lavaSense[4] = lavaSense4;
    lavaSense[5] = lavaSense5;
    lavaSense[6] = lavaSense6;

    for( int i = 0; i < N_LAVA; i++ )
    {
        if (! lavaSense[i].begin()) {
            lavaSensorOk[i] = false;
//             cmdDebug.print("No sensor found at address 0x4");
//             cmdDebug.println(i);
        } else {
            lavaSensorOk[i] = true;
        }
    }
    lavaSensorOk[1] = true;
}

void loop()
{
    checkLavaSenseIfAvailable();
    if( checkIfSerialCompleted(&cmdSerial, cmdInFrameBuffer, &cmdPtr) )
    {
        ProcessPacket();
        // cmdSerial.println("Ready to print");
        cmdSerial.print(cmdOutFrameBuffer);
    }
    doHousekeeping();
}

/** ProcessPacket
 * 
 * This function checks the packet and fills the response buffer.
 * 
 * Created 14 Feb 2015 by J. Donovan for Ted Donovan
 * 
 **/
int ProcessPacket()
{
//     for( int i = 0; i < cmdPtr; i++ )
//         Serial.write(cmdInFrameBuffer[i]);
    /* Find the first character after FEND */
    int p = 0;
    char cCmd = cmdInFrameBuffer[p++];
    while( (cCmd == FEND) && (p < cmdPtr) )
        cCmd = cmdInFrameBuffer[p++];

    switch( cCmd )
    {
        case 'T':
            cmdDebug.println("Found telemetry Request");
            return processTelemetryRequest(cmdOutFrameBuffer, cmdInFrameBuffer, &cmdPtr);
            break;
        case 'A':
            return processAckPacketRequest(cmdOutFrameBuffer, cmdInFrameBuffer, &cmdPtr);
        default:
            return processNakPacketRequest(cmdOutFrameBuffer, cmdInFrameBuffer, &cmdPtr);
    }

    cmdPtr = 0; // reset the pointer.
    return 0;
}

int processTelemetryRequest( char* outBuffer, char* inBuffer, int* inPtr )
{
    int outPtr = 0;
    sprintf(outBuffer, "%cTELM",FEND);
    outPtr += 5;
    for( int i = 0; i < N_LAVA; i++ )
        sprintf(outBuffer, "%s, %s%d.%.2d", outBuffer, SIGN(lavaFovT[i]), int(lavaFovT[i]), abs(int(lavaFovT[i]*100) % 100) );
    sprintf(outBuffer, "%s%c", outBuffer,FEND);
    return 0;
}

int processAckPacketRequest( char* outBuffer, char* inBuffer, int* inPtr )
{
    int p = 0;
    sprintf(outBuffer, "%cOKAY%c", FEND, FEND);
    p += 6;
    return p;
}

int processNakPacketRequest( char* outBuffer, char* inBuffer, int* inPtr )
{
    int p = 0;
    sprintf(outBuffer, "%cNACK%c", FEND, FEND);
    p += 6;
    return p;
}

/** doHousekeeping
 * 
 * This function is a catch-all for everything the Mega has to do
 * in response to a received packet, where the time to do the action
 * exceeds the required reply time.
 * This function should run quickly, and do long actions in pieces
 * to ensure the serial is polled often enough.
 * 
 * Created 14 Feb 2015 by J. Donovan for Ted Donovan
 * 
 **/
void doHousekeeping()
{
    
}

/** checkIfSerialAvailable
 * 
 * This function polls the serial port and returns all characters up 
 * to the FEND character.
 * 
 * Created 14 Feb 2015 by J. Donovan for Ted Donovan
 * 
 **/
bool checkIfSerialCompleted( HardwareSerial* mySerial, char* buffer, int* ptr )
{
    char c = 'o';

    while( mySerial->available() && (c != FEND) )
    {
        c = mySerial->read();
        buffer[ptr[0]++ % SZ_COMMAND_BYTES] = c;
    }

    if( c == FEND ) // is packet complete?
        return true;
    return false;
}


/** checkLavaSenseIfAvailable
 * 
 * This function polls the IR Temp Sensors
 * 
 * Created 14 Feb 2015 by J. Donovan for Ted Donovan
 * 
 **/
void checkLavaSenseIfAvailable()
{
    if( (lastLavaSense + PERIOD_LAVA_SENSE_MS) < millis() )
    {
        for( int i = 0; i < N_LAVA; i++ )
        {
            if( lavaSensorOk[i] ) 
            {
                lavaFovT[i] = lavaSense[i].readObjTempC();
//                 cmdDebug.print("Object Temperature: "); 
//                 cmdDebug.print(lavaFovT[i]); cmdSerial.print("*C\t");
                lavaDieT[i] = lavaSense[i].readDieTempC();
//                 cmdDebug.print("Die Temperature: "); 
//                 cmdDebug.print(lavaDieT[i]); cmdSerial.println("*C");
            } else {
                if (! lavaSense[i].begin()) {
                    lavaSensorOk[i] = false;
//                     cmdDebug.print("No sensor found at address 0x4");
//                     cmdDebug.println(i);
                } else {
                    lavaSensorOk[i] = true;
                }
            }
        }
        lastLavaSense = millis();
    }
    lavaFovT[3] = -0.03;//lavaSense[i].readObjTempC();
}

