/*
 * softPwm.c
 *
 *  Created on: 29 déc. 2012
 *      Author: aurelien
 */

#include "softPwm.h"

unsigned long ulP_Base;
unsigned long ulP_Pin;

unsigned long ulP_ticksForFreq;
unsigned long ulP_ticksHigh;
unsigned long ulP_ticksLow;

void softPwmInit() {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
}

void _update() {

	if (GPIOPinRead(ulP_Base, ulP_Pin)) {
		GPIOPinWrite(ulP_Base, ulP_Pin, 0);

		// stay low while period - (period * duty)
		TimerLoadSet(TIMER0_BASE, TIMER_A, ulP_ticksLow - 1);
	} else {
		GPIOPinWrite(ulP_Base, ulP_Pin, ulP_Pin);

		// stay high for (period * duty)
		TimerLoadSet(TIMER0_BASE, TIMER_A, ulP_ticksHigh - 1);
	}
}

void Timer0IntHandler() {

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	_update();
}

void softPwmConfig(unsigned long ulBase, unsigned long ulPin,
		unsigned long frequency, unsigned long value) {

	// set wild static vars

	ulP_Base = ulBase;
	ulP_Pin = ulPin;

	ulP_ticksForFreq = (SysCtlClockGet() / frequency);
	ulP_ticksHigh = ulP_ticksForFreq * value / 100;
	if (ulP_ticksHigh <= 0)
		ulP_ticksHigh = 1;

	ulP_ticksLow = ulP_ticksForFreq - ulP_ticksHigh;
	if (ulP_ticksLow <= 0)
		ulP_ticksLow = 1;
	_update();
}
