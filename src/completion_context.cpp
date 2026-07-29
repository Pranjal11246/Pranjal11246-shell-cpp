#include "completion_context.hpp"

CompletionContext buildContext(
    const std::string& line,
    size_t cursorPos)
{
    CompletionContext ctx;

    ctx.line = line;
    ctx.cursorPos = cursorPos;

    int i = static_cast<int>(cursorPos) - 1;

    // Move left until we find the previous delimiter
    while (i >= 0 && line[i] != ' ')
    {
        i--;
    }

    ctx.tokenStart = i + 1;

    ctx.token = line.substr(
        ctx.tokenStart,
        cursorPos - ctx.tokenStart
    );

   ctx.firstToken = true;

for (size_t j = 0; j < ctx.tokenStart; ++j)
{
    if (!std::isspace(static_cast<unsigned char>(line[j])))
    {
        ctx.firstToken = false;
        break;
    }
}
    return ctx;
}