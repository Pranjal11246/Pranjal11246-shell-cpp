#include"completion.hpp"
#include <algorithm>      
#include <cstring>        
#include <readline/readline.h>
#include <readline/history.h>
#include <filesystem>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include "filesystem_provider.hpp"
#include "directory_provider.hpp"
namespace fs = std::filesystem;

static std::vector<std::string> matches;

char* commandGenerator(const char* text, int state)
{
    static size_t index;

    if (state == 0)
        index = 0;

    if (index >= matches.size())
        return nullptr;

    return strdup(matches[index++].c_str());
}

char** completionCallback(
    const char* text,
    int start,
    int end)
{
    (void)text;
    (void)start;

    std::string line(rl_line_buffer);

    const CompletionContext ctx = buildContext(line, end);
    matches = Completion::getCompletions(ctx);

    return rl_completion_matches("", commandGenerator);
}
namespace Completion{
std::vector<std::string> getBuiltins()
{
    return {
        "echo",
        "exit",
        "type",
        "pwd",
        "cd"
    };
}




std::vector<std::string> getCompletions(const CompletionContext& ctx)
{
    if (ctx.firstToken)
    {
        std::vector<std::string> finalVec = getBuiltins();

        auto executables = getExecutables();

        finalVec.insert(
            finalVec.end(),
            executables.begin(),
            executables.end()
        );

        std::erase_if(finalVec, [&](const std::string& s) {
            return !s.starts_with(ctx.token);
        });

        std::sort(finalVec.begin(), finalVec.end());

        finalVec.erase(
            std::unique(finalVec.begin(), finalVec.end()),
            finalVec.end()
        );

        return finalVec;
    }

    std::istringstream iss(ctx.line);
    std::string command;
    iss >> command;

    if (command == "cd")
    {
        return DirectoryProvider::getCompletions(ctx);
    }

    return FilesystemProvider::getCompletions(ctx);
}

std::vector<std::string> getExecutables()
{
    std::vector<std::string> executables;

    char* path = getenv("PATH");

    if (path == nullptr)
        return executables;

    std::stringstream ss(path);
    std::string directory;
    while (std::getline(ss, directory, ':'))
    {
    if (!fs::exists(directory) || !fs::is_directory(directory))
        continue;

    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
            continue;

        if (access(entry.path().c_str(), X_OK) == 0)
        {
            executables.push_back(
                entry.path().filename().string()
            );
        }
    }
}

return executables;
}

}