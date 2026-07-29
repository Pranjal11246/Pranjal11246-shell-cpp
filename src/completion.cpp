#include"completion.hpp"
#include <algorithm>      
#include <cstring>        
#include <readline/readline.h>
#include <readline/history.h>

static std::vector<std::string> matches;

char* commandGenerator(const char* text, int state)
{
    static size_t index;

    if (state == 0)
        index = 0;

    if (index >= matches.size())
        return nullptr;

    return strdup(matches[index++].c_str());
}

char** completionCallback(
    const char* text,
    int start,
    int end)
{
    (void)text;
    (void)start;

    std::string line(rl_line_buffer);

    matches = Completion::getCompletions(line, end);

    return rl_completion_matches("", commandGenerator);
}
namespace Completion{
std::vector<std::string> getBuiltins()
{
    return {
        "echo",
        "exit",
        "type",
        "pwd",
        "cd"
    };
}

std::vector<std::string> matchPrefix(const std::vector<std::string>& choices,const std::string& prefix){
    std::vector<std::string> res;
    for(auto const& choice:choices){
        if(choice.starts_with(prefix))res.push_back(choice);
    }
    return res;
}

std::string getCurrentWord(const std::string& line,int cursorPos){
    int i = cursorPos-1;
    std::string res="";
    while(i>=0){
        if(line[i]==' ')break;
        res+=line[i];
        i--;
    }

    std::reverse(res.begin(),res.end());
    return res;
}

std::vector<std::string> getCompletions(const std::string& line,int cursorPos){
    std::string prefix = getCurrentWord(line, cursorPos);
    return matchPrefix(getBuiltins(),prefix);
}
}