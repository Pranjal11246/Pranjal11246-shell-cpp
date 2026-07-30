#include "script_provider.hpp"

#include <cstdio>
#include <array>
#include "tokenizer.hpp"

namespace ScriptProvider
{

std::vector<std::string> run(const CompletionSpec& spec,const CompletionContext& ctx){
    std::vector<std::string> completions;

    std::string command = spec.script;

    auto tokens = tokenize(ctx.line);
    std::string commandName = tokens.empty() ? "" : tokens[0];
    std::string currentWord = ctx.token;
    std::string previousWord = "";

    if (tokens.size() >= 2)
    {
        previousWord = tokens[tokens.size() - 2];
    }

    command += " '" + commandName + "'";
    command += " '" + currentWord + "'";
    command += " '" + previousWord + "'";

    setenv("COMP_LINE", ctx.line.c_str(), 1);

    std::string point = std::to_string(ctx.cursorPos);
    setenv("COMP_POINT", point.c_str(), 1);
    
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe)
        return completions;

    std::array<char, 1024> buffer;

    while (fgets(buffer.data(), buffer.size(), pipe))
    {
        std::string line(buffer.data());

        while (!line.empty() &&
              (line.back() == '\n' || line.back() == '\r'))
        {
            line.pop_back();
        }

        if (!line.empty())
            completions.push_back(line);
    }

    pclose(pipe);

    return completions;
}

}