#include "../../Defines.h"

#ifdef USE_FAST_LED

#include "M5Display.h"
#include "LED_DisPlay.h"
//! the LED DIsplay Object
LED_DisPlay _dis;
boolean _M5DisplaySetup = false;

void setup_M5Display()
{
    SerialDebug.println("setup_M5Display");
  _dis.begin();
  _dis.setTaskName("LEDs");
  _dis.setTaskPriority(2);
  _dis.setCore(1);
  _dis.start();
   
  _M5DisplaySetup = true;
    
}

char *colorName(CRGB Color)
{
    if (Color == L_RED)
        return (char*)"L_RED";
    else if (Color == L_GREEN)
        return (char*)"L_GREEN";
    else if (Color == L_BLUE)
        return (char*)"L_BLUE";
    else if (Color == L_WHITE)
        return (char*)"L_WHITE";
    else if (Color == L_YELLOW)
        return (char*)"L_YELLOW";
    else
        return (char*)"UNKNOWN";
}

void animation(uint8_t *buffptr, uint8_t amspeed, uint8_t ammode, int64_t amcount )
{
    _dis.animation(buffptr, amspeed, ammode, amcount);
}
void displaybuff(uint8_t *buffptr, int8_t offsetx, int8_t offsety )
{
    _dis.displaybuff(buffptr, offsetx, offsety);
}
void setBrightness(uint8_t brightness)
{
    _dis.setBrightness(brightness);
}
void drawpix(uint8_t xpos, uint8_t ypos, CRGB Color)
{
    _dis.drawpix(xpos, ypos, Color);
}
void drawpix(uint8_t Number, CRGB Color)
{
    SerialDebug.printf("drawpix(%s)\n", colorName(Color));
    _dis.drawpix(Number, Color);
}
void clear()
{
    if (_M5DisplaySetup)
        _dis.clear();
}

void fillpix(CRGB Color)
{
    SerialDebug.printf("atom.fillPix %s\n", colorName(Color));
    if (_M5DisplaySetup)
        _dis.fillpix(Color);
    else
        SerialDebug.printf("***atom.fillPix NOT Setup yet");

}

long _randomColorIndex = 0;
#define MAX_COLORS 5
//! 7.24.25 return a (semi) random color
CRGB getRandomColor()
{
    _randomColorIndex = (++_randomColorIndex) % MAX_COLORS;
    switch (_randomColorIndex)
    {
        case 0: return L_RED;
        case 1: return L_GREEN;
        case 2: return L_BLUE;
        case 3: return L_WHITE;
            default:
        case 4: return L_YELLOW;
            
    }
}

#endif // USE_FAST_LED
