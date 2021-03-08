
#include "DEADONRTC.h"
#include "string.h"
#include <sstream>
#include <iomanip>
#include "esp_log.h"
#include "freertos/semphr.h"

// Credit to SparkFun Library for the Build Dates: https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library
// Parse the __DATE__ predefined macro to generate date defaults:
// __Date__ Format: MMM DD YYYY (First D may be a space if <10)
// <MONTH>
#define BUILD_MONTH_JAN ((__DATE__[0] == 'J') && (__DATE__[1] == 'a')) ? 1 : 0
#define BUILD_MONTH_FEB (__DATE__[0] == 'F') ? 2 : 0
#define BUILD_MONTH_MAR ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'r')) ? 3 : 0
#define BUILD_MONTH_APR ((__DATE__[0] == 'A') && (__DATE__[1] == 'p')) ? 4 : 0
#define BUILD_MONTH_MAY ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'y')) ? 5 : 0
#define BUILD_MONTH_JUN ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'n')) ? 6 : 0
#define BUILD_MONTH_JUL ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'l')) ? 7 : 0
#define BUILD_MONTH_AUG ((__DATE__[0] == 'A') && (__DATE__[1] == 'u')) ? 8 : 0
#define BUILD_MONTH_SEP (__DATE__[0] == 'S') ? 9 : 0
#define BUILD_MONTH_OCT (__DATE__[0] == 'O') ? 10 : 0
#define BUILD_MONTH_NOV (__DATE__[0] == 'N') ? 11 : 0
#define BUILD_MONTH_DEC (__DATE__[0] == 'D') ? 12 : 0
#define BUILD_MONTH BUILD_MONTH_JAN | BUILD_MONTH_FEB | BUILD_MONTH_MAR |     \
                        BUILD_MONTH_APR | BUILD_MONTH_MAY | BUILD_MONTH_JUN | \
                        BUILD_MONTH_JUL | BUILD_MONTH_AUG | BUILD_MONTH_SEP | \
                        BUILD_MONTH_OCT | BUILD_MONTH_NOV | BUILD_MONTH_DEC
// <DATE>
#define BUILD_DATE_0 ((__DATE__[4] == ' ') ? 0 : (__DATE__[4] - 0x30))
#define BUILD_DATE_1 (__DATE__[5] - 0x30)
#define BUILD_DATE ((BUILD_DATE_0 * 10) + BUILD_DATE_1)
// <YEAR>
#define BUILD_YEAR (((__DATE__[7] - 0x30) * 1000) + ((__DATE__[8] - 0x30) * 100) + \
                    ((__DATE__[9] - 0x30) * 10) + ((__DATE__[10] - 0x30) * 1))

// Parse the __TIME__ predefined macro to generate time defaults:
// __TIME__ Format: HH:MM:SS (First number of each is padded by 0 if <10)
// <HOUR>
#define BUILD_HOUR_0 ((__TIME__[0] == ' ') ? 0 : (__TIME__[0] - 0x30))
#define BUILD_HOUR_1 (__TIME__[1] - 0x30)
#define BUILD_HOUR ((BUILD_HOUR_0 * 10) + BUILD_HOUR_1)
// <MINUTE>
#define BUILD_MINUTE_0 ((__TIME__[3] == ' ') ? 0 : (__TIME__[3] - 0x30))
#define BUILD_MINUTE_1 (__TIME__[4] - 0x30)
#define BUILD_MINUTE ((BUILD_MINUTE_0 * 10) + BUILD_MINUTE_1)
// <SECOND>
#define BUILD_SECOND_0 ((__TIME__[6] == ' ') ? 0 : (__TIME__[6] - 0x30))
#define BUILD_SECOND_1 (__TIME__[7] - 0x30)
#define BUILD_SECOND ((BUILD_SECOND_0 * 10) + BUILD_SECOND_1)

SemaphoreHandle_t semiphore;

int GetInterruptSemiphore()
{
    return xSemaphoreTake(semiphore, 0);
}

/**
 * @brief Convert a Binary Coded Decimal number
 *        into a decimal number
 * 
 * @param bcd 
 * @return uint8_t 
 */
static uint8_t BCDtoDEC(uint8_t bcd)
{
    uint8_t dec = ((bcd / 0x10) * 10) + (bcd % 0x10);
    return dec;
}

/**
 * @brief Convert a decimal number to
 *        a binary coded decimal number
 * 
 * @param dec 
 * @return uint8_t 
 */
static uint8_t DECtoBCD(uint8_t dec)
{
    uint8_t bcd = (((dec / 10) * 0x10) | (dec % 10));
    return bcd;
}

RTCDS3234::RTCDS3234()
    : intr_enable(false), alarm1_enable(false), alarm2_enable(false),
      seconds(0), minutes(0), hours(0), day(0), date(0), month(0), year(0),
      hour12_not24(false), PM_notAM(false), century(false)
{
    memset(&raw_time, 0, sizeof(uint8_t));
}

void RTCDS3234::Begin()
{
    volatile int clock_speed = 4 * 1000 * 1000; // Clock speed 4MHz
    esp_err_t err = false;
    err = spi.Init(clock_speed);
    if (err != ESP_OK)
    {
        ESP_LOGE("RTC", "Spi Configuration Error");
    }
}

void RTCDS3234::READ_DATETIME()
{
    Register_Burst_Read(REG_SECONDS, raw_time, 7);

    seconds = BCDtoDEC(raw_time[0]);
    minutes = BCDtoDEC(raw_time[1]);

    hour12_not24 = (raw_time[2] & HOUR_12_N24) == HOUR_12_N24;
    if (hour12_not24)
    {
        PM_notAM = (raw_time[2] & PM_NOTAM) == PM_NOTAM;
        hours = BCDtoDEC(raw_time[2] & HOURS_BITS_12);
    }
    else
    {
        hours = BCDtoDEC(raw_time[2] & HOURS_BITS_24);
    }
    day = BCDtoDEC(raw_time[3]);
    date = BCDtoDEC(raw_time[4]);
    century = (raw_time[5] & CENTURY_BIT) == CENTURY_BIT;
    month = BCDtoDEC(raw_time[5] & MONTHS_BITS);
    year = BCDtoDEC(raw_time[6]);
}

/**
 * @brief Write the date and time
 * 
 * @param seconds 
 * @param minutes 
 * @param hours 
 * @param day 
 * @param date 
 * @param month 
 * @param year 
 */
void RTCDS3234::WRITE_DATETIME(uint8_t seconds, uint8_t minutes, uint8_t hours,
                               uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    uint8_t time_config[7];

    time_config[0] = DECtoBCD(seconds);
    time_config[1] = DECtoBCD(minutes);
    time_config[2] = DECtoBCD(hours);
    time_config[3] = DECtoBCD(day);
    time_config[4] = DECtoBCD(date);
    time_config[5] = DECtoBCD(month);
    time_config[6] = DECtoBCD(year);

    Register_Burst_Write(REG_SECONDS, time_config, 7);
}

void RTCDS3234::WRITE_BUILD_DATETIME()
{
    uint8_t time_config[7];

    time_config[0] = DECtoBCD(BUILD_SECOND);
    time_config[1] = DECtoBCD(BUILD_MINUTE);
    time_config[2] = DECtoBCD(BUILD_HOUR);
    time_config[4] = DECtoBCD(BUILD_DATE);
    time_config[5] = DECtoBCD(BUILD_MONTH);
    time_config[6] = DECtoBCD(BUILD_YEAR - 2000);

    // Calculate weekday (from here: http://stackoverflow.com/a/21235587)
    // Result: 0 = Sunday, 6 = Saturday
    int d = BUILD_DATE;
    int m = BUILD_MONTH;
    int y = BUILD_YEAR;
    int weekday = (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;
    weekday += 1; // Library defines Sunday=1, Saturday=7
    time_config[3] = DECtoBCD(weekday);

    Register_Burst_Write(REG_SECONDS, time_config, 7);
}

std::string RTCDS3234::DATE_TOSTRING()
{
    std::stringstream ss;
    // date is day of month
    ss << std::setfill('0') << std::setw(2) << (int)month << "/";
    ss << std::setfill('0') << std::setw(2) << (int)date << "/";
    ss << (int)(year + 2000);

    return ss.str();
}

std::string RTCDS3234::TIME_TOSTRING()
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << (int)hours << ":";
    ss << std::setfill('0') << std::setw(2) << (int)minutes << ":";
    ss << std::setfill('0') << std::setw(2) << (int)seconds;
    if (hour12_not24)
    {
        ss << " " << (PM_notAM ? "PM" : "AM");
    }

    return ss.str();
}

void RTCDS3234::WRITE_SECONDS(uint8_t seconds)
{
    Register_Write(REG_SECONDS, DECtoBCD(seconds));
}

uint8_t RTCDS3234::READ_SECONDS()
{
    uint8_t reg_data = Register_Read(REG_SECONDS);
    uint8_t seconds = BCDtoDEC(reg_data);
    return seconds;
}

void RTCDS3234::WRITE_MINUTES(uint8_t minutes)
{
    Register_Write(REG_MINUTES, DECtoBCD(minutes));
}

uint8_t RTCDS3234::READ_MINUTES()
{
    uint8_t reg_data = Register_Read(REG_MINUTES);
    uint8_t minutes = BCDtoDEC(reg_data);
    return minutes;
}

void RTCDS3234::WRITE_12HOURS(uint8_t hours, bool PM_NotAM)
{
    uint8_t reg_data = 0x00;
    if (hours > 12)
        hours = 12;
    if (hours < 1)
        hours = 1;
    if (PM_NotAM)
    {
        reg_data |= HOUR_12_N24;
        reg_data |= PM_NOTAM;
        reg_data |= DECtoBCD(hours);
    }
    else
    {
        reg_data |= HOUR_12_N24;
        reg_data |= DECtoBCD(hours);
    }
    Register_Write(REG_HOURS, reg_data);
}

void RTCDS3234::WRITE_24HOURS(uint8_t hours)
{
    if (hours > 23)
        hours = 23;
    Register_Write(REG_HOURS, DECtoBCD(hours));
}

void RTCDS3234::WRITE_DAYS(DAYS days)
{
    Register_Write(REG_DAYS, BCDtoDEC((uint8_t)days));
}

void RTCDS3234::WRITE_DATE(uint8_t date)
{
    int year = BCDtoDEC(RTCDS3234::Register_Read(REG_YEAR)) + 2000;
    int month = BCDtoDEC(RTCDS3234::Register_Read(REG_MONTH));
    // Credit to : http://www.codecodex.com/wiki/Calculate_the_number_of_days_in_a_month
    // This protects against invalid months
    int numberOfDays;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        numberOfDays = 30;
    else if (month == 2)
    {
        bool isLeapYear = (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
        if (isLeapYear)
            numberOfDays = 29;
        else
            numberOfDays = 28;
    }
    else
        numberOfDays = 31;

    if (date > numberOfDays)
        date = numberOfDays;

    Register_Write(REG_DATE, DECtoBCD(date));
}

void RTCDS3234::WRITE_MONTH(uint8_t month)
{
    if (month > 12)
        month = 12;
    Register_Write(REG_MONTH, DECtoBCD(month));
}

void RTCDS3234::WRITE_YEAR(uint8_t year)
{
    if (year > 99)
        year = 99;
    Register_Write(REG_YEAR, DECtoBCD(year));
}

void RTCDS3234::WRITE_ALARM1(uint8_t seconds, uint8_t minutes,
                             uint8_t hours, uint8_t date, ALARM1_MODES mode)
{
    uint8_t alarm_config[4] = {0};

    alarm_config[0] = DECtoBCD(seconds);
    alarm_config[1] = DECtoBCD(minutes);
    alarm_config[2] = DECtoBCD(hours);
    alarm_config[3] = DECtoBCD(date);

    switch (mode)
    {
    case ALARM1_PER_SECOND:
        alarm_config[0] |= A1M1_Bit;
        alarm_config[1] |= A1M2_Bit;
        alarm_config[2] |= A1M3_Bit;
        alarm_config[3] |= A1M4_Bit;
        break;
    case ALARM1_SECONDS_MATCH:
        alarm_config[1] |= A1M2_Bit;
        alarm_config[2] |= A1M3_Bit;
        alarm_config[3] |= A1M4_Bit;
        break;
    default:
        break;
    }

    Register_Burst_Write(REG_ALARM1_SECONDS, alarm_config, 4);
}

void RTCDS3234::WRITE_ALARM2(uint8_t minutes,
                             uint8_t hours, uint8_t date, ALARM2_MODES mode)
{
    uint8_t alarm_config[3] = {0};

    alarm_config[0] = DECtoBCD(minutes);
    alarm_config[1] = DECtoBCD(hours);
    alarm_config[2] = DECtoBCD(date);

    switch (mode)
    {
    case ALARM2_PER_MIN:
        alarm_config[0] |= A2M2_Bit;
        alarm_config[1] |= A2M3_Bit;
        alarm_config[2] |= A2M4_Bit;
        break;
    case ALARM2_MIN_MATCH:
        alarm_config[1] |= A2M3_Bit;
        alarm_config[2] |= A2M4_Bit;
        break;
    default:
        break;
    }

    Register_Burst_Write(REG_ALRAM2_MINUTES, alarm_config, 3);
}

bool RTCDS3234::READ_ALARM1_FLAG()
{
    uint8_t status = Register_Read(REG_CONTROL_STATUS);

    if ((status & A1F_BIT) == A1F_BIT)
    {
        uint8_t mask = 0xFE;
        status &= mask;
        Register_Write(REG_CONTROL_STATUS, status);
        return true;
    }
    else
    {
        return false;
    }
}

bool RTCDS3234::READ_ALARM2_FLAG()
{
    uint8_t status = Register_Read(REG_CONTROL_STATUS);

    if ((status & A2F_BIT) == A2F_BIT)
    {
        uint8_t mask = 0xFD;
        status &= mask;
        Register_Write(REG_CONTROL_STATUS, status);
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Alarm ISR Handler
 * 
 * @param arg 
 */
static void IRAM_ATTR alert_isr_handler(void *arg)
{
    static BaseType_t xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR(semiphore, &xHigherPriorityTaskWoken);
}

void RTCDS3234::ISR_Init()
{

    semiphore = xSemaphoreCreateBinary();

    gpio_config_t io_conf;
    io_conf.intr_type = (gpio_int_type_t)GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1ULL << DEADON_ALERT_PIN_NUM;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);

    gpio_set_intr_type(DEADON_ALERT_PIN_NUM, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(DEADON_RTC_INTR_FLAGS_DEFAULT);

    gpio_isr_handler_add(DEADON_ALERT_PIN_NUM, alert_isr_handler, (void *)NULL);
}

void RTCDS3234::Enable_Interrupt(bool enable)
{
    uint8_t config = Register_Read(REG_CONTROL);
    uint8_t mask = 0xFB;
    config &= mask;

    if (enable)
    {
        config |= INTCN_BIT;
    }
    else
    {
        config |= 0x00;
    }

    Register_Write(REG_CONTROL, config);
    intr_enable = enable;
}

void RTCDS3234::Enable_Alarms(bool alarm1, bool alarm2)
{
    uint8_t config = Register_Read(REG_CONTROL);
    uint8_t mask = 0xFC;
    config &= mask;

    if (alarm1)
    {
        config |= A1IE_BIT;
    }
    if (alarm2)
    {
        config |= A2IE_BIT;
    }

    Register_Write(REG_CONTROL, config);
    alarm1_enable = alarm1;
    alarm2_enable = alarm2;
}

uint8_t RTCDS3234::SRAM_Read(uint8_t address)
{
    Register_Write(REG_SRAM_ADDR, address);
    return Register_Read(REG_SRAM_DATA);
}

void RTCDS3234::SRAM_Write(uint8_t address, uint8_t data)
{
    Register_Write(REG_SRAM_ADDR, address);
    Register_Write(REG_SRAM_DATA, address);
}

void RTCDS3234::SRAM_Burst_Read(uint8_t address, uint8_t *data, uint32_t len)
{
    Register_Write(REG_SRAM_ADDR, address);
    Register_Burst_Read(REG_SRAM_DATA, data, len);
}

void RTCDS3234::SRAM_Burst_Write(uint8_t address, uint8_t *data, uint32_t len)
{
    Register_Write(REG_SRAM_ADDR, address);
    Register_Burst_Write(REG_SRAM_DATA, data, len);
}

uint8_t RTCDS3234::Register_Read(uint8_t register_address)
{
    uint8_t data = spi.readReg(register_address & 0x7F);
    return data;
}

void RTCDS3234::Register_Write(uint8_t register_address, uint8_t data)
{
    spi.writeReg(register_address | 0x80, data);
}

void RTCDS3234::Register_Burst_Read(uint8_t address, uint8_t *data, uint32_t len)
{
    spi.burstRead(address & 0x7F, data, len);
}

void RTCDS3234::Register_Burst_Write(uint8_t address, uint8_t *data, uint32_t len)
{
    spi.burstWrite(address | 0x80, data, len);
}