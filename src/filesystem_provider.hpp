#pragma once

#include <vector>
#include <string>

#include "completion_context.hpp"

namespace FilesystemProvider
{
    std::vector<std::string> getCompletions(
        const CompletionContext& ctx
    );
}