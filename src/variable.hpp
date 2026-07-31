#pragma once

#include <string>

struct Variable
{
    std::string name;
    std::string value;

    bool exported = false;
    bool readonly = false;
};