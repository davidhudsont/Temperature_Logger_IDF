#pragma once

#include <string>
#include <map>

class Setting
{
public:
    Setting(int min, int max, int startingValue = 0);
    Setting();
    void set(int new_value);
    void increment();
    void decrement();
    int get();
    void set_max(int new_max);

private:
    int value;
    int max;
    int min;
};

class Settings
{
public:
    void addSetting(std::string name, Setting &setting);
    virtual std::string displayString() = 0;
    Setting &getSetting(std::string name);

private:
    std::map<std::string, Setting> settingsMap;
};