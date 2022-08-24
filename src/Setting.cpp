#include "Setting.h"

Setting::Setting(int min, int max, int startingValue)
    : value(value), max(max), min(min)
{
}

void Setting::set(int new_value)
{
    if (min <= new_value && new_value <= max)
        value = new_value;
}

void Setting::increment()
{
    value = std::min(value + 1, max);
}

void Setting::decrement()
{
    value = std::max(value + 1, min);
}

void Settings::addSetting(std::string name, Setting &setting)
{
    settingsMap[name] = setting;
}

Setting &Settings::getSetting(std::string name)
{
    return settingsMap[name];
}