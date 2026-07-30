#include <iostream>
#include <string>
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

namespace fs= std::filesystem;
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  
  rl_attempted_completion_function = completionCallback;
  while(true){
       char* line = readline("$ ");
      if (line == nullptr)break;

      std::string input(line);
      free(line);

      auto tokens = tokenize(input);



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

      bool handled=false;


      if(tokens[0] == "exit")break;
      if(tokens[0]=="echo"){
        for(int i=1;i<tokens.size();i++){
          std::cout << tokens[i] << " ";
        }
        std::cout<<std::endl;

        handled=true;
      }

      if (tokens[0] == "complete")
      {
          if (tokens.size() >= 2)
          {
              if (tokens[1] == "-p")
              {
                  if (tokens.size() == 2)
                  {
                      auto specs = CompletionRegistry::list();

                      for (const auto& spec : specs)
                      {
                          std::cout
                              << "complete -C '"
                              << spec.script
                              << "' "
                              << spec.command
                              << '\n';
                      }
                  }
                  else if (tokens.size() == 3)
                  {
                      auto spec = CompletionRegistry::find(tokens[2]);

                      if (spec)
                      {
                          std::cout
                              << "complete -C '"
                              << spec->script
                              << "' "
                              << spec->command
                              << '\n';
                      }
                      else
                      {
                          std::cout << "complete: "<<tokens[2]<<": no completion specification\n";
                      }
                  }
              }

              else if (tokens[1] == "-C")
              {
                  if (tokens.size() >= 4)
                  {
                      CompletionRegistry::registerCompletion(
                          tokens[3],
                          tokens[2]
                      );
                  }
              }

              else if (tokens[1] == "-r")
              {
                  if (tokens.size() >= 3)
                  {
                      CompletionRegistry::unregisterCompletion(tokens[2]);
                  }
              }
          }

          handled = true;
      }
      if (tokens[0] == "jobs"){

         auto finished = JobManager::reapFinishedJobs();

        for (const auto& reaped : finished){
            std::cout
                << "[" << reaped.job.id << "]"
                << reaped.marker
                << "  "
                << std::left
                << std::setw(24)
                << "Done"
                << reaped.job.command
                << '\n';
        }

          const auto& jobs = JobManager::jobs();

          for (size_t i = 0; i < jobs.size(); ++i)
          {
              const auto& job = jobs[i];

              std::cout
                  << "[" << job.id << "]"
                  << JobManager::marker(i)
                  << "  "
                  << std::left
                  << std::setw(24)
                  << (job.state == JobState::Running ? "Running" : "Done")
                  << job.command
                  << '\n';
          }

          handled = true;
      }

      if(tokens[0] == "type"){

        if (tokens.size() < 2)
        {
            handled = true;
        }
        if(tokens[1] == "echo" || tokens[1]=="exit" || tokens[1] == "type" || tokens[1]=="pwd" || tokens[1] == "complete" || tokens[1]=="cd" || tokens[1]=="jobs"){
          std::cout << tokens[1] << " is a shell builtin" << std::endl;
          
        }else{
          char* path = getenv("PATH");
          std::stringstream ss(path);
          std::string directory;
          bool found = false;
          while(std::getline(ss,directory,':')){
            std::filesystem::path fullPath = std::filesystem::path(directory)/tokens[1];

            if(std::filesystem::exists(fullPath) && access(fullPath.string().c_str(),X_OK)==0){
              std::cout << tokens[1] << " is " << fullPath.string() << std::endl;
              found = true;
              break;
            }
            
          }

          if(!found){
            std::cerr << tokens[1] <<": not found"<<std::endl;
          }
        }
        handled=true;
        
      }

      if(tokens[0]=="pwd"){
        fs::path cwd = fs::current_path();
        std::cout << cwd.string() << std::endl;
        handled=true;
        
      }

      if(tokens[0]=="cd"){
        if (tokens.size() < 2)
        {
            handled = true;
        }
        fs::path new_path = tokens[1];
        if(tokens[1]=="~"){
          char* home = std::getenv("HOME");
          new_path = home;
        }
        if(fs::exists(new_path) && fs::is_directory(new_path)){
          fs::current_path(new_path);
        }else{
          std::cerr<< tokens[0] << ": "<< tokens[1] << ": " << "No such file or directory" << std::endl;
        }
        handled=true;

      }
      
      if(handled){
              if (redirectInfo.redirect) {
                  if (redirectInfo.fd == STDOUT_FILENO)
                      std::cout.rdbuf(oldBuffer);
                  else
                      std::cerr.rdbuf(oldBuffer);

                  outFile.close();
              }

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
