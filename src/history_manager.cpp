#include "history_manager.hpp"
#include <cctype>
#include <string>
#include <fstream>

namespace
{
    std::vector<HistoryEntry> historyList;
    int lastSavedIndex = 0;
    void insertEntry(const std::string& command) {
        HistoryEntry entry;

        entry.number = static_cast<int>(historyList.size()) + 1;
        entry.command = command;

        historyList.push_back(entry);
    }
}


const std::vector<HistoryEntry>& HistoryManager::history()
{
    return historyList;
}

void HistoryManager::add(const std::string& command)
{
    insertEntry(command);
}

void HistoryManager::clear()
{
    historyList.clear();
    lastSavedIndex = 0;
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

bool HistoryManager::empty()
{
    return historyList.empty();
}

int HistoryManager::size()
{
    return static_cast<int>(historyList.size());
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

bool HistoryManager::load(const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open()){
        return false;
    }

    std::string line;

    while(std::getline(file, line)){

        if(line.empty()){
            continue;
        }

        add(line);
    }

    lastSavedIndex = static_cast<int>(historyList.size());

    return true;
}

bool HistoryManager::save(const std::string& path)
{
    std::ofstream file(path);

    if(!file.is_open()){
        return false;
    }

    for(const auto& entry : historyList){
        file << entry.command << '\n';
    }
    lastSavedIndex = static_cast<int>(historyList.size());

    return true;
}

bool HistoryManager::append(const std::string& path)
{
    std::ofstream file(path, std::ios::app);
    if(!file.is_open()){
            return false;
        }
    for(int i = lastSavedIndex; i < static_cast<int>(historyList.size()); i++){
        
        file << historyList[i].command << '\n';
    }

    lastSavedIndex = static_cast<int>(historyList.size());
    return true;
}