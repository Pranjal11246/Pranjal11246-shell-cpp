#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include<vector>
#include "tokenizer.hpp"
namespace fs= std::filesystem;

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

      if(tokens[0] == "exit")break;
      if(tokens[0]=="echo"){
        for(int i=1;i<tokens.size();i++){
          std::cout << tokens[i] << " ";
        }
        std::cout<<std::endl;
        continue;
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
        continue;
        
      }

      if(tokens[0]=="pwd"){
        fs::path cwd = fs::current_path();
        std::cout << cwd.string() << std::endl;
        continue;
        
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
        continue;

      }

       char* path = getenv("PATH");
          std::stringstream ss(path);
          std::string directory;
          bool found = false;
          while(std::getline(ss,directory,':')){
            std::filesystem::path fullPath = std::filesystem::path(directory)/tokens[0];

            if(std::filesystem::exists(fullPath) && access(fullPath.string().c_str(),X_OK)==0){
              std::system(input.c_str());
              found = true;
              break;
            }
          }


          if(!found){
            std::cout<< tokens[0] << ": command not found" << std::endl;
          }else{
            continue;
          }
  }
}
