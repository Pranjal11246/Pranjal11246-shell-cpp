#pragma once

#include <string>
#include <vector>

struct RedirectInfo {
    bool redirect = false;
    std::string filename;
};

RedirectInfo parseRedirection(std::vector<std::string>& tokens);