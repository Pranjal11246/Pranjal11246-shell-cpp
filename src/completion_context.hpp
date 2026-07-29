#pragma once

#include <string>

struct CompletionContext
{
    std::string line;
    std::string token;

    size_t cursorPos;
    size_t tokenStart;

    bool firstToken;
};

CompletionContext buildContext(
    const std::string& line,
    size_t cursorPos
);