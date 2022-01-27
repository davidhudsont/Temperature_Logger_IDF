#include "LCD.h"

// Source https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library/blob/master/src/SerLCD.cpp

// -------------------------------------Static Functions--------------------------------------------------

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void delay(int ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

// -------------------------------------LCD Private Functions-----------------------------------------------------
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

// -------------------------------------LCD Public Functions-----------------------------------------------------
void LCD::Begin()
{
    uart.Setup(9600, UART2_TX_PIN_NUM, UART2_RX_PIN_NUM, UART_NUM_2);
    uart.ToggleRTS();

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

void LCD::Clear()
{
    Command(CLEAR_COMMAND);
    delay(10);
}

void LCD::Home()
{
    SpecialCommand(LCD_RETURNHOME);
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

void LCD::NoCursor()
{
    displayControl &= ~LCD_CURSORON;
    SpecialCommand(LCD_DISPLAYCONTROL | displayControl);
}

void LCD::Cursor()
{
    displayControl |= LCD_CURSORON;
    SpecialCommand(LCD_DISPLAYCONTROL | displayControl);
}

void LCD::NoBlink()
{
    displayControl &= ~LCD_BLINKON;
    SpecialCommand(LCD_DISPLAYCONTROL | displayControl);
}

void LCD::Blink()
{
    displayControl |= LCD_BLINKON;
    SpecialCommand(LCD_DISPLAYCONTROL | displayControl);
}

void LCD::MoveCursorLeft()
{
    SpecialCommand(LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT);
}

void LCD::MoveCursorRight()
{
    SpecialCommand(LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT);
}

void LCD::MoveCursorLeft(uint8_t count)
{
    SpecialCommand(LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT, count);
}

void LCD::MoveCursorRight(uint8_t count)
{
    SpecialCommand(LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT, count);
}

void LCD::SetBackLight(uint8_t r, uint8_t g, uint8_t b)
{

    uint8_t red = 128 + map(r, 0, 255, 0, 29);
    uint8_t green = 158 + map(g, 0, 255, 0, 29);
    uint8_t blue = 188 + map(b, 0, 255, 0, 29);

    BeginTransmit();

    displayControl &= ~LCD_DISPLAYON;
    Write(SPECIAL_COMMAND);
    Write(LCD_DISPLAYCONTROL | displayControl);

    Write(SETTING_COMMAND);
    Write(red);
    Write(SETTING_COMMAND);
    Write(green);
    Write(SETTING_COMMAND);
    Write(blue);

    displayControl |= LCD_DISPLAYON;
    Write(SPECIAL_COMMAND);
    Write(LCD_DISPLAYCONTROL | displayControl);
    EndTransmit();

    delay(50);
}

void LCD::SetBackLightFast(uint8_t r, uint8_t g, uint8_t b)
{
    //send commands to the display to set backlights
    BeginTransmit();        // transmit to device
    Write(SETTING_COMMAND); //Send special command character
    Write(SET_RGB_COMMAND); //Send the set RGB character '+' or plus
    Write(r);               //Send the red value
    Write(g);               //Send the green value
    Write(b);               //Send the blue value
    EndTransmit();          //Stop transmission
    delay(10);
}

void LCD::SetContrast(uint8_t contrast)
{
    BeginTransmit();
    Write(SETTING_COMMAND);
    Write(CONTRAST_COMMAND);
    Write(contrast);
    EndTransmit();

    delay(10);
}

void LCD::Command(uint8_t command)
{
    uint8_t commands[2] = {SETTING_COMMAND, command};

    BeginTransmit();
    WriteBurst(commands, 2);
    EndTransmit();

    delay(20);
}

void LCD::SpecialCommand(uint8_t command)
{
    uint8_t commands[2] = {SPECIAL_COMMAND, command};

    BeginTransmit();
    WriteBurst(commands, 2);
    EndTransmit();

    delay(50);
}

void LCD::SpecialCommand(uint8_t command, uint8_t count)
{
    uint8_t commands[2] = {SPECIAL_COMMAND, command};

    BeginTransmit();
    for (int i = 0; i < count; i++)
    {
        WriteBurst(commands, 2);
    }
    EndTransmit();

    delay(50);
}

void LCD::DisableSystemMessages()
{
    BeginTransmit();                       // transmit to device
    Write(SETTING_COMMAND);                //Send special command character
    Write(DISABLE_SYSTEM_MESSAGE_DISPLAY); //Send the set '.' character
    EndTransmit();                         //Stop transmission
    delay(10);
}

void LCD::ResetCursor()
{
    uint8_t commands[2] = {'|', '-'};

    BeginTransmit();
    WriteBurst(commands, 2);
    EndTransmit();

    delay(50);
}

void LCD::ClearRow(uint8_t row)
{
    if (row > 3)
        row = 3;
    SetCursor(row, 0);
    char emptyStr[] = "                    ";
    WriteCharacters(emptyStr, 20);
}

void LCD::WriteDigit(char digit)
{
    if (digit > 10)
        return;
    char c = digit + '0';
    WriteCharacter(c);
}
