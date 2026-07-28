#pragma once

#include <string>
#include <vector>
#include<unistd.h>

struct RedirectInfo {
    bool redirect = false;
    int fd = STDOUT_FILENO;
    std::string filename;
};

RedirectInfo parseRedirection(std::vector<std::string>& tokens);