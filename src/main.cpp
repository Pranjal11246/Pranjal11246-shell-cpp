#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include<vector>
#include "tokenizer.hpp"
#include<sys/wait.h>
#include <fstream>
#include "command_parser.hpp"
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "completion.hpp"
#include "completion_registry.hpp"
#include "job_manager.hpp"
#include <iomanip>
#include "pipeline.hpp"
#include "builtin.hpp"
#include "history_manager.hpp"

namespace fs= std::filesystem;
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  
  rl_attempted_completion_function = completionCallback;
  
  while(true){

    auto finished = JobManager::reapFinishedJobs();

    for (const auto& reaped : finished)
    {
        std::string command = reaped.job.command;

        if (command.size() >= 2 &&
            command.compare(command.size() - 2, 2, " &") == 0)
        {
            command.erase(command.size() - 2);
        }

        std::cout
            << "[" << reaped.job.id << "]"
            << reaped.marker
            << "  "
            << std::left
            << std::setw(21)
            << "Done"
            << command
            << '\n';
    }
    
      char* line = readline("$ ");
      if (line == nullptr)break;

      std::string input(line);

      if(HistoryManager::shouldExpand(input)){
        input = HistoryManager::expand(input);
      }

      free(line);

      auto tokens = tokenize(input);

      if (tokens.empty()) {
          continue;
      }

      HistoryManager::add(input);
      add_history(input.c_str());

      bool hasPipe = std::find(tokens.begin(), tokens.end(), "|") != tokens.end();
      if(hasPipe){
        executePipeline(tokens);
        continue;
      }


      if(tokens.empty())continue;

      RedirectInfo redirectInfo = parseRedirection(tokens);
      bool background = false;

      if (!tokens.empty() && tokens.back() == "&"){
          background = true;
          tokens.pop_back();
      }

      vector<char*> args;
      for(auto& token:tokens){
        args.push_back(token.data());
      }
      args.push_back(nullptr);



      std::ofstream outFile;
      std::streambuf* oldBuffer = nullptr;

      if (redirectInfo.redirect) {
        if (redirectInfo.append)
            outFile.open(redirectInfo.filename,std::ios::out | std::ios::app);
            
        else
            outFile.open(redirectInfo.filename,std::ios::out | std::ios::trunc);
            

        if (redirectInfo.fd == STDOUT_FILENO)
          oldBuffer = std::cout.rdbuf(outFile.rdbuf());
        else
          oldBuffer = std::cerr.rdbuf(outFile.rdbuf());
      }

      bool shouldExit = false;

      if (executeBuiltin(tokens, shouldExit))
      {
          if (redirectInfo.redirect)
          {
              if (redirectInfo.fd == STDOUT_FILENO)
                  std::cout.rdbuf(oldBuffer);
              else
                  std::cerr.rdbuf(oldBuffer);

              outFile.close();
          }

          if (shouldExit)
              break;

          continue;
      }

       char* path = getenv("PATH");
          std::stringstream ss(path);
          std::string directory;
          bool found = false;
          while(std::getline(ss,directory,':')){
            std::filesystem::path fullPath = std::filesystem::path(directory)/tokens[0];

            if(std::filesystem::exists(fullPath) && access(fullPath.string().c_str(),X_OK)==0){
              pid_t pid = fork();
              if(pid==0){
                if(redirectInfo.redirect){

                  int flags = O_WRONLY | O_CREAT;

                  if (redirectInfo.append)
                      flags |= O_APPEND;
                  else
                      flags |= O_TRUNC;

                  int fd = open(
                  redirectInfo.filename.c_str(),
                  flags,
                  0644
                );

                if(fd==-1){
                  perror("open");
                  exit(1);
                }

                if (dup2(fd, redirectInfo.fd) == -1) {
                      perror("dup2");
                      exit(1);
                }

                close(fd);

                }
                execv(fullPath.string().c_str(),args.data());
                perror("execv");
                exit(1);
              }else if(pid>0){
                if (background){

                  JobManager::add(pid, input);

                  const Job& job = JobManager::jobs().back();

                  std::cout
                      << "[" << job.id << "] "
                      << job.pid << '\n';
              }
                else{
                    waitpid(pid, nullptr, 0);
                }
              }else{
                perror("fork");
              }
              found = true;
              break;
            }
          }
          
          if(redirectInfo.redirect){

                  if (redirectInfo.fd == STDOUT_FILENO)
                      std::cout.rdbuf(oldBuffer);
                  else
                      std::cerr.rdbuf(oldBuffer);

                  outFile.close();
              

          }

          if(!found){
            std::cerr<< tokens[0] << ": command not found" << std::endl;
          }else{
            continue;
          }
  }
}
