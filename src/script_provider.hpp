#pragma once

#include <vector>
#include <string>

#include "completion_context.hpp"
#include "completion_spec.hpp"

namespace ScriptProvider
{
    std::vector<std::string> run(
        const CompletionSpec& spec,
        const CompletionContext& ctx
    );
}