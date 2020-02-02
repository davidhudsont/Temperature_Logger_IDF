/**
 * @file bspSpi.h
 * @author David Hudson
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * 
 */

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "driver\spi_common.h"
#include "driver\spi_master.h"
#include "driver\spi_slave.h"


#define PIN_NUM_MISO (19) 
#define PIN_NUM_MOSI (23)
#define PIN_NUM_SCK  (18)
#define PIN_NUM_CS   (2)

// Configures spi with commands bits
esp_err_t BSP_SPI_Init(int clock_speed);


uint8_t SPI_readReg(const uint8_t address);
void SPI_writeReg(const uint8_t address, const uint8_t data);

void SPI_Burst_Read(const uint8_t address, uint8_t * buf, uint32_t size);
void SPI_Burst_Write(const uint8_t address, uint8_t * buf, uint32_t size);

// These functions are currently not used
void SPI_SendByte8(const uint8_t data);
uint8_t SPI_GetByte8();
void SPI_SendBuffer(uint8_t * buf, uint32_t buffer_length);
void SPI_GetBuffer(uint8_t * buf, uint32_t buffer_length);
void SPI_SetDataRate();



#endif