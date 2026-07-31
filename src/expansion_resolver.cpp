#include "expansion_resolver.hpp"

#include "variable_manager.hpp"

std::string
resolveExpansion(const Expansion& expansion)
{
    switch(expansion.type)
    {
        case ExpansionType::Variable:
            return VariableManager::get(expansion.value);

        case ExpansionType::Special:
            return "";

        case ExpansionType::Command:
            return "";

        case ExpansionType::Arithmetic:
            return "";

        case ExpansionType::Invalid:
            return "";
    }

    return "";
}