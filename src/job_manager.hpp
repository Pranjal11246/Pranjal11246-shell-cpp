#pragma once

#include <vector>
#include <string>
#include "job.hpp"

namespace JobManager
{
    void add(pid_t pid, const std::string& command);

    void remove(pid_t pid);

    const std::vector<Job>& jobs();

    Job* find(pid_t pid);

    void markDone(pid_t pid);
}