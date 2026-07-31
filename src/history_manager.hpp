#pragma once

#include <string>
#include <vector>

struct HistoryEntry
{
    int number;
    std::string command;
};

namespace HistoryManager
{
    void add(const std::string& command);

    const std::vector<HistoryEntry>& history();

    const HistoryEntry* get(int number);
    const HistoryEntry* last();

    void clear();
}