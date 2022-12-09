/*!
 * @file MICS_4514.cpp
 * @brief Simple I2C driver for MICS 4514 MEMS gas sensor
 * @license The MIT License (MIT)
 * @author Rob Tipping
 * @version V0.1
 * @date 05/12/2022
 * @url
 */
#include "MICS_4514.h"

#define	MICS_ADDRESS	 (MICS_ADDRESS_0 << 1) // Define sensor address and bit shift by 1

//variables to store calibrated base sensor readings
int16_t  r0_ox;
int16_t  r0_red;

//Initialise
uint8_t MICS_Initialise(MICS *dev, I2C_HandleTypeDef *i2cHandle)
{
	dev->i2cHandle = i2cHandle;
	dev-> carbonMonoxide = 0;
	dev-> methane = 0;
	dev-> ethanol = 0;
	dev-> propane = 0;
	dev-> isoButane = 0;
	dev-> hydrogen = 0;
	dev-> hydrothion = 0;
	dev-> ammonia = 0;
	dev-> nitricOxide = 0;
	dev-> nitrogenDioxide = 0;

	uint8_t errNum = 0;
	// need to write handshake

	return errNum;
}

// Get base readings should be taken 3 minutes after sensor is powered on.
HAL_StatusTypeDef MICS_Calabrate(MICS *dev)
{
	uint16_t oxData    = 0;
	uint16_t redData   = 0;
	uint16_t powerData = 0;
	HAL_StatusTypeDef status = MICS_getData( dev, &oxData, &redData, &powerData);
	r0_ox = powerData - oxData;
	r0_red = powerData - redData;
	return status;
}

// Turn sensor on and off
HAL_StatusTypeDef MICS_WakeUp(MICS *dev)
{
	uint8_t regData = WAKE_UP_MODE;
	return MICS_WriteRegister(dev, POWER_MODE_REGISTER, &regData);
}

HAL_StatusTypeDef MICS_Sleep(MICS *dev)
{
	uint8_t regData = SLEEP_MODE;
	return MICS_WriteRegister(dev, POWER_MODE_REGISTER, &regData);
}


//Read Data
HAL_StatusTypeDef MICS_ReadSingleGasData(MICS *dev, uint8_t type)
{
	uint16_t oxData    = 0;
	uint16_t redData   = 0;
	uint16_t powerData = 0;
	HAL_StatusTypeDef status = MICS_getData( dev, &oxData, &redData, &powerData); // gets all sensor data
	float RS_R0_RED_data = (float)(powerData - redData) / (float)r0_red; // converts to red data
	float RS_R0_OX_data = (float)(powerData - oxData) / (float)r0_ox;	// converts to ox data
	switch(type){
	case CO:
		dev->carbonMonoxide = MICS_ReadCarbonMonoxide(RS_R0_RED_data);
		break;
	case CH4:
		dev->methane = MICS_ReadMethane(RS_R0_RED_data);
		break;
	case C2H5OH:
		dev->ethanol = MICS_ReadEthanol(RS_R0_RED_data);
		break;
	case H2:
		dev->hydrogen = MICS_ReadHydrogen(RS_R0_RED_data);
		break;
	case NH3:
		dev->ammonia = MICS_ReadAmmonia(RS_R0_RED_data);
		break;
	case NO2:
		dev->nitrogenDioxide = MICS_ReadNitrogenDioxide(RS_R0_OX_data);
		break;
	default:
		return 1;
		break;
	}
	return status;
}

HAL_StatusTypeDef MICS_ReadAllGasData(MICS *dev)
{
	uint16_t oxData    = 0;
	uint16_t redData   = 0;
	uint16_t powerData = 0;
	HAL_StatusTypeDef status = MICS_getData( dev, &oxData, &redData, &powerData);
	float RS_R0_RED_data = (float)(powerData - redData) / (float)r0_red;
	float RS_R0_OX_data = (float)(powerData - oxData) / (float)r0_ox;

	dev->carbonMonoxide = MICS_ReadCarbonMonoxide(RS_R0_RED_data);
	dev->methane = MICS_ReadMethane(RS_R0_RED_data);
	dev->ethanol = MICS_ReadEthanol(RS_R0_RED_data);
	dev->hydrogen = MICS_ReadHydrogen(RS_R0_RED_data);
	dev->ammonia = MICS_ReadAmmonia(RS_R0_RED_data);
	dev->nitrogenDioxide = MICS_ReadNitrogenDioxide(RS_R0_OX_data);

	return status;
}

//Calculate gas reading from Red or ox data
float MICS_ReadCarbonMonoxide(float data)
{
	if(data > 0.425)
		return 0.0;
	float co = (0.425 - data) / 0.000405;
	if(co > 1000.0)
		return 1000.0;
	if(co < 1.0)
		return 0.0;
	return co;
}
float MICS_ReadMethane(float data)
{
	if(data > 0.786)
		return 0.0;
	float methane = (0.786 - data) / 0.000023;
	if(methane < 1000.0) methane = 0.0;
	if(methane > 25000.0) methane = 25000.0;
	return methane;
}
float MICS_ReadEthanol(float data)
{
	if(data > 0.306)
		return 0.0;
	float ethanol = (0.306 - data) / 0.00057;
	if(ethanol < 10.0)
		return 0.0;
	if(ethanol > 500.0)
		return 500.0;
	return ethanol;
}
float MICS_ReadHydrogen(float data)
{
	if(data > 0.279)
		return 0.0;
	float hydrogen = (0.279 - data) / 0.00026;
	if(hydrogen < 1.0)
		return 0.0;
	if(hydrogen > 1000.0)
		return 1000.0;
	return hydrogen;
}
float MICS_ReadAmmonia(float data)
{
	if(data > 0.8)
		return 0.0;
	float ammonia = (0.8 - data) / 0.0015;
	if(ammonia < 1.0)
		return 0.0;
	if(ammonia > 500.0)
		return 500.0;
	return ammonia;
}
float MICS_ReadNitrogenDioxide(float data)
{
	if(data < 1.1) return 0;
	float nitrogendioxide = (data - 0.045) / 6.13;
	if(nitrogendioxide < 0.1)
		return 0.0;
	if(nitrogendioxide > 10.0)
		return 10.0;
	return nitrogendioxide;
}


//Get Sensor Data
HAL_StatusTypeDef MICS_getData(MICS *dev, uint16_t *oxData, uint16_t *redData, uint16_t *powerData)
{
	uint8_t regData[20] = {0x00};
	HAL_StatusTypeDef status = MICS_ReadRegisters( dev, OX_REGISTER_HIGH, regData, 6);
	*oxData    = (((uint16_t)regData[0] << 8) + (uint16_t)regData[1]);
	*redData   = (((uint16_t)regData[2] << 8) + (uint16_t)regData[3]);
	*powerData = (((uint16_t)regData[4] << 8) + (uint16_t)regData[5]);
	return status;
}


//Low-Level Functions
HAL_StatusTypeDef MICS_ReadRegister(MICS *dev, uint8_t reg, uint8_t *data)
{
	return HAL_I2C_Mem_Read( dev->i2cHandle, MICS_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MICS_ReadRegisters(MICS *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
	return HAL_I2C_Mem_Read( dev->i2cHandle, MICS_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MICS_WriteRegister(MICS *dev, uint8_t reg, uint8_t *data)
{
	return HAL_I2C_Mem_Write( dev->i2cHandle, MICS_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

