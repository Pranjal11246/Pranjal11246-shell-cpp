#pragma once

#include <string>
#include <vector>
#include "completion_context.hpp"

char** completionCallback(const char* text,int start,int end);
namespace Completion
{

    std::vector<std::string> getCompletions(
        const CompletionContext& ctx
    );


    std::vector<std::string> matchPrefix(
        const std::vector<std::string>& choices,
        const std::string& prefix
    );


    std::vector<std::string> getBuiltins();
    std::vector<std::string> getExecutables();



}