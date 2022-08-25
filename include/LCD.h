#pragma once

// Source https://github.com/sparkfun/SparkFun_SerLCD_Arduino_Library/blob/master/src/SerLCD.h

#include <string>

// User Headers
#include "BSP_UART.h"

#define DISPLAY_ADDRESS 0x72

#define MAX_ROWS 4
#define MAX_COLUMNS 20

// OpenLCD command characters
#define SPECIAL_COMMAND 254  // Magic number for sending a special command
#define SETTING_COMMAND 0x7C // 124, |, the pipe character: The command to change settings: baud, lines, width, backlight, splash, etc

// OpenLCD commands
#define CLEAR_COMMAND 0x2D                  // 45, -, the dash character: command to clear and home the display
#define CONTRAST_COMMAND 0x18               // Command to change the contrast setting
#define ADDRESS_COMMAND 0x19                // Command to change the i2c address
#define SET_RGB_COMMAND 0x2B                // 43, +, the plus character: command to set backlight RGB value
#define ENABLE_SYSTEM_MESSAGE_DISPLAY 0x2E  // 46, ., command to enable system messages being displayed
#define DISABLE_SYSTEM_MESSAGE_DISPLAY 0x2F // 47, /, command to disable system messages being displayed
#define ENABLE_SPLASH_DISPLAY 0x30          // 48, 0, command to enable splash screen at power on
#define DISABLE_SPLASH_DISPLAY 0x31         // 49, 1, command to disable splash screen at power on
#define SAVE_CURRENT_DISPLAY_AS_SPLASH 0x0A // 10, Ctrl+j, command to save current text on display as splash

// special commands
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

const uint8_t DEGREE_SYMBOL = 223;

class LCD
{
public:
    LCD();
    ~LCD() = default;
    LCD(const LCD &) = delete;
    LCD &operator=(const LCD &) = delete;

    // Reset LCD DTR Pin
    void Reset();

    // Clear the screen of characters
    void Clear();
    // Return the cursors to starting position
    void Home();
    // Set the cursors position
    void SetCursor(uint8_t row, uint8_t col);
    // Create a custom character
    // void CreateCustomCharacter(uint8_t location, uint8_t charmap[]);
    // write a custom character to the LCD
    // void WriteCustomCharacter(uint8_t location);
    // Write a string of characters
    void WriteString(std::string &str);
    // Write a string of characters
    void WriteCharacters(const char *str, uint32_t len);
    // Write a single character
    void WriteCharacter(char c);
    // Disable displaying characters
    void NoDisplay();
    // Enable displaying characters
    void Display();
    // Disable the underline cursor
    void NoCursor();
    // Enable the underline cursor
    void Cursor();
    // Disable cursor blinking
    void NoBlink();
    // Enable cursor blinking
    void Blink();

    // void scrollDisplayLeft();
    // void scrollDisplayRight();
    // void scrollDisplayLeft(uint8_t count);
    // void scrollDisplayRight(uint8_t count);
    void MoveCursorLeft();
    void MoveCursorRight();
    void MoveCursorLeft(uint8_t count);
    void MoveCursorRight(uint8_t count);
    // void setBacklight(unsigned long rgb);
    void SetBackLight(uint8_t r, uint8_t g, uint8_t b);
    // void setFastBacklight(unsigned long rgb);
    void SetBackLightFast(uint8_t r, uint8_t g, uint8_t b);
    // void leftToRight();
    // void rightToLeft();
    // void autoscroll();
    // void noAutoscroll();
    void SetContrast(uint8_t contrast);
    // void setAddress(uint8_t new_addr);
    // Settings command
    void Command(uint8_t command);
    // LCD special command
    void SpecialCommand(uint8_t command);
    // LCD special command, count times
    void SpecialCommand(uint8_t command, uint8_t count);

    // void enableSystemMessages();
    void DisableSystemMessages();
    // void enableSplash();
    // void disableSplash();
    // void saveSplash();
    // uint8_t getAddress();

    // Clears the screen and reset the cursor
    void ResetCursor();

    void ClearRow(uint8_t row);
    void WriteDigit(char digit);

private:
    BSP::UART uart;
    uint8_t displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    uint8_t displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    void BeginTransmit();
    void Write(uint8_t data);
    void WriteBurst(uint8_t *data, uint32_t len);
    void EndTransmit();
};
