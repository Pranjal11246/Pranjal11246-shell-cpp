#include "builtin.hpp"

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "completion_registry.hpp"
#include "job_manager.hpp"
#include "history_manager.hpp"

namespace fs = std::filesystem;

bool isBuiltin(const std::string& command)
{
    return command == "echo" ||
           command == "exit" ||
           command == "type" ||
           command == "pwd"  ||
           command == "cd"   ||
           command == "jobs" ||
           command == "complete" ||
           command == "history";
}

bool executeBuiltin(const std::vector<std::string>& tokens,bool& shouldExit)
{
    if (tokens.empty())
        return false;

    if (tokens[0] == "exit"){
    shouldExit = true;
    return true;
    }

      if(tokens[0]=="echo"){
        for(int i=1;i<tokens.size();i++){
            std::cout << tokens[i];

            if(i!=tokens.size()-1){
                std::cout << " ";
            }
        }

        std::cout << std::endl;
        return true;
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

          return true;
      }
      if (tokens[0] == "jobs")
      {
          JobManager::refreshJobs();

          const auto& jobs = JobManager::jobs();

          for (size_t i = 0; i < jobs.size(); ++i)
          {
              const Job& job = jobs[i];

              std::string command = job.command;
              std::string state;

              if (job.state == JobState::Done)
              {
                  state = "Done";

                  if (command.size() >= 2 &&
                      command.compare(command.size() - 2, 2, " &") == 0)
                  {
                      command.erase(command.size() - 2);
                  }
              }
              else
              {
                  state = "Running";
              }

              std::cout
                  << "[" << job.id << "]"
                  << JobManager::marker(i)
                  << "  "
                  << std::left
                  << std::setw(24)
                  << state
                  << command
                  << '\n';
          }

          JobManager::removeDoneJobs();
          return true;
      }

      if (tokens[0] == "type"){
    if (tokens.size() < 2)
        return true;

    if (isBuiltin(tokens[1]))
    {
        std::cout << tokens[1] << " is a shell builtin\n";
    }
    else
    {
        char* path = std::getenv("PATH");
        if (!path){
            std::cout << tokens[1] << ": not found\n";
            return true;
        }
        std::stringstream ss(path);
        std::string directory;
        bool found = false;

        while (std::getline(ss, directory, ':'))
        {
            std::filesystem::path fullPath =
                std::filesystem::path(directory) / tokens[1];

            if (std::filesystem::exists(fullPath) &&
                access(fullPath.string().c_str(), X_OK) == 0)
            {
                std::cout << tokens[1]
                          << " is "
                          << fullPath.string()
                          << '\n';

                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cout << tokens[1] << ": not found\n";
        }
    }

    return true;
}

    if(tokens[0]=="pwd"){
        fs::path cwd = fs::current_path();
        std::cout << cwd.string() << std::endl;
        return true;
        
      }

      if(tokens[0]=="cd"){
        if (tokens.size() < 2)
        {
            return true;
        }
        fs::path new_path = tokens[1];
        if(tokens[1]=="~"){
          char* home = std::getenv("HOME");
          if (home)new_path = home;
        }
        if(fs::exists(new_path) && fs::is_directory(new_path)){
          fs::current_path(new_path);
        }else{
          std::cerr<< tokens[0] << ": "<< tokens[1] << ": " << "No such file or directory" << std::endl;
        }
        return true;

      }

      if(tokens[0] == "history"){

        if(tokens.size() == 3 && tokens[1] == "-r"){

            HistoryManager::load(tokens[2]);

            return true;
        }

        if(tokens.size() == 3 && tokens[1] == "-w"){

            HistoryManager::save(tokens[2]);

            return true;
        }

        if(tokens.size() == 3 && tokens[1] == "-a"){

            HistoryManager::append(tokens[2]);

            return true;
        }

        const auto& history = HistoryManager::history();

        int start = 0;

        if(tokens.size() == 2){

            int count = std::stoi(tokens[1]);

            if(count < static_cast<int>(history.size())){
                start = static_cast<int>(history.size()) - count;
            }
        }

        for(int i = start; i < static_cast<int>(history.size()); i++){

            std::cout
                << std::setw(5)
                << history[i].number
                << "  "
                << history[i].command
                << '\n';
        }

        return true;
    }

return false;
}