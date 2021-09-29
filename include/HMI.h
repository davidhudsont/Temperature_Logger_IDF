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
        SETTING_TEMP
    };

    HMIState displayState = DISPLAYING;
    HMISettings settingState = SETTING_DATE;
    int entriesToEdit;

    void editMonth(bool increase);
    void editDayOfMonth(bool increase);
    void editYear(bool increase);
    void editingDate();
    void editHour(bool increase);
    void editMinute(bool increase);
    void editSecond(bool increase);
    void editingTime();
    void changeTemp();

    void editMode();
    void displayMode();
    void displayDate();
    void displayTime();
    void displayCurrentState();
    void displayTemperature();
    void updateDisplay();

public:
    HMI();
    void process();
    void setDisplayTemperature(float temperatureF, float temperatureC);
    void setDisplayDateTime(DATE_TIME &dateTime);
    HMIState getCurrentState();
};
