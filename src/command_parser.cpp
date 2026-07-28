#include "command_parser.hpp"

RedirectInfo parseRedirection(std::vector<std::string>& tokens){
    std::string filename = "";
    for(int i=0;i<tokens.size();i++){
        if(tokens[i]==">" || tokens[i]=="1>"){
            filename = tokens[i+1];
            tokens.erase(tokens.begin()+i);
            tokens.erase(tokens.begin()+i); 
            return {true,filename};
        }

    }

    return {false,""};
}