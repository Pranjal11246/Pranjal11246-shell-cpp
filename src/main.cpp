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

namespace fs= std::filesystem;
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while(true){
       std::cout << "$ ";
      std::string input;
      std::getline(std::cin,input);

      auto tokens = tokenize(input);

      if(tokens.empty())continue;

      RedirectInfo redirectInfo = parseRedirection(tokens);

      vector<char*> args;
      for(auto& token:tokens){
        args.push_back(token.data());
      }
      args.push_back(nullptr);

      std::ofstream outFile;
      std::streambuf* oldBuffer = nullptr;

      if (redirectInfo.redirect) {
        outFile.open(redirectInfo.filename);
        oldBuffer = std::cout.rdbuf(outFile.rdbuf());
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

      if(tokens[0] == "type"){
        if(tokens[1] == "echo" || tokens[1]=="exit" || tokens[1] == "type" || tokens[1]=="pwd"){
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
            std::cout << tokens[1] <<": not found"<<std::endl;
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
        fs::path new_path = tokens[1];
        if(tokens[1]=="~"){
          char* home = std::getenv("HOME");
          new_path = home;
        }
        if(fs::exists(new_path) && fs::is_directory(new_path)){
          fs::current_path(new_path);
        }else{
          std::cout<< tokens[0] << ": "<< tokens[1] << ": " << "No such file or directory" << std::endl;
        }
        handled=true;

      }
      
      if(handled){
        if(redirectInfo.redirect){
              cout.rdbuf(oldBuffer);
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
                  int fd = open(
                  redirectInfo.filename.c_str(),
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644
                );

                if(fd==-1){
                  perror("open");
                  exit(1);
                }

                if (dup2(fd, STDOUT_FILENO) == -1) {
                      perror("dup2");
                      exit(1);
                }

                close(fd);

                }
                execv(fullPath.string().c_str(),args.data());
                perror("execv");
                exit(1);
              }else if(pid>0){
                waitpid(pid,nullptr,0);
              }else{
                perror("fork");
              }
              found = true;
              break;
            }
          }
          
          if(redirectInfo.redirect){
              cout.rdbuf(oldBuffer);
              outFile.close();
          }

          if(!found){
            std::cout<< tokens[0] << ": command not found" << std::endl;
          }else{
            continue;
          }
  }
}
