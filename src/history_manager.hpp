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

    const HistoryEntry* relative(int offset);

    bool empty();

    int size();


    void clear();

    bool shouldExpand(const std::string& input);

    std::string expand(const std::string& input);

    bool load(const std::string& path);

    bool save(const std::string& path);

    bool append(const std::string& path);
}