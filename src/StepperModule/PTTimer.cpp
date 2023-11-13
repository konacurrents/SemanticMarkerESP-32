/*
 Name:		esp32TIMER.ino
 Created:	9/4/2021 6:55:06 PM
 Author:	wes
*/


/* Copyright (c) 2017 pcbreflux. All Rights Reserved.
 *  https://www.youtube.com/watch?v=LONGI_JcwEQ&ab_channel=pcbreflux
 *  https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_multitimer
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. *
 *
 */
 //#include "PTTimer.h"
#if 0 //build errors   ex Severity	Code	Description	Project	File	Line	Suppression State
//Error		33:1 : error : 'hw_timer_t' does not name a type		C : \Projects\NimBLE_repo\NimBLE_PetTutor_Server\PTTimer.cpp	33

#define PRESCALE0 80
#define PRESCALE1 80
#define COUNT_UP true
#define COUNT_DOWN false
#define	EDGE_TRIGGERED true
#define	LEVEL_TRIGGERED false

hw_timer_t* timer0 = NULL;
//hw_timer_t* timer1 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
//portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

//volatile uint8_t led1stat = 0; 
//volatile uint8_t led2stat = 0; 

/* Global Countdown Timers */
unsigned int u16CountDown0 = 0; // use for the x second timer on startup to update the device#
unsigned int u16CountDown1 = 0; // use for timing the 5 seconds after a manual feed button press on the feeder to lower and then raise the power
unsigned int u16CountDown2 = 0; // this is used to alert the program when flash is being updated
unsigned int u16CountDown3 = 0;
unsigned int u16CountDown4 = 0;

void TickService0(void)
{
	if (u16CountDown0) u16CountDown0--;
	if (u16CountDown1) u16CountDown1--;
	if (u16CountDown2) u16CountDown2--;
	if (u16CountDown3) u16CountDown3--;
	if (u16CountDown4) u16CountDown4--;
}

void IRAM_ATTR onTimer0() {
	portENTER_CRITICAL_ISR(&timerMux0);
	TickService0();
	portEXIT_CRITICAL_ISR(&timerMux0);
}


void PTTimerSetup() {
	timer0 = timerBegin(0, PRESCALE0, COUNT_UP);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
	timerAttachInterrupt(timer0, &onTimer0, EDGE_TRIGGERED); // edge (not level) triggered 
	timerAlarmWrite(timer0, 1000, true); // 1000 * 1 us = 1 ms, autoreload true

	// at least enable the timer alarms
	timerAlarmEnable(timer0); // enable
	u16CountDown0 = 0;
	u16CountDown1 = 0;
	u16CountDown2 = 0;
	u16CountDown3 = 0;
	u16CountDown4 = 0;
}

void test() {
	// nope nothing here
	if (!u16CountDown2) {
//		SerialDebug.print(u16CountDown2);
//		SerialDebug.println(String("  ...onTimer0() ") + String(millis()));
		if (millis() > 10000) {
			u16CountDown2 = 1000;
		}
		else {
			u16CountDown2 = 200;
		}

	}
}

#endif
