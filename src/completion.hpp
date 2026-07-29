#pragma once

#include <string>
#include <vector>

char** completionCallback(const char* text,int start,int end);
namespace Completion
{

    std::vector<std::string> getCompletions(
        const std::string& line,
        int cursorPos
    );


    std::vector<std::string> matchPrefix(
        const std::vector<std::string>& choices,
        const std::string& prefix
    );


    std::vector<std::string> getBuiltins();
    std::vector<std::string> getExecutables();

    std::string getCurrentWord(const std::string& line,int cursorPos);


}