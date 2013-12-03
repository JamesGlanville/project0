/**
 *
 */

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

/**
 * Initialize default timer
 * TIMER0_BASE, TIMER_A
 */
extern void softPwmInit();

extern void Timer0IntHandler();

/**
 *
 */
extern void softPwmConfig(unsigned long ulBase, unsigned long ulPin, unsigned long frequency, unsigned long value);
