#include "../../Defines.h"

#ifdef USE_FAST_LED

#include "M5Display.h"
#include "LED_DisPlay.h"
//! the LED DIsplay Object
LED_DisPlay _dis;
boolean _M5DisplaySetup = false;

void setup_M5Display()
{
  _dis.begin();
  _dis.setTaskName("LEDs");
  _dis.setTaskPriority(2);
  _dis.setCore(1);
  _dis.start();
   
  _M5DisplaySetup = true;
    
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
    _dis.drawpix(Number, Color);
}
void clear()
{
    if (_M5DisplaySetup)
        _dis.clear();
}

void fillpix(CRGB Color)
{
    if (_M5DisplaySetup)
        _dis.fillpix(Color);
}


#endif // USE_FAST_LED
