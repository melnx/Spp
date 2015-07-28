#ifndef PARSER_H
#define PARSER_H

#include "tree.h"
#include "lex.h"
#include "var.h"
#include "func.h"
#include "type.h"

#include <vector>
#include <map>
#include <string>
#include <stack>
#include <sstream>

class parser
{
    public:
    
    std::map<std::string,var> Vars;
    std::map<std::string,func> Funcs;
    std::vector<type> Types;
	std::map<std::string,int> TypeMap;
      
    void ParseResWord( std::vector<token> Tokens, tree& Tree );  
    void LoadTree( std::vector<token> Tokens, tree& Tree );
    
    void Declare(std::string Script);
    void Declare(std::vector<token>& Tokens, std::string Class, int Depth=0);
    
    std::vector< std::vector<token> > 
    GetStatements( std::vector<token> Tokens, int Div, bool AllowBraceTerm, bool AddTail );
    
    std::vector<token> 
    GetScope(std::vector<token> Tokens, int Start, int &End, int Open, int Close);
    void PreParse(std::vector<token>& Tokens);
    void PreParseFuncCalls( std::vector<token>& Tokens );

};

#endif
