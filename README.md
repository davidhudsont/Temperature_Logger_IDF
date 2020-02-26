# Temperature_Logger IDF Project
A temperature logging device using the ESP32 Micro-controller with the IDF RTOS

## Devices:

1. TMP102 Temperature Sensor
2. DS3234 Real Time Clock
3. SparkFun OpenLog

# Console Commands

* 'help' - Displays the various console commands and options

* 'version' - Prints information related to the ESP32 Chip
  
* 'time' - Set the time of the Real Time Clock (RTC)
  
  * Argument -s: Set the seconds of the RTC (0-59) seconds
  * Argument -m: Set the minutes of the RTC (0-59) minutes
  * Argument -h: Set the hours of the RTC in 12 hour mode (1-12)
  * Argument -h: Add a second argument to set RTC in either AM or PM time
  * Argument -t: Set the hours of the RTC in 34 hour mode (0-23) 

* 'datetime' - Set the current date and time

## Console Examples

* time -s 32 (Set clock to second 32)
* time -m 59 (Set clock to minute 59)
* time -h 7 -h 0 (Set clock to hour 7 AM)
* time -t 13 (Set clock to hour 13)
# Links

## TMP102

* [TMP102 DataSheet](https://www.sparkfun.com/datasheets/Sensors/Temperature/tmp102.pdf)
* [TMP102 Hookup Guide](https://learn.sparkfun.com/tutorials/tmp102-digital-temperature-sensor-hookup-guide)

## DS3234 DeadOn RTC

* [DS3234 DataSheet](https://www.sparkfun.com/datasheets/BreakoutBoards/DS3234.pdf)
* [DS3234 Hookup Guide](https://learn.sparkfun.com/tutorials/deadon-rtc-breakout-hookup-guide)


## SparkFun OpenLog

* [OpenLog Hookup Guide](https://learn.sparkfun.com/tutorials/openlog-hookup-guide)
