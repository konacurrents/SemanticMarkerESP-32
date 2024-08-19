
#include "SPIFFModule.h"

#ifdef USE_SPIFF_MODULE

#define SPIFF_FILE_NAME (char*)"/messages.txt"

#include "FS.h"
#include "SPIFFS.h"

/* You only need to format SPIFFS the first time you run a
 test or else use the SPIFFS plugin to create a partition
 https://github.com/me-no-dev/arduino-esp32fs-plugin */

//! @see https://techtutorialsx.com/2019/06/02/esp8266-spiffs-reading-a-file/
#define FORMAT_SPIFFS_IF_FAILED true

//!helper for using the spiff or not..
boolean useSpiff()
{
    return getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_SETTING);

}

//!list the directory of the SPIFF
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    if (!useSpiff())
        return;
    
    SerialDebug.printf("Listing directory: %s\r\n", dirname);
    
    File root = fs.open(dirname);
    if(!root){
        SerialDebug.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        SerialDebug.println(" - not a directory");
        return;
    }
    
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            SerialDebug.print("  DIR : ");
            SerialDebug.println(file.name());
            if(levels){
                //listDir(fs, file.path(), levels -1);
                listDir(fs, file.name(), levels -1);

            }
        } else {
            SerialDebug.print("  FILE: ");
            SerialDebug.print(file.name());
            SerialDebug.print("\tSIZE: ");
            SerialDebug.println(file.size());
        }
        file = root.openNextFile();
    }
}

//!reads the file name specified. The result is SerialDebug.write
void readFile(fs::FS &fs, const char * path)
{
    if (!useSpiff())
        return;
    
    SerialDebug.printf("Reading file: %s\r\n", path);
    SerialDebug.printf(" *** Done reading ****");
    
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        SerialDebug.println("- failed to open file for reading");
        return;
    }
    
    SerialDebug.println("- read from file:");
    while(file.available()){
        SerialDebug.write(file.read());
    }
    file.close();
}

//!writes the message to the file specified
void writeFile(fs::FS &fs, const char * path, const char * message)
{
    if (!useSpiff())
        return;
    
    SerialDebug.printf("Writing file: %s\r\n", path);
    
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        SerialDebug.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        SerialDebug.println("- file written");
    } else {
        SerialDebug.println("- write failed");
    }
    file.close();
}

//! appends the message to the file
void appendFile(fs::FS &fs, const char * path, const char * message)
{
    if (!useSpiff())
        return;
    
    SerialLots.printf("Appending to file: %s\r\n", path);
    
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        SerialDebug.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        SerialLots.println("- message appended");
    } else {
        SerialDebug.println("- append failed");
    }
    file.close();
}

//! renames the file
void renameFile(fs::FS &fs, const char * path1, const char * path2)
{
    if (!useSpiff())
        return;
    
    SerialMin.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        SerialMin.println("- file renamed");
    } else {
        SerialDebug.println("- rename failed");
    }
}

//!deletes the file specified
void deleteFile(fs::FS &fs, const char * path)
{
    if (!useSpiff())
        return;
    
    SerialMin.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        SerialMin.println("- file deleted");
    } else {
        SerialDebug.println("- delete failed");
    }
}

#ifdef NOT_USED
void testFileIO(fs::FS &fs, const char * path)
{
    SerialLots.printf("Testing file I/O with %s\r\n", path);
    
    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        SerialDebug.println("- failed to open file for writing");
        return;
    }
    
    size_t i;
    SerialDebug.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
            SerialDebug.print(".");
        }
        file.write(buf, 512);
    }
    SerialDebug.println("");
    uint32_t end = millis() - start;
    SerialDebug.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();
    
    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        SerialDebug.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
                SerialDebug.print(".");
            }
            len -= toRead;
        }
        SerialDebug.println("");
        end = millis() - start;
        SerialDebug.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        SerialDebug.println("- failed to open file for reading");
    }
}
#endif

    
    //    writeFile(SPIFFS, "/hello.txt", "Hello ESP ");
    //appendFile(SPIFFS, "/hello.txt", "Another line of text\r\n");
    
    //    appendFile(SPIFFS, "/hello.txt", "World!\r\n");
    
    
    //readFile(SPIFFS, "/hello.txt");
    //    renameFile(SPIFFS, "/hello.txt", "/foo.txt");
    //    readFile(SPIFFS, "/foo.txt");
    //    deleteFile(SPIFFS, "/foo.txt");
    //    testFileIO(SPIFFS, "/test.txt");
    //    deleteFile(SPIFFS, "/test.txt");
    //SerialDebug.println( "Test complete" );

//! The SPIFF module is for storing messages that are retrievable later as it stores on a folder area of the ESP chip

//!print a string to spiff (a new line is added)
void println_SPIFFModule(char *string)
{
    print_SPIFFModule(string);
    print_SPIFFModule((char*)"\r\n");
}

//!print a string to spiff (NO new line is added)
void print_SPIFFModule(char *string)
{
    appendFile(SPIFFS, SPIFF_FILE_NAME, string);
}
//!print a int to spiff (NO new line is added)
void printInt_SPIFFModule(int val)
{
    char str[20];
    sprintf(str,"%d",val);
    print_SPIFFModule(str);
}

//! delete the spiff files..
void deleteFiles_SPIFFModule()
{
    deleteFile(SPIFFS, SPIFF_FILE_NAME);
}

//! prints the spiff file to the SerialDebug output
void printFile_SPIFFModule()
{
    readFile(SPIFFS, SPIFF_FILE_NAME);
}


//! calculate the length of the SPFF file
int len_SPIFFFile(fs::FS &fs, const char * path)
{
    int len = 0;
    File file = fs.open(path);

    if(!file || file.isDirectory()){
        SerialDebug.println("- failed to open file for reading");
        return 0;
    }
    
    //now the rest of the lines can be sent..
    while(file.available())
    {
        String line = file.readString();
        
        //char * cstr = new char [str.length()+1];
        if (line)
        {
            len += line.length();
        }
    }
    file.close();
    return len;
}

//!reads the file name specified. returning the number of lines..
int linesInFile_SPIFFModule(fs::FS &fs, const char * path)
{

    if (!useSpiff())
        return 0;
    
    //count the lines
    int lines = 0;
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        SerialDebug.println("- failed to open file for reading");
        return lines;
    }
    
    while(file.available()){
        lines++;
        file.readString();
    }
    file.close();
    return lines;
}

#ifdef OLD
#define BUFFER_MAX 15000f
char *_buffer = NULL;
#endif

//! sends SPIFF module strings over MQTT, starting at the number back specified. This will use the current users MQTT credentials..
void sendStrings_SPIFFModule(int numberOfLines)
{
    if (!useSpiff())
        return;
    //! publish a binary file..
    //! fileExtension is .jpg, .json, .txt etc
    
    int len = len_SPIFFFile(SPIFFS, SPIFF_FILE_NAME);
    SerialDebug.printf("sendStrings_SPIFFModule (%d)\n", len);
    publishSPIFFFile((char*)"usersP/bark/images", SPIFF_FILE_NAME, len);
#ifdef OLD
    
    SerialDebug.println("sendStrings_SPIFFModule");
    
    fs::FS fs = SPIFFS;
    char *path = SPIFF_FILE_NAME;
    int linesMax = linesInFile_SPIFFModule(SPIFFS, SPIFF_FILE_NAME);
    SerialDebug.printf("linesMax = %d\n", linesMax);
    //somehow send over MQTT..
#ifdef USE_MQTT_NETWORKING
//    if (numberOfLines > 120)
//        numberOfLines = 120;
    // no limit
    numberOfLines = 500;

    
    //fast forward lines - numberOfLines
    // eg. if # = 100, but only want last 10, then
    int skipLines = linesMax - numberOfLines;
    if (skipLines < 0)
        skipLines = 0;
    
    //! PROBLEM: if only N lines fit into buffer .. how to delete only up to those lines?
    
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        SerialDebug.println("- failed to open file for reading");
        return;
    }
    
    int counter = 0;
    while(file.available() && counter < skipLines){
        file.readString();
        counter++;
    }
#define POST_AS_BUFFER
#ifdef  POST_AS_BUFFER

    if (!_buffer)
    {
        _buffer = (char*) calloc(1,BUFFER_MAX);
    }
    
    //!send a header
    //sendMessageNoChangeMQTT((char*)"Sending Post Mortum Debug");
    counter = 0;
//#define BUFFER_MAX 15000
//    char buffer[BUFFER_MAX];
    //now the rest of the lines can be sent..
    while(file.available() && counter < numberOfLines)
    {
        String line = file.readString();
        SerialDebug.println(line);
        //char * cstr = new char [str.length()+1];
        if (line)
        {
            
            //! get out if too big..
            if (strlen(_buffer) > line.length() + 100)
            {
                sendMessageNoChangeMQTT((char*)"BUFFER too big for SPIFF POST");

                SerialDebug.println(" **** BUFFER too big stopping");
                numberOfLines = 0;
            }
            else
            
                //strcpy (buffer, line.c_str());
                strcpy (_buffer, line.c_str());
            
            
            //send the string verbatum..
            //sendMessageNoChangeMQTT(buffer);
        }
        counter++;
    }
    file.close();
    //!send a header
    //sendMessageNoChangeMQTT((char*)"Done Post Mortum Debug");
    SerialDebug.printf("now publishBinary(%d)\n", strlen(_buffer));
    publishBinaryFile((char*)"usersP/bark/images",(uint8_t*) _buffer, strlen(_buffer),"json");

#else
    sendMessageNoChangeMQTT((char*)"Sending Post Mortum Debug ..todo");

#endif
#endif
#endif // OLD
}

//!writes a FB to a file..
void writeFB_SPIFFModule(
                         uint8_t * buf,              /*!< Pointer to the pixel data */
                         size_t len,                 /*!< Length of the buffer in bytes */
                         char *fileName)
{
    if (!useSpiff())
        return;
    
    File file = SPIFFS.open(fileName, FILE_WRITE);
    if(!file){
        SerialDebug.println("- failed to open file for writing");
        return;
    }
    if(file.write(buf,len)){
        SerialDebug.println("- file written");
    } else {
        SerialDebug.println("- write failed");
    }
    file.close();
}

//! the setup for this module
void setup_SPIFFModule()
{
    SerialDebug.println("SPIFFS setup");

    if (!useSpiff())
    {
        SerialDebug.println("not being used: SPIFFS");

        return;
    }

    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        SerialDebug.println("SPIFFS Mount Failed");
        return;
    }
    listDir(SPIFFS, "/", 0);
    
//testing:
    boolean test1 = false;
    if (test1)
    {
        // try reading the file
        printFile_SPIFFModule();
    }
    
    boolean test2 = false;
    if (test2)
    {
        // try reading the file
        sendStrings_SPIFFModule(5);
    }
}

//! a loop if anything (nothing right now)
void loop_SPIFFModule()
{
}

//! sends the Semantic Marker onto the SPIFF
//! format-  {'time':time, 'SM':'<sm>'}
void printSM_SPIFFModule(char *semanticMarker)
{
    println_SPIFFModule_JSON((char*)"SM",semanticMarker);

}

//! 4.4.24 output a line in JSON format adding timestamp as well
//!  {'time':time, 'attribute':'value'}
void println_SPIFFModule_JSON(char *attribute, char *value)
{
    if (getPreferenceBoolean_mainModule(PREFERENCE_USE_SPIFF_QRATOM_SETTING))
    {
        //!print a time too..
        //!NEED a format for this to distinguish from others..
        print_SPIFFModule((char*) "{\"time\":\"");
        printInt_SPIFFModule(getTimeStamp_mainModule());
        print_SPIFFModule((char*) "\",");
        print_SPIFFModule((char*) "\"");
        print_SPIFFModule(attribute);
        print_SPIFFModule((char*)"\":\"");
        print_SPIFFModule(value);
        println_SPIFFModule((char*)"\"},");
    }
}

//!prints a timestamp  time: <time> :
void printTimestamp_SPIFFModule()
{
    if (!useSpiff())
        return;
    
    print_SPIFFModule((char*)" time: ");
    printInt_SPIFFModule(getTimeStamp_mainModule());
    print_SPIFFModule((char*)": ");
}

#ifdef M5_CAPTURE_SCREEN
/***************************************************************************************
 * Function name:          M5Screen2bmp
 * Description:            Dump the screen to a WiFi client
 * Image file format:      Content-type:image/bmp
 * return value:           always true
 ***************************************************************************************/
bool M5Screen2bmp(WiFiClient &client){
    TFT_eSprite canvas = TFT_eSprite(&M5.Lcd);

    int image_height = M5.Lcd.height();
    int image_width = M5.Lcd.width();
    
    //https://docs.m5stack.com/en/api/core2/lcd_api
    canvas.createSprite(image_width, image_height);
    //canvas.println("HELLO M5 Canvas");
    canvas.fillSprite(RED);
    canvas.fillCircle(100,100,20,GREEN);
    canvas.pushSprite(0,0,WHITE);
    
    const uint pad=(4-(3*image_width)%4)%4;
    uint filesize=54+(3*image_width+pad)*image_height;
    unsigned char header[54] = {
        'B','M',  // BMP signature (Windows 3.1x, 95, NT, …)
        0,0,0,0,  // image file size in bytes
        0,0,0,0,  // reserved
        54,0,0,0, // start of pixel array
        40,0,0,0, // info header size
        0,0,0,0,  // image width
        0,0,0,0,  // image height
        1,0,      // number of color planes
        24,0,     // bits per pixel
        0,0,0,0,  // compression
        0,0,0,0,  // image size (can be 0 for uncompressed images)
        0,0,0,0,  // horizontal resolution (dpm)
        0,0,0,0,  // vertical resolution (dpm)
        0,0,0,0,  // colors in color table (0 = none)
        0,0,0,0 };// important color count (0 = all colors are important)
                  // fill filesize, width and heigth in the header array
    for(uint i=0; i<4; i++) {
        header[ 2+i] = (char)((filesize>>(8*i))&255);
        header[18+i] = (char)((image_width   >>(8*i))&255);
        header[22+i] = (char)((image_height  >>(8*i))&255);
    }
    // write the header to the file
    client.write(header, 54);
    
    // To keep the required memory low, the image is captured line by line
    unsigned char line_data[image_width*3+pad];
    // initialize padded pixel with 0
    for(int i=(image_width-1)*3; i<(image_width*3+pad); i++){
        line_data[i]=0;
    }
    // The coordinate origin of a BMP image is at the bottom left.
    // Therefore, the image must be read from bottom to top.
    for(int y=image_height; y>0; y--){
        // get one line of the screen content
      //  M5.Lcd.readRectRGB(0, y-1, image_width, 1, line_data);
      canvas.readRectRGB(0, y-1, image_width, 1, line_data);

        // BMP color order is: Blue, Green, Red
        // return values from readRectRGB is: Red, Green, Blue
        // therefore: R und B need to be swapped
        for(int x=0; x<image_width; x++){
            unsigned char r_buff = line_data[x*3];
            SerialTemp.print(r_buff);
            line_data[x*3] = line_data[x*3+2];
            line_data[x*3+2] = r_buff;
        }
        SerialTemp.println();
        // write the line to the file
        client.write(line_data, (image_width*3)+pad);
    }
    return true;
}

//!https://github.com/m5stack/M5StickC/issues/74
//!Says replace M5.Lcd with LCD::canvas

//!save the screen to the SPIFF .. testing
//!@see https://github.com/electricidea/M5Stack-Screen-Capture
//!@see https://www.hackster.io/hague/m5stack-screen-capture-and-remote-control-142cfe
//!
/***************************************************************************************
 * Function name:          M5Screen2bmp
 * Description:            Dump the screen to a bmp image File
 * Image file format:      .bmp
 * return value:           true:  succesfully wrote screen to file
 *                         false: unabel to open file for writing
 * example for screen capture onto SD-Card:
 *                         M5Screen2bmp(SD, "/screen.bmp");
 * inspired by: https://stackoverflow.com/a/58395323
 ***************************************************************************************/
bool M5Screen2bmp(fs::FS &fs, const char * path){
    // Open file for writing
    // The existing image file will be replaced
    File file = fs.open(path, FILE_WRITE);
    if(file){
        // M5Stack:      TFT_WIDTH = 240 / TFT_HEIGHT = 320
        // M5StickC:     TFT_WIDTH =  80 / TFT_HEIGHT = 160
        // M5StickCplus: TFT_WIDTH =  135 / TFT_HEIGHT = 240
        int image_height = M5.Lcd.height();
        int image_width = M5.Lcd.width();
        SerialDebug.printf("Saving screen to bmp file (%d,%d)\n", image_width, image_height);

        // horizontal line must be a multiple of 4 bytes long
        // add padding to fill lines with 0
        const uint pad=(4-(3*image_width)%4)%4;
        // header size is 54 bytes:
        //    File header = 14 bytes
        //    Info header = 40 bytes
        uint filesize=54+(3*image_width+pad)*image_height;
        unsigned char header[54] = {
            'B','M',  // BMP signature (Windows 3.1x, 95, NT, …)
            0,0,0,0,  // image file size in bytes
            0,0,0,0,  // reserved
            54,0,0,0, // start of pixel array
            40,0,0,0, // info header size
            0,0,0,0,  // image width
            0,0,0,0,  // image height
            1,0,      // number of color planes
            24,0,     // bits per pixel
            0,0,0,0,  // compression
            0,0,0,0,  // image size (can be 0 for uncompressed images)
            0,0,0,0,  // horizontal resolution (dpm)
            0,0,0,0,  // vertical resolution (dpm)
            0,0,0,0,  // colors in color table (0 = none)
            0,0,0,0 };// important color count (0 = all colors are important)
                      // fill filesize, width and heigth in the header array
        for(uint i=0; i<4; i++) {
            header[ 2+i] = (char)((filesize>>(8*i))&255);
            header[18+i] = (char)((image_width   >>(8*i))&255);
            header[22+i] = (char)((image_height  >>(8*i))&255);
        }
        // write the header to the file
        file.write(header, 54);
        
        // To keep the required memory low, the image is captured line by line
        unsigned char line_data[image_width*3+pad];
        // initialize padded pixel with 0
        for(int i=(image_width-1)*3; i<(image_width*3+pad); i++){
            line_data[i]=0;
        }
        // The coordinate origin of a BMP image is at the bottom left.
        // Therefore, the image must be read from bottom to top.
        for(int y=image_height; y>0; y--){
            // get one line of the screen content
            M5.Lcd.readRectRGB(0, y-1, image_width, 1, line_data);
            // BMP color order is: Blue, Green, Red
            // return values from readRectRGB is: Red, Green, Blue
            // therefore: R und B need to be swapped
            for(int x=0; x<image_width; x++){
                unsigned char r_buff = line_data[x*3];
                line_data[x*3] = line_data[x*3+2];
                line_data[x*3+2] = r_buff;
            }
            // write the line to the file
            file.write(line_data, (image_width*3)+pad);
        }
        file.close();
        return true;
    }
    else
    {
        SerialDebug.printf(" *** Cannot open file: %s\n", path);
    }
    return false;
}
#else

#endif // M5_CAPTURE_SCREEN

//!save the screen to a file on the SPIFF
void saveScreen_SPIFFModule()
{
#define SPIFF_SCREEN_FILE_NAME (char*)"/M5Screen.bmp"

#ifdef M5_CAPTURE_SCREEN
    boolean linesMax = M5Screen2bmp(SPIFFS, SPIFF_SCREEN_FILE_NAME);
    
    listDir(SPIFFS, "/", 0);
#endif
}
    
#else  //**** PLACEHOLDERS so the callers don't need ifdef

//!save the screen to a file on the SPIFF
void saveScreen_SPIFFModule()
{
    
}

//!print a string to spiff (a new line is added)
void println_SPIFFModule(char *string)
{
    
}

//!print a string to spiff (NO new line is added)
void print_SPIFFModule(char *string)
{
    
}

//!print a int to spiff (NO new line is added)
void printInt_SPIFFModule(int val)
{
    
}

//! delete the spiff files..
void deleteFiles_SPIFFModule()
{
    
}

//! prints the spiff file to the SerialDebug output
void printFile_SPIFFModule()
{
    
}

//! sends SPIFF module strings over MQTT, starting at the number back specified. This will use the current users MQTT credentials..
void sendStrings_SPIFFModule(int numberOfLines)
{
    
}

//! the setup for this module
void setup_SPIFFModule()
{
    
}

//! a loop if anything (nothing right now)
void loop_SPIFFModule()
{
    
}

//!prints a timestamp
void printTimestamp_SPIFFModule()
{
    
}

//! sends the Semantic Marker onto the SPIFF
//! format-  {'time':time, 'SM':'<sm>'}
void printSM_SPIFFModule(char *semanticMarker)
{
    
}
#endif // USE_SPIFF_MODULE
