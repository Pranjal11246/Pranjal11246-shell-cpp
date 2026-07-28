#include "tokenizer.hpp"
#include<vector>


std::vector<std::string> tokenize(const std::string &input)
{
   
    std::vector<std::string> tokens;
    std::string current;
    bool inSingleQuotes = false;
    bool inDoubleQuotes = false;
    bool escaped = false;

    for(char c: input){
        if(escaped){
            current.push_back(c);
            escaped = false;
        }else if(c=='\\'){
            escaped = true;
        }else{
            if(c=='\'' && !inDoubleQuotes && !escaped){
            inSingleQuotes = !inSingleQuotes;
        }else if(c=='"' && !inSingleQuotes && !escaped){
            inDoubleQuotes = !inDoubleQuotes;
        }else if(c==' ' && inSingleQuotes==false && !inDoubleQuotes){
            if(!current.empty()){
                tokens.push_back(current);
                current.clear();
                continue;
            }
        }else{
            current.push_back(c);
        }
        }
        
    }
    if(!current.empty())tokens.push_back(current);
    return tokens;
}