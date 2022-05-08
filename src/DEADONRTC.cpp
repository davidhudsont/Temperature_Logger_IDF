
#include "DEADONRTC.h"
#include "DEADONRTC_Registers.h"
#include <string.h>
#include "esp_log.h"
#include "freertos/semphr.h"

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
    : spi(SPI_CLOCK_SPEED), intr_enable(false), alarm1_enable(false), alarm2_enable(false),
      seconds(0), minutes(0), hours(0), day(0), date(0), month(0), year(0),
      hour12_not24(false), PM_notAM(false), century(false)
{
    memset(&raw_time, 0, sizeof(uint8_t));
}

void RTCDS3234::ReadDateTime()
{
    RegisterBurstRead(REG_SECONDS, raw_time, 7);

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

void RTCDS3234::WriteDateTime(uint8_t second, uint8_t minute, uint8_t hour,
                              uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    uint8_t time_config[7];

    time_config[0] = DECtoBCD(second);
    time_config[1] = DECtoBCD(minute);
    time_config[2] = DECtoBCD(hour);
    time_config[3] = DECtoBCD(day);
    time_config[4] = DECtoBCD(date);
    time_config[5] = DECtoBCD(month);
    time_config[6] = DECtoBCD(year);

    RegisterBurstWrite(REG_SECONDS, time_config, 7);
}

void RTCDS3234::WriteDate(uint8_t date, uint8_t month, uint8_t year)
{
    uint8_t date_config[3];

    date_config[0] = DECtoBCD(date);
    date_config[1] = DECtoBCD(month);
    date_config[2] = DECtoBCD(year);

    RegisterBurstWrite(REG_DATE, date_config, 3);
}

void RTCDS3234::WriteTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t time_config[3] = {0};

    time_config[0] = DECtoBCD(second);
    time_config[1] = DECtoBCD(minute);
    time_config[2] = DECtoBCD(hour);

    RegisterBurstWrite(REG_SECONDS, time_config, 3);
}

void RTCDS3234::WriteTime12(uint8_t hour, uint8_t minute, uint8_t second, bool PM_NotAM)
{
    uint8_t time_config[3] = {0};

    time_config[0] = DECtoBCD(second);
    time_config[1] = DECtoBCD(minute);

    if (hour > 12)
        hour = 12;
    else if (hour < 1)
        hour = 1;

    if (PM_NotAM)
    {
        time_config[2] |= HOUR_12_N24;
        time_config[2] |= PM_NOTAM;
        time_config[2] |= DECtoBCD(hour);
    }
    else
    {
        time_config[2] |= HOUR_12_N24;
        time_config[2] |= DECtoBCD(hour);
    }

    RegisterBurstWrite(REG_SECONDS, time_config, 3);
}

void RTCDS3234::WriteBuildDateTime()
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

    RegisterBurstWrite(REG_SECONDS, time_config, 7);
}

void RTCDS3234::WriteBuildDateTime12()
{
    uint8_t time_config[7];

    time_config[0] = DECtoBCD(BUILD_SECOND);
    time_config[1] = DECtoBCD(BUILD_MINUTE);
    uint8_t hourOut;
    bool PM_notAMOut;
    ConvertTo12Hours(BUILD_HOUR, hourOut, PM_notAMOut);
    if (PM_notAMOut)
    {
        time_config[2] |= HOUR_12_N24;
        time_config[2] |= PM_NOTAM;
        time_config[2] |= DECtoBCD(hourOut);
    }
    else
    {
        time_config[2] |= HOUR_12_N24;
        time_config[2] |= DECtoBCD(hourOut);
    }
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

    RegisterBurstWrite(REG_SECONDS, time_config, 7);
}

DATE_TIME RTCDS3234::GetDateTime()
{
    DATE_TIME dateTime;
    dateTime.year = year;
    dateTime.month = month;
    dateTime.dayofMonth = date;
    dateTime.hour = hours;
    dateTime.minute = minutes;
    dateTime.second = seconds;
    dateTime.hour12_not24 = hour12_not24;
    dateTime.PM_notAM = PM_notAM;

    return dateTime;
}

void RTCDS3234::WriteSeconds(uint8_t second)
{
    RegisterWrite(REG_SECONDS, DECtoBCD(second));
}

uint8_t RTCDS3234::ReadSeconds()
{
    uint8_t reg_data = RegisterRead(REG_SECONDS);
    uint8_t seconds = BCDtoDEC(reg_data);
    return seconds;
}

void RTCDS3234::WriteMinutes(uint8_t minute)
{
    RegisterWrite(REG_MINUTES, DECtoBCD(minute));
}

uint8_t RTCDS3234::ReadMinutes()
{
    uint8_t reg_data = RegisterRead(REG_MINUTES);
    uint8_t minutes = BCDtoDEC(reg_data);
    return minutes;
}

void RTCDS3234::Write12Hours(uint8_t hour, bool PM_NotAM)
{
    uint8_t reg_data = 0x00;
    if (hour > 12)
        hour = 12;
    if (hour < 1)
        hour = 1;
    if (PM_NotAM)
    {
        reg_data |= HOUR_12_N24;
        reg_data |= PM_NOTAM;
        reg_data |= DECtoBCD(hour);
    }
    else
    {
        reg_data |= HOUR_12_N24;
        reg_data |= DECtoBCD(hour);
    }
    RegisterWrite(REG_HOURS, reg_data);
}

void RTCDS3234::Write24Hours(uint8_t hour)
{
    if (hour > 23)
        hour = 23;
    RegisterWrite(REG_HOURS, DECtoBCD(hour));
}

void RTCDS3234::WriteDays(DAYS day)
{
    RegisterWrite(REG_DAYS, BCDtoDEC((uint8_t)day));
}

void RTCDS3234::WriteDate(uint8_t date)
{
    int year = BCDtoDEC(RTCDS3234::RegisterRead(REG_YEAR)) + 2000;
    int month = BCDtoDEC(RTCDS3234::RegisterRead(REG_MONTH));
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

    RegisterWrite(REG_DATE, DECtoBCD(date));
}

void RTCDS3234::WriteMonth(uint8_t month)
{
    if (month > 12)
        month = 12;
    RegisterWrite(REG_MONTH, DECtoBCD(month));
}

void RTCDS3234::WriteYear(uint8_t year)
{
    if (year > 99)
        year = 99;
    RegisterWrite(REG_YEAR, DECtoBCD(year));
}

void RTCDS3234::WriteAlarm1(uint8_t second, uint8_t minute,
                            uint8_t hour, uint8_t date, ALARM1_MODES mode)
{
    uint8_t alarm_config[4] = {0};

    alarm_config[0] = DECtoBCD(second);
    alarm_config[1] = DECtoBCD(minute);
    alarm_config[2] = DECtoBCD(hour);
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
    case ALARM1_HR_MIN_SEC_MATCH:
        alarm_config[3] |= A1M4_Bit;
        break;
    default:
        break;
    }

    RegisterBurstWrite(REG_ALARM1_SECONDS, alarm_config, 4);
}

void RTCDS3234::WriteAlarm1(uint8_t hour, uint8_t minute)
{
    uint8_t alarm_config[2] = {0};
    uint8_t current_config[2] = {0};
    RegisterBurstRead(REG_ALARM1_MINUTES, current_config, 2);
    uint8_t mask = 0x80;
    current_config[0] &= mask;
    current_config[1] &= mask;
    alarm_config[0] = DECtoBCD(minute) | current_config[0];
    alarm_config[1] = DECtoBCD(hour) | current_config[1];

    RegisterBurstWrite(REG_ALARM1_MINUTES, alarm_config, 2);
}

void RTCDS3234::WriteAlarm1(uint8_t hour, uint8_t minute, bool PM_NotAM)
{
    uint8_t alarm_config[2] = {0};
    uint8_t current_config[2] = {0};
    RegisterBurstRead(REG_ALARM1_MINUTES, current_config, 2);
    uint8_t mask = 0x80;
    current_config[0] &= mask;
    current_config[1] &= mask;
    alarm_config[0] = DECtoBCD(minute) | current_config[0];
    alarm_config[1] = current_config[1];
    if (hour > 12)
        hour = 12;
    if (hour < 1)
        hour = 1;
    if (PM_NotAM)
    {
        alarm_config[1] |= HOUR_12_N24;
        alarm_config[1] |= PM_NOTAM;
        alarm_config[1] |= DECtoBCD(hour);
    }
    else
    {
        alarm_config[1] |= HOUR_12_N24;
        alarm_config[1] |= DECtoBCD(hour);
    }

    RegisterBurstWrite(REG_ALARM1_MINUTES, alarm_config, 2);
}

void RTCDS3234::WriteAlarm2(uint8_t minute,
                            uint8_t hour, uint8_t date, ALARM2_MODES mode)
{
    uint8_t alarm_config[3] = {0};

    alarm_config[0] = DECtoBCD(minute);
    alarm_config[1] = DECtoBCD(hour);
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

    RegisterBurstWrite(REG_ALARM2_MINUTES, alarm_config, 3);
}

bool RTCDS3234::ReadAlarm1Flag()
{
    uint8_t status = RegisterRead(REG_CONTROL_STATUS);

    if ((status & A1F_BIT) == A1F_BIT)
    {
        uint8_t mask = 0xFE;
        status &= mask;
        RegisterWrite(REG_CONTROL_STATUS, status);
        return true;
    }
    else
    {
        return false;
    }
}

bool RTCDS3234::ReadAlarm2Flag()
{
    uint8_t status = RegisterRead(REG_CONTROL_STATUS);

    if ((status & A2F_BIT) == A2F_BIT)
    {
        uint8_t mask = 0xFD;
        status &= mask;
        RegisterWrite(REG_CONTROL_STATUS, status);
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

void RTCDS3234::ISRInitialize()
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

    gpio_isr_handler_add(DEADON_ALERT_PIN_NUM, alert_isr_handler, (void *)NULL);
}

void RTCDS3234::EnableInterrupt(bool enable)
{
    uint8_t config = RegisterRead(REG_CONTROL);
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

    RegisterWrite(REG_CONTROL, config);
    intr_enable = enable;
}

void RTCDS3234::EnableAlarms(bool alarm1, bool alarm2)
{
    uint8_t config = RegisterRead(REG_CONTROL);
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

    RegisterWrite(REG_CONTROL, config);
    alarm1_enable = alarm1;
    alarm2_enable = alarm2;
}

uint8_t RTCDS3234::SRAMRead(uint8_t address)
{
    RegisterWrite(REG_SRAM_ADDR, address);
    return RegisterRead(REG_SRAM_DATA);
}

void RTCDS3234::SRAMWrite(uint8_t address, uint8_t data)
{
    RegisterWrite(REG_SRAM_ADDR, address);
    RegisterWrite(REG_SRAM_DATA, address);
}

void RTCDS3234::SRAMBurstRead(uint8_t address, uint8_t *data, uint32_t len)
{
    RegisterWrite(REG_SRAM_ADDR, address);
    RegisterBurstRead(REG_SRAM_DATA, data, len);
}

void RTCDS3234::SRAMBurstWrite(uint8_t address, uint8_t *data, uint32_t len)
{
    RegisterWrite(REG_SRAM_ADDR, address);
    RegisterBurstWrite(REG_SRAM_DATA, data, len);
}

uint8_t RTCDS3234::RegisterRead(uint8_t register_address)
{
    uint8_t data = spi.ReadRegister(register_address & 0x7F);
    return data;
}

void RTCDS3234::RegisterWrite(uint8_t register_address, uint8_t data)
{
    spi.WriteRegister(register_address | 0x80, data);
}

void RTCDS3234::RegisterBurstRead(uint8_t address, uint8_t *data, uint32_t len)
{
    spi.BurstRead(address & 0x7F, data, len);
}

void RTCDS3234::RegisterBurstWrite(uint8_t address, uint8_t *data, uint32_t len)
{
    spi.BurstWrite(address | 0x80, data, len);
}

void RTCDS3234::RegisterDump()
{
    int size = 0x13;
    uint8_t registers[size] = {0};
    RegisterBurstRead(0x00, registers, size);
    ESP_LOGI("RTC", "Dumping registers");
    for (int i = 0; i < size; i++)
    {
        printf("REG[0x%02x] = [0x%02x]\n", i, registers[i]);
    }
}
