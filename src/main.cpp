#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  while(true){
       std::cout << "$ ";
      std::string input;
      std::string command;
      std::string parameter;
      std::getline(std::cin,input);

      command = input.substr(0,input.find(' '));
      const int parameterSize = input.find(' ')+1;

      if(parameterSize!=std::string::npos){
        parameter = input.substr(parameterSize);
      }

      if(command == "exit")break;
      if(command=="echo"){
        std::cout << input.substr(5) << std::endl;
        continue;  
      }

      if(command == "type"){
        if(parameter == "echo" || parameter=="exit" || parameter == "type" ){
          std::cout << parameter << " is a shell builtin" << std::endl;
          
        }else{
          char* path = getenv("PATH");
          std::stringstream ss(path);
          std::string directory;
          bool found = false;
          while(std::getline(ss,directory,':')){
            std::filesystem::path fullPath = std::filesystem::path(directory)/parameter;

            if(std::filesystem::exists(fullPath) && access(fullPath.string().c_str(),X_OK)==0){
              std::cout << parameter << " is " << fullPath.string() << std::endl;
              found = true;
              break;
            }
          }

          if(!found){
            std::cout << parameter <<": not found"<<std::endl;
          }
        }
        continue;
        
      }
      std::cout<< command << ": command not found" << std::endl;
  }
}
