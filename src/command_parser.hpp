#pragma once

#include <string>
#include <vector>
#include<unistd.h>

struct RedirectInfo {
    bool redirect = false;
    int fd = STDOUT_FILENO;
    bool append = false;
    std::string filename;
};

RedirectInfo parseRedirection(std::vector<std::string>& tokens);