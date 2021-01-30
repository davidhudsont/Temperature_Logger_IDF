
#ifndef _DEADON_RTC_H_
#define _DEADON_RTC_H_

#include "DEADONRTC_Registers.h"
#include "bspSpi.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// Active Low INT_BAR
#define DEADON_ALERT_PIN_NUM (GPIO_NUM_25) // Interrupt Pin
#define DEADON_RTC_INTR_FLAGS_DEFAULT (0)  //

QueueHandle_t queue;

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

typedef struct DEADONRTC
{
    uint8_t raw_time[7];

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

    bool intr_enable;
    bool alarm1_enable;
    bool alarm2_enable;

} DEADONRTC;

void DEADON_RTC_Begin(DEADONRTC *rtc);

void DEADON_RTC_WRITE_SECONDS(uint8_t second);
uint8_t DEADON_RTC_READ_SECONDS();

void DEADON_RTC_WRITE_MINUTES(uint8_t minutes);
uint8_t DEADON_RTC_READ_MINUTES();

void DEADON_RTC_WRITE_12HOURS(uint8_t hours, bool PM_NotAM);
void DEADON_RTC_WRITE_24HOURS(uint8_t hours);

void DEADON_RTC_WRITE_DAYS(DAYS days);
void DEADON_RTC_WRITE_DATE(uint8_t date);
void DEADON_RTC_WRITE_MONTH(uint8_t month);
void DEADON_RTC_WRITE_YEAR(uint8_t year);

void DEADON_RTC_READ_DATETIME(DEADONRTC *rtc);
void DEADON_RTC_WRITE_DATETIME(uint8_t seconds, uint8_t minutes, uint8_t hours,
                               uint8_t day, uint8_t date, uint8_t month,
                               uint8_t year);

void DEADON_RTC_WRITE_BUILD_DATETIME();

void DEADON_RTC_WRITE_ALARM1(uint8_t seconds, uint8_t minutes,
                             uint8_t hours, uint8_t date, ALARM1_MODES mode);

void DEADON_RTC_WRITE_ALARM2(uint8_t minutes,
                             uint8_t hours, uint8_t date, ALARM2_MODES mode);

bool DEADON_RTC_READ_ALARM1_FLAG(DEADONRTC *rtc);
bool DEADON_RTC_READ_ALARM2_FLAG(DEADONRTC *rtc);

void DEADON_RTC_ISR_Init(DEADONRTC *rtc);
void DEADON_RTC_Enable_Interrupt(DEADONRTC *rtc, bool enable);
void DEADON_RTC_Enable_Alarms(DEADONRTC *rtc, bool alarm1, bool alarm2);

uint8_t DEADON_RTC_SRAM_Read(uint8_t address);
void DEADON_RTC_SRAM_Write(uint8_t address, uint8_t data);

void DEADON_RTC_SRAM_Burst_Read(uint8_t address, uint8_t *data, uint32_t len);
void DEADON_RTC_SRAM_Burst_Write(uint8_t address, uint8_t *data, uint32_t len);

uint8_t DEADON_RTC_Register_Read(uint8_t register_address);
void DEADON_RTC_Register_Write(uint8_t register_address, uint8_t data);

void DEADON_RTC_Register_Burst_Read(uint8_t address, uint8_t *data, uint32_t len);
void DEADON_RTC_Register_Burst_Write(uint8_t address, uint8_t *data, uint32_t len);

#endif