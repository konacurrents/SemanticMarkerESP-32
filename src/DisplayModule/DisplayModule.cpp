#include "DisplayModule.h"
#ifdef USE_DISPLAY_MODULE
//! \link DisplayModule
//!@see https://www.carletonsheets.com/assets/shared/usr/share/doc/doxygen-1.8.5/html/commands.html#cmdlink

//!@see https://docs.m5stack.com/en/api/stickc/lcd_m5stickc
//!@see https://github.com/m5stack/M5StickC-Plus
//!@see https://docs.m5stack.com/en/core/m5stickc_plus
//!@see https://docs.m5stack.com/en/api/core2/lcd_api  (for the Core2)
/**
 *  System:
 *    M5.begin();
 *
 *  LCD:
 * M5.Lcd.drawPixel(int16_t x, int16_t y, uint16_t color);
*  M5.Lcd.drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
*  M5.Lcd.fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
*  M5.Lcd.fillScreen(uint16_t color);
*  M5.Lcd.drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
*  M5.Lcd.drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,uint16_t color);
*  M5.Lcd.fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
*  M5.Lcd.fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta, uint16_t color);
*  M5.Lcd.drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
*  M5.Lcd.fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
*  M5.Lcd.drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
*  M5.Lcd.fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
*  M5.Lcd.drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
*  M5.Lcd.drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h),
*  M5.Lcd.drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
*   M5.Lcd.setCursor(uint16_t x0, uint16_t y0);
*  M5.Lcd.setTextColor(uint16_t color);
*  M5.Lcd.setTextColor(uint16_t color, uint16_t backgroundcolor);
*  M5.Lcd.setTextSize(uint8_t size);
*  M5.Lcd.setTextWrap(boolean w);
*  M5.Lcd.printf();
*  M5.Lcd.print();
*  M5.Lcd.println();
*  M5.Lcd.drawCentreString(const char *string, int dX, int poY, int font);
*  M5.Lcd.drawRightString(const char *string, int dX, int poY, int font);
 //M5.Lcd.drawJpg(const uint8_t *jpg_data, size_t jpg_len, uint16_t x, uint16_t y);
 //M5.Lcd.drawJpgFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);
 //M5.Lcd.drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);
 *
 */
/***************************************************************************************
  ** Function name:           setRotation  In
  ** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
  ***************************************************************************************/
/*
 void TFT_eSPI::setRotation(uint8_t m)
 */
#ifdef ESP_M5_LATER

//!new design for the pairing and dynamic discovery and pairing..

//!defined in icon.c
//extern const unsigned char bitmap[13424];
//! draws the bitmap .. for testing
void drawBitmapIcon()
{
    // M5.Lcd.drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
    //  M5.Lcd.drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h),

    M5.Lcd.drawRGBBitmap(0,0, (uint16_t *)bitmap, 135,100);
}
#endif //later..

#ifdef M5CORE2_MODULE
boolean _showScrollingTextWindow = false;

#define SHOW_BIG_BUTTON
//! the BIG BUTTON is green and says Feed
/**
 1.24.24 First attempt at using the M5Core2 display to show something other than the M5 display
 1. scrolling text
 */
#endif

//! 1.24.24 as alternate display windows are introducted,
//! let others know and don't overright stuff..
boolean inAlternateDisplay_displayModule()
{
#ifdef M5CORE2_MODULE
    //! if showing the scrolling window .. don't do the rest ..
    return _showScrollingTextWindow;
#else
    return false;
#endif
}

//!prints the text up to the maxLen (the setup of position, etc is already done)
void printText(String text, int maxLen);

//!default semantic marker (or the current one)
String _currentSemanticMarkerAddress="https://iDogWatch.com";
/**
* \image html IMG_4616.PNG "AR + Semantic Marker"
*/
//!returns the current semantic marker (eg. guest page)
const char* currentSemanticMarkerAddress_displayModule()
{
    return _currentSemanticMarkerAddress.c_str();
}
String _lastSemanticMarkerAddress = "";
//!default semantic title
String _semanticTitle = "M5 Clicker";
//!base http address of semantic markers
String _currentSemanticMarkerAddressBase = "https://iDogWatch.com";
//!stores the statusFunc (it's a pointer to a function)
const char* (*_getStatusFunc)(void) = NULL;

//!if blank, then only show something after a click (not a message)
boolean _semanticMarkerShown = false;

//! 1.26.24 10 seems to be working..
#define SEMANTIC_MARKER_QR_VERSION 10

//!The type of screen (not the SM_home, etc, but using the SemanticTitle)
//!NORMAL for normal display   .. we can use NORMAL for the first page..
#define SCREEN_TYPE_NORMAL 0
//! for home screen
#define SCREEN_TYPE_HOME 1
#define SCREEN_TYPE_STATUS 2
#define SCREEN_TYPE_HELP 3
#define SCREEN_TYPE_WIFI 4
#define SCREEN_TYPE_TILT 5
#define SCREEN_TYPE_BUZZ 6
#define SCREEN_TYPE_AP 7
#define SCREEN_TYPE_GUEST_PAGE 8
#define SCREEN_TYPE_FEED_GUEST 9
#define SCREEN_TYPE_PAIRED_DEV 10
#define SCREEN_TYPE_RESET 11
#define SCREEN_TYPE_REBOOT 12
//!try simple menu
#define SCREEN_TYPE_HOME_SIMPLE 13    //tilt
#define SCREEN_TYPE_HOME_SIMPLE_1 14  //buzz
#define SCREEN_TYPE_HOME_SIMPLE_2 15  //feed
#define SCREEN_TYPE_HOME_SIMPLE_3 16  //advanced
#define SCREEN_TYPE_DOC_FOLLOW 17  //docFollow
#define SCREEN_TYPE_TIMER 18 //timer

//!the current screen type (a side effect of asking for it)
int _currentScreenType = SCREEN_TYPE_NORMAL;

//!returns the screen type (f0..n) for the title (such as HOME)  NOTE: if the name changes .. ramifications
//!  So they match the MQTTNetwork title strings..
int screenTypeForTitle_displayModule(String title)
{
    //!NOTE: All the comparisons are ALL CAPS ...
    title.toUpperCase();
    int screenType = SCREEN_TYPE_NORMAL;
    if (title.compareTo("MINI CLICKER")==0)
    {
        screenType = SCREEN_TYPE_HOME_SIMPLE;
    }
    else if (title.compareTo("MINI-1")==0)
    {
        screenType = SCREEN_TYPE_HOME_SIMPLE_1;
    }
    else if (title.compareTo("MINI-2")==0)
    {
        screenType = SCREEN_TYPE_HOME_SIMPLE_2;
    }
    //!ADD SCREEN_TYPE_HOME

    else if (title.compareTo("MINI-3")==0)
    {
        screenType = SCREEN_TYPE_HOME_SIMPLE_3;
    }
    //rest are normal
    else if (title.compareTo("WIFI FEED")==0)
    {
        screenType = SCREEN_TYPE_HOME;
    }
    else if (title.startsWith("HELP"))
    {
        screenType = SCREEN_TYPE_HELP;
    }
    else if (title.startsWith("WIFI"))
    {
        screenType = SCREEN_TYPE_WIFI;
    }
    else if (title.startsWith("TILT"))
    {
        screenType = SCREEN_TYPE_TILT;
    }
    else if (title.startsWith("BUZZ"))
    {
        screenType = SCREEN_TYPE_BUZZ;
    }
    else if (title.startsWith("AP"))
    {
        screenType = SCREEN_TYPE_AP;
    }
    else if (title.startsWith("GUEST PAGE"))
    {
        screenType = SCREEN_TYPE_GUEST_PAGE;
    }
    else if (title.startsWith("FEED GUEST"))
    {
        screenType = SCREEN_TYPE_FEED_GUEST;
    }
    else if (title.startsWith("STATUS"))
    {
        screenType = SCREEN_TYPE_STATUS;
    }
    else if (title.startsWith("RESET"))
    {
        screenType = SCREEN_TYPE_RESET;
    }
    else if (title.startsWith("DOC FOLLOW"))
    {
        screenType = SCREEN_TYPE_DOC_FOLLOW;
    }
    else if (title.startsWith("REBOOT"))
    {
        screenType = SCREEN_TYPE_REBOOT;
    }
    else if (title.startsWith("TIMER"))
    {
        screenType = SCREEN_TYPE_TIMER;
    }
    //! the "P:" is a special token from the MQTT that denotes this is a pairing feed. The P: is replaced with the paired device
    else if (title.startsWith("P:"))
    {
        screenType = SCREEN_TYPE_PAIRED_DEV;
    }
    //set global used by callers of addToTextMessages_displayModule
    _currentScreenType = screenType;
    return screenType;
}



//!These are when the ESP_M5 isn't used, so the code below compiles..
#ifdef ESP_M5
#else
//!unused .. for now since no display
#define BLACK 100
#define WHITE 101
#define YELLOW 102
#define BLUE 103
#define RED 104
#define ORANGE 105

#endif
#ifdef M5CORE2_MODULE
#define WIDTH 320
#else
#define WIDTH 135
#endif

#define HEIGHT_3 20
#define HEIGHT_3_2LINE 60

#define HEIGHT_4 40

#define HEIGHT_2 10
#define HEIGHT_1 10

#define ROTATE_0 0
#define FONT_3 3
#define FONT_2 2
#define FONT_1 1

//!defines the position constants
//!Note some uses below just use the text color and size attributes, (Too much work to make a variable struct)
//!   eg. ORANGE,WHITE,BLACK
typedef struct TextPosition
{
    uint16_t x,y;
    uint16_t width, height;
    uint16_t textSize;
    uint16_t textColor;
    uint16_t textColorBackground;
    uint16_t blankColor;

    uint16_t rotation; // 0
    uint16_t maxLen; // 0

} TextPositionStruct;

//!EG:  #actMe (M5) { ksjdfkdsjfk
//!   Feed Guest
//!based on text size 3
TextPositionStruct _actionTextPosition          = {0,2,WIDTH,HEIGHT_2,FONT_2,BLUE,WHITE,BLACK,ROTATE_0,11};
TextPositionStruct _actionTextPositionZoomed    = {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,BLUE,WHITE,BLACK,ROTATE_0,14};
TextPositionStruct _actionTextHomePositionZoomed= {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,ORANGE,BLACK,BLACK,ROTATE_0,14}; //28};
// the simple title (MINI CLICKER)
TextPositionStruct _actionTextHomeSimplePositionZoomed  = {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,YELLOW,BLUE,BLACK,ROTATE_0,15}; //28};
TextPositionStruct _actionTextHomeSimplePositionZoomed_1= {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,BLUE,YELLOW,BLACK,ROTATE_0,15}; //28};
TextPositionStruct _actionTextHomeSimplePositionZoomed_2= {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,BLACK,GREEN,BLACK,ROTATE_0,15}; //28};
TextPositionStruct _actionTextHomeSimplePositionZoomed_3= {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,BLACK,WHITE,BLACK,ROTATE_0,15}; //28};
TextPositionStruct _actionTextHomeSimplePositionZoomed_4= {0,2,WIDTH,HEIGHT_3_2LINE,FONT_3,WHITE,RED,BLACK,ROTATE_0,15}; //28};

//! these are grouped in an array for the color index to use. If index > max then use max.. (-1)
#define MAX_ACTION_TEXT_HOME_COLORS 5
TextPositionStruct _actionTextHomeSimpleColor[MAX_ACTION_TEXT_HOME_COLORS] =
{
    _actionTextHomeSimplePositionZoomed,
    _actionTextHomeSimplePositionZoomed_1,
    _actionTextHomeSimplePositionZoomed_2,
    _actionTextHomeSimplePositionZoomed_3,
    _actionTextHomeSimplePositionZoomed_4,
};

//! mode is the status of sensors .
TextPositionStruct _sensorStatusTextPosition   = {0,27,WIDTH,HEIGHT_2,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,22};
TextPositionStruct _sensorStatusTextPositionZoomed   = {0,65,WIDTH,HEIGHT_3,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,22};

TextPositionStruct _helpStatusTextPosition =
    {0,65,WIDTH,HEIGHT_3,FONT_2,ORANGE,BLACK,BLACK,ROTATE_0,22};
//! Help
TextPositionStruct _multilineStatusTextPosition   = {0,27,WIDTH,HEIGHT_4,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,40};
TextPositionStruct _multilineStatusTextPositionZoomed   = {0,65,WIDTH,HEIGHT_4,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,40};

#ifdef M5CORE2_MODULE
//! 2.10.24 on M5Core2 there is a line under "MINI CLICKER" that can show the Device name and pair=
TextPositionStruct _moduleDeviceNameTextPosition   = {0,40,WIDTH,HEIGHT_3,FONT_3,YELLOW,BLUE,BLACK,ROTATE_0,30};
#endif
//!status are the state modules (WIFI ON/off, etc)
TextPositionStruct _moduleStatusTextPosition   = {0,67,WIDTH,HEIGHT_3,FONT_3,YELLOW,BLUE,BLACK,ROTATE_0,7};
TextPositionStruct _moduleStatusTextPositionZoomed   = {0,105,WIDTH,HEIGHT_3,FONT_3,YELLOW,BLUE,BLACK,ROTATE_0,7};

//! new for the SM_HOME_SIMPLE
TextPositionStruct _homeSimpleStatusTextPositionZoomed   = {0,105,WIDTH,HEIGHT_3,FONT_3,ORANGE,BLACK,BLACK,ROTATE_0,7};
//! new for the SM_HOME_SIMPLE - draws FEED as red if over 13 feeds..
TextPositionStruct _homeSimpleStatusTextPositionZoomedRED   = {0,105,WIDTH,HEIGHT_3,FONT_3,RED,BLACK,BLACK,ROTATE_0,7};

//! the position of "connected" at the bottom on the first 3 screens..
TextPositionStruct _connectedStatusTextPositionZoomed   = {0,213,WIDTH,HEIGHT_2,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,11};

//!new Menu selection (if 4 ..)
TextPositionStruct _menuTextPositionZoomed_4less  = {0,105,WIDTH,HEIGHT_3,FONT_3,ORANGE,BLACK,BLACK,ROTATE_0,7};
TextPositionStruct _menuTextPositionZoomed_4plus  = {0,105,WIDTH,HEIGHT_2,FONT_2,ORANGE,BLACK,BLACK,ROTATE_0,22};


//top == feed, bottom = semantic marker (or the action of it)..{
#define BIG_ACTION 0
#define SM_ACTION 1
#define SM_ACTION_SMALL 2
#define QRAVATAR_ACTION 3

#define MAX_ACTIONS 4

#define TRY_SWAPPED_COLORS
#ifdef  TRY_SWAPPED_COLORS
TextPositionStruct _zoomedTextPositions[MAX_ACTIONS] = {
    {0,145,WIDTH,HEIGHT_3,FONT_3,ORANGE,BLACK,BLACK,ROTATE_0,7},
    {0,180,WIDTH,HEIGHT_3,FONT_3,WHITE,BLUE,BLACK,ROTATE_0,7},

    //the smaller WIFI words (without feed..)
    {0,145,WIDTH,HEIGHT_2,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,40},
    //the avatar character.. not perfect Blue on Yellow  -- how about Red
    {50,150,1,HEIGHT_3,FONT_3,RED,YELLOW,BLACK,ROTATE_0,2}
};
#else
TextPositionStruct _zoomedTextPositions[MAX_ACTIONS] = {
    {0,145,WIDTH,HEIGHT_3,FONT_3,WHITE,BLUE,BLACK,ROTATE_0,7},
    {0,180,WIDTH,HEIGHT_3,FONT_3,WHITE,BLACK,BLACK,ROTATE_0,7},
    //!home screens bottom action
    {0,180,WIDTH,HEIGHT_3,FONT_3,ORANGE,BLACK,BLACK,ROTATE_0,7},
    
    //the smaller WIFI words (without feed..)
    {0,145,WIDTH,HEIGHT_2,FONT_2,WHITE,BLACK,BLACK,ROTATE_0,40},
    //the avatar character.. not perfect Blue on Yellow  -- how about Red
    {50,150,1,HEIGHT_3,FONT_3,RED,YELLOW,BLACK,ROTATE_0,2}
};
#endif

//! init the text - but override the text color (stil using the textP.blankCOlor
void setupTextPositionOverrideTextColor(TextPositionStruct textP, TextPositionStruct textColor)
{
    //SerialTemp.printf("setupText(%d,%d,w=%d,h=%d)\n", textP.x, textP.y, textP.width, textP.height);
#ifdef ESP_M5
#ifdef M5CORE2_MODULE
    //! DON"T rotate -- for the M5Core2 display
#else
    M5.Lcd.setRotation(textP.rotation);
#endif
    M5.Lcd.fillRect(textP.x, textP.y, textP.width, textP.height, textColor.blankColor );
    M5.Lcd.setTextColor(textColor.textColor, textColor.textColorBackground);
    M5.Lcd.setCursor(textP.x + 2, textP.y);
    M5.Lcd.setTextSize(textP.textSize);
#endif
}

//!show an OTA message..
void showOTAUpdatingMessage()
{
#ifdef ESP_M5
    // blank the area
    M5.Lcd.fillRect(0, 60, 135, 140, ORANGE);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(10, 90);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("\nStarting   \nOTA Update \nPlease Wait\n2-5 minutes");
#endif
}

//! set so it can be erased on next loop
boolean _fullScreenMessageDisplayed = false;

//! 4.3.24 show text in big letters
//!@see https://github.com/konacurrents/ESP_IOT/issues/323
//!show full screen message..
void displayFullscreenMessage(String text, boolean turnOn)
{
#ifdef ESP_M5
    if (turnOn)
    {
        _fullScreenMessageDisplayed = true;
       // M5.Lcd.fillScreen(BLUE);

        // blank the area
        M5.Lcd.fillRect(0, 0, 135, 240, BLUE);
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.setCursor(10, 90);
        M5.Lcd.setTextSize(3);
        M5.Lcd.print(text);
        
        // delay then clear
      //  delay(50);
      //  redrawSemanticMarker_displayModule(false);
        //wakeupScreen_displayModule();
    }
    else
    {
        _fullScreenMessageDisplayed = false;

        wakeupScreen_displayModule();

       // M5.blankscreen();
    }
#endif
}

//! init the text
void setupTextPosition(TextPositionStruct textP)
{
    //use same color
    setupTextPositionOverrideTextColor(textP, textP);

}

//!use the textPositionStruct - but override the text color
void printTextAtTextPositionOverrideTextColor(String text, TextPositionStruct textPosition, TextPositionStruct textColor)
{
    setupTextPositionOverrideTextColor(textPosition, textColor);
    //SerialDebug.printf("'%s'\n", text);
#ifdef ESP_M5
    M5.Lcd.print(text);
#endif
}

//!use the textPositionStruct
void printTextAtTextPosition(String text, TextPositionStruct textPosition)
{
    setupTextPosition(textPosition);
#ifdef ESP_M5
    M5.Lcd.print(text);
#endif
}


//! SCREEN COLOR
//!Increment the screen color 0..n cache for getting the screen color 0..max (max provided by sender)
//!This is implemented by incrementScreenColor_mainModule() since it knows the MAX value of colors
void incrementScreenColor_displayModule()
{
    int screenColor = getPreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING);
    screenColor++;
    if (screenColor >= MAX_ACTION_TEXT_HOME_COLORS)
    {
        // change color and store
        screenColor = 0;
    }
    savePreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING, screenColor);
    
    SerialCall.printf("incrementScreenColor_displayModule = %d\n", screenColor);
}

//!cache for getting the screen color 0..n. Will reset the cache as well
int getScreenColor_displayModule()
{
    //normalize
    int screenColor = getPreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING);
    if (screenColor < 0 || screenColor >= MAX_ACTION_TEXT_HOME_COLORS)
    {
        // normalize the number, and store it..
        screenColor = screenColor % MAX_ACTION_TEXT_HOME_COLORS;
        savePreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING, screenColor);
    }
    SerialLots.printf("getScreenColor_displayModule = %d\n", screenColor);

    return screenColor;
}

//!cache for getting the screen color 0..n. Will reset the cache as well
void setScreenColor_displayModule(int screenColor)
{
    //normalize
    if (screenColor < 0 || screenColor >= MAX_ACTION_TEXT_HOME_COLORS)
    {
        // normalize the number, and store it..
        screenColor = screenColor % MAX_ACTION_TEXT_HOME_COLORS;
    }
    savePreferenceInt_mainModule(PREFERENCE_SCREEN_COLOR_SETTING, screenColor);
    SerialCall.printf("setScreenColor_displayModule = %d\n", screenColor);

}



//!markerTimer is a semi timer for the number of times before doing anything. Kinda a kludge.
int _markerTimer = 0;
//! ticks.. when it rest? (show to blank screen)
int _loopTimer = 0;
//!increment the loop timer
void incrementLoopTimer()
{
    _loopTimer = (_loopTimer+1) % 100;
}
//!stop the loop timer
void stopLoopTimer()
{
    _loopTimer = 0;
}
//!returns the loop timer (just a timer..)
int getLoopTimer_displayModule()
{
    return _loopTimer;
}

//!reset the loop timer .. useful for testing,
void resetLoopTimer_displayModule()
{
    _loopTimer = 0;
}
/**
* \image html SMStatus1.PNG "Semantic Marker"
*/
//!NOTE: need to add dev:deviceName
//!creates semantic address, and if _getStatusFunc is non nil, call it to add to the address SemanticMarker&trade;
String createSemanticAddress()
{
    //!if there is a status function to call, call it and add to the address (and tack on the _loopTimer)
    //!Otherwise, just use the provided address (not a status like address)
    if (_getStatusFunc)
    {
        SerialTemp.print("createDynamicSemanticAddress: ");

        //check if this is right (the base)
       // _currentSemanticMarkerAddress = _currentSemanticMarkerAddressBase + "/" + String((*_getStatusFunc)()) + "&t=" + String(_loopTimer);
        _currentSemanticMarkerAddress = String((*_getStatusFunc)()) + "&t=" + String(_loopTimer);

    }
    else
    {
        SerialCall.print("createSemanticAddress: ");
        _currentSemanticMarkerAddress = _currentSemanticMarkerAddressBase;
    }
    

    int maxLenForIphone = 250;

    // strip down to 155 .. yanking off each parameter?? or just chop it.. for not just chop it..
    if (_currentSemanticMarkerAddress.length() > maxLenForIphone)
    {
        SerialTemp.printf("1. Truncating chartacters to: %d\n", maxLenForIphone);
        SerialTemp.println(_currentSemanticMarkerAddress);
        _currentSemanticMarkerAddress = _currentSemanticMarkerAddress.substring(0,maxLenForIphone);
    }

    SerialTemp.println(_currentSemanticMarkerAddress);
    return _currentSemanticMarkerAddress;
}


//**** Delay Methods*******
#define SINGLE_DELAY
#ifdef SINGLE_DELAY
//https://www.forward.com.au/pfod/ArduinoProgramming/TimingDelaysInArduino.html
//! the time the delay started
unsigned long _delayStart;
//! true if still waiting for delay to finish
boolean _delayRunning = false;
//! length of delay
int _delaySeconds;
//!init the delay starting
void startDelay(int seconds)
{
    SerialCall.printf("startDelay_displayModule: %d\n", seconds);

    _delayStart = millis();   // start delay
    _delayRunning = true; // not finished yet
    _delaySeconds = seconds;
    
}
//!if finished..
boolean delayFinished()
{
    if (_delayRunning && ((millis() - _delayStart) >= (_delaySeconds * 1000)))
    {
        _delayRunning = false;
        SerialCall.println("delayFinished..");
        return true;
    }
    return false;
}

//!stop the delay
void stopDelay()
{
    SerialCall.println("stopDelay _delayRunning=false");

    _delayRunning = false;
}
#endif //SINGLE_DELAY


//****** SCREEN DISPLAY ************
//!new 12.29.22
//!
//!    //! https://github.com/Bodmer/TFT_eSPI/issues/226
//! TRY THIS:
#define BLK_PWM_CHANNEL 7 // LEDC_CHANNEL_7
void setBrightness_displayModule(int brightness)
{
#ifdef LATER
    ledcWrite(BLK_PWM_CHANNEL, brightness);
#endif
}

//!blanks the screen
void blankScreen_displayModule()
{
    SerialCall.println("blankScreen_displayModule");
    stopLoopTimer();
    
    //!stop the delay which says delay finished .. so don't show anything..  8.9.22
    stopDelay();
    
    //nothing displayed
    _semanticMarkerShown = false;
    
#ifdef ESP_M5
    M5.Lcd.fillScreen(BLACK);
    //not available.. without M5Stack.h ???
    //        M5.Lcd.setBrightness(0);
    //        M5.Lcd.setBrightness(200);
    
    //M5.Lcd.sleep();
    
    // M5.Lcd.fillScreen(ORANGE);
    
    //get out of all modes..
    // #issue #144  show something to let know its screen saver not OFF
    
    printTextAtTextPosition("  CLICKER",_connectedStatusTextPositionZoomed);
    
  
  
#define TRY_DEVICES_AND_PAIRED
#ifdef  TRY_DEVICES_AND_PAIRED
    {
        char infoString[150];
        boolean multilineScreenType;

        //!update the model for those drawing Dev and Connected Dev
        updateMenuState(pairedDeviceModel);
        
        multilineScreenType = true;
        //! just ask the deviceState...
        ModelStateStruct *model = getModel(pairedDeviceModel);
        switch (model->pairedDeviceStateEnum)
        {
                //paired to a device, but BLE NOT connected right now
            case pairedButNotConnectedEnum:
                sprintf(infoString,"%s\n+%.*s",deviceName_mainModule(),10,getPairedDeviceOrAddress_mainModule() );
                break;
                //paired to a device, and BLE connected right now
            case pairedAndConnectedEnum:
                sprintf(infoString,"%s\n%.*s",deviceName_mainModule(),10,getPairedDeviceOrAddress_mainModule() );
                break;
                //not paired (but could be paired as it's a named device])
            case pairableAndConnectedEnum:
                sprintf(infoString,"%s\n-%.*s",deviceName_mainModule(),9,connectedBLEDeviceName_mainModule());
                break;
                //!factory default but not connecte to anything..
            case notConnectedEnum:
                //sprintf(infoString,"%s\n%s",deviceName_mainModule(),(char*)"scanning..");
                sprintf(infoString,"%s\n",deviceName_mainModule());

                break;
        }
        SerialCall.println(infoString);
        
        //!use the zoomed amount
        setupTextPosition(multilineScreenType?_multilineStatusTextPositionZoomed:_sensorStatusTextPositionZoomed);
        
        //!draw the text
        printText(infoString,multilineScreenType?_multilineStatusTextPosition.maxLen:_sensorStatusTextPosition.maxLen);
    }
#else
    //! also show the device name..
    //! at same location as before.. the small text below the top..
    //!use the zoomed amount
    setupTextPosition(_sensorStatusTextPositionZoomed);
    
    //!draw the device name (not including the parired or not paired .. as it can change dynaically...
    printTextAtTextPosition(deviceName_mainModule(),_sensorStatusTextPosition);
#endif
#endif
}

//!wakes up the screen
void wakeupScreen_displayModule()
{
    
    //clear screen first.. since the KEEP_SAME won't clear the screen..
    clearScreen_displayModule();

 //   _semanticMarkerShown = true;
    SerialCall.println("wakeupScreen_displayModule");
    
    //!see if this works .. vs too flickering.. (if so we have to slow it down..)
    redrawSemanticMarker_displayModule(KEEP_SAME);
    
}

//!clears the screen (but not a blank screen) and stops the timer..
void clearScreen_displayModule()
{
    SerialCall.println("clearScreen_displayModule");
    
    //reset the timer..  NEW!! 8.9.22  (a mini blankScreen)
    //??
    stopLoopTimer();

#ifdef ESP_M5
#ifdef M5CORE2_MODULE
    //! DON"T rotate -- for the M5Core2 display
#else
    M5.Lcd.setRotation(0);
#endif
    M5.Lcd.fillScreen(BLACK);
#endif
}

//!whether screen is blank (so the button can be a wake, vs action..)
boolean isBlankScreen_displayModule()
{
    SerialLots.print("isBlankScreen_displayModule:");
    SerialLots.println(!_delayRunning?"BLANK":"NOT BLANK");

    //! if the delay is running, then not blanked
    return !_delayRunning;

}

//! whether to display on a blank screen
boolean displayOnBlankScreen()
{
    return getPreferenceBoolean_mainModule(PREFERENCE_DISPLAY_ON_BLANK_SCREEN_VALUE);
}

//! current screen timeout
int currentScreenTimeout()
{
    int val =  getPreferenceInt_mainModule(PREFERENCE_DISPLAY_SCREEN_TIMEOUT_VALUE);
    return val;
}

#ifdef ESP_M5
//!setup the Lcd, etc..
void setupESP_M5()
{
    SerialMin.println("setupESP_M5");
    
#ifdef M5STICKCPLUS2
//! 4.20.24 add config
    M5.config();
#endif
#ifdef M5CORE2_MODULE
    /*
     https://arduino.stackexchange.com/questions/9092/how-do-you-call-a-class-method-with-named-parameters
    begin(bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true,
          bool I2CEnable = false, mbus_mode_t mode = kMBusModeOutput,
          bool SpeakerEnable = true);
    
    M5.begin(true, true, true, false);
    */
    M5.begin( /*LCDEnable*/true,  /*SDEnable*/true,  /*SerialEnable*/ true,
           /*I2CEnable*/ false,  /*mode*/ kMBusModeOutput,
           /*SpeakerEnable*/ true);
    
    //! see if this lets battery work on M5 Core2 v1.1
    //! 2.11.24 Super Bowl day
    //! STILL NOT WORKING on the v1.1 device
    /*
    M5.Axp.SetLcdVoltage(3300);
    M5.Axp.SetBusPowerMode(0);
    M5.Axp.SetCHGCurrent(AXP192::kCHG_190mA);
    */
    /*
    M5.Power.setPowerBoostOnOff(false); //Change the power on / off method. The power does not turn off when connected via USB. true=Press and hold to turn on / off. false=Turn on / off with two short presses.
    M5.Power.setPowerBoostSet(false); //Change the power on / off method true=ON / OFF in one short press. false=same as above
    M5.Power.setPowerVin(true); //When the power supply from USB etc. is cut off, Decide whether to turn on the power again.
    M5.Power.setPowerBtnEn(true); //Set whether to accept the power button.
    M5.Power.setPowerBoostKeepOn(false); //Always output power. True= Always output power. False=not Always output power.
    M5.Power.setAutoBootOnLoad(true); //Set whether to automatically start when power consumption occurs
     */
#else
    //!Init M5StickC Plus.  初始化 M5StickC Plus
    M5.begin();
#endif
    
    //!Set font size.
    //!Set the font color to yellow.  设置字体颜色为黄色
    M5.Lcd.setTextColor(WHITE);
    //!rotate to landscape (vs vertical)
    //!M5.Lcd.setRotation(3);
#ifdef M5CORE2_MODULE
    //! DON"T rotate -- for the M5Core2 display
    SerialMin.println("M5CORE2_MODULE");
    M5.Lcd.drawCircle(65,220, 10, WHITE);
    M5.Lcd.drawCircle(155,220, 10, WHITE);
    M5.Lcd.drawCircle(255,220, 10, WHITE);

    //! *  M5.Lcd.drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

#else
    //!Rotate the screen. 将屏幕旋转
    M5.Lcd.setRotation(0);
#endif
    //!set the cursor location.  设置光标位置
    
    //!Set font size.

    M5.Lcd.setTextSize(2);
    // M5.Lcd.printf("Click Controller v1.0");
    
    //! STARTUP SCREEN showing VERSION
    M5.Lcd.printf("%s", VERSION);

    //!Draws a SemanticMarker&trade; (using QR code)
    //!Create a QR code with a width of 135 QR code  at (0, 100).  在(0,0)处创建一个宽为135二维码
    //!Please select the appropriate QR code version according to the number of characters.  请根据字符数量选择合适的二维码版本
    M5.Lcd.qrcode("https://iDogWatch.com/bot/help",0,100,135,SEMANTIC_MARKER_QR_VERSION);
    //try to draw a character in the middle ...
    printTextAtTextPosition("SM",_zoomedTextPositions[QRAVATAR_ACTION]);
    //delay so they can read it a little bit..
    delay(550);
    
    //blank screen is on..
}
#endif //ESP_M5

//!THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//!This will perform preference initializtion as well
//! called from the setup()
void setup_displayModule()
{
#ifdef ESP_M5
    setupESP_M5();
#endif

    //this is supposed to say it's not in blank screen...
    //stopDelay();
    startDelay(60);

}

//! used to create a mini
int _fullscreenLoopCount = 0;

//! called for the loop() of this plugin
void loop_displayModule()
{
    //! set so it can be erased on next loop
    if (_fullScreenMessageDisplayed)
    {
        _fullscreenLoopCount ++;
        if (_fullscreenLoopCount > 10)
        {
            displayFullscreenMessage("", false);
            _fullscreenLoopCount = 0;
        }
        else
            return;

    }
#ifdef M5CORE2_MODULE
    //! 1.24.24 if alternate display then return ..
    if (inAlternateDisplay_displayModule())
    {

        loop_Alternate_displayModule();
        return;
    }
#endif

    //!check if a delay was running..
    if (delayFinished())
    {
        SerialCall.println("loop_displayModule: delayFinished");
        blankScreen_displayModule();
    }
  
    //TODO:
    // change sensors, and change any SemanticMarkers using said sensors..
    // or every second update SM (whatever is shown - and only change if sensors are showing (parms), eg. status
//either delay or redraw right now.. from loop()
    
    //4.20.22 (9 pm .. listening to Mike Campbell with drummer talking story about Tom Petty
    // lets use a counter vs time .. simplier??
    _markerTimer++;
//#define LOOP_COUNTER_MICROSECONDS 30
#define LOOP_COUNTER_MICROSECONDS 20   // 2 seconds it seems..  11.7.22

    if (_markerTimer > LOOP_COUNTER_MICROSECONDS) //10 was working .. got to 20 how fast it loop across all the modules (their loop() ..
    {
        
        //! 9.2.22 new .. update the timer. This is on blank screen as well .. so don't check _semanticMarkerShown
        updateMenuState(timerModel);
#define TRY_BLANKSCREEN_IN_LOOP
#ifdef  TRY_BLANKSCREEN_IN_LOOP
        //!check if a delay was running..
        if (isBlankScreen_displayModule())
        {
            SerialCall.println("loop_displayModule: isBlankScreen");
            blankScreen_displayModule();
        }
#endif
        _markerTimer = 0;
        if (_semanticMarkerShown)
        {
            SerialCall.println("loop_displayModule: redrawSemanticMarker(KEEP_SAME)");

            //!see if this works .. vs too flickering.. (if so we have to slow it down..)
            redrawSemanticMarker_displayModule(KEEP_SAME);
        }
    }
    
#ifdef M5CORE2_MODULE
    //! try to show where the 3 buttons are below the following box..
    M5.Lcd.drawRect(50, 235, 10, 3, WHITE);
    M5.Lcd.drawRect(155,235, 10, 3, WHITE);
    M5.Lcd.drawRect(258,235, 10, 3, WHITE);
    //! 240 too low..
#endif
}

// ************ Drawing of the Action, Mode and Status  ***************

//!prints the text up to the maxLen (the setup of position, etc is already done)
void printText(String text, int maxLen)
{
#ifdef ESP_M5
    if (text.length() < maxLen)
    {
        M5.Lcd.print(text);
    }
    else
    {
        M5.Lcd.print(text.substring(0,maxLen));
    }
#endif
}

//! print the text, but pad with background color spaces..
void printTextPadSpaces(String text, TextPositionStruct textColor)
{
#ifdef ESP_M5

    int maxLen = textColor.maxLen;
    printText(text, maxLen);
    
    // now pad with spaces .. using the color provided..
    // pad with spaces .., but in the background color..
    M5.Lcd.setTextColor(textColor.textColor, textColor.textColorBackground);
    for (int i=text.length(); i< maxLen; i++)
    {
        M5.Lcd.print(" ");
    }
#endif
}

//!sets the "ACTION"  -- The TOP line
void drawAction(String title, int screenType)
{
   
    SerialCall.print("drawAction: ");
    SerialCall.print(title);
    SerialCall.printf(" screenType=%d\n",screenType);
    
    //!setup the position for the Action text
    if (getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE)
        && (screenType != SCREEN_TYPE_HELP))
    {
        //!only add the CR on the zoomed
        title.replace(" ","\r\n");
        
        //!get the color index .. 0..MAX-1, if outside range, pick 0. Use 'mod' == %  syntax.
        //!This is because the set from the web  (MQTT) doesn't know the size.. or have it done here..
        int colorIndex = getScreenColor_displayModule();
        
        //!different for home screen
        switch (screenType)
        {
            case SCREEN_TYPE_HOME_SIMPLE:
                //!print preferences
                //printPreferenceValues_mainModule();
                //fall through to same as simple_1/2
            case SCREEN_TYPE_HOME_SIMPLE_1:
            case SCREEN_TYPE_HOME_SIMPLE_2:
            case SCREEN_TYPE_AP:

            {
                //Hard wire back to "MINI CLICKER" since the MQTT code used a name to bind to this code, eg. MINI-2
                setupTextPosition(_actionTextHomeSimpleColor[colorIndex]);
                printText(" MINI  CLICKER", _actionTextHomeSimplePositionZoomed.maxLen);
#ifdef M5CORE2_MODULE
                setupTextPosition(_moduleDeviceNameTextPosition);
                //sprintf(infoString,"%s, %.*s",deviceName_mainModule(),getPairedDeviceOrAddress_mainModule() );
                printTextAtTextPosition(deviceName_mainModule(), _moduleDeviceNameTextPosition);
#endif
            }
                break;
            case SCREEN_TYPE_HOME_SIMPLE_3:
            {
                //Hard wire back to "MINI CLICKER" since the MQTT code used a name to bind to this code, eg. MINI-2
                setupTextPosition(_actionTextHomeSimpleColor[colorIndex]);
                printText("CLICKER MENU  ", _actionTextHomeSimplePositionZoomed.maxLen);
            }
                break;
                //rest are the next set of screens.
            case SCREEN_TYPE_HOME:
            {
                // This now comes in as "FEED ALL" .. so change to SMART CLICKER
                setupTextPosition(_actionTextHomeSimpleColor[colorIndex]);
                printText(" SMART CLICKER", _actionTextHomeSimplePositionZoomed.maxLen);            }
                break;
                //same title.. for now..
            case SCREEN_TYPE_PAIRED_DEV:
            {
                // the title == P: so replace with the paired name..
                setupTextPosition(_actionTextHomeSimpleColor[colorIndex]);

                //!use the zoomed amount
                printText("PAIR   DEVICE ", _actionTextPositionZoomed.maxLen);
            }
                break;
            default:
                //!use the zoomed amount
                //setupTextPosition(_actionTextPositionZoomed);
                setupTextPosition(_actionTextHomeSimpleColor[colorIndex]);
                printText(title, _actionTextPositionZoomed.maxLen);
        }
    }
    else
    {
        //don't show the P: but rather the paired device name
        if (screenType == SCREEN_TYPE_PAIRED_DEV)
        {
            // the title == P: so replace with the paired name..
            setupTextPosition(_actionTextPosition);
            //!use the zoomed amount
            printTextPadSpaces("PAIRDEV", _actionTextPosition);

        }
        else
        {
            setupTextPosition(_actionTextPosition);
            //printText(title, _actionTextPosition.maxLen);
            
            printTextPadSpaces(title, _actionTextPosition);

        }
    }
}

//!sets the "Sensor Status"
void drawSensorStatus(int screenType)
{
    char infoString[150];
    boolean multilineScreenType = false;

#ifdef ESP_M5

    //!The "INFO"
    //float batVoltage = M5.Axp.GetBatVoltage();
    float batPercentage = getBatPercentage_mainModule();
    boolean buzzerOn = getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE);
    boolean gatewayOn = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE);
    boolean inMinMenu = getPreferenceBoolean_mainModule(PREFERENCE_IS_MINIMAL_MENU_SETTING);
    boolean isConnectedBLE =
#ifdef USE_BLE_CLIENT_NETWORKING
    isConnectedBLEClient();
#else
    false;
#endif
    
    //!add the Gateway status.. Now, if the preference is only GEN3 and not connected, then show a Gx, but if connected then G3, and g if any kind but not connected
    String gStatus = "g";
    String gStatusHome = "";
    if (isConnectedBLE && gatewayOn)
    {
        gStatus = "G3";
        gStatusHome = "G3";
    }
    else if (getPreferenceBoolean_mainModule(PREFERENCE_ONLY_GEN3_CONNECT_SETTING))
    {
        gStatus = "Gx";
        gStatusHome = "Gx";
    }
    //!Special layout for HELP screen type
    switch (screenType)
    {
 
        case SCREEN_TYPE_HELP:
            multilineScreenType = true;
           // sprintf(infoString,inMinMenu?"Show All\nmenus":"Minimize\nmenus");
           sprintf(infoString,"Scan Image\nFor Help");

            break;
        case SCREEN_TYPE_WIFI:
            multilineScreenType = true;
            if (getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE))
            {
#ifdef USE_MQTT_NETWORKING

                if (isConnectedWIFI_MQTTState())
                    sprintf(infoString,"Swap WIFI");
                else
                    sprintf(infoString,"Swap/Retry\nWIFI");
#endif
            }
            else
            {
                //! #issue 136 show the WIFI in the semantic marker, and label for scanning
                sprintf(infoString,"Scan to\nShare WIFI");
            }
            break;
        case SCREEN_TYPE_AP:
            multilineScreenType = true;
#ifdef USE_WIFI_AP_MODULE
            if (doneWIFI_APModule_Credentials)
                sprintf(infoString,"Enter AP\n192.168.4.1");
            else
#endif
                sprintf(infoString,"In AP\n192.168.4.1");
            break;
        case SCREEN_TYPE_GUEST_PAGE:
            multilineScreenType = true;
            sprintf(infoString,"Share WIFI with Feeder");
            break;
#ifdef NOMORE
        case SCREEN_TYPE_TIMER:  //will be it's own with the time values...
        {
            //printf(infoString,"TIMER...");
            updateMenuState(timerModel);
            ModelStateStruct *model = getModel(timerModel);
            sprintf(infoString,"Seconds=%d", model->delaySeconds);

        }
#endif
            break;
        case SCREEN_TYPE_TIMER:
        case SCREEN_TYPE_DOC_FOLLOW:
        //case SCREEN_TYPE_HOME_SIMPLE:
        case SCREEN_TYPE_HOME_SIMPLE_2:
        case SCREEN_TYPE_HOME_SIMPLE_3:
        case SCREEN_TYPE_PAIRED_DEV:
        case SCREEN_TYPE_REBOOT:
        {
            //!update the model for those drawing Dev and Connected Dev
            updateMenuState(pairedDeviceModel);
            
            multilineScreenType = true;
            //! just ask the deviceState...
            ModelStateStruct *model = getModel(pairedDeviceModel);
            switch (model->pairedDeviceStateEnum)
            {
                    //paired to a device, but BLE NOT connected right now
                case pairedButNotConnectedEnum:
                    sprintf(infoString,"%s\n+%.*s",deviceName_mainModule(),10,getPairedDeviceOrAddress_mainModule() );
                    break;
                    //paired to a device, and BLE connected right now
                case pairedAndConnectedEnum:
                    sprintf(infoString,"%s\n%.*s",deviceName_mainModule(),10,getPairedDeviceOrAddress_mainModule() );
                    break;
                    //not paired (but could be paired as it's a named device])
                case pairableAndConnectedEnum:
                    sprintf(infoString,"%s\n-%.*s",deviceName_mainModule(),9,connectedBLEDeviceName_mainModule());
                    break;
                    //!factory default but not connecte to anything..
                case notConnectedEnum:
                    sprintf(infoString,"%s\n%s",deviceName_mainModule(),(char*)"scanning..");
                    break;
            }
            
        }
            break;

        case SCREEN_TYPE_FEED_GUEST:
            multilineScreenType = true;
            sprintf(infoString,"Feed Guest");
            break;
            
  
        case SCREEN_TYPE_HOME:
        case SCREEN_TYPE_HOME_SIMPLE:
        case SCREEN_TYPE_HOME_SIMPLE_1:

            sprintf(infoString,"Feed=%0d(%0d) Bat=%2.0f%% %s", getFeedCount_mainModule(), feedCountMax_mainModule(), batPercentage,gStatusHome);
            //!could add BUZZER (but it's a distributed issue - it can be sent elsewhere..
            break;
            
        case SCREEN_TYPE_STATUS:
 //       case SCREEN_TYPE_REBOOT:
        default:
            sprintf(infoString,"%s|%2.0f%%|%2.0ff c%0d|%c|%s|t%0d",VERSION_SHORT, batPercentage, getTemperature_mainModule(), getFeedCount_mainModule(), buzzerOn?'B':'b', gStatus,_loopTimer);
            //!could add BUZZER (but it's a distributed issue - it can be sent elsewhere..
            break;
    }
    
    //!setup the position for the SensorStatus .. HELP is always not-zoomed (show semantic marker)
    if (getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE)
        && !(screenType == SCREEN_TYPE_HELP))
    {
        //!use the zoomed amount
        setupTextPosition(multilineScreenType?_multilineStatusTextPositionZoomed:_sensorStatusTextPositionZoomed);
    }
    else
    {
        setupTextPosition(multilineScreenType?_multilineStatusTextPosition:_sensorStatusTextPosition);
    }
    
                          //!draw the text
    printText(infoString,multilineScreenType?_multilineStatusTextPosition.maxLen:_sensorStatusTextPosition.maxLen);
#endif //m5
}
//!draws the text and differeent if ON or OFF
void drawStatusText(String text, boolean isOn)
{
#ifdef ESP_M5
    if (isOn)
        M5.Lcd.setTextColor(BLUE, YELLOW);
    else
        M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.print(text);
#endif
}

//! returns if the modesDesired contains a mode character, but also true if modesDesired in null
boolean containsMode(char *modesDesired, char mode)
{
    if (!modesDesired)
        return true;
    else return index(modesDesired,mode) != NULL;
}

//!sets the "Module Status", will show only the modesDesired if non null
void drawModuleStatus(char *modesDesired)
{
    //!setup the position for the SensorStatus
    if (getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE))
    {
        //!Zoomed .. setup the position for the ModuleStatus
        setupTextPosition(_moduleStatusTextPositionZoomed);
    }
    else
    {
        //!setup the position for the ModuleStatus
        setupTextPosition(_moduleStatusTextPosition);
    }
    // would be nice to get the device name.. of our connected BLE..
    //!value of WIFI connected
#ifdef USE_MQTT_NETWORKING
    if (containsMode(modesDesired,'W'))
        drawStatusText("W", isConnectedWIFI_MQTTState());
    if (containsMode(modesDesired,'M'))
        drawStatusText("M", isConnectedMQTT_MQTTState());
#endif
#ifdef USE_BLE_CLIENT_NETWORKING
    //!useBLECLient == it's linked in and running (vs not running)
    if (containsMode(modesDesired,'B'))
        drawStatusText("B", useBLEClient());
    //! connected == we are connected to another BLEServer
    if (containsMode(modesDesired,'C'))
        drawStatusText("C", isConnectedBLEClient());
#endif
#ifdef USE_WIFI_AP_MODULE
    //!not done is what we look for ..
    if (containsMode(modesDesired,'A'))
        drawStatusText("A", !doneWIFI_APModule_Credentials());
#endif
    if (containsMode(modesDesired,'T'))
        drawStatusText("T", getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE));
    
#ifdef USE_BLE_SERVER_NETWORKING
    if (containsMode(modesDesired,'S'))
    {
        //maybe only show if ON..
        if (getPreferenceBoolean_mainModule(PREFERENCE_MAIN_BLE_SERVER_VALUE))
            drawStatusText("S",true);
    }
#endif
    //Buzzer (but only sometimes)
    if (containsMode(modesDesired,'Z'))
        drawStatusText("Z",getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE));

    //! Reset
    if (containsMode(modesDesired,'R'))
        drawStatusText("R",true);
    //! Reboot
    if (containsMode(modesDesired,'X'))
        drawStatusText("X",true);
    //! Help
    if (containsMode(modesDesired,'H'))
        drawStatusText("H",true);

    //! Paired
    if (containsMode(modesDesired,'P'))
    {
        //if the paired device is NONE then 
        drawStatusText("P",isValidPairedDevice_mainModule());
    }
}

//!just draw the simple status at bottom
void displaySimpleStatus()
{
    //!show the BLE connected status at the bottom (G3 if gen3), WIFI, and AP if APmode
    boolean isConnectedBLE =
#ifdef USE_BLE_CLIENT_NETWORKING
    isConnectedBLEClient();
#else
    false;
#endif
    // whether the gateway is on.. this isn't using the Gx just BLE or GE3
    boolean gatewayOn = getPreferenceBoolean_mainModule(PREFERENCE_MAIN_GATEWAY_VALUE);
    //WIFI means the MQTT is working (not just WIFI)
    boolean isConnectedWIFI =
#ifdef USE_MQTT_NETWORKING
    isConnectedWIFI_MQTTState();
#else
    false;
#endif
    char label[30];
    String gStatus = "   ";  //3 characters
    if (isConnectedBLE && gatewayOn)
    {
        gStatus = "GE3";
    }
    else if (isConnectedBLE)
    {
        gStatus = "BLE";
    }
   
#ifdef USE_WIFI_AP_MODULE
    sprintf(label,"%s %s %s", gStatus, isConnectedWIFI?"WIFI":"    ",!doneWIFI_APModule_Credentials()?"AP":"  ");
    printTextAtTextPosition(label,_connectedStatusTextPositionZoomed);
#endif
}

//!perform a zoom .. of the semantic marker - or it might be something else..
void displayZoomedSemanticMarker(int screenType)
{
    SerialCall.print("displayZoomedSemanticMarker: ");
    SerialCall.println(_semanticTitle);
    // use _currentSemanticMarkerAddress
    
    //!used in the new model versions below..
    ModelKindEnum modelKind;

    
    //2nd line..
    switch (screenType)
    {
        case SCREEN_TYPE_WIFI:
        {
            //show the current WIFI..
            //TODO: pull out the SSID only...
            // {'ssid':<name we want>,'ssidPassword':p}
            //try JSON..  NOT WORKING ...
            //https://linux.die.net/man/3/strstr
            //I'm cheeting, for now .. and grabbing both 1 * 3
            char wifi1[100];
            strcpy(wifi1,getPreferenceString_mainModule(PREFERENCE_WIFI_CREDENTIAL_1_SETTING));
            char wifi2[100];
            strcpy(wifi2,getPreferenceString_mainModule(PREFERENCE_WIFI_CREDENTIAL_2_SETTING));
            char *json = main_JSONStringForWIFICredentials();
            
            SerialTemp.println(wifi1);
            SerialTemp.println(wifi2);
            boolean wifi1Current = strcmp(json,wifi1)==0;
            
            SerialTemp.printf("wifi1Current == %d\n", wifi1Current);
            
            char buffer[100];
            
            char *json1 = strstr(wifi1,":");
            int json1Len = 1;
            if (json1 && strlen(json1) > 3)
            {
                json1 += 2; //move past the ':'"
            }
            else
                json1 = wifi1;
            {
                //find the '
                char *comma = strchr(json1,'\'');
                json1Len = comma?((int)(comma - json1)):strlen(json1);
            }
            SerialTemp.println(json1);
            
            char *json2 = strstr(wifi2,":");
            int json2Len = 1;
            if (json2 && strlen(json2) > 3)
            {
                json2 += 2; //move past the ':'"
            }
            else
                json2 = wifi2;
            {
                //find the '
                char *comma = strchr(json2,'\'');
                json2Len = comma?((int)(comma - json2)):strlen(json2);
            }
            SerialTemp.println(json2);
            
            strcpy(buffer,"+");
            strncat(buffer, wifi1Current?json1:json2, wifi1Current?json1Len:json2Len );
            strcat(buffer,"\n\r");
            printTextAtTextPosition(buffer,_zoomedTextPositions[SM_ACTION_SMALL]);

            strcpy(buffer,"-");
            strncat(buffer, !wifi1Current?json1:json2, !wifi1Current?json1Len:json2Len );
            //make it blue..
            M5.Lcd.setTextColor(WHITE, BLUE);
            M5.Lcd.print(buffer);
            
            SerialCall.println(buffer);
        }
            break;
        
            //!ADD SCREEN_TYPE_HOME
        case SCREEN_TYPE_HOME_SIMPLE:
        {
            //Draw the tilt normal, and the buzz as
            // new 7.25.22 per issue #126
            // This is for the TILT
            //THE BIG ACTION == first, but that color is ORANGE, so wap to the SM_ACTION colors
            if (getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE))
                printTextAtTextPositionOverrideTextColor("TiltOn",_zoomedTextPositions[BIG_ACTION],_zoomedTextPositions[SM_ACTION] );
            else
                printTextAtTextPositionOverrideTextColor("TiltOff",_zoomedTextPositions[BIG_ACTION], _zoomedTextPositions[SM_ACTION]);
            
            //THE BIG ACTION == first, but that color is ORANGE
            if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE))
                printTextAtTextPositionOverrideTextColor("BuzzOn",_zoomedTextPositions[SM_ACTION],_zoomedTextPositions[BIG_ACTION] );
            else
                printTextAtTextPositionOverrideTextColor("BuzzOff",_zoomedTextPositions[SM_ACTION], _zoomedTextPositions[BIG_ACTION]);
            // show the BLE connected status at the bottom
            displaySimpleStatus();
        }
            break;
        case SCREEN_TYPE_HOME_SIMPLE_1:
        {
            // This is for the BUZZ
            // new 7.25.22 per issue #126
            //swap them .. for now .. or TODO: change the zoomText to be swapped colors..
            //DRAFT ...
            //swap colors, passing the _zoomedTextPositions of the opposite
            if (getPreferenceBoolean_mainModule(PREFERENCE_SENSOR_TILT_VALUE))
                printTextAtTextPosition("TiltOn",_zoomedTextPositions[BIG_ACTION]);
            else
                printTextAtTextPosition("TiltOff",_zoomedTextPositions[BIG_ACTION]);
            
            if (getPreferenceBoolean_mainModule(PREFERENCE_STEPPER_BUZZER_VALUE))
                printTextAtTextPosition("BuzzOn",_zoomedTextPositions[SM_ACTION]);
            else
                printTextAtTextPosition("BuzzOff",_zoomedTextPositions[SM_ACTION]);
            // show the BLE connected status at the bottom
            displaySimpleStatus();
        }
            break;
        case SCREEN_TYPE_HOME_SIMPLE_2:
        {
            // new 7.25.22 per issue #126, special feeding page
            // Feeds left:
    
            char tmpString[30];
            //blue as this is an option (for reset counter)
            sprintf(tmpString,"%0d(%0d)",   getFeedCount_mainModule(), feedCountMax_mainModule());
            printTextAtTextPositionOverrideTextColor(tmpString,_zoomedTextPositions[BIG_ACTION], _zoomedTextPositions[SM_ACTION]);
            // battery percent, but orange not selected ..
            sprintf(tmpString,"B:%2.0f%%", getBatPercentage_mainModule());
            printTextAtTextPositionOverrideTextColor(tmpString,_zoomedTextPositions[SM_ACTION],_zoomedTextPositions[BIG_ACTION]);
            // show the BLE connected status at the bottom
            displaySimpleStatus();
        }
            break;

        case SCREEN_TYPE_AP:
        {
            //TODO: change the top simple line .. to a messaging
            // new 7.25.22 per issue #126
            printTextAtTextPositionOverrideTextColor("AP",_zoomedTextPositions[BIG_ACTION], _zoomedTextPositions[SM_ACTION]);
            printTextAtTextPositionOverrideTextColor("Mode...",_zoomedTextPositions[SM_ACTION],_zoomedTextPositions[SM_ACTION]);
            // show the BLE connected status at the bottom
            displaySimpleStatus();
        }
            break;
        case SCREEN_TYPE_TIMER:
        case SCREEN_TYPE_HOME_SIMPLE_3:
        case SCREEN_TYPE_PAIRED_DEV:
        case SCREEN_TYPE_REBOOT:
        {
            
            if (screenType == SCREEN_TYPE_REBOOT)
                modelKind = rebootModel;
            else if (screenType == SCREEN_TYPE_PAIRED_DEV)
                modelKind = pairedDeviceModel;
            else if (screenType == SCREEN_TYPE_HOME_SIMPLE_3)
                modelKind = menusModel;
            else if (screenType == SCREEN_TYPE_TIMER)
                modelKind = timerModel;
            else
                SerialMin.println(" *** INVALID screenType ***");
            
            char buffer[50];
        
            //!upate the model (done at beginning of loop()
            //updateMenuState(modelKind);
            //!grab this model.
            ModelStateStruct *model = getModel(modelKind);
            
            // empty the 4 lines.. with black characters..
            //4 lines max
            
            // blank the area
            M5.Lcd.fillRect(0, 80, 135, 130, BLACK);
            
            // start over..
            strcpy(buffer,"");
            int windowSize = 6;
            int textLength = 10;
            if (model->perfersBigText)
            {
                windowSize = 4;
                textLength = 6;
            }
            
            if (model->maxItems < windowSize)
                windowSize = model->maxItems;
            
            printTextAtTextPosition(buffer, (windowSize <= 4)?_menuTextPositionZoomed_4less:_menuTextPositionZoomed_4plus);

            //SerialDebug.printf("current = %d, max = %d\n",model->currentItem,  model->maxItems);
            //get the menu items to draw..
            //!try a rolling window..
            // current 3   1..8
            // if there maxItems > 6 .. then when hit end .. it scrolls to next one..
            int startItem = 0;
            int maxItems = model->maxItems;
            int currentItem = model->currentItem;
            if (currentItem >= windowSize)
            {
                //  scrolling time..
                // move start the number past maxWindow
                // 0..5,  1..6, 2..7  3..8 (and current always last item on scrolling)
                //say it's 8: current = 8, start = 8 - windowSize
                startItem = currentItem - windowSize + 1;
            }
            maxItems = startItem + windowSize;
            for (int i=startItem; i < maxItems; i++)
            {
                char *menu = menuForState(modelKind, i);
                if (i == model->currentItem)
                {
                    // use blue...
                    M5.Lcd.setTextColor(WHITE, BLUE);
                }
                else
                {
                    M5.Lcd.setTextColor(ORANGE, BLACK);
                }
                
                sprintf(buffer,"-%.*s\n\r", textLength, menu);
                M5.Lcd.print(buffer);
            }
            
            // show the BLE connected status at the bottom
            // in this case, it's not connected so BLE should be off..
            displaySimpleStatus();
        }
            break;

        default:
        {
            // FEED ..
            printTextAtTextPosition("+FEED",_zoomedTextPositions[BIG_ACTION]);
            
            String title = "-" + _semanticTitle;
            title.replace(" ","\r\n");
            
            switch (screenType)
            {
                case SCREEN_TYPE_HOME:
                    printTextAtTextPosition(title,_zoomedTextPositions[SM_ACTION]);
                    break;
                    
                    //wont get here... the case is above..
                case SCREEN_TYPE_PAIRED_DEV:
                    break;
                case SCREEN_TYPE_HELP:
                {
                    // Instead of "Help Info" show the "MINI CLICKER"
                    String title = "-MINI\r\nCLICKER";
                    printTextAtTextPosition(title,_zoomedTextPositions[SM_ACTION]);
                }
                    break;
                case SCREEN_TYPE_GUEST_PAGE:
                {
                    //!This sends the M5's WIFI credentials down to the paired device over BLE
                    // Instead of "Help Info" show the "MINI CLICKER"
                    String title = "-SHARE\r\nWIFI";
                    printTextAtTextPosition(title,_zoomedTextPositions[SM_ACTION]);
                }
                    break;
                default:
                    printTextAtTextPosition(title,_zoomedTextPositions[SM_ACTION]);
            }
        }
            break;
    } //switch
}

//!redraws the image..
void redrawSemanticMarker_displayModule(boolean startNew)
{
    int screenType = screenTypeForTitle_displayModule(_semanticTitle);
        
    SerialCall.printf("redrawSemanticMarker_displayModule(startNew:%d, ScreenType = %d)\n", startNew, screenType);;
    if (startNew)
    {
        //! resets the preferences so the mini menu is running, and resets other preferences
        switch (screenType)
        {
                //!ADD SCREEN_TYPE_HOME
            case SCREEN_TYPE_HOME_SIMPLE:
            {
                //!set some defaults on boot - that override EPROM this can be called on the HOME screen to set back to normal mode..
                setOnBootPreferences_mainModule();
            }
                break;
            case SCREEN_TYPE_HOME_SIMPLE_1:
            case SCREEN_TYPE_HOME_SIMPLE_2:
            case SCREEN_TYPE_HOME_SIMPLE_3:
                //DO THIS HERE TOO.. just check if simple_3 lets things still happen..
                // setOnboot sets min menu and the zoomedSM
            {
                //!set some defaults on boot - that override EPROM this can be called on the HOME screen to set back to normal mode..
                setOnBootPreferences_mainModule();
            }
                break;
        }
        
#define TRY_CLEAR_WITH_RESET_TIMER

#ifdef TRY_CLEAR_WITH_RESET_TIMER
        //! clears the screen, and resets timer
        clearScreen_displayModule();
#else
        //also resets the loop timer
        blankScreen_displayModule();
#endif
        //empty the lastSM
        _lastSemanticMarkerAddress = String("");

    }
    else
    {
        incrementLoopTimer();
    }
    
    
    //!update the model (ModelController.h) for the type of screen displayed (what model)
    if (screenType == SCREEN_TYPE_REBOOT)
        //!upate the model -- each loop, before drawing stuff (obviously)
        updateMenuState(rebootModel);
    else if (screenType == SCREEN_TYPE_PAIRED_DEV)
        updateMenuState(pairedDeviceModel);
    else if (screenType == SCREEN_TYPE_HOME_SIMPLE_3)
        updateMenuState(menusModel);
    else if (screenType == SCREEN_TYPE_TIMER)
        updateMenuState(timerModel);
    
    //!sets flag that being shown..
    _semanticMarkerShown = true;
    
    //!this re-creates the address (maybe dynamically if the func() callback is defined)
    _currentSemanticMarkerAddress = createSemanticAddress();
    
    // ** NOTE the zoom is a way to figure out what else to show..
    if (getPreferenceBoolean_mainModule(PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE)
        && (screenType != SCREEN_TYPE_HELP))
    {
        displayZoomedSemanticMarker(screenType);
    }
    else if (screenType == SCREEN_TYPE_DOC_FOLLOW)
    {
        //! this draws QR but using the lastDocFollowSemanticMarker
#define QRAVATAR
#ifdef ESP_M5
#ifdef M5CORE2_MODULE
        //! DON"T rotate -- for the M5Core2 display
#else
        M5.Lcd.setRotation(0);
#endif
        //!Draws a SemanticMarker&trade; (using QR code)
        //!Create a QR code with a width of 135 QR code  at (0, 100).  在(0,0)处创建一个宽为135二维码
        //!Please select the appropriate QR code version according to the number of characters.  请根据字符数量选择合适的二维码版本
        //try brightness
        setBrightness_displayModule(0);
#ifdef USE_MQTT_NETWORKING
        M5.Lcd.qrcode(getLastDocFollowSemanticMarker_MQTTNetworking(),0,100,135, SEMANTIC_MARKER_QR_VERSION);
#else
        M5.Lcd.qrcode("https://SemanticMarker.org",0,100,135,SEMANTIC_MARKER_QR_VERSION);

#endif
#ifdef QRAVATAR
        //try to draw a character in the middle ...  (D, etc)
        printTextAtTextPosition("SM",_zoomedTextPositions[QRAVATAR_ACTION]);
#endif
#endif //ESP_M5
    }
    else
    {
#ifdef ESP_M5
#ifdef M5CORE2_MODULE
        //! DON"T rotate -- for the M5Core2 display
#else
        M5.Lcd.setRotation(0);
#endif
        SerialLots.print("last vs current: ");
        SerialLots.print(_lastSemanticMarkerAddress);
        SerialLots.println(" vs ");
        SerialLots.println(_currentSemanticMarkerAddress);
        //!only redraw if changed ...
        if (!_lastSemanticMarkerAddress.compareTo(_currentSemanticMarkerAddress)==0)
        {
            SerialLots.println(_currentSemanticMarkerAddress);
            //try brightness
            setBrightness_displayModule(0);

            //!Draws a SemanticMarker&trade; (using QR code)
            //!Create a QR code with a width of 135 QR code  at (0, 100).  在(0,0)处创建一个宽为135二维码
            //!Please select the appropriate QR code version according to the number of characters.  请根据字符数量选择合适的二维码版本
            M5.Lcd.qrcode(_currentSemanticMarkerAddress,0,100,135, SEMANTIC_MARKER_QR_VERSION);
#ifdef QRAVATAR
            //try to draw a character in the middle ...  (D, etc)
            String smChar = _semanticTitle.substring(0,1);
            printTextAtTextPosition(smChar,_zoomedTextPositions[QRAVATAR_ACTION]);
#endif
        }

#endif
        
        _lastSemanticMarkerAddress = _currentSemanticMarkerAddress;
    }
    
    //!The "ACTION"
    drawAction(_semanticTitle, screenType);
    
    //!The "SensorStatus", which uses current info..
    drawSensorStatus(screenType);
    
    //!then which modules to draw for the different screens
    switch (screenType)
    {
        case SCREEN_TYPE_HOME_SIMPLE_3:
            //advanced is now ModelController..
            break;
            //!ADD SCREEN_TYPE_HOME
        case SCREEN_TYPE_HOME_SIMPLE:
        case SCREEN_TYPE_HOME_SIMPLE_1:
        case SCREEN_TYPE_HOME_SIMPLE_2:
        case SCREEN_TYPE_AP:
        {
            // 7.25.22
            //!Draw FEED in red if the feed count is > 13
            if (getFeedCount_mainModule() > 13)
                setupTextPosition(_homeSimpleStatusTextPositionZoomedRED);
            else
                setupTextPosition(_homeSimpleStatusTextPositionZoomed);
            // if count > 13 make it red
            M5.Lcd.print("+FEED ");
                    // 1234567
        }
            break;
            //REST are previous..
            //NOTE: that the HELP long press also goes home..
        case SCREEN_TYPE_HOME:
            //!The " ModuleStats"
            drawModuleStatus((char*)"WMCTZAS");
            break;
        case SCREEN_TYPE_TILT:
            //!The " ModuleStats"
            drawModuleStatus((char*)"T");
            break;
        case SCREEN_TYPE_BUZZ:
            //! module status with BUZZ
            drawModuleStatus((char*)"Z");
            break;
        case SCREEN_TYPE_WIFI:
            drawModuleStatus((char*)"WM");
            break;
//        case SCREEN_TYPE_AP:
//            drawModuleStatus((char*)"WMA");
//            break;
        case SCREEN_TYPE_HELP:
        //    drawModuleStatus((char*)"H");
            // maybe draw text: iDogWatch.com/bot/help
            setupTextPosition(_helpStatusTextPosition);
            
            //!draw the text
            printText((char*)"iDogWatch.com/bot/help",_helpStatusTextPosition.maxLen);
            break;
            
        case SCREEN_TYPE_RESET:
            drawModuleStatus((char*)"R");
            break;
        case SCREEN_TYPE_TIMER:
            break;
        case SCREEN_TYPE_REBOOT:
            break;
        case SCREEN_TYPE_PAIRED_DEV:
            break;

        case SCREEN_TYPE_GUEST_PAGE:
            drawModuleStatus((char*)"WMC");
            break;
        case SCREEN_TYPE_FEED_GUEST:
            drawModuleStatus((char*)"WMC");
            break;
            
        case SCREEN_TYPE_STATUS:
        default:
            //!The " ModuleStats"
            //drawModuleStatus((char*)"WMBCATZ");
            drawModuleStatus((char*)"WMBCATS");
            
    }
    //!startNew is that the screen needs new timeout
    if (startNew)
    {
        //        boolean zoomed = getPreferenceBoolean_mainModule( PREFERENCE_SEMANTIC_MARKER_ZOOMED_VALUE);
        //if the SemanticMarker is show, delay 50
        //!startDelay(zoomed?25: currentScreenTimeout());
        startDelay(currentScreenTimeout());
        
        //! called by the feed operation to say the device is still running.. and count it as a button click.
        //!  Issue #145  8.8.22
        refreshDelayButtonTouched_MainModule();
        
        //!NOTE: this gets called 2 times if buttonB short press .. but not othertimes..
        
    }

}


//! ISSUE: need to re-generate the SemanticAddress but only for "Status".
//! could kluge and use the Title?? == "Status"

//**** Specific Methods*******
//!displays the Semantic Marker (a super QR code) on the M5 screen (with an N second display)
void showSemanticMarker_displayModule(String semanticMarkerAddress, String title)
{
    
    SerialTemp.print("showSemanticMarker(nulls func) ");
    SerialTemp.print(title);
    SerialTemp.print("  SM=");
    SerialTemp.println(semanticMarkerAddress);
    
    //!NOTE: issue #134 shows that the 'buttonProcessing _currentSMMode isn't getting updated .. Which is only an issue if the message is comming in from outside -- versus a button clik..
    //!cache for the redraw()
    _semanticTitle = title;
    //!no function, the addresss is the address provided
    //!NOTE: we could add our own function .. TODO .. less of a kluge..
    _getStatusFunc = NULL;

    _currentSemanticMarkerAddressBase = semanticMarkerAddress;
    //!save the marker..
    _lastSemanticMarkerAddress = String("");
    
   // clearScreen_displayModule();
    
    //!draw a semantic marker (zoomed or not)
    redrawSemanticMarker_displayModule(START_NEW);

    //startDelay(20);
//issue is another display can show up while this is being shown. like FEED , etc
}


//UNUSED SO FAR..

//!finish the address after calling the function provided, called from MQTT for STATUS
void showSemanticMarkerFunc_displayModule(String semanticMarkerAddressBase, String title, const char* (*getStatusFunc)(void))
{
    SerialCall.print("showSemanticMarkerFunc: ");
    SerialCall.print(title);
    SerialCall.print(" SM=");
    SerialCall.println(semanticMarkerAddressBase);
    
    //!save for use on redraw..
    _getStatusFunc = getStatusFunc;
    _currentSemanticMarkerAddressBase = semanticMarkerAddressBase;
     _semanticTitle = title;
    _lastSemanticMarkerAddress = String("");

  //  clearScreen_displayModule();

    redrawSemanticMarker_displayModule(START_NEW);

    //startDelay(20);

}


//!
//! shows a FEED (or whatever) then blanks the screen after N seconds
void showText_displayModule(String text)
{
    drawAction(text, _currentScreenType);
   // startDelay(10);
}





//! adds messages that aren't shown unless in message window mode.
//! NOTE: This will be a scrolling text as sometime ..
void addToTextMessages_displayModule(String text)
{
    SerialLots.printf("addToTextMessages_displayModule:(%d)",displayOnBlankScreen());
    SerialLots.println(text);
    
#ifdef M5CORE2_MODULE
    //! FOR now .. isBlankScreen isn't cared about ..
    //! if showing the scrolling window .. don't do the rest ..
    if (_showScrollingTextWindow)
    {
#ifdef  SHOW_BIG_BUTTON
#else
        //! 1.24.24 Adding scrolling text (if that window on the M5Core2 is shown)
        addToScrollingText_displayModule(text);
#endif
        return;
    }
#endif

    //!whether screen is blank (so the button can be a wake, vs action..)
    if (isBlankScreen_displayModule() and !displayOnBlankScreen())
    {
        SerialLots.println("Not displaying since blank screen");
        return;
    }

    
    //!different for home screen
    switch (_currentScreenType)
    {
            //!ADD SCREEN_TYPE_HOME
        case SCREEN_TYPE_HOME_SIMPLE:
        case SCREEN_TYPE_HOME_SIMPLE_1:
        case SCREEN_TYPE_HOME_SIMPLE_2:
        case SCREEN_TYPE_AP:
            //don't show anything over the SMART CLICKER at the top
            break;
        case SCREEN_TYPE_HOME_SIMPLE_3:
            //TODO.. for now..
            // draw the title but on the 2nd smaller line
            //!use the zoomed amount
            setupTextPosition(_sensorStatusTextPositionZoomed);
            
            //!draw the text
            printText(text,_sensorStatusTextPosition.maxLen);
            break;
        default:
            drawAction(text, _currentScreenType);
            
         
    }
    
    //! testing 4.3.24 for large text
    if (text.compareTo("FEED")==0)
    {
        displayFullscreenMessage(text, true);
    }
}

#ifdef M5CORE2_MODULE
//! add a loop for the alternate display
void loop_Alternate_displayModule()
{
    //no op for now..
}
/**
 First attempt at using the M5Core2 display to show something other than the M5 display
 1. scrolling text
 */
//! 1.24.24 Adding scrolling text (if that window on the M5Core2 is shown)
void addToScrollingText_displayModule(String textString)
{
    if (_showScrollingTextWindow)
    {
        M5.Lcd.setTextSize(1);
        M5.Lcd.println(textString);
        M5.Lcd.println();
        int X = M5.Lcd.getCursorX();
        int Y = M5.Lcd.getCursorY();
        SerialDebug.printf("cursor(%d,%d)\n", X, Y);
        
        if (Y > 213)
            M5.Lcd.setCursor(0,0);
    }
}

//! 1.24.24 Goto the scrolling text mode ..
void toggleShowingScrollingTextMode_displayModule()
{
    boolean flag = !_showScrollingTextWindow;
    setShowingScrollingTextMode_displayModule(flag);
    //! now change perminately ...
    //_showScrollingTextWindow = !_showScrollingTextWindow;

}

//! 2.29.24 Leap Year day
//! Returns whether the Scrolling Text window is shown
boolean scrollingTextWindowVisible_displayModule()
{
    return _showScrollingTextWindow;
}

//! 2.27.24 make the button seem to be touched, if being shown
void showButtonTouched_displayModule()
{
#ifdef  SHOW_BIG_BUTTON
    //! but only if _showScrollingTextWindow
    if (_showScrollingTextWindow)
    {
        SerialDebug.println("showButtonTouched_displayModule");
        delay(200);
        //try a blink..
        M5.Lcd.fillCircle(150,120,118, BLACK);
        M5.Lcd.drawCircle(150,120,118, WHITE);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(5);
        M5.Lcd.setCursor(100, 110);
        M5.Lcd.print("Feed");
        
        delay(500);
        M5.Lcd.fillCircle(150,120,118, GREEN);
        M5.Lcd.drawCircle(150,120,118, WHITE);
        M5.Lcd.setTextColor(BLACK);
        M5.Lcd.setTextSize(5);
        M5.Lcd.setCursor(100, 110);
        M5.Lcd.print("Feed");
    }
#endif
}

//! 1.24.24 Goto the scrolling text mode .. if flag
void setShowingScrollingTextMode_displayModule(boolean flag)
{
    SerialDebug.printf("setShowingScrollingTextMode_displayModule(%d, s=%d)\n", flag,_showScrollingTextWindow );

    //! now change perminately ...
    _showScrollingTextWindow = flag;

    //! only do something if changed state
//    if (flag == _showScrollingTextWindow)
//        return;
    
    if (_showScrollingTextWindow)
    {
        //! stop timers, etc
        //!stop the delay which says delay finished .. so don't show anything..  8.9.22
        clearScreen_displayModule();
        
#ifdef  SHOW_BIG_BUTTON
        // draw big green button.. for the heck of it..
        M5.Lcd.fillCircle(150,120,118, GREEN);
        M5.Lcd.drawCircle(150,120,118, WHITE);
        
        M5.Lcd.setTextColor(BLACK);
        M5.Lcd.setTextSize(5);
        M5.Lcd.setCursor(100, 110);
        M5.Lcd.print("Feed");
        
        //M5.Lcd.setCursor(0,0);
#else
        // blank the area
        //  M5.Lcd.fillRect(0, 60, 135, 140, WHITE);
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println("Messages from the Network");
        //M5.Lcd.setTextWrap(true);
#endif
    }
    else
    {
        //! GO back to a normal screen..
        //M5.Lcd.setTextWrap(false);
        
        //!wakes up the screen
        M5.Lcd.fillScreen(BLACK);
        wakeupScreen_displayModule();
        
    }
}

//! scrolls the displayed text   NOT YET ...
void scrollText_displayModule()
{
    if (_showScrollingTextWindow)
    {
        //!TODO..
        //!for now just clear the text...
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 0);
        //! maybe the text keeps going ... can we ask the cursor location??
    }

}
#endif //M5CORE2_MODULE

#else // not DISPLAY_MODULE

//!Increment the screen color 0..n cache for getting the screen color 0..max (max provided by sender)
//!This is implemented by incrementScreenColor_mainModule() since it knows the MAX value of colors
void incrementScreenColor_displayModule()
{
    
}


//!cache for getting the screen color 0..n. Will reset the cache as well
void setScreenColor_displayModule(int screenColor)
{
    
}

//!THIS IS the setup() and loop() but using the "component" name, eg MQTTNetworking()
//!This will perform preference initializtion as well
//! called from the setup()
void setup_displayModule()
{
    
}

//! called for the loop() of this plugin
void loop_displayModule()
{
    
}

//! shows a FEED (or whatever) then blanks the screen after N seconds
//! NOTE: This will be a scrolling text as sometime ..
void showText_displayModule(String text)
{
    
}

//! adds messages that aren't shown unless in message window mode.
//! NOTE: This will be a scrolling text as sometime ..
void addToTextMessages_displayModule(String text)
{
    
}

//!blanks the screen
void blankScreen_displayModule()
{
    
}

//!wakes up the screen
void wakeupScreen_displayModule()
{
    
}

//!whether screen is blank (so the button can be a wake, vs action..)
boolean isBlankScreen_displayModule()
{
    return true;
}

//!clears the screen
void clearScreen_displayModule()
{
    
}

//!displays the Semantic Marker (a super QR code) on the M5 screen (title = to display)
void showSemanticMarker_displayModule(String semanticMarkerAddress, String title)
{
    
}

//!returns the current semantic marker (eg. guest page)
const char* currentSemanticMarkerAddress_displayModule()
{
    return (char*)"unused";
}

//!the function to call to get the 'status', return char*
//char* (*getStatusFunc)(void)
//typedef String (*getStatusFunc)(void);

//!displays the Semantic Marker (a super QR code) on the M5 screen (title = to display)
void showSemanticMarkerFunc_displayModule(String semanticMarkerAddressBase, String title, const char* (*getStatusFunc)(void))
{
    
}

//!redraws the Semantic Marker image..
void redrawSemanticMarker_displayModule(boolean startNew)
{
    
}

int _loopCounter_displayModule = 0;
//!returns the loop timer (just a timer..)
int getLoopTimer_displayModule()
{
    return _loopCounter_displayModule;
}

//!reset the loop timer .. useful for testing,
void resetLoopTimer_displayModule()
{
    _loopCounter_displayModule = 0;
}
#endif
