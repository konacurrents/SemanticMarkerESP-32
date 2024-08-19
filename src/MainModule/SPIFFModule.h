

#ifndef SPIFFModule_h
#define SPIFFModule_h

#include "../../Defines.h"
#ifdef USE_REST_MESSAGING
#include <WiFiClientSecure.h>
#else
#include <WiFi.h>
#endif

//! The SPIFF module is for storing messages that are retrievable later as it stores on a folder area of the ESP chip
//!prints a timestamp  time: <time> :
void printTimestamp_SPIFFModule();

//! 4.4.24 output a line in JSON format adding timestamp as well
void println_SPIFFModule_JSON(char *attribute, char *value);

//!print a string to spiff (a new line is added)
void println_SPIFFModule(char *string);

//!print a string to spiff (NO new line is added)
void print_SPIFFModule(char *string);

//!print a int to spiff (NO new line is added)
void printInt_SPIFFModule(int val);

//! delete the spiff files..
void deleteFiles_SPIFFModule();

//! prints the spiff file to the SerialDebug output
void printFile_SPIFFModule();

//! sends SPIFF module strings over MQTT, starting at the number back specified. This will use the current users MQTT credentials..
void sendStrings_SPIFFModule(int numberOfLines);

//! the setup for this module
void setup_SPIFFModule();

//! a loop if anything (nothing right now)
void loop_SPIFFModule();

//!save the screen to a file on the SPIFF
void saveScreen_SPIFFModule();

//! sends the Semantic Marker onto the SPIFF
//! format-  {'time':time, 'SM':'<sm>'}
void printSM_SPIFFModule(char *semanticMarker);

#ifdef M5_CAPTURE_SCREEN
/***************************************************************************************
 * Function name:          M5Screen2bmp
 * Description:            Dump the screen to a WiFi client
 * Image file format:      Content-type:image/bmp
 * return value:           always true
 ***************************************************************************************/
bool M5Screen2bmp(WiFiClient &client);

//!writes a FB to a file..
void writeFB_SPIFFModule(
uint8_t * buf,              /*!< Pointer to the pixel data */
size_t len,                 /*!< Length of the buffer in bytes */
char *fileName);

#endif

#endif //SPIFFModule_h
