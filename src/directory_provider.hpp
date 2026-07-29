#pragma once

#include "completion_context.hpp"
#include <vector>
#include <string>

namespace DirectoryProvider
{
    std::vector<std::string> getCompletions(
        const CompletionContext& ctx
    );
}