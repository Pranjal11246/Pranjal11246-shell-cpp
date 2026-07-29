#include "filesystem_provider.hpp"
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace FilesystemProvider
{

std::vector<std::string> getCompletions(
    const CompletionContext& ctx)
{
    fs::path path(ctx.token);
    fs::path parent = path.parent_path();
    if (parent.empty())
    {
    parent = ".";
    }
    std::string prefix = path.filename().string();
    std::vector<std::string> completions;
    if (!fs::exists(parent) || !fs::is_directory(parent))
    {
    return completions;
    }

    for (const auto& entry : fs::directory_iterator(parent))
    {
    std::string name = entry.path().filename().string();

    if (name.starts_with(prefix))
    {
             std::string completion;

            if (parent == ".")
            {
                completion = name;
            }
            else
            {
                completion = (parent / name).string();
            }

            if (entry.is_directory())
            {
                completion += "/";
            }

            completions.push_back(completion);
    }
    }
    std::sort(completions.begin(),completions.end());

    return completions;
}

}