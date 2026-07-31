#include "expansion_parser.hpp"

#include <cctype>

namespace
{
    std::optional<Expansion>
    parseVariableExpansion(
        const std::string& token,
        size_t dollar
    );

    std::optional<Expansion>
    parseSpecialExpansion(
        const std::string& token,
        size_t dollar
    );

    std::optional<Expansion>
    parseBracedExpansion(
        const std::string& token,
        size_t dollar
    );

    std::optional<Expansion>
    parseCommandExpansion(
        const std::string& token,
        size_t dollar
    );

    std::optional<Expansion>
    parseArithmeticExpansion(
        const std::string& token,
        size_t dollar
    );
}

std::optional<Expansion>
parseExpansion(
    const std::string& token,
    size_t cursor)
{
    size_t dollar = token.find('$', cursor);

    if(dollar == std::string::npos)
    {
        return std::nullopt;
    }

    if(dollar + 1 >= token.size())
    {
        return std::nullopt;
    }

    char next = token[dollar + 1];

    switch(next)
    {
        case '?':
        case '$':
            return parseSpecialExpansion(token, dollar);

        case '{':
            return parseBracedExpansion(token, dollar);

        case '(':
        {
            if(dollar + 2 < token.size() &&
               token[dollar + 2] == '(')
            {
                return parseArithmeticExpansion(token, dollar);
            }

            return parseCommandExpansion(token, dollar);
        }

        default:
            return parseVariableExpansion(token, dollar);
    }
}

namespace
{

std::optional<Expansion>
parseVariableExpansion(
    const std::string& token,
    size_t dollar)
{
    size_t start = dollar + 1;

    // '$' cannot be the last character.
    if(start >= token.size())
    {
        return std::nullopt;
    }

    // Variable names must begin with [A-Za-z_].
    if(!(std::isalpha(static_cast<unsigned char>(token[start])) ||
         token[start] == '_'))
    {
        return std::nullopt;
    }

    size_t end = start;

    // Continue while [A-Za-z0-9_].
    while(end < token.size() &&
          (std::isalnum(static_cast<unsigned char>(token[end])) ||
           token[end] == '_'))
    {
        ++end;
    }

    Expansion expansion;

    expansion.type = ExpansionType::Variable;
    expansion.begin = dollar;
    expansion.end = end;
    expansion.value = token.substr(start, end - start);

    return expansion;
}

std::optional<Expansion>
parseSpecialExpansion(
    const std::string& token,
    size_t dollar)
{
    (void)token;
    (void)dollar;

    return std::nullopt;
}

std::optional<Expansion>
parseBracedExpansion(
    const std::string& token,
    size_t dollar)
{
    (void)token;
    (void)dollar;

    return std::nullopt;
}

std::optional<Expansion>
parseCommandExpansion(
    const std::string& token,
    size_t dollar)
{
    (void)token;
    (void)dollar;

    return std::nullopt;
}

std::optional<Expansion>
parseArithmeticExpansion(
    const std::string& token,
    size_t dollar)
{
    (void)token;
    (void)dollar;

    return std::nullopt;
}

} // namespace