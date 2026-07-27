#include <tokenizer.hpp>
#include<vector>

std::vector<std::string> tokenize(const std::string &input)
{
    std::vector<std::string> tokens;
    std::string current;
    bool inSingleQuotes = false;


    for(char c: input){
        if(c=='\''){
            inSingleQuotes = !inSingleQuotes;
        }else if(c==' ' && inSingleQuotes==false){
            if(!current.empty()){
                tokens.push_back(current);
                current.clear();
                continue;
            }
        }else{
            current.push_back(c);
        }
    }
    if(!current.empty())tokens.push_back(current);
    return tokens;
}