#pragma once

#include <string>
#include <vector>
#include "completion_spec.hpp"

namespace CompletionRegistry
{
    void registerCompletion(
        const std::string& command,
        const std::string& script);

    bool unregisterCompletion(
        const std::string& command);

    const CompletionSpec* find(
        const std::string& command);

    std::vector<CompletionSpec> list();
}