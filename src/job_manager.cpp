#include "job_manager.hpp"
#include <algorithm>
#include <sys/wait.h>

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

Job* JobManager::find(pid_t pid){
    for (auto& job : jobList)
    {
        if (job.pid == pid)
            return &job;
    }

    return nullptr;
}

void JobManager::markDone(pid_t pid){
    Job* job = find(pid);

    if (job)
        job->state = JobState::Done;
}

char JobManager::marker(size_t index){
    if (index + 1 == jobList.size())return '+';

    if (index + 2 == jobList.size())return '-';

    return ' ';
}



void JobManager::refreshJobs()
{
    for (Job& job : jobList)
    {
        if (job.state == JobState::Done)
            continue;

        int status;
        pid_t result = waitpid(job.pid, &status, WNOHANG);

        if (result > 0 && (WIFEXITED(status) || WIFSIGNALED(status)))
        {
            job.state = JobState::Done;
        }
    }
}

void JobManager::removeDoneJobs()
{
    for (auto it = jobList.begin(); it != jobList.end();)
    {
        if (it->state == JobState::Done)
        {
            it = jobList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::vector<ReapedJob> JobManager::reapFinishedJobs()
{
    refreshJobs();

    std::vector<ReapedJob> finished;

    for (size_t i = 0; i < jobList.size(); ++i)
    {
        if (jobList[i].state == JobState::Done)
        {
            finished.push_back({
                jobList[i],
                marker(i)
            });
        }
    }

    removeDoneJobs();

    return finished;
}