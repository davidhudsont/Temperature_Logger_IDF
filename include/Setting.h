#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>

class Setting
{
public:
    Setting(std::string name, int min, int max, int startingValue = 0);
    Setting();
    void set(int new_value);
    void increment();
    void decrement();
    int get();
    void set_max(int new_max);
    std::string &getName() { return name; }

private:
    std::string name;
    int min;
    int max;
    int value;
};

enum class Input
{
    UP,
    DOWN,
    ENTER,
    BACK,
};

class Settings
{
public:
    virtual bool getInput(const Input &input) = 0;
    virtual std::string displayString() = 0;
    void addCallback(std::function<void(int)> &func);
    void setSetting(std::string name, int value);
    int getSetting(std::string name);

protected:
    Settings(int maxEntries);
    void addSetting(Setting &setting);
    std::vector<Setting> settingsList;
    int currentEntry = 0;
    int maxEntries = 0;

private:
};