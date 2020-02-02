/**
 * @file bspI2C.h
 * @author David Hudson (you@domain.com)
 * @brief I2C Boar Support Header File
 * @version 0.1
 * @date 2020-01-05
 * 
 * 
 */

#include "driver\i2c.h"


#define SDA_PIN_NUM (21) // I2C SDA PIN Number
#define SCL_PIN_NUM (22) // I2C SCL PIN Number

#define I2C_MASTER_FREQ (100000) // 100 KHz
#define I2C_MASTER_NUM (1)

#define I2C_TIMEOUT (10) // 10ms

void BSP_I2C_Setup();

void BSP_I2C_Write_Byte(uint8_t dev_address, uint8_t reg_address, uint8_t data);
uint8_t BSP_I2C_Read_Byte(uint8_t dev_address, uint8_t reg_address);

void BSP_I2C_Write_Burst(uint8_t dev_address, uint8_t reg_address, uint8_t * buf, uint32_t len);
void BSP_I2C_Read_Burst(uint8_t dev_address, uint8_t reg_address, uint8_t * buf, uint32_t len);


