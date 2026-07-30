#include "job_manager.hpp"
#include <algorithm>

namespace
{
    std::vector<Job> jobList;
    int nextJobId = 1;
}

void JobManager::add(pid_t pid, const std::string& command){
    jobList.push_back({nextJobId++,pid,command});
}

const std::vector<Job>& JobManager::jobs(){
    return jobList;
}

void JobManager::remove(pid_t pid){
    jobList.erase(
        std::remove_if(
            jobList.begin(),
            jobList.end(),
            [pid](const Job& job)
            {
                return job.pid == pid;
            }),
        jobList.end());
}