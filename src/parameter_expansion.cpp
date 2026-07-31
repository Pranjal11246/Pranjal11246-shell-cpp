#include "parameter_expansion.hpp"

#include "expansion_parser.hpp"
#include "expansion_resolver.hpp"

namespace
{
    void expandToken(std::string& token);
}

void expandParameters(std::vector<std::string>& tokens)
{
    for(auto& token : tokens)
    {
        expandToken(token);
    }
}

namespace
{

void expandToken(std::string& token)
{
    size_t cursor = 0;

    while(true)
    {
        auto expansion = parseExpansion(token, cursor);

        if(!expansion)
        {
            break;
        }

        std::string replacement = resolveExpansion(*expansion);

        token.replace(
            expansion->begin,
            expansion->end - expansion->begin,
            replacement
        );

        cursor = expansion->begin + replacement.size();
    }
}

} 