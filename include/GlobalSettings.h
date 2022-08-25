#pragma once

#include "Setting.h"

class DateSetting : public Settings
{
public:
    DateSetting();
    std::string displayString();
};