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

class AlarmEnableSetting : public Settings
{
public:
    AlarmEnableSetting();
    std::string displayString();
};

class TemperatureSetting : public Settings
{
public:
    TemperatureSetting();
    std::string displayString();
};

class BacklightSetting : public Settings
{
public:
    BacklightSetting();
    std::string displayString();
};