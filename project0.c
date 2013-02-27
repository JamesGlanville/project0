#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "inc/lm4f120h5qr.h"

#define DIN GPIO_PIN_1 //these are all on port F.
#define CLK GPIO_PIN_2
#define LOAD GPIO_PIN_3

//#define BOOTSTRAP_DUTY_CYCLE=12

//#define SMPS GPIO_PIN_4
#define SECONDPULSE GPIO_PIN_6 //B
#define SMPS_PWM GPIO_PIN_2 // on port B.
//#define SMPS_FEEDBACK GPIO_PIN_?

#define dutyCycle 40

#include "inc/hw_timer.h"

#include <stdint.h>


#include "softPwm.h"
#define DELAY 2000000



void send_VFD(int digit, int number);
void rest();

unsigned long grid[6]={1<<0,1<<3,1<<9,1<<2,1<<13,1<<1};				//0'th digit, 1st digit... etc
unsigned long segments[10]={1<<11|1<<17|1<<6|1<<4|1<<19|1<<10,		//0
							1<<4|1<<10,								//1
							1<<11|1<<4|1<<6|1<<4|1<<7|1<<19,		//2
							1<<11|1<<4|1<<6|1<<4|1<<7|1<<10,		//3
							1<<17|1<<4|1<<7|1<<10,					//4
							1<<11|1<<17|1<<6|1<<7|1<<10,			//5
							1<<11|1<<17|1<<7|1<<19|1<<10,			//6
							1<<4|1<<6|1<<10,						//7
							1<<11|1<<17|1<<4|1<<6|1<<7|1<<19|1<<10,	//8
							1<<4|1<<6|1<<7|1<<10|1<<17};			//9

volatile int hour;
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
	GPIOPinIntClear(GPIO_PORTC_BASE,SECONDPULSE);

	//aww shit.
	second++;
	if (second	== 60){second	= 0; minute++;	}
	if (minute	== 60){minute	= 0; hour++;	}
	if (hour	== 24){hour		= 0;			}
	numbers[0]=hour/10;
	numbers[1]=hour%10;
	numbers[2]=minute/10;
	numbers[3]=minute%10;
	numbers[4]=second/10;
	numbers[5]=second%10;
}

void send_VFD(int digit, int number)
{
	long data=0;
	int i;
	data |= grid[digit];
	data |= segments[number];

rest();
	GPIO_PORTF_DATA_R &= ~LOAD;
	rest();
	GPIO_PORTF_DATA_R =0;
	rest();
	for (i = 0; i<20;i++)
	{
		GPIO_PORTF_DATA_R &= ~CLK;
		rest();
		if (data & (1 << i)) {		GPIO_PORTF_DATA_R |= DIN; }
		else {GPIO_PORTF_DATA_R &= ~DIN;}
		rest();
		GPIO_PORTF_DATA_R |= CLK;
		rest();
	}
	rest();
	rest();
	rest();
	rest();
	GPIO_PORTF_DATA_R |=LOAD;
	//SysCtlDelay(10000000);
}

void rest()
{
    SysCtlDelay(20);

}
/*
void Timer0IntHandler()
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	if(GPIOPinRead(GPIO_PORTF_BASE,SMPS_PWM))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, SMPS_PWM, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, SMPS_PWM, SMPS_PWM);
	}
}*/

int
main(void)
{
	 //  uint16_t pulse;


    // Setup the system clock to run at 50 Mhz from PLL with crystal reference
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
                    SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, SMPS_PWM);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,SECONDPULSE);
	//GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_7);
	GPIOPinIntClear(GPIO_PORTC_BASE,0xFF);
	GPIOPortIntRegister(GPIO_PORTC_BASE, &NewSecond);
	GPIOPadConfigSet(GPIO_PORTC_BASE, SECONDPULSE, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	//GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(GPIO_PORTC_BASE, SECONDPULSE, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTC_BASE,SECONDPULSE);
	IntEnable(INT_GPIOC);
    IntMasterEnable();





	// clear any existing state
	GPIOPinWrite(GPIO_PORTB_BASE, SMPS_PWM, 0);

	softPwmInit();

	unsigned long base_freq = 6000; // 500Hz

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, DIN | LOAD | CLK);
/*
    GPIO_PORTF_DIR_R = DIN | LOAD | CLK;// | SMPS_PWM;
    GPIO_PORTF_DEN_R = DIN | LOAD | CLK;// | SMPS_PWM;
    GPIO_PORTF_DATA_R =  0 ;*/

  //  config_timer(1,200);


    getDS1307time();
    softPwmConfig(GPIO_PORTB_BASE, SMPS_PWM, base_freq, dutyCycle); // This should work, just tweak dutyCycle to keep a steady voltage (or hard-code?)
    //long laststate = GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_7)&(1<<7);
    while(1)
    {

    	  //set_pwm(0,pulse);
    	    //  set_pwm(1,(pulse+100));
    	    //  set_pwm(2,(pulse+200));
    	     // set_pwm(3,(pulse+300));
    	     // set_pwm(4,(pulse+400));
    	     // set_pwm(5,(pulse+500));
    	     // set_pwm(6,(pulse+600));
    	     // set_pwm(7,(pulse+700));

    //	      if (pulse<4000) pulse++; else pulse=0;
    	      //SysCtlDelay(5000);
    	update_VFD();
    //	if (laststate != GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_7)&(1<<7)){NewSecond();}
    }
}
