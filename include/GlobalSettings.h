#pragma once

#include "Setting.h"

class DateSetting : public Settings
{
public:
    DateSetting();
    std::string displayString();
};

class TimeSetting : public Settings
{
public:
    TimeSetting();
    std::string displayString();
};