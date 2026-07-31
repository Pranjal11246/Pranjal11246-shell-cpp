#include "history_manager.hpp"
#include <cctype>
#include <string>

namespace
{
    std::vector<HistoryEntry> historyList;
}

void HistoryManager::add(const std::string& command)
{
    HistoryEntry entry;

    entry.number = static_cast<int>(historyList.size()) + 1;
    entry.command = command;

    historyList.push_back(entry);
}

const std::vector<HistoryEntry>& HistoryManager::history()
{
    return historyList;
}

void HistoryManager::clear()
{
    historyList.clear();
}

const HistoryEntry* HistoryManager::get(int number)
{
    for(const auto& entry : historyList){

        if(entry.number==number){
            return &entry;
        }
    }

    return nullptr;
}

const HistoryEntry* HistoryManager::last()
{
    if(historyList.empty()){
        return nullptr;
    }

    return &historyList.back();
}

const HistoryEntry* HistoryManager::relative(int offset)
{
    if(offset <= 0 || offset > static_cast<int>(historyList.size())){
        return nullptr;
    }

    return &historyList[historyList.size() - offset];
}

bool HistoryManager::shouldExpand(const std::string& input)
{
    return !input.empty() && input[0] == '!';
}



std::string HistoryManager::expand(const std::string& input)
{
    if(input=="!!"){
        const HistoryEntry* entry = last();

        if(entry){
            return entry->command;
        }

        return input;
    }

    if(input.size()>2 && input[1]=='-'){
        int offset = std::stoi(input.substr(2));

        const HistoryEntry* entry = relative(offset);

        if(entry){
            return entry->command;
        }

        return input;
    }

    if(input.size()>1 && std::isdigit(static_cast<unsigned char>(input[1]))){
        int number = std::stoi(input.substr(1));

        const HistoryEntry* entry = get(number);

        if(entry){
            return entry->command;
        }

        return input;
    }

    return input;
}