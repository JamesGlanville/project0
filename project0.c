#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "inc/lm4f120h5qr.h"
#include <stdint.h>
#include "softPwm.h"

#define DIN GPIO_PIN_1	//These are all on port F.
#define CLK GPIO_PIN_2	//They correspond to the 3 control wires going to the MAX6921.
#define LOAD GPIO_PIN_3

#define SECONDPULSE GPIO_PIN_6	//These are all on port B. SECONDPULSE is connected to the 1Hz SQW output of the ds1307
#define SMPS_PWM GPIO_PIN_2		//This pin is connected to the SMPS mosfet gate for switching.

#define dutyCycle 40	//This is trial and error open-loop control - hacky but works well.

void send_VFD(int digit, int number);
void rest();
void update_VFD();

unsigned long grid[6]={1<<0,1<<3,1<<9,1<<2,1<<13,1<<1};				//0'th digit, 1st digit... etc
unsigned long segments[10]={1<<11|1<<17|1<<6|1<<4|1<<19|1<<10,		//0		There might be a nicer way to do this, this works.
							1<<4|1<<10,								//1
							1<<11|1<<4|1<<6|1<<4|1<<7|1<<19,		//2
							1<<11|1<<4|1<<6|1<<4|1<<7|1<<10,		//3
							1<<17|1<<4|1<<7|1<<10,					//4
							1<<11|1<<17|1<<6|1<<7|1<<10,			//5
							1<<11|1<<17|1<<7|1<<19|1<<10,			//6
							1<<4|1<<6|1<<10,						//7
							1<<11|1<<17|1<<4|1<<6|1<<7|1<<19|1<<10,	//8
							1<<4|1<<6|1<<7|1<<10|1<<17};			//9

volatile int hour; //These must all be marked volatile as the NewSecond interrupt will use them.
volatile int minute;
volatile int second;

volatile int numbers[6];

void update_VFD()
{
	int i;
	for (i = 0; i < 6; i ++)
	{
		send_VFD(i, numbers[i]);
	}

}

void getDS1307time()
{
	/*
	blah do i2c shit;
	*/
	hour = 18;
	minute = 36;
	second=21;

	numbers[0]=hour/10;
	numbers[1]=hour%10;
	numbers[2]=minute/10;
	numbers[3]=minute%10;
	numbers[4]=second/10;
	numbers[5]=second%10;
}

void NewSecond()
{
	GPIOPinIntClear(GPIO_PORTC_BASE,SECONDPULSE); //Clears interrupt.

	second++;
	if (second	== 60){second	= 0; minute++;	}
	if (minute	== 60){minute	= 0; hour++;	}
	if (hour	== 24){hour		= 0;			}
	numbers[0]=hour/10; //numbers[] is an integer array, and the calculations are always rounded down so this works.
	numbers[1]=hour%10;
	numbers[2]=minute/10;
	numbers[3]=minute%10;
	numbers[4]=second/10;
	numbers[5]=second%10;
}

void send_VFD(int digit, int number)
{
	long data=0; //This will become the blob of data that is pushed to the MAXX6921
	int i;
	data |= grid[digit];		//the grid bit is OR'ed to add it to data.
	data |= segments[number];	//Likewise with the segments.

	rest();
	GPIO_PORTF_DATA_R &= ~LOAD; //Pulls LOAD low. This should really use a higher level function for neatness.
	rest();
	GPIO_PORTF_DATA_R =0; //Pulls the whole of port F low (why?!?)
	rest();
	for (i = 0; i<20;i++)
	{
		GPIO_PORTF_DATA_R &= ~CLK; //Pulls CLK low.
		rest();
		if (data & (1 << i)) {		GPIO_PORTF_DATA_R |= DIN; } //If needed, DIN goes HIGH.
		else {GPIO_PORTF_DATA_R &= ~DIN;}						//Otherwise, DIN goes LOW.
		rest();
		GPIO_PORTF_DATA_R |= CLK; //Pulls CLK high.
		rest();
	}
	rest();
	rest();
	rest();
	rest();
	GPIO_PORTF_DATA_R |=LOAD; //LOAD goes high.
	//SysCtlDelay(10000000);
}

void rest()
{
    SysCtlDelay(20); //This is the delay to give the MAX6921 time to react to data. It may not be needed.

}

int
main(void)
{
    // Setup the system clock to run at 50 Mhz from PLL with crystal reference
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
                    SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Enables the GPIO ports needed.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, SMPS_PWM); //Sets the various pins to be inputs or outputs.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, DIN | LOAD | CLK);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,SECONDPULSE);

	GPIOPinIntClear(GPIO_PORTC_BASE,0xFF); //Clear all interrupts from port C.
	GPIOPortIntRegister(GPIO_PORTC_BASE, &NewSecond); //Registers NewSecond() as the interrupt handler.
	GPIOPadConfigSet(GPIO_PORTC_BASE, SECONDPULSE, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //Enables a weak (2mA) pullup on the pin.
	GPIOIntTypeSet(GPIO_PORTC_BASE, SECONDPULSE, GPIO_FALLING_EDGE); //Sets the interrupt to trigger on a falling edge.
	GPIOPinIntEnable(GPIO_PORTC_BASE,SECONDPULSE); //Enables the interrupt.
	IntEnable(INT_GPIOC); //Enables the port interrupt.
    IntMasterEnable(); //Enables all interrupts.

	// clear any existing state
	GPIOPinWrite(GPIO_PORTB_BASE, SMPS_PWM, 0);

	softPwmInit(); //Sets up the pwm (see softPwm.c)

	unsigned long base_freq = 6000; // 6kHz pwm. I have no idea why, but higher frequencies resulted in a lower output voltage, which makes no sense.

    getDS1307time(); //Grab the current time
    softPwmConfig(GPIO_PORTB_BASE, SMPS_PWM, base_freq, dutyCycle); //Starts the SMPS PWM.

    while(1)
    {
    	update_VFD(); //Keep updating the VFD. interrupts will manage the changing time.
    }
}
