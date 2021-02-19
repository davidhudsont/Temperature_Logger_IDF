
#pragma once

#include "DEADONRTC_Registers.h"
#include "BSP_SPI.h"
#include "driver/gpio.h"
#include <string>

// Active Low INT_BAR
#define DEADON_ALERT_PIN_NUM (GPIO_NUM_25) // Interrupt Pin
#define DEADON_RTC_INTR_FLAGS_DEFAULT (0)  //

typedef enum DAYS
{
    SUNDAY = 1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
} DAYS;

typedef enum ALARM1_MODES
{
    ALARM1_PER_SECOND = 0,
    ALARM1_SECONDS_MATCH,
    ALARM1_MIN_SEC_MATCH,
    ALARM1_HR_MIN_SEC_MATCH,
    ALARM1_DT_HR_MIN_SEC_MATCH
} ALARM1_MODES;

typedef enum ALARM2_MODES
{
    ALARM2_PER_MIN = 0,
    ALARM2_MIN_MATCH,
    ALARM2_HR_MIN_MATCH,
    ALARM2_DT_HR_MIN_MATCH

} ALARM2_MODES;

int GetInterruptSemiphore();

class RTCDS3234
{
private:
    uint8_t raw_time[7];

    bool intr_enable;
    bool alarm1_enable;
    bool alarm2_enable;

    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;

    bool hour12_not24;
    bool PM_notAM;
    bool century;

    BSP::SPI spi;

    /**
     * @brief Read a register
     * 
     * @param register_address 
     * @return uint8_t 
     */
    uint8_t Register_Read(uint8_t register_address);

    /**
     * @brief Write to a register
     * 
     * @param register_address 
     * @param data 
     */
    void Register_Write(uint8_t register_address, uint8_t data);

    /**
     * @brief Read multiple bytes from registers
     * 
     * @param address 
     * @param data 
     * @param len 
     */
    void Register_Burst_Read(uint8_t address, uint8_t *data, uint32_t len);

    /**
     * @brief Write to bytes to registers
     * 
     * @param address 
     * @param data 
     * @param len 
     */
    void Register_Burst_Write(uint8_t address, uint8_t *data, uint32_t len);

public:

    /**
     * @brief Construct a new RTCDS3234 object
     * 
     */
    RTCDS3234();

    /**
     * @brief Initialize the RTC device structure
     *        and start the SPI bus.
     */
    void Begin();

    /**
     * @brief Read the current data and time from the RTC.
     */
    void READ_DATETIME();

    /**
     * @brief Write the date and time
     * @param seconds 
     * @param minutes 
     * @param hours 
     * @param day 
     * @param date 
     * @param month 
     * @param year 
     */
    void WRITE_DATETIME(uint8_t seconds, uint8_t minutes, uint8_t hours,
                        uint8_t day, uint8_t date, uint8_t month,
                        uint8_t year);

    /**
     * @brief Get the date and time from the build date
     * 
     */
    void WRITE_BUILD_DATETIME();

    /**
     * @brief Get Date time string
     * 
     * @return std::string 
     */
    std::string DATETIME_TOSTRING();

    void WRITE_SECONDS(uint8_t second);
    uint8_t READ_SECONDS();

    void WRITE_MINUTES(uint8_t minutes);
    uint8_t READ_MINUTES();

    // input : hours 1-12
    void WRITE_12HOURS(uint8_t hours, bool PM_NotAM);
    // input : hours 0-23
    void WRITE_24HOURS(uint8_t hours);

    void WRITE_DAYS(DAYS days);
    void WRITE_DATE(uint8_t date);
    void WRITE_MONTH(uint8_t month);
    void WRITE_YEAR(uint8_t year);

    /**
     * @brief 
     * 
     * @param seconds 
     * @param minutes 
     * @param hours 
     * @param date 
     * @param mode 
     */
    void WRITE_ALARM1(uint8_t seconds, uint8_t minutes,
                      uint8_t hours, uint8_t date, ALARM1_MODES mode);

    /**
     * @brief 
     * 
     * @param minutes 
     * @param hours 
     * @param date 
     * @param mode 
     */
    void WRITE_ALARM2(uint8_t minutes,
                      uint8_t hours, uint8_t date, ALARM2_MODES mode);

    /**
     * @brief 
    * @return true if flag was set, false otherwise
     */
    bool READ_ALARM1_FLAG();

    /**
     * @brief Read Alarm 2 flag
     * @return true if flag was set, false otherwise
     */
    bool READ_ALARM2_FLAG();

    /**
     * @brief Initialize ISR for Alert Pin
     */
    void ISR_Init();

    /**
     * @brief Enable the RTC's interrupts
     * @param bool enable 
     */
    void Enable_Interrupt(bool enable);

    /**
     * @brief Enable Alarm 1 and/or Alarm 2
     * @param bool alarm1 - enable alarm 1 if true
     * @param bool alarm2 - enable alarm 2 if true
     */
    void Enable_Alarms(bool alarm1, bool alarm2);

    /**
     * @brief Read the RTC's SRAM at a specified address
     * @note The size of the memoy if 256 bytes
     * @param address range: 0x00 - 0xff
     * @return uint8_t 
     */
    uint8_t SRAM_Read(uint8_t address);

    /**
     * @brief Write to the RTC's SRAM at a specified address
     * @note The size of the memoy if 256 bytes
     * @param address range: 0x00 - 0xff
     * @param data - The data we want to write 
     */
    void SRAM_Write(uint8_t address, uint8_t data);

    /**
     * @brief Read the RTC's SRAM at a specified address
     * @note The size of the memoy if 256 bytes
     * @param address - Starting address
     * @param data - data buffer
     * @param len - Number of bytes to read
     */
    void SRAM_Burst_Read(uint8_t address, uint8_t *data, uint32_t len);

    /**
     * @brief Write the RTC's SRAM at a specified address
     * @note The size of the memoy if 256 bytes
     * @param address - Starting address
     * @param data - data buffer
     * @param len - Number of bytes to write
     */
    void SRAM_Burst_Write(uint8_t address, uint8_t *data, uint32_t len);
};
