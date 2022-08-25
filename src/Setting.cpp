#include "Setting.h"

Setting::Setting(int min, int max, int startingValue)
    : value(startingValue), max(max), min(min)
{
}

Setting::Setting()
    : value(0), max(0), min(0)
{
}

void Setting::set(int new_value)
{
    if (min <= new_value && new_value <= max)
        value = new_value;
}

void Setting::set_max(int new_max)
{
    max = new_max;
}

void Setting::increment()
{
    value = std::min(value + 1, max);
}

void Setting::decrement()
{
    value = std::max(value - 1, min);
}

int Setting::get()
{
    return value;
}

void Settings::addSetting(std::string name, Setting &setting)
{
    settingsMap[name] = setting;
}

Setting &Settings::getSetting(std::string name)
{
    return settingsMap[name];
}