# Clock Display IDF Project

A device that displays the current date, time and temperature to a LCD display. The project is powered using the ESP32 Micro-controller with the IDF RTOS.

## Devices

1. TMP102 Temperature Sensor
   * [TMP102 DataSheet](https://www.sparkfun.com/datasheets/Sensors/Temperature/tmp102.pdf)
   * [TMP102 Hookup Guide](https://learn.sparkfun.com/tutorials/tmp102-digital-temperature-sensor-hookup-guide)
2. DS3234 Real Time Clock
   * [DS3234 DataSheet](https://www.sparkfun.com/datasheets/BreakoutBoards/DS3234.pdf)
   * [DS3234 Hookup Guide](https://learn.sparkfun.com/tutorials/deadon-rtc-breakout-hookup-guide)
3. SparkFun 20x4 SerLCD - RGB Backlight (Qwiic)
   * [LCD Display](https://www.sparkfun.com/products/16398)
4. SparkFun Breadboard Power Supply - 5V/3.3V
   * [Power Supply](https://www.sparkfun.com/products/13032)

## Console Commands

* 'help' - Displays the various console commands and options

* 'time' - Set the time of the Real Time Clock (RTC)
  * -s <0-59>: Set the seconds of the RTC seconds
  * -m <0-59>:Set the minutes of the RTC minutes
  * -h <1-12>: Set the hours of the RTC in 12 hour mode
  * -h <0-1>: Add a second argument to set RTC in either AM or PM time
  * -t <0-23>: Set the hours of the RTC in 24 hour mode

* 'date' - Set the current date
  * -w <1-7>: Set the weekday
  * -d <1-31>: Set the day of month
  * -m <1-12>: Set the month
  * -y <0-99>: Set the year

* 'datetime' - Print the current date and time to the console

* 'temperature' - Print the current temperature to the console
  * -f: Print the temperature in Fahrenheit
  * -c: Print the temperature in Celsius

* 'loglevel' - Set the current log level
  * -l <0-5>: Set the current log level

* 'lcd' - Controls various functions of the LCD Display
  * -d <0-1>: Turn Display On/Off
  * -c <0-255>: Set the LCD's Contrast
  * -b <0-255>r <0-255>g <0-255>b: Set the backlight color
  * -r: Clear the display

## Schematic and PCB

* [Schematic PDF](hardware/Schematic_ClockAndThermometer.pdf)
* [PCB PDF](hardware/PCB_ClockAndThermometer.pdf)
