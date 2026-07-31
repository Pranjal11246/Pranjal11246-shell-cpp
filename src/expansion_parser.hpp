#pragma once

#include <optional>
#include <string>

#include "expansion.hpp"

std::optional<Expansion>
parseExpansion(
    const std::string& token,
    size_t cursor = 0
);