#pragma once

#include <vector>
#include <string>

bool isBuiltin(const std::string& command);

bool executeBuiltin(
    const std::vector<std::string>& tokens,
    bool& shouldExit
);

bool builtinDeclare(const std::vector<std::string>& tokens);