#include "history_manager.hpp"

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