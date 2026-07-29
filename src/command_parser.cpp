#include "command_parser.hpp"

RedirectInfo parseRedirection(std::vector<std::string>& tokens){
    std::string filename = "";
    int fd = STDOUT_FILENO;
    for(int i=0;i<tokens.size();i++){
        if(tokens[i]==">" || tokens[i]=="1>"){
            filename = tokens[i+1];
            tokens.erase(tokens.begin()+i);
            tokens.erase(tokens.begin()+i);
            return {true,fd,false,filename};
            
        }else if(tokens[i]=="2>"){
            fd = STDERR_FILENO;
            filename = tokens[i+1];
            tokens.erase(tokens.begin()+i);
            tokens.erase(tokens.begin()+i); 
            return {true,fd,false,filename};
        }else if(tokens[i]==">>" || tokens[i]=="1>>"){
            fd = STDOUT_FILENO;
            filename = tokens[i+1];
            tokens.erase(tokens.begin()+i);
            tokens.erase(tokens.begin()+i);
            return {true,fd,true,filename};
        }else if(tokens[i]=="2>>"){
            fd = STDERR_FILENO;
            filename = tokens[i+1];
            tokens.erase(tokens.begin()+i);
            tokens.erase(tokens.begin()+i);
            return {true,fd,true,filename};
        }

        

    }

    return {false,fd,false,""};
}