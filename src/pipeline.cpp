#include "pipeline.hpp"

#include <algorithm>
#include <vector>
#include <string>
#include <unistd.h>   
#include <sys/wait.h>   
#include <cstdlib>      
#include <cstdio>       
#include <sstream>
#include <filesystem>
#include <iostream>

std::vector<char*> makeArgs(const std::vector<std::string>& command)
{
    std::vector<char*> args;

    for (const auto& token : command)
    {
        args.push_back(const_cast<char*>(token.c_str()));
    }

    args.push_back(nullptr);

    return args;
}

std::string findExecutable(const std::string& command)
{
    char* path = getenv("PATH");

    if (path == nullptr)
        return "";

    std::stringstream ss(path);
    std::string directory;

    while (std::getline(ss, directory, ':'))
    {
        std::filesystem::path fullPath =
            std::filesystem::path(directory) / command;

        if (std::filesystem::exists(fullPath) &&
            access(fullPath.string().c_str(), X_OK) == 0)
        {
            return fullPath.string();
        }
    }

    return "";
}

void executePipeline(const std::vector<std::string>& tokens)
{
    auto pipePos = std::find(tokens.begin(), tokens.end(), "|");
    if (pipePos == tokens.begin() || pipePos + 1 == tokens.end()){
        std::cerr << "Invalid pipeline\n";
        return;
    }

    std::vector<std::string> left(tokens.begin(), pipePos);
    std::vector<std::string> right(pipePos + 1, tokens.end());

    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return;
    }

    pid_t leftPid = fork();

    if (leftPid == -1)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        perror("fork");
        return;
    }

    if (leftPid == 0)
    {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1){
            perror("dup2");
            _exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        auto args = makeArgs(left);

        std::string path = findExecutable(left[0]);

        if (path.empty())
        {
            std::cerr << left[0] << ": command not found\n";
            _exit(127);
        }

        execv(path.c_str(), args.data());

        perror("execv");
        _exit(1);
    }

    pid_t rightPid = fork();

if (rightPid == -1)
{
    close(pipefd[0]);
    close(pipefd[1]);
    perror("fork");
    return;
}

if (rightPid == 0){
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            _exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);

        auto args = makeArgs(right);

        std::string path = findExecutable(right[0]);

        if (path.empty())
        {
            std::cerr << right[0] << ": command not found\n";
            _exit(127);
        }

        execv(path.c_str(), args.data());

        perror("execv");
        _exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(leftPid, nullptr, 0);
    waitpid(rightPid, nullptr, 0);
}