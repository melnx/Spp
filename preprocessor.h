#ifndef PRE_H
#define PRE_H

#include "lex.h"

#include <vector>
#include <map>
#include <string>
#include <stack>


class preprocessor
{
    struct macro
	{
        std::string Name;
        std::vector<std::string> Args;
        std::vector<token> Data;
    };

    public:  
    std::string PreProcessFile(std::string Path);
    private:
    std::stack<bool> IfTrue;
    std::map<std::string,macro> Macros;    
    std::string PreProcess(std::string Path, std::vector<token> Tokens,bool& InComment);
};

#endif
