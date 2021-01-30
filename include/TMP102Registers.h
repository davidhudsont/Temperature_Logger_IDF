
#ifndef _TMP102Registers_H_
#define _TMP102Registers_H_

// Device Addresses
#define TMP102_DEV_ADDR_A 0x48 // A0 PIN Connection (Ground)
#define TMP102_DEV_ADDR_B 0x49 // A0 PIN Connection (V+)
#define TMP102_DEV_ADDR_C 0x4A // A0 PIN Connection (SDA)
#define TMP102_DEV_ADDR_D 0x4B // A0 PIN Connection (SCL)

// Register Addresses
#define REG_TEMPERATURE 0x00 // Temperature Register
#define REG_CONFIG 0x01      // Configuration Register
#define REG_TLOW 0x02        // Temperature Limit Low Register
#define REG_THIGH 0x03       // Temperature Limit High Register

// Temperature Resolution
#define TEMPERATURE_SCALE (0.0625f)   // Degrees C / Lsb(Count)
#define CONVERSION16LSB_TO_12LSB (16) // Conversion factor from 16Bit signed to 12bit

// Configuration

// Configuration Byte 1
#define CONFIG_ONESHOT 0x80 // [R/W] OneShot Configuration Bit
#define CONFIG_R1 0x40      // [R]   Converter Resolution Bit 1
#define CONFIG_RO 0x20      // [R]   Converter Resolution Bit 0
#define CONFIG_F1 0x10      // [R/W] Fault Queue Bit 1
#define CONFIG_F0 0x08      // [R/W] Fault Queue Bit 0
#define CONFIG_POL 0x04     // [R/W] Polarity of ALERT Pin
#define CONFIG_TM 0x02      // [R/W] Thermostat Mode Bit
#define CONFIG_SD 0x01      // [R/W] Shutdowm Mode Bit

// Configuration Byte 2
#define CONFIG_CR1 0x80 // [R/W] Conversion Rate Bit 1
#define CONFIG_CR0 0x40 // [R/W] Conversion Rate Bit 0
#define CONFIG_AL 0x20  // [R]   Alert Bit for when thresholds are reached
#define CONFIG_EM 0x10  // [R/W] Extended Mode Bit

// Conversion Rates
#define CONVERSION_RATE1 (0x00)                    // 0.25Hz
#define CONVERSION_RATE2 (CONFIG_CR0)              // 1Hz
#define CONVERSION_RATE3 (CONFIG_CR1)              // 4Hz
#define CONVERSION_RATE4 (CONFIG_CR0 | CONFIG_CR1) // 8Hz

#endif