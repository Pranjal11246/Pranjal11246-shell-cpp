#pragma once

#include <vector>
#include <string>
#include "job.hpp"

namespace JobManager
{
    void add(pid_t pid, const std::string& command);

    void remove(pid_t pid);

    const std::vector<Job>& jobs();
}