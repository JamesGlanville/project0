//*****************************************************************************
//
// I2C_Stellaris_API.c - Stellaris I2C Master Driver.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHOR SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// AUTHOR: JOERG QUINTEN
// E2E-NICKNAME: aBUGSworstnightmare
//
//*****************************************************************************

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/i2c.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "I2C_Stellaris_API.h"

//*****************************************************************************
//
//! \internal
//! Checks a I2C master base address.
//!
//! \param ulI2CBase is the base address of the I2C Master module.
//!
//! This function determines if a I2C master module base address is valid.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef DEBUG
static tBoolean
I2CMasterBaseValid(unsigned long ulI2CBase)
{
	return(	(ulI2CBase == I2C0_MASTER_BASE) || (ulI2CBase == I2C1_MASTER_BASE) ||
			(ulI2CBase == I2C2_MASTER_BASE) || (ulI2CBase == I2C3_MASTER_BASE));
}
#endif

//*****************************************************************************
//
//! Initializes and enables the specified I2C block.
//!
//! \param ulI2CBase is the I2C peripheral to be used.
//! \param ulI2CSpeed defines the normal (100kbps) or fast (400kbps) I2C mode.
//!
//! This function enables the specified I2C block and sets it up to run at
//! the either 100kbps or 400kbps.  If the \e ulI2CSpeed is false, the I2C will
//! run at 100kbps and if true, then the I2C will run at 400kbps.  The
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return None.
//
//*****************************************************************************
void I2CSetup(unsigned long ulI2CBase, unsigned long ulI2CSpeed)
{
    //
    // Check the arguments.
    //
	ASSERT(I2CMasterBaseValid(ulI2CBase));
    ASSERT((ulI2CSpeed == true) || (ulI2CSpeed == false));

    switch (ulI2CBase)
    {
		// I2C_PERIPH_0
		case  I2C0_MASTER_BASE:
		    //
		    // I2C0 is used with PortB[3:2].  The actual port and
		    // pins used may be different on your part, consult the data sheet for
		    // more information.  GPIO port B needs to be enabled so these pins can
		    // be used.
		    //
		    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

		    //
		    // Select the I2C function for these pins.  This function will also
		    // configure the GPIO pins for I2C operation, setting them to
		    // open-drain operation with weak pull-ups.  Consult the data sheet
		    // to see which functions are allocated per pin.
		    //
		    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);	//	special I2CSCL treatment for M4F devices
		    ROM_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

		    //
		    // Configure the pin muxing for I2C0 functions on port B2 and B3.
		    // This step is not necessary if your part does not support pin muxing.
		    //
		    ROM_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
		    ROM_GPIOPinConfigure(GPIO_PB3_I2C0SDA);

			//
			// The I2C0 peripheral must be enabled before use.
			//
			ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

			//
			// Enable and initialize the I2C0 master module.
			//
			ROM_I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), ulI2CSpeed);
			break;

		// I2C_PERIPH_1
		case  I2C1_MASTER_BASE:
		    //
		    // I2C1 is used with PortA[7:6].  The actual port and
		    // pins used may be different on your part, consult the data sheet for
		    // more information.  GPIO port A needs to be enabled so these pins can
		    // be used.
		    //
		    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

		    //
		    // Select the I2C function for these pins.  This function will also
		    // configure the GPIO pins for I2C operation, setting them to
		    // open-drain operation with weak pull-ups.  Consult the data sheet
		    // to see which functions are allocated per pin.
		    //
		    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);	//	special I2CSCL treatment for M4F devices
		    ROM_GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

		    //
		    // Configure the pin muxing for I2C1 functions on port A6 and A7.
		    // This step is not necessary if your part does not support pin muxing.
		    //
		    ROM_GPIOPinConfigure(GPIO_PA6_I2C1SCL);
		    ROM_GPIOPinConfigure(GPIO_PA7_I2C1SDA);

			//
			// The I2C1 peripheral must be enabled before use.
			//
			ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

			//
			// Enable and initialize the I2C1 master module.
			//
			ROM_I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), ulI2CSpeed);
			break;

		// I2C_PERIPH_2
		case  I2C2_MASTER_BASE:
		    //
		    // I2C2 is used with PortE[5:4].  The actual port and
		    // pins used may be different on your part, consult the data sheet for
		    // more information.  GPIO port E needs to be enabled so these pins can
		    // be used.
		    //
		    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

		    //
		    // Select the I2C function for these pins.  This function will also
		    // configure the GPIO pins for I2C operation, setting them to
		    // open-drain operation with weak pull-ups.  Consult the data sheet
		    // to see which functions are allocated per pin.
		    //
		    GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);	//	special I2CSCL treatment for M4F devices
		    ROM_GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

		    //
		    // Configure the pin muxing for I2C2 functions on port E4 and E5.
		    // This step is not necessary if your part does not support pin muxing.
		    //
		    ROM_GPIOPinConfigure(GPIO_PE4_I2C2SCL);
		    ROM_GPIOPinConfigure(GPIO_PE5_I2C2SDA);

			//
			// The I2C2 peripheral must be enabled before use.
			//
			ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

			//
			// Enable and initialize the I2C2 master module.
			//
			ROM_I2CMasterInitExpClk(I2C2_MASTER_BASE, SysCtlClockGet(), ulI2CSpeed);
			break;

		// I2C_PERIPH_3
		case  I2C3_MASTER_BASE:
		    //
		    // I2C3 is used with PortD[1:0].  The actual port and
		    // pins used may be different on your part, consult the data sheet for
		    // more information.  GPIO port D needs to be enabled so these pins can
		    // be used.
		    //
		    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

		    //
		    // Select the I2C function for these pins.  This function will also
		    // configure the GPIO pins for I2C operation, setting them to
		    // open-drain operation with weak pull-ups.  Consult the data sheet
		    // to see which functions are allocated per pin.
		    //
		    GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);	//	special I2CSCL treatment for M4F devices
		    ROM_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

		    //
		    // Configure the pin muxing for I2C2 functions on port D0 and D1.
		    // This step is not necessary if your part does not support pin muxing.
		    //
		    ROM_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
		    ROM_GPIOPinConfigure(GPIO_PD1_I2C3SDA);

			//
			// The I2C3 peripheral must be enabled before use.
			//
			ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);

			//
			// Enable and initialize the I2C3 master module.
			//
			ROM_I2CMasterInitExpClk(I2C3_MASTER_BASE, SysCtlClockGet(), ulI2CSpeed);
			break;

    }

}

//*****************************************************************************
//
//! Reads the I2C slave register.
//!
//! \param ulI2CBase is the base for the I2C module.
//! \param ucSlaveAdress is the 7-bit address of the slave to be addressed.
//! \param ucReg is the register to read from.
//!
//! This function initiates a read from the slave device.
//! The ulI2CBase parameter is the I2C modules master base address.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return Register value in an unsigned long format.  Note that 0 will be
//! returned if there is ever an error, 1 if there was not.
//
//*****************************************************************************
unsigned long
I2CRegRead(unsigned long ulI2CBase, unsigned char ucSlaveAdress, unsigned char ucReg)
{
unsigned long ulRegValue = 0;

	//
	// Check the arguments.
	//
	ASSERT(I2CMasterBaseValid(ulI2CBase));

	//
	// Wait until master module is done transferring.
	//
	while(ROM_I2CMasterBusy(ulI2CBase))
	{
	};

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, 0);

    //
    // Place the command to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_SINGLE_SEND);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Tell the master module what address it will place on the bus when
    // reading from the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, 1);

    //
    // Tell the master to read data.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //
    // Wait until master module is done receiving.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Read the data from the master.
    //
    ulRegValue = ROM_I2CMasterDataGet(ulI2CBase);

    //
    // Return the register value.
    //
    return ulRegValue;
}

//*****************************************************************************
//
//! Writes to the specified I2C slave register.
//!
//! \param ulI2CBase is the base for the I2C module.
//! \param ucSlaveAdress is the 7-bit address of the slave to be addressed.
//! \param ucReg is the register to write data to.
//! \param ucValue is the 8-bit data to be written.
//!
//! This function initiates a read from the I2C slave device.
//! The ulI2CBase parameter is the I2C modules master base address.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return Register value in an unsigned long format.  Note that 0 will be
//! returned if there is ever an error, 1 if there was not.
//
//*****************************************************************************
unsigned long
I2CRegWrite(unsigned long ulI2CBase, unsigned char ucSlaveAdress,
		 unsigned char ucReg, unsigned char ucValue)
{
	//
	// Check the arguments.
	//
	ASSERT(I2CMasterBaseValid(ulI2CBase));

	//
	// Wait until master module is done transferring.
	//
	while(ROM_I2CMasterBusy(ulI2CBase))
	{
	};

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, 0);

    //
    // Place the command to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Place the value to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucValue);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_CONT);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_FINISH);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

    //
    // Return 1 if there is no error.
    //
    return 1;
}


//*****************************************************************************
//
//! Reads one/multiple bytes of data from an I2C slave device.
//!
//! \param ulI2CBase is the base for the I2C module.
//! \param ucSlaveAdress is the 7-bit address of the slave to be addressed.
//! \param ucReg is the register to start reading from.
//! \param cReadData is a pointer to the array to store the data.
//! \param uiSize is the number of bytes to read from the slave.
//!
//! This function reads one/multiple bytes of data from an I2C slave device.
//! The ulI2CBase parameter is the I2C modules master base address.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return 0 if there was an error or 1 if there was not.
//
//*****************************************************************************
unsigned long
I2CReadData(unsigned long ulI2CBase, unsigned char ucSlaveAdress,
		unsigned char ucReg, char* cReadData, unsigned int uiSize)
{
	unsigned int uibytecount;		// local variable used for byte counting/state determination
	int MasterOptionCommand;		// used to assign the commands for ROM_I2CMasterControl() function

	//
	// Check the arguments.
	//
	ASSERT(I2CMasterBaseValid(ulI2CBase));

	//
	// Wait until master module is done transferring.
	//
	while(ROM_I2CMasterBusy(ulI2CBase))
	{
	};

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, 0);

    //
    // Place the command to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_SINGLE_SEND);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }


	//
	// Tell the master module what address it will place on the bus when
	// reading from the slave.
	//
	ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, true);

	//
	// Start with BURST with more than one byte to write
	//
	MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_START;


	for(uibytecount = 0; uibytecount < uiSize; uibytecount++)
	{
		//
		// The second and intermittent byte has to be read with CONTINUE control word
		//
		if(uibytecount == 1)
			MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_CONT;

		//
		// The last byte has to be send with FINISH control word
		//
		if(uibytecount == uiSize - 1)
			MasterOptionCommand = I2C_MASTER_CMD_BURST_RECEIVE_FINISH;

		//
		// Re-configure to SINGLE if there is only one byte to read
		//
		if(uiSize == 1)
			MasterOptionCommand = I2C_MASTER_CMD_SINGLE_RECEIVE;

		//
		// Initiate read of data from the slave.
		//
		ROM_I2CMasterControl(ulI2CBase, MasterOptionCommand);

		//
		// Wait until master module is done reading.
		//
		while(ROM_I2CMasterBusy(ulI2CBase))
		{
		};

		//
        // Check for errors.
        //
        if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
        {
            return 0;
        }

        //
        // Move byte from register
        //
		cReadData[uibytecount] = I2CMasterDataGet(ulI2CBase);
	}

	// send number of received bytes
	return uibytecount;
}

//*****************************************************************************
//
//! Writes one/multiple bytes of data to an I2C slave device.
//! Ensure to use auto-increment options on some devices
//! (Control Registers, refer to data sheet).
//! I.e. store related command in the first position of your data array.
//!
//! \param ulI2CBase is the base for the I2C module.
//! \param ucSlaveAdress is the 7-bit address of the slave to be addressed.
//! \param ucReg is the register to start writing to.
//! \param cSendData is a pointer to the array to be send.
//! \param uiSize is the number of bytes to send from array cSendData[].
//!
//! This function writes multiple bytes of data an I2C slave device.
//! The ulI2CBase parameter is the I2C modules master base address.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return 0 if there was an error or 1 if there was not.
//
//*****************************************************************************
unsigned long
I2CWriteData(unsigned long ulI2CBase, unsigned char ucSlaveAdress,
		unsigned char ucReg, char* cSendData, unsigned int uiSize)
{
	unsigned int uibytecount;		// local variable used for byte counting/state determination
	int MasterOptionCommand;		// used to assign the commands for ROM_I2CMasterControl() function

	//
	// Check the arguments.
	//
	ASSERT(I2CMasterBaseValid(ulI2CBase));

	//
	// Wait until master module is done transferring.
	//
	while(ROM_I2CMasterBusy(ulI2CBase))
	{
	};

    //
    // Tell the master module what address it will place on the bus when
    // writing to the slave.
    //
    ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucSlaveAdress, false);

    //
    // Place the value to be sent in the data register.
    //
    ROM_I2CMasterDataPut(ulI2CBase, ucReg);

    //
    // Initiate send of data from the master.
    //
    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Wait until master module is done transferring.
    //
    while(ROM_I2CMasterBusy(ulI2CBase))
    {
    };

    //
    // Check for errors.
    //
    if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
    {
        return 0;
    }

	//
	// Start with CONT for more than one byte to write
	//
	MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_CONT;


	for(uibytecount = 0; uibytecount < uiSize; uibytecount++)
	{
		//
		// The second and intermittent byte has to be send with CONTINUE control word
		//
		if(uibytecount == 1)
			MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_CONT;

		//
		// The last byte has to be send with FINISH control word
		//
		if(uibytecount == uiSize - 1)
			MasterOptionCommand = I2C_MASTER_CMD_BURST_SEND_FINISH;

		//
		// Re-configure to SINGLE if there is only one byte to write
		//
		if(uiSize == 1)
			MasterOptionCommand = I2C_MASTER_CMD_SINGLE_SEND;

		//
		// Send data byte
		//
		ROM_I2CMasterDataPut(ulI2CBase, cSendData[uibytecount]);

		//
		// Initiate send of data from the master.
		//
		ROM_I2CMasterControl(ulI2CBase, MasterOptionCommand);

		//
		// Wait until master module is done transferring.
		//
		while(ROM_I2CMasterBusy(ulI2CBase))
		{
		};

		//
        // Check for errors.
        //
        if(ROM_I2CMasterErr(ulI2CBase) != I2C_MASTER_ERR_NONE)
        {
            return 0;
        }
	}

    //
    // Return 1 if there is no error.
    //
    return 1;
}

//*****************************************************************************
//
//! Probes the selected I2C bus for available slave devices
//!
//! \param ulI2CBase is the base for the I2C module.
//!
//! This function scans the selected I2C bus for available I2C slave device.
//! The ulI2CBase parameter is the I2C modules master base address.
//! \e ulI2CBase parameter can be one of the following values:
//!
//! - \b I2C0_MASTER_BASE
//! - \b I2C1_MASTER_BASE
//! - \b I2C2_MASTER_BASE
//! - \b I2C3_MASTER_BASE
//!
//! \return 0 if there was an error or 1 if there was not.
//
//*****************************************************************************
unsigned long
I2CBusScan(unsigned long ulI2CBase)
{
	unsigned char ucProbeAdress;
	unsigned long ucerrorstate;

	//
	// Check the arguments.
	//
	ASSERT(I2CMasterBaseValid(ulI2CBase));

	//
	// Wait until master module is done transferring.
	//
	while(ROM_I2CMasterBusy(ulI2CBase))
	{
	};

	//
	// I2C Addresses are 7-bit values
	// probe the address range of 0 to 127 to find I2C slave devices on the bus
	//
	for (ucProbeAdress = 0; ucProbeAdress < 127; ucProbeAdress++)
	{
	    //
	    // Tell the master module what address it will place on the bus when
	    // writing to the slave.
	    //
		ROM_I2CMasterSlaveAddrSet(ulI2CBase, ucProbeAdress, false);
	    ROM_SysCtlDelay(50000);

	    //
	    // Place the command to be sent in the data register.
	    //
	    ROM_I2CMasterDataPut(ulI2CBase, 0x00);

	    //
	    // Initiate send of data from the master.
	    //
	    ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

	    //
	    // Make some delay
	    //
	    ROM_SysCtlDelay(500000);

	    //
	    // Read the I2C Master Control/Status (I2CMCS) Register to a local
	    // variable
	    //
	    ucerrorstate = ROM_I2CMasterErr(ulI2CBase);

	    //
	    // Examining the content I2C Master Control/Status (I2CMCS) Register
	    // to see if the ADRACK-Bit (Acknowledge Address) is TRUE (1)
	    // ( 1: The transmitted address was not acknowledged by the slave)
	    //
	    if(ucerrorstate & I2C_MASTER_ERR_ADDR_ACK)
	    {
	    	//
	    	// device at selected address did not acknowledge --> there's no device
	    	// with this address present on the I2C bus
	    	//
	        //
	        // Print a message to Stdio
	        //
	    	//UARTprintf("Address not found: 0x%2x - %3d\n",ucProbeAdress,ucProbeAdress);
		    //
		    // Make some delay
		    //
	    	//ROM_SysCtlDelay(1500000);
	    }

	    //
	    // ( 0: The transmitted address was acknowledged by the slave)
	    //
	    else
	    {
	    	//
	    	// device at selected address acknowledged --> there is a device
	    	// with this address present on the I2C bus
	    	//
	        //
	        // Print a message to Stdio
	        //
	    	UARTprintf("Address found: 0x%2x - %3d\n",ucProbeAdress,ucProbeAdress);

		    //
		    // Make some delay
		    //
	    	ROM_SysCtlDelay(1500000);
	    }
	}

	//
	// End transfer of data from the master.
	//
	ROM_I2CMasterControl(ulI2CBase, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    //
    // Print a message to Stdio
    //
	UARTprintf("I2C Bus-Scan done...\n");

    //
    // Return 1 if there is no error.
    //
    return 1;
}

