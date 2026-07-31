#pragma once

#include "variable.hpp"

#include <string>
#include <vector>

namespace VariableManager
{
    bool set(
        const std::string& name,
        const std::string& value
    );

    const Variable* get(const std::string& name);

    bool exists(const std::string& name);

    bool erase(const std::string& name);

    void clear();

    std::vector<Variable> list();

    bool isValidName(const std::string& name);
}