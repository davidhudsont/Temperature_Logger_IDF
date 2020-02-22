/**
 * @file DEADONRTC_Registers.h
 * @author David Hudson (you@domain.com)
 * @brief 
 * @date 2020-01-17
 * 
 */

#ifndef _DEADON_RTC_REGISTERS_H_
#define _DEADON_RTC_REGISTERS_H_


// Read/Write Bit
#define READ_BIT     0x00 // 
#define WRITE_BIT    0x80 // Bitwise or with register address


// Register Addresses
#define REG_SECONDS             0x00
#define REG_MINUTES             0x01
#define REG_HOURS               0x02
#define REG_DAYS                0x03
#define REG_DATE                0x04
#define REG_MONTH               0x05
#define REG_YEAR                0x06

#define REG_ALARM1_SECONDS      0x07
#define REG_ALRAM1_MINUTES      0x08
#define REG_ALRAM1_HOURS        0x09
#define REG_ALRAM1_DAYSDATE     0x0A

#define REG_ALRAM2_MINUTES      0x0B
#define REG_ALRAM2_HOURS        0x0C
#define REG_ALRAM2_DAYSDATE     0x0D

#define REG_CONTROL             0x0E
#define REG_CONTROL_STATUS      0x0F
#define REG_AGING_OFFSET        0x10
#define REG_TMP_MSB             0x11
#define REG_TMP_LSB             0x12
#define REG_TEMP_CONTROL        0x13
#define REG_SRAM_ADDR           0x18
#define REG_SRAM_DATA           0x19


// Seconds Register Bits
#define SECONDS_10_BITS         0x70
#define SECONDS_BITS            0x0F


// Minutes Register Bits
#define MINUTES_10_BITS         0x70
#define MINUTES_BITS            0x0F   

// Hours Register Bits
#define HOUR_12_N24             0x40  // Mask for reading whether it's in 12 hour or 24 hour mode
#define PM_NOTAM                0x20  // Mask for reading whether it's am or pm in 12 hour mode
#define HOURS_BITS_24           0x3F  // Mask for reading BITS 0-5 in 24 Hour mode
#define HOURS_BITS_12           0x1F  // Mask for reading BITS 0-4 in 12 Hour mode


// Months
#define CENTURY_BIT             0x80
#define MONTHS_BITS             0x1F


// Control Register Bits
#define EOSC_BIT        0x80    // Enable OsCillator
#define BBSQW_EN_BIT    0x40    // Battery-Backed Square-Wave Enable
#define CONV_BIT        0x20    // Convert Temperature
#define RS2_BIT         0x10    // Rate Select2
#define RS1_BIT         0x08    // Rate Select1
#define INTCN_BIT       0x04    // Interrupt Control
#define A2IE_BIT        0x02    // Alarm 2 Interrupt Enable
#define A1IE_BIT        0x01    // Alarm 1 Interrupt Enable

// Control/Status Register Bits
#define OSF_BIT         0x80 
#define BB32kHz_BIT     0x40 
#define CRATE1_BIT      0x20 
#define CRATE0_BIT      0x10 
#define EN32kHz_BIT     0x08 
#define BSY_BIT         0x04 
#define A2F_BIT         0x02 
#define A1F_BIT         0x01 


// Alarm Control Bits
#define A1M1_Bit        0x80
#define A1M2_Bit        0x80
#define A1M3_Bit        0x80
#define A1M4_Bit        0x80

#define A2M2_Bit        0x80
#define A2M3_Bit        0x80
#define A2M4_Bit        0x80


#endif