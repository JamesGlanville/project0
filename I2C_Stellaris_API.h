//*****************************************************************************
//
// I2C_Stellaris_API.h - Stellaris I2C Master Driver.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHOR SHALL NOT, UNDER
// ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// AUTHOR: JOERG QUINTEN
// E2E-NICKNAME: aBUGSworstnightmare
//
//*****************************************************************************

#ifndef I2C_STELLARIS_API_H_
#define I2C_STELLARIS_API_H_

//*****************************************************************************
//
// Additional Defines for the API.
//
//*****************************************************************************


//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
// setup code for all I2C modules present on the LM4F120E5QR
extern void I2CSetup(unsigned long ulI2CPeriph,
								unsigned long ulI2CSpeed);

// functions for writing/reading single bytes of data
extern unsigned long I2CRegRead(unsigned long ulI2CBase,
								unsigned char ucSlaveAdress,
                                unsigned char ucReg);
extern unsigned long I2CRegWrite(unsigned long ulI2CBase,
								unsigned char ucSlaveAdress,
                                unsigned char ucReg,
                                unsigned char ucValue);

// functions for writing/reading multiple bytes of data
extern unsigned long I2CReadData(unsigned long ulI2CBase,
								unsigned char ucSlaveAdress,
								unsigned char ucReg,
								char* cReadData,
								unsigned int uiSize);

unsigned long I2CWriteData(unsigned long ulI2CBase,
								unsigned char ucSlaveAdress,
								unsigned char ucReg,
								char* cSendData,
								unsigned int uiSize);


unsigned long I2CBusScan(unsigned long ulI2CBase);

#endif /* I2C_STELLARIS_API_H_ */
