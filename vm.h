#ifndef VM_H
#define VM_H

#include "var.h"
#include "parser.h"
#include "preprocessor.h"
#include "func.h"
#include "type.h"
#include <map>
#include <string>
#include <vector>
#include <stack>

std::ostream& operator<<(std::ostream& stm, var& v);

struct exception
{
    std::string Type;
};

class vm : public parser, public preprocessor
{
    public:
    
    int LoopInterrupt;
    
    bool Eval( std::string Script );
    var* Eval( tree& Tree, std::map<std::string,var>& Vars, ptr This );  
    bool PreEval( tree& Tree, std::map<std::string,var>& Vars, ptr& This, int TypeId=-1, int Depth=0, int* DeepestClass=0x0 );
    
    int DeclareHostFuncs();
	int DeclareHostTypes();
    
    std::stack<bool> IfResults;
    
    vm();
    vm(parser& Parser);
    ~vm();
    
    int SizeOf(var Var);
    int SizeOf(int Type);
    int TokToType(int TokenType);
    
    std::string TypeId(var Var);
	std::string ClassIdToName( int Id );
};

#endif
