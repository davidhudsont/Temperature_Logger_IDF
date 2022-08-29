#pragma once

#include "Setting.h"

class DateSetting : public Settings
{
public:
    DateSetting();
    std::string displayString();
    bool getInput(const Input &input);
};

class TimeSetting : public Settings
{
public:
    TimeSetting();
    std::string displayString();
    bool getInput(const Input &input);
};

class AlarmSetting : public Settings
{
    AlarmSetting();
    std::string displayString();
    bool getInput(const Input &input);
};

class AlarmEnableSetting : public Settings
{
public:
    AlarmEnableSetting();
    std::string displayString();
    bool getInput(const Input &input);
};

class TemperatureSetting : public Settings
{
public:
    TemperatureSetting();
    std::string displayString();
    bool getInput(const Input &input);
};

class BacklightSetting : public Settings
{
public:
    BacklightSetting();
    std::string displayString();
    bool getInput(const Input &input);
};