#pragma once

#include <string>

enum class ExpansionType
{
    Variable,
    Special,
    Command,
    Arithmetic,
    Invalid
};

struct Expansion
{
    ExpansionType type;

    size_t begin;

    size_t end;

    std::string value;
};