//#ifndef _M5DISPLAY_H_
//#define _M5DISPLAY_H_

//! CRGB is RGB
//! https://github.com/FastLED/FastLED/wiki/Pixel-reference
//! https://github.com/FastLED/FastLED/wiki/RGBSet-Reference
//! https://gist.github.com/chemdoc77?page=3
//! https://github.com/marmilicious/FastLED_examples/blob/master/blink_variations.ino

//! @see https://community.m5stack.com/topic/5550/atom-lite-with-different-led-controllers-for-internal-led/7



#include <FastLED.h>

//! the setup()   no loop()
void setup_M5Display();

void animation(uint8_t *buffptr, uint8_t amspeed, uint8_t ammode, int64_t amcount = -1);
void displaybuff(uint8_t *buffptr, int8_t offsetx = 0, int8_t offsety = 0);
void setBrightness(uint8_t brightness);
void drawpix(uint8_t xpos, uint8_t ypos, CRGB Color);
void drawpix(uint8_t Number, CRGB Color);
void clear();
//! color the button light
void fillpix(CRGB Color);


//! colors
#define L_RED    (CRGB) 0xff0000
#define L_GREEN  (CRGB) 0x00ff00
#define L_BLUE   (CRGB) 0x0000ff
#define L_WHITE  (CRGB) 0xe0e0e0
#define L_YELLOW (CRGB) 0xfff000


//#endif //_M5DISPLAY_H_
