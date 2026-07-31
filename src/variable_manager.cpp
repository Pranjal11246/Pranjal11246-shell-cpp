#include "variable_manager.hpp"
#include <cctype>
#include <unordered_map>


namespace
{
    std::unordered_map<std::string, Variable> variableTable;
}

bool VariableManager::exists(const std::string& name)
{
    return variableTable.find(name) != variableTable.end();
}

const Variable* VariableManager::get(const std::string& name)
{
    auto it = variableTable.find(name);

    if(it == variableTable.end()){
        return nullptr;
    }

    return &it->second;
}

bool VariableManager::set(
    const std::string& name,
    const std::string& value
)
{
    auto it = variableTable.find(name);

    if(it == variableTable.end()){
        Variable variable;

        variable.name = name;
        variable.value = value;

        variableTable[name] = variable;
    }
    else{
        it->second.value = value;
    }

    return true;
}

bool VariableManager::erase(const std::string& name)
{
    return variableTable.erase(name) > 0;
}

void VariableManager::clear()
{
    variableTable.clear();
}

std::vector<Variable> VariableManager::list()
{
    std::vector<Variable> variables;

    for(const auto& entry : variableTable){
        variables.push_back(entry.second);
    }

    return variables;
}


bool VariableManager::isValidName(const std::string& name)
{
    if(name.empty()){
        return false;
    }

    if(!(std::isalpha(static_cast<unsigned char>(name[0])) ||
         name[0] == '_')){
        return false;
    }

    for(size_t i = 1; i < name.size(); i++){

        if(!(std::isalnum(static_cast<unsigned char>(name[i])) ||
             name[i] == '_')){
            return false;
        }
    }

    return true;
}