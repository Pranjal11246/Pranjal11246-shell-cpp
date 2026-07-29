#include "directory_provider.hpp"
#include "filesystem_provider.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace DirectoryProvider
{

std::vector<std::string> getCompletions(
    const CompletionContext& ctx)
{
    auto completions =
        FilesystemProvider::getCompletions(ctx);

    std::vector<std::string> directories;

    for (const auto& completion : completions)
    {
        if (!completion.empty() &&
            completion.back() == '/')
        {
            directories.push_back(completion);
        }
    }

    return directories;
}

}