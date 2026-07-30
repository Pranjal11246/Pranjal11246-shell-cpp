#include "completion_registry.hpp"

#include <unordered_map>
#include <algorithm>

namespace
{
    std::unordered_map<std::string, CompletionSpec> registry;
}

namespace CompletionRegistry
{

void registerCompletion(
    const std::string& command,
    const std::string& script)
{
    registry[command] = {
        command,
        script
    };
}

bool unregisterCompletion(
    const std::string& command)
{
    return registry.erase(command) > 0;
}

const CompletionSpec* find(
    const std::string& command)
{
    auto it = registry.find(command);

    if(it == registry.end())
        return nullptr;

    return &it->second;
}

std::vector<CompletionSpec> list()
{
    std::vector<CompletionSpec> specs;

    for(const auto& [_, spec] : registry)
        specs.push_back(spec);

    std::sort(
        specs.begin(),
        specs.end(),
        [](const auto& a,const auto& b)
        {
            return a.command < b.command;
        });

    return specs;
}

}