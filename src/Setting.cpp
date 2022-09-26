#include "Setting.h"

Setting::Setting(std::string name, int min, int max, int startingValue)
    : name(name), min(min), max(max), value(startingValue)
{
}

Setting::Setting()
    : name(""), min(0), max(0), value(0)
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
    if (value == max)
    {
        value = min;
    }
    else
    {
        value++;
    }
}

void Setting::decrement()
{
    if (value == min)
    {
        value = max;
    }
    else
    {
        value--;
    }
}

int Setting::get()
{
    return value;
}

void Settings::addSetting(Setting &setting)
{
    settingsList.push_back(setting);
}

void Settings::setSetting(std::string name, int value)
{
    for (auto &setting : settingsList)
    {
        if (name == setting.getName())
        {
            setting.set(value);
        }
    }
}

int Settings::getSetting(std::string name)
{
    for (auto &setting : settingsList)
    {
        if (name == setting.getName())
        {
            return setting.get();
        }
    }
    return -1;
}

Settings::Settings(int maxEntries)
    : maxEntries(maxEntries)
{
}