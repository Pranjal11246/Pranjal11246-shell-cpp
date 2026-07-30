#pragma once

#include <string>
#include <sys/types.h>

enum class JobState
{
    Running,
    Done
};

struct Job
{
    int id;
    pid_t pid;
    std::string command;
    JobState state = JobState::Running;
};

struct ReapedJob
{
    Job job;
    char marker;
};