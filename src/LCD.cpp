#include "LCD.h"

static void delay(int ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void LCD::Begin()
{
    uart.Begin(9600, UART2_TX_PIN_NUM, UART2_RX_PIN_NUM, UART_NUM_2);

    delay(20);

    BeginTransmit();
    Write(SPECIAL_COMMAND);
    Write(LCD_DISPLAYCONTROL | displayControl);
    Write(SPECIAL_COMMAND);
    Write(LCD_ENTRYMODESET | displayMode);
    Write(SETTING_COMMAND);
    Write(CLEAR_COMMAND);
    EndTransmit();
    delay(50);
}

void LCD::BeginTransmit()
{
}

void LCD::Write(uint8_t data)
{
    uart.Write(data);
}

void LCD::WriteBurst(uint8_t *data, uint32_t len)
{
    uart.WriteBurst(data, len);
}

void LCD::EndTransmit()
{
}

void LCD::SpecialCommand(uint8_t command)
{
    uint8_t commands[2] = {SPECIAL_COMMAND, command};

    BeginTransmit();
    WriteBurst(commands, 2);
    EndTransmit();

    delay(50);
}

void LCD::Command(uint8_t command)
{
    uint8_t commands[2] = {SETTING_COMMAND, command};

    BeginTransmit();
    WriteBurst(commands, 2);
    EndTransmit();

    delay(20);
}

void LCD::Display()
{
    displayControl |= LCD_DISPLAYON;
    SpecialCommand(LCD_DISPLAYCONTROL | displayControl);
}

void LCD::NoDisplay()
{
    displayControl &= ~LCD_DISPLAYON;
    SpecialCommand(LCD_DISPLAYCONTROL | displayControl);
}

void LCD::ResetCursor()
{
    uint8_t commands[2] = {'|', '-'};

    BeginTransmit();
    WriteBurst(commands, 2);
    EndTransmit();

    delay(50);
}

void LCD::WriteCharacters(const char *str, uint32_t len)
{

    BeginTransmit();
    WriteBurst((uint8_t *)str, len);
    EndTransmit();

    delay(10);
}

void LCD::WriteCharacter(char c)
{
    BeginTransmit();
    Write(c);
    EndTransmit();

    delay(10);
}

void LCD::SetCursor(uint8_t row, uint8_t col)
{
    uint8_t row_offsets[4] = {0x00, 0x40, 0x14, 0x54};

    if (row > 3)
        row = 3;
    if (col > 19)
        col = 19;

    SpecialCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}