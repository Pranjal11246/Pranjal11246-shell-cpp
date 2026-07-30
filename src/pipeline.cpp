#include "pipeline.hpp"
#include <array>
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
#include "builtin.hpp"

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
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> current;

    for(int i=0;i<tokens.size();i++){

        if(tokens[i]=="|"){

            if(current.empty()){
                std::cerr<<"Invalid pipeline\n";
                return;
            }

            commands.push_back(current);
            current.clear();
        }

        else{
            current.push_back(tokens[i]);
        }
    }

    if(current.empty()){
        std::cerr<<"Invalid pipeline\n";
        return;
    }

    commands.push_back(current);

    std::vector<std::array<int,2>> pipes;
    std::vector<pid_t> pids;

    for(int i=0;i<commands.size()-1;i++){

        std::array<int,2> pipefd;

        if(pipe(pipefd.data())==-1){
            perror("pipe");
            return;
        }

        pipes.push_back(pipefd);
    }
        for(int i=0;i<commands.size();i++){

        pid_t pid = fork();

        if(pid==-1){
            perror("fork");

            for(int j=0;j<pipes.size();j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            return;
        }

        if(pid==0){
            if(i!=0){
                if(dup2(pipes[i-1][0],STDIN_FILENO)==-1){
                    perror("dup2");
                    _exit(1);
                }
            }
            if(i!=commands.size()-1){
                if(dup2(pipes[i][1],STDOUT_FILENO)==-1){
                    perror("dup2");
                    _exit(1);
                }
            }
            for(int j=0;j<pipes.size();j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            if(isBuiltin(commands[i][0])){
                bool shouldExit=false;
                executeBuiltin(commands[i],shouldExit);
                _exit(0);
            }

            auto args=makeArgs(commands[i]);

            std::string path=findExecutable(commands[i][0]);

            if(path.empty()){
                std::cerr<<commands[i][0]<<": command not found\n";
                _exit(127);
            }

            execv(path.c_str(),args.data());

            perror("execv");
            _exit(1);
        }

        pids.push_back(pid);
    }

        for(int i=0;i<pipes.size();i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
        }

        for(int i=0;i<pids.size();i++){
            waitpid(pids[i],nullptr,0);
        }
}