
/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with Atom-Lite/Matrix sample source code
*                          配套  Atom-Lite/Matrix 示例源代码
* Visit for more information: https://docs.m5stack.com/en/atom/atomic_qr
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/atom/atomic_qr
*
* Product:  ATOM QR-CODE.
* Date: 2021/8/30
*******************************************************************************
  Please connect to Port ,Press button to scan, serial monitor will print
information 请连接端口,按下按钮扫描，串行显示器将打印信息
*/

#include "../../Defines.h"
#ifdef ATOM_QRCODE_MODULE

#include "ATOMQRButtons.h"

//#include <M5Atom.h>
//@see https://github.com/m5stack/M5StickC-Plus
//@see https://github.com/m5stack/M5StickC-Plus/blob/master/src/utility/Button.h

//! 3.3.22 Using the new JSON library which is supposed to catch syntax errors without blowing up
//https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

/**
 class Button {
 public:
 Button(uint8_t pin, uint8_t invert, uint32_t dbTime);
 uint8_t read();
 */
/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
/*
 uint8_t isPressed();
 uint8_t isReleased();
 */
/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
/*
 uint8_t wasPressed();
 uint8_t wasReleased();
 */
/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (1) accordingly.     *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/

/*
 uint8_t pressedFor(uint32_t ms);
 uint8_t releasedFor(uint32_t ms);
 uint8_t wasReleasefor(uint32_t ms);
 */
/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
/*
 uint32_t lastChange();
 */
/*
 class Button {
 public:
 Button(uint8_t pin, uint8_t invert, uint32_t dbTime);
 uint8_t read();
 uint8_t isPressed();
 uint8_t isReleased();
 uint8_t wasPressed();
 uint8_t wasReleased();
 uint8_t pressedFor(uint32_t ms);
 uint8_t releasedFor(uint32_t ms);
 uint8_t wasReleasefor(uint32_t ms);
 uint32_t lastChange();
 */

#ifdef ESP_M5
#include <M5StickCPlus.h>
#endif

//#define DO_BUTTONS
#ifdef DO_BUTTONS
//!big button on front of M5StickC Plus
void checkButtonA_ButtonProcessing()
{
    boolean buttonTouched = true;
#ifdef ESP_M5
    //was 1000
    if (M5.BtnA.wasReleasefor(500))
    {
//        buttonA_LongPress();
        SerialDebug.println(" **** LONG PRESS ***");
    }
    else if (M5.BtnA.wasReleased())
    {
//        buttonA_ShortPress();
        SerialDebug.println(" **** SHORT PRESS ***");
    }
    else
    {
        buttonTouched = false;
    }
    //if a button was touched, update the delay since no touch..
    if (buttonTouched)
    {
//        refreshDelayButtonTouched();
    }
    
    
    
#endif //ESP_M5
}


#define TRIG 23
#define DLED 33

void setup_ATOMQRButtons() {
    M5.begin(false, false, true);
//    Serial.begin(9600);
    Serial2.begin(
        9600, SERIAL_8N1, 22,
        19);  // Set the baud rate of serial port 2 to 9600,8 data bits, no
              // parity bits, and 1 stop bit, and set RX to 22 and TX to 19.
              // 设置串口二的波特率为9600,8位数据位,没有校验位,1位停止位,并设置RX为22,TX为19
  //  M5.dis.fillpix(0xfff000);  // YELLOW 黄色
    pinMode(TRIG, OUTPUT);
    pinMode(DLED, INPUT);
    digitalWrite(TRIG, HIGH);
}

void loop_ATOMQRButtons() {
    M5.update();
    
    checkButtonA_ButtonProcessing();
#ifdef LATER
    if (M5.BtnA.isPressed()) {
        SerialDebug.println("5.BtnA.isPressed");
        digitalWrite(TRIG, LOW);
    } else {
        digitalWrite(TRIG, HIGH);
    }
#endif
    if (digitalRead(DLED) == HIGH) {  // If read the QR code.  如果读取到二维码
        SerialDebug.println("READ QR");
        
      //  M5.dis.fillpix(0x00ff00);  // GREEN  绿色
        while (Serial2.available() > 0) {
            char ch = Serial2.read();
            Serial.print(ch);
        }
    } else {
      //  M5.dis.fillpix(0xfff000);
    }
}

#endif
#endif
