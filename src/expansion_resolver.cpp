#include "expansion_resolver.hpp"

#include "variable_manager.hpp"

std::string
resolveExpansion(const Expansion& expansion)
{
    switch(expansion.type)
    {
        case ExpansionType::Variable:
        {
            const Variable* variable = VariableManager::get(expansion.value);

            if(variable != nullptr)
            {
                return variable->value;
            }

            return "";
        }

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