/*!
 * @file MICS_4514.h
 * @brief Simple I2C driver for MICS 4514 MEMS gas sensor
 * @license The MIT License (MIT)
 * @author Rob Tipping
 * @version V0.1
 * @date 05/12/2022
 * @url
 */
#ifndef __DFROBOT_MICS_H__
#define __DFROBOT_MICS_H__

#include "stm32L0xx_hal.h" // Needed for I2C

#define           MICS_ADDRESS_0            0x75
#define           MICS_ADDRESS_1            0x76
#define           MICS_ADDRESS_2            0x77
#define           MICS_ADDRESS_3            0x78
#define           OX_REGISTER_HIGH          0x04
#define           OX_REGISTER_LOW           0x05
#define           RED_REGISTER_HIGH         0x06
#define           RED_REGISTER_LOW          0x07
#define           POWER_REGISTER_HIGH       0x08
#define           POWER_REGISTER_LOW        0x09
#define           POWER_MODE_REGISTER       0x0a
#define           SLEEP_MODE                0x00
#define           WAKE_UP_MODE              0x01
#define           OX_MODE                   0x00
#define           RED_MODE                  0x01
#define           ERROR                     -1
#define           EXIST                     0x00
#define           NO_EXIST                  0x02

#define           CO                        0x01          ///< Carbon Monoxide
#define           CH4                       0x02          ///< Methane
#define           C2H5OH                    0x03          ///< Ethanol
#define           C3H8                      0x04          ///< Propane
#define           C4H10                     0x05          ///< Iso Butane
#define           H2                        0x06          ///< Hydrogen
#define           H2S                       0x07          ///< Hydrothion
#define           NH3                       0x08          ///< Ammonia
#define           NO                        0x09          ///< Nitric Oxide
#define           NO2                       0x0A          ///< Nitrogen Dioxide


typedef struct{
	I2C_HandleTypeDef *i2cHandle;
	float carbonMonoxide;
	float methane;
	float ethanol;
	uint8_t propane;
	uint8_t isoButane;
	float hydrogen;
	uint8_t hydrothion;
	float ammonia;
	uint8_t nitricOxide;
	float nitrogenDioxide;

}MICS;


//Initialisation
uint8_t MICS_Initialise(MICS *dev, I2C_HandleTypeDef *i2cHandle);
HAL_StatusTypeDef MICS_Calabrate(MICS *dev);

//Settings
HAL_StatusTypeDef MICS_WakeUp(MICS *dev);
HAL_StatusTypeDef MICS_Sleep(MICS *dev);

//Read Data
HAL_StatusTypeDef MICS_ReadSingleGasData(MICS *dev, uint8_t type);
HAL_StatusTypeDef MICS_ReadAllGasData(MICS *dev);

//Calculate data
float MICS_ReadCarbonMonoxide(float data);
float MICS_ReadMethane(float data);
float MICS_ReadEthanol(float data);
float MICS_ReadHydrogen(float data);
float MICS_ReadAmmonia(float data);
float MICS_ReadNitrogenDioxide(float data);

//Data 	Acquisition
HAL_StatusTypeDef MICS_getData(MICS *dev, uint16_t *oxData, uint16_t *redData, uint16_t *powerData);

//low level functions
HAL_StatusTypeDef MICS_ReadRegister(MICS *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef MICS_ReadRegisters(MICS *dev, uint8_t reg, uint8_t *data, uint8_t length);

HAL_StatusTypeDef MICS_WriteRegister(MICS *dev, uint8_t reg, uint8_t *data);

#endif
