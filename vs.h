#ifndef VIRTUAL_STACK_H
#define VIRTUAL_STACK_H

#include "var.h"
#include <stack>

class vs
{
    public:  
             
    static ptr StackBegin;
    static ptr StackPointer;
    static std::stack<ptr> Scopes;
    static ptr push_int(int Int);
    static ptr push_int64(__int64 Int);
    static ptr push_char(char Char);
    static ptr push_float(float Float);
	static ptr push_mem(ptr Address, int Size);
    static ptr push_string(std::string String);
    static ptr pop(int Bytes);
    static ptr alloc(int Bytes);
    static void enter_scope();
    static void exit_scope();
};


#endif
