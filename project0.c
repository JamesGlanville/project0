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
#include "softPwm.h" //From here: https://github.com/arcadien/pwm_test_stellpad1/tree/master/lab_project
#include "I2C_Stellaris_API.h" //From here: http://e2e.ti.com/support/microcontrollers/stellaris_arm/f/471/t/235926.aspx

#define DIN GPIO_PIN_1	//These are all on port F.
#define CLK GPIO_PIN_2	//They correspond to the 3 control wires going to the MAX6921.
#define LOAD GPIO_PIN_3

#define SECONDPULSE GPIO_PIN_6	//Port C. SECONDPULSE is connected to the 1Hz SQW output of the ds1307
#define SMPS_PWM GPIO_PIN_2		//Port B. This pin is connected to the SMPS mosfet gate for switching.

#define dutyCycle 40	//This is trial and error open-loop control - hacky but works well.
#define DS1307ADDRESS 0x68 //The i2c slave address of the DS1307
#define HAD_LENGTH	19

#define H1 0 //The time to set, with the time being H1,H2:M1,M2:S1,S2 so this time is 23:02:00
#define H2 1
#define M1 0
#define M2 8
#define S1 0
#define S2 0
//#define SETTIME //Comment this line out when you have set the time once.

//   Time-setting procedure:
// - Set the correct values in H1,H2....,S2 for a time a couple of minutes in the future.
// - Uncomment SETTIME and compile.
// - Upload the compiled program to the launchpad.
// - Press reset when the current time is what you set it to.
// - WITHOUT REMOVING POWER, comment out the SETTIME define, recompile, and upload.
// - The clock should now keep time on its own, just repeat this process when the clocks change.

void send_VFD(int digit, int number);
void rest();
void update_VFD();

unsigned long grid[6]={1<<0,1<<3,1<<9,1<<2,1<<13,1<<1};				//0'th digit, 1st digit... etc
unsigned long segments[10]={1<<11|1<<17|1<<6|1<<4|1<<19|1<<10,		//0		There might be a nicer way to do this, this works.
							1<<4|1<<10,								//1		This depends on how you have wired up your VFD
							1<<11|1<<4|1<<6|1<<4|1<<7|1<<19,		//2
							1<<11|1<<4|1<<6|1<<4|1<<7|1<<10,		//3
							1<<17|1<<4|1<<7|1<<10,					//4
							1<<11|1<<17|1<<6|1<<7|1<<10,			//5
							1<<11|1<<17|1<<7|1<<19|1<<10,			//6
							1<<4|1<<6|1<<10,						//7
							1<<11|1<<17|1<<4|1<<6|1<<7|1<<19|1<<10,	//8
							1<<4|1<<6|1<<7|1<<10|1<<17};			//9

unsigned long segments_hackaday[HAD_LENGTH]={	1<<4|1<<10|1<<17|1<<19|1<<7, //H
										1<<17|1<<19|1<<7|1<<11, //t
										1<<17|1<<19|1<<7|1<<11, //t
										1<<17|1<<19|1<<6|1<<7|1<<4, //P
										1<<6|1<<11, //:
										1<<19|1<<7|1<<4, // /
										1<<19|1<<7|1<<4, // /
										1<<4|1<<10|1<<17|1<<19|1<<7, //H
										1<<17|1<<4|1<<6|1<<7|1<<19|1<<10, //A
										1<<6|1<<17|1<<19|1<<11, //C
										1<<17|1<<6|1<<7|1<<19|1<<10, //k
										1<<17|1<<4|1<<6|1<<7|1<<19|1<<10, //A
										1<<7|1<<19|1<<11|1<<10|1<<4, //d
										1<<17|1<<4|1<<6|1<<7|1<<19|1<<10, //A
										1<<11|1<<17|1<<4|1<<7|1<<10, //y
										1<<11, //_
										1<<6|1<<17|1<<19|1<<11, //C
										1<<11|1<<17|1<<6|1<<4|1<<19|1<<10, //O
										1<<11|1<<17|1<<6|1<<7|1<<19 //M sideways
};

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

void update_HAD()
{
	int i;
	int j;
	while(1)
	{
		for (i=0;i<=13;i++)
		{
			for(j=0;j<6000;j++)
			{
				send_VFD(j%6,10+i+j%6);
			}
		}
		for (i=13;i>=0;i--)
		{
			for(j=0;j<6000;j++)
			{
				send_VFD(j%6,10+i+j%6);
			}
		}
	}
}

void getDS1307time()
{
	unsigned long temp;
	I2CSetup(I2C2_MASTER_BASE, false); //Port E: pin 4 is SCL, pin 5 is SDA. The false value sets the speed to 100kHz (I think)
#ifdef SETTIME
	SysCtlDelay(1000);
	I2CRegWrite(I2C2_MASTER_BASE, DS1307ADDRESS, 0, (S1<<4)|S2); //set seconds - push the first digit left 4 bits into bits 6,5,4 (see datasheet), then OR with S2 which is already in the right place bit-wise.
	I2CRegWrite(I2C2_MASTER_BASE, DS1307ADDRESS, 1, (M1<<4)|M2); //set minutes
	I2CRegWrite(I2C2_MASTER_BASE, DS1307ADDRESS, 2, (H1<<4)|H2); //set 24 hour bit and hours
	//Don't bother setting days/months/years - should be easy to work out what to do from the datasheet.
	I2CRegWrite(I2C2_MASTER_BASE, DS1307ADDRESS, 7, 0x90); //sets up SQW output at 1Hz 0x90 = 10010000. The first bit (bit 7) is actually unneeded. The 4th (bit 4) turns on the SQW output.
#endif
	temp = I2CRegRead(I2C2_MASTER_BASE, DS1307ADDRESS, 0);
	second = (temp&0x0F)+10*((temp>>4)&0x07); //Messy bit manipulations to extract the seconds. 0x0F = 00001111, so OR'ing the temp value with it gives you only the last four bits.
	temp = I2CRegRead(I2C2_MASTER_BASE, DS1307ADDRESS, 1);
	minute = (temp&0x0F)+10*((temp>>4)&0x07);
	temp = I2CRegRead(I2C2_MASTER_BASE, DS1307ADDRESS, 2);
	hour = (temp&0x0F)+10*((temp>>4)&0x03);

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
	if (second	== 60){second	= 0; minute++;	} //Simple code to get the time to work correctly.
	if (minute	== 60){minute	= 0; hour++;	}
	if (hour	== 24){hour		= 0;			}
	numbers[0]=hour/10; //numbers[] is an integer array, and the calculations are always rounded down so this works.
	numbers[1]=hour%10;
	numbers[2]=minute/10;
	numbers[3]=minute%10;
	numbers[4]=second/10;
	numbers[5]=second%10;
}

void send_VFD(int digit, int number) //numbers 10->29 are http://hackaday.com
{
	long data=0; //This will become the blob of data that is pushed to the MAX6921
	int i;
	data |= grid[digit];		//the grid bit is OR'ed to add it to data.
	if(number<10)
	{
		data |= segments[number];	//Likewise with the segments.
	}
	else
	{
		data |= segments_hackaday[number-10];
	}
	rest(); //pause for a bit
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
	rest(); //Not sure why I need more than 1 here.
	rest();
	rest();
	rest();
	GPIO_PORTF_DATA_R |=LOAD; //LOAD goes high.
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

    getDS1307time(); //Grab the current time, before interrupts start.

	GPIOPinIntClear(GPIO_PORTC_BASE,0xFF); //Clear all interrupts from port C.
	GPIOPortIntRegister(GPIO_PORTC_BASE, &NewSecond); //Registers NewSecond() as the interrupt handler.
	GPIOPadConfigSet(GPIO_PORTC_BASE, SECONDPULSE, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //Enables a weak (2mA) pullup on the pin. This is needed as the SQW output is open-drain.
	GPIOIntTypeSet(GPIO_PORTC_BASE, SECONDPULSE, GPIO_FALLING_EDGE); //Sets the interrupt to trigger on a falling edge.
	GPIOPinIntEnable(GPIO_PORTC_BASE,SECONDPULSE); //Enables the interrupt.
	IntEnable(INT_GPIOC); //Enables the port interrupt.
    IntMasterEnable(); //Enables all interrupts.

	// clear any existing state
	GPIOPinWrite(GPIO_PORTB_BASE, SMPS_PWM, 0); //Don't think this matters much since it's about to start PWM'ing.

	softPwmInit(); //Sets up the pwm (see softPwm.c)

	unsigned long base_freq = 6000; // 6kHz pwm. I have no idea why, but higher frequencies resulted in a lower output voltage, which makes no sense.

    softPwmConfig(GPIO_PORTB_BASE, SMPS_PWM, base_freq, dutyCycle); //Starts the SMPS PWM.

    if(second==37)
    {
    	update_HAD();
    }

    while(1)
    {
    	update_VFD(); //Keep updating the VFD. interrupts will manage the changing time.
    }
}
