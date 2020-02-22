/**
 * @file DEADONRTC.c
 * @author David Hudson
 * @brief 
 * @date 2020-01-19
 * 
 */


#include "DEADONRTC.h"
#include "string.h"

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
#define BUILD_MONTH BUILD_MONTH_JAN | BUILD_MONTH_FEB | BUILD_MONTH_MAR | \
                    BUILD_MONTH_APR | BUILD_MONTH_MAY | BUILD_MONTH_JUN | \
                    BUILD_MONTH_JUL | BUILD_MONTH_AUG | BUILD_MONTH_SEP | \
                    BUILD_MONTH_OCT | BUILD_MONTH_NOV | BUILD_MONTH_DEC
// <DATE>
#define BUILD_DATE_0 ((__DATE__[4] == ' ') ? 0 : (__DATE__[4] - 0x30))
#define BUILD_DATE_1 (__DATE__[5] - 0x30)
#define BUILD_DATE ((BUILD_DATE_0 * 10) + BUILD_DATE_1)
// <YEAR>
#define BUILD_YEAR (((__DATE__[7] - 0x30) * 1000) + ((__DATE__[8] - 0x30) * 100) + \
                    ((__DATE__[9] - 0x30) * 10)  + ((__DATE__[10] - 0x30) * 1))

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


/**
 * @brief Convert a Binary Coded Decimal number
 *        into a decimal number
 * 
 * @param bcd 
 * @return uint8_t 
 */
static uint8_t BCDtoDEC(uint8_t bcd)
{
    uint8_t dec = ( ( bcd / 0x10) * 10 ) + ( bcd % 0x10 );
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
    uint8_t bcd = ( ((dec / 10 ) * 0x10) | (dec % 10) );
    return bcd;
}


/**
 * @brief Initialize the RTC device structure
 *        and start the SPI bus.
 * 
 * @param rtc 
 */
void DEADON_RTC_Begin(DEADONRTC * rtc)
{
    memset(rtc, 0, sizeof(DEADONRTC));

    volatile int clock_speed = 4*1000*1000; // Clock speed 4MHz
    esp_err_t err = false;
    err = BSP_SPI_Init(clock_speed);
    if (err != ESP_OK)
    {
        printf("Spi Configuration Error\n");
    }
}


/**
 * @brief Read the current data and time from the RTC.
 * 
 * @param rtc 
 */
void DEADON_RTC_READ_DATETIME(DEADONRTC * rtc)
{
    DEADON_RTC_Register_Burst_Read(REG_SECONDS, rtc->raw_time, 7);

    rtc->seconds = BCDtoDEC(rtc->raw_time[0]);
    rtc->minutes = BCDtoDEC(rtc->raw_time[1]);

    rtc->hour12_not24 = (rtc->raw_time[2] & HOUR_12_N24) == HOUR_12_N24;
    if (rtc->hour12_not24)
    {
        rtc->PM_notAM = (rtc->raw_time[2] & PM_NOTAM) == PM_NOTAM;
        rtc->hours = BCDtoDEC(rtc->raw_time[2] & HOURS_BITS_12);
    }
    else
    {
        rtc->hours   = BCDtoDEC(rtc->raw_time[2] & HOURS_BITS_24);
    }
    rtc->day     = BCDtoDEC(rtc->raw_time[3]);
    rtc->date    = BCDtoDEC(rtc->raw_time[4]);
    rtc->century = (rtc->raw_time[5] & CENTURY_BIT) ==  CENTURY_BIT;
    rtc->month   = BCDtoDEC(rtc->raw_time[5] & MONTHS_BITS);
    rtc->year    = BCDtoDEC(rtc->raw_time[6]);
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
void DEADON_RTC_WRITE_DATETIME(uint8_t seconds, uint8_t minutes, uint8_t hours,
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

    DEADON_RTC_Register_Burst_Write(REG_SECONDS, time_config, 7);
}


/**
 * @brief Get the date and time from the build date
 * 
 */
void DEADON_RTC_WRITE_BUILD_DATETIME()
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
	int weekday = (d+=m<3?y--:y-2,23*m/9+d+4+y/4-y/100+y/400)%7;
	weekday += 1; // Library defines Sunday=1, Saturday=7
    time_config[3] = DECtoBCD(weekday);

    DEADON_RTC_Register_Burst_Write(REG_SECONDS, time_config, 7);

}



void DEADON_RTC_WRITE_SECONDS(uint8_t seconds)
{
    DEADON_RTC_Register_Write(REG_SECONDS, DECtoBCD(seconds));

}


uint8_t DEADON_RTC_READ_SECONDS()
{
    uint8_t reg_data = DEADON_RTC_Register_Read(REG_SECONDS);
    uint8_t seconds = BCDtoDEC(reg_data);
    return seconds;

}


void DEADON_RTC_WRITE_MINUTES(uint8_t minutes)
{
    DEADON_RTC_Register_Write(REG_MINUTES, DECtoBCD(minutes));

}

uint8_t DEADON_RTC_READ_MINUTES()
{
    uint8_t reg_data = DEADON_RTC_Register_Read(REG_MINUTES);
    uint8_t minutes = BCDtoDEC(reg_data);
    return minutes;

}


// input : hours 1-12
void DEADON_RTC_WRITE_12HOURS(uint8_t hours, bool PM_NotAM)
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
    DEADON_RTC_Register_Write(REG_HOURS, reg_data);
}


// input : hours 0-23
void DEADON_RTC_WRITE_24HOURS(uint8_t hours)
{
    if (hours > 23)
        hours = 23;
    DEADON_RTC_Register_Write(REG_HOURS, DECtoBCD(hours));
}


/**
 * @brief 
 * 
 * @param seconds 
 * @param minutes 
 * @param hours 
 * @param date 
 * @param mode 
 */
void DEADON_RTC_WRITE_ALARM1(uint8_t seconds, uint8_t minutes, 
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

    DEADON_RTC_Register_Burst_Write(REG_ALARM1_SECONDS, alarm_config, 4);
}


/**
 * @brief 
 * 
 * @param minutes 
 * @param hours 
 * @param date 
 * @param mode 
 */
void DEADON_RTC_WRITE_ALARM2(uint8_t minutes, 
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

    DEADON_RTC_Register_Burst_Write(REG_ALRAM2_MINUTES, alarm_config, 3);
}


/**
 * @brief 
 * 
 * @param rtc 
 * @return true 
 * @return false 
 */
bool DEADON_RTC_READ_ALARM1_FLAG(DEADONRTC * rtc)
{
    uint8_t status = DEADON_RTC_Register_Read(REG_CONTROL_STATUS);

    if ((status & A1F_BIT) == A1F_BIT)
    {
        uint8_t mask = 0xFE;
        status &= mask;
        DEADON_RTC_Register_Write(REG_CONTROL_STATUS, status);
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * @brief Read Alarm 2 flag
 * 
 * @param rtc - DEADONRTC device structure
 * @return true if flag was set, false otherwise
 */
bool DEADON_RTC_READ_ALARM2_FLAG(DEADONRTC * rtc)
{
    uint8_t status = DEADON_RTC_Register_Read(REG_CONTROL_STATUS);

    if ((status & A2F_BIT) == A2F_BIT)
    {
        uint8_t mask = 0xFD;
        status &= mask;
        DEADON_RTC_Register_Write(REG_CONTROL_STATUS, status);
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
static void IRAM_ATTR alert_isr_handler(void * arg)
{
    char msg = 'r';
    BaseType_t base;
    xQueueSendFromISR(queue, (void * )&msg, &base);

}


/**
 * @brief Initialize ISR for Alert Pin
 * 
 * @param rtc 
 */
void DEADON_RTC_ISR_Init(DEADONRTC * rtc)
{
    gpio_config_t io_conf;

    queue = xQueueCreate(3, 1);

    io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;

    io_conf.pin_bit_mask = 1ULL << DEADON_ALERT_PIN_NUM;

    io_conf.mode = GPIO_MODE_INPUT;

    io_conf.pull_down_en = false;
    io_conf.pull_up_en = true;

    gpio_config(&io_conf);

    gpio_set_intr_type(DEADON_ALERT_PIN_NUM, GPIO_PIN_INTR_NEGEDGE);

    gpio_install_isr_service(DEADON_RTC_INTR_FLAGS_DEFAULT);

    gpio_isr_handler_add(DEADON_ALERT_PIN_NUM, alert_isr_handler, (void*) rtc);

}


/**
 * @brief Enable the RTC's interrupts
 * 
 * @param rtc 
 * @param enable 
 */
void DEADON_RTC_Enable_Interrupt(DEADONRTC * rtc, bool enable)
{
    uint8_t config = DEADON_RTC_Register_Read(REG_CONTROL);
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

    DEADON_RTC_Register_Write(REG_CONTROL, config);
    rtc->intr_enable = enable;

}


/**
 * @brief Enable Alarm 1 and/or Alarm 2
 * 
 * @param rtc - DEADONRTC device structure
 * @param alarm1 - enable alarm 1 if true
 * @param alarm2 - enable alarm 2 if true
 */
void DEADON_RTC_Enable_Alarms(DEADONRTC * rtc, bool alarm1, bool alarm2)
{
    uint8_t config = DEADON_RTC_Register_Read(REG_CONTROL);
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

    DEADON_RTC_Register_Write(REG_CONTROL, config);
    rtc->alarm1_enable = alarm1;
    rtc->alarm2_enable = alarm2;

}


/**
 * @brief Read the RTC's SRAM at a specified address
 * @note The size of the memoy if 256 bytes
 * @param address range: 0x00 - 0xff
 * @return uint8_t 
 */
uint8_t DEADON_RTC_SRAM_Read(uint8_t address)
{
    DEADON_RTC_Register_Write(REG_SRAM_ADDR, address);
    return DEADON_RTC_Register_Read(REG_SRAM_DATA);

}


/**
 * @brief Write to the RTC's SRAM at a specified address
 * @note The size of the memoy if 256 bytes
 * @param address range: 0x00 - 0xff
 * @param data - The data we want to write 
 */
void DEADON_RTC_SRAM_Write(uint8_t address, uint8_t data)
{
    DEADON_RTC_Register_Write(REG_SRAM_ADDR, address);
    DEADON_RTC_Register_Write(REG_SRAM_DATA, address);

}


/**
 * @brief Read the RTC's SRAM at a specified address
 * @note The size of the memoy if 256 bytes
 * @param address - Starting address
 * @param data - data buffer
 * @param len - Number of bytes to read
 */
void DEADON_RTC_SRAM_Burst_Read(uint8_t address, uint8_t * data, uint32_t len)
{
    DEADON_RTC_Register_Write(REG_SRAM_ADDR, address);
    DEADON_RTC_Register_Burst_Read(REG_SRAM_DATA, data, len);
}


/**
 * @brief Write the RTC's SRAM at a specified address
 * @note The size of the memoy if 256 bytes
 * @param address - Starting address
 * @param data - data buffer
 * @param len - Number of bytes to write
 */
void DEADON_RTC_SRAM_Burst_Write(uint8_t address, uint8_t * data, uint32_t len)
{
    DEADON_RTC_Register_Write(REG_SRAM_ADDR, address);
    DEADON_RTC_Register_Burst_Write(REG_SRAM_DATA, data, len);    
}


/**
 * @brief Read a register
 * 
 * @param register_address 
 * @return uint8_t 
 */
uint8_t DEADON_RTC_Register_Read(uint8_t register_address)
{
    uint8_t data = SPI_readReg(register_address & 0x7F);
    return data;
}


/**
 * @brief Write to a register
 * 
 * @param register_address 
 * @param data 
 */
void DEADON_RTC_Register_Write(uint8_t register_address, uint8_t data)
{
    SPI_writeReg(register_address | 0x80, data);
}


/**
 * @brief Read multiple registers
 * 
 * @param address 
 * @param data 
 * @param len 
 */
void DEADON_RTC_Register_Burst_Read(uint8_t address, uint8_t * data, uint32_t len)
{
    SPI_Burst_Read(address & 0x7F, data, len);

}


/**
 * @brief Write to multiple registers
 * 
 * @param address 
 * @param data 
 * @param len 
 */
void DEADON_RTC_Register_Burst_Write(uint8_t address, uint8_t * data, uint32_t len)
{
    SPI_Burst_Write(address | 0x80, data, len);

}