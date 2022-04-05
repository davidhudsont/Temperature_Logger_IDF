
#pragma once

#include "DEADONRTC_Registers.h"
#include "BSP_SPI.h"
#include "driver/gpio.h"
#include <string>

// Active Low INT_BAR
#define DEADON_ALERT_PIN_NUM (GPIO_NUM_25) // Interrupt Pin
constexpr int SPI_CLOCK_SPEED = 4 * 1000 * 1000;

uint8_t calculateMaxDayOfMonth(uint8_t month, uint8_t year);

enum DAYS
{
    SUNDAY = 1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
};

enum ALARM1_MODES
{
    ALARM1_PER_SECOND = 0,
    ALARM1_SECONDS_MATCH,
    ALARM1_MIN_SEC_MATCH,
    ALARM1_HR_MIN_SEC_MATCH,
    ALARM1_DT_HR_MIN_SEC_MATCH
};

enum ALARM2_MODES
{
    ALARM2_PER_MIN = 0,
    ALARM2_MIN_MATCH,
    ALARM2_HR_MIN_MATCH,
    ALARM2_DT_HR_MIN_MATCH

};

struct DATE_TIME
{
    uint8_t year;
    uint8_t month;
    uint8_t dayofMonth;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    bool hour12_not24;
    bool PM_notAM;
};

int GetInterruptSemiphore();

class RTCDS3234
{
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
    void ReadDateTime();

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
    void WriteDateTime(uint8_t seconds, uint8_t minutes, uint8_t hours,
                       uint8_t day, uint8_t date, uint8_t month,
                       uint8_t year);

    void WriteDate(uint8_t date, uint8_t month, uint8_t year);

    void WriteTime(uint8_t hour, uint8_t minute, uint8_t second);

    /**
     * @brief Get the date and time from the build date
     *
     */
    void WriteBuildDateTime();

    /**
     * @brief Get Date string
     *
     * @return std::string
     */
    std::string DateToString();

    /**
     * @brief Get the Time string
     *
     * @return std::string
     */
    std::string TimeToString();

    DATE_TIME GetDateTime();

    void WriteSeconds(uint8_t second);
    uint8_t ReadSeconds();

    void WriteMinutes(uint8_t minutes);
    uint8_t ReadMinutes();

    // @param hours 1-12
    void Write12Hours(uint8_t hours, bool PM_NotAM);
    // @param hours 0-23
    void Write24Hours(uint8_t hours);
    // @param days
    void WriteDays(DAYS days);
    // @param date 1-31
    void WriteDate(uint8_t date);
    // @param month 1-12
    void WriteMonth(uint8_t month);
    // @param year 0-99
    void WriteYear(uint8_t year);

    /**
     * @brief
     *
     * @param seconds
     * @param minutes
     * @param hours
     * @param date
     * @param mode
     */
    void WriteAlarm1(uint8_t seconds, uint8_t minutes,
                     uint8_t hours, uint8_t date, ALARM1_MODES mode);

    /**
     * @brief
     *
     * @param minutes
     * @param hours
     * @param date
     * @param mode
     */
    void WriteAlarm2(uint8_t minutes,
                     uint8_t hours, uint8_t date, ALARM2_MODES mode);

    /**
     * @brief
     * @return true if flag was set, false otherwise
     */
    bool ReadAlarm1Flag();

    /**
     * @brief Read Alarm 2 flag
     * @return true if flag was set, false otherwise
     */
    bool ReadAlarm2Flag();

    /**
     * @brief Initialize ISR for Alert Pin
     */
    void ISRInitialize();

    /**
     * @brief Enable the RTC's interrupts
     * @param bool enable
     */
    void EnableInterrupt(bool enable);

    /**
     * @brief Enable Alarm 1 and/or Alarm 2
     * @param bool alarm1 - enable alarm 1 if true
     * @param bool alarm2 - enable alarm 2 if true
     */
    void EnableAlarms(bool alarm1, bool alarm2);

    /**
     * @brief Read the RTC's SRAM at a specified address
     * @note The size of the memory if 256 bytes
     * @param address range: 0x00 - 0xff
     * @return uint8_t
     */
    uint8_t SRAMRead(uint8_t address);

    /**
     * @brief Write to the RTC's SRAM at a specified address
     * @note The size of the memory if 256 bytes
     * @param address range: 0x00 - 0xff
     * @param data - The data we want to write
     */
    void SRAMWrite(uint8_t address, uint8_t data);

    /**
     * @brief Read the RTC's SRAM at a specified address
     * @note The size of the memory if 256 bytes
     * @param address - Starting address
     * @param data - data buffer
     * @param len - Number of bytes to read
     */
    void SRAMBurstRead(uint8_t address, uint8_t *data, uint32_t len);

    /**
     * @brief Write the RTC's SRAM at a specified address
     * @note The size of the memory if 256 bytes
     * @param address - Starting address
     * @param data - data buffer
     * @param len - Number of bytes to write
     */
    void SRAMBurstWrite(uint8_t address, uint8_t *data, uint32_t len);

private:
    uint8_t raw_time[7];

    BSP::SPI spi;
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

    uint8_t RegisterRead(uint8_t register_address);
    void RegisterWrite(uint8_t register_address, uint8_t data);
    void RegisterBurstRead(uint8_t address, uint8_t *data, uint32_t len);
    void RegisterBurstWrite(uint8_t address, uint8_t *data, uint32_t len);
};
