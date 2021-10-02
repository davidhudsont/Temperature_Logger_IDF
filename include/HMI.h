#pragma once

#include "LCD.h"
#include "DEADONRTC.h"

enum HMIState
{
    DISPLAYING,
    EDITING
};

class HMI
{
private:
    LCD lcd;

    bool displayTempF_notC;
    float temperatureF;
    float temperatureC;
    DATE_TIME dateTime;

    enum HMISettings
    {
        SETTING_DATE,
        SETTING_TIME,
        SETTING_TEMP,
        SETTINGS_COUNT
    };

    HMIState displayState = DISPLAYING;
    HMISettings settingState = SETTING_DATE;
    int entriesToEdit;

    // Display Mode related functions
    void displayMode();
    void displayDate();
    void displayTime();
    void displayTemperature();
    void displayCurrentState();
    void updateDisplay();

    // Edit Mode related functions
    void editMode();
    void editMonth(bool increase);
    void editDayOfMonth(bool increase);
    void editYear(bool increase);
    void editingDate();
    void editHour(bool increase);
    void editMinute(bool increase);
    void editSecond(bool increase);
    void editingTime();
    void changeTemp();

public:
    HMI();
    void process();
    void setDisplayTemperature(float temperatureF, float temperatureC);
    void setDisplayDateTime(DATE_TIME &dateTime);
    HMIState getCurrentState();
};
