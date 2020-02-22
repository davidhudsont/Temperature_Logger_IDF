/**
 * @file DEADONRTC_Registers.h
 * @author David Hudson 
 * @brief 
 * @date 2020-01-17
 * 
 */

#ifndef _DEADON_RTC_REGISTERS_H_
#define _DEADON_RTC_REGISTERS_H_


// Register Addresses
#define REG_SECONDS             0x00  // [R/W] Seconds Register
#define REG_MINUTES             0x01  // [R/W] Minutes Register
#define REG_HOURS               0x02  // [R/W] Hours   Register
#define REG_DAYS                0x03  // [R/W] Days    Register
#define REG_DATE                0x04  // [R/W] Date    Register
#define REG_MONTH               0x05  // [R/W] Month   Register
#define REG_YEAR                0x06  // [R/W] Year    Register
#define REG_ALARM1_SECONDS      0x07  // [R/W] Alarm 1 Seconds Register
#define REG_ALRAM1_MINUTES      0x08  // [R/W] Alarm 1 Minutes Register
#define REG_ALRAM1_HOURS        0x09  // [R/W] Alarm 1 Hours   Register
#define REG_ALRAM1_DAYSDATE     0x0A  // [R/W] Alarm 1 Days & Date Register
#define REG_ALRAM2_MINUTES      0x0B  // [R/W] Alarm 2 Minutes Register
#define REG_ALRAM2_HOURS        0x0C  // [R/W] Alarm 2 Hours   Register
#define REG_ALRAM2_DAYSDATE     0x0D  // [R/W] Alarm 2 Days & Date Register
#define REG_CONTROL             0x0E  // [R/W] Control Register
#define REG_CONTROL_STATUS      0x0F  // [R/W] Control/Status Register
#define REG_AGING_OFFSET        0x10  // [R/W] Aging Offset Register
#define REG_TMP_MSB             0x11  // [R]   Temperature MSB Register
#define REG_TMP_LSB             0x12  // [R]   Temperature LSB Register
#define REG_TEMP_CONTROL        0x13  // [R/W] Temperature Control Register
#define REG_SRAM_ADDR           0x18  // [R/W] SRAM Address Register
#define REG_SRAM_DATA           0x19  // [R/W] SRAM Data Register


// Seconds Register Bits
#define SECONDS_10_BITS         0x70  // 10's place bits
#define SECONDS_BITS            0x0F  // 1's place bits

// Minutes Register Bits
#define MINUTES_10_BITS         0x70  // 10's place bits
#define MINUTES_BITS            0x0F  // 1's place bits   

// Hours Register Bits
#define HOUR_12_N24             0x40  // Bit mask for setting 12 hour or 24 hour mode
#define PM_NOTAM                0x20  // Bit mask for setting am or pm in 12 hour mode
#define HOURS_BITS_24           0x3F  // Bit mask for setting hours in 24 Hour mode
#define HOURS_BITS_12           0x1F  // Bit mask for setting hours in 12 Hour mode

// Months
#define CENTURY_BIT             0x80  // Bit mask for setting the century
#define MONTHS_BITS             0x1F  // Bit mask fpr setting months


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
#define OSF_BIT         0x80   // Oscillator Stop Flag bit mask
#define BB32kHz_BIT     0x40   // Battery Backed 32kHz Output bit mask
#define CRATE1_BIT      0x20   // Conversion Rate Bit 1 mask
#define CRATE0_BIT      0x10   // Converion Rate Bit 0 mask
#define EN32kHz_BIT     0x08   // Enable 32kHz ouput bit mask 
#define BSY_BIT         0x04   // Busy bit mask
#define A2F_BIT         0x02   // Alarm 2 Flag bit mask
#define A1F_BIT         0x01   // Alarm 1 Flag bit mask


// Alarm Control Bits
#define A1M1_Bit        0x80  // Bit masks for configuring different 
#define A1M2_Bit        0x80  // alarm modes
#define A1M3_Bit        0x80
#define A1M4_Bit        0x80

#define A2M2_Bit        0x80  // Bit masks for configuring different 
#define A2M3_Bit        0x80  // alarm modes
#define A2M4_Bit        0x80


#endif