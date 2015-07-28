#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include "vm.h"
#include "token_types.h"
#include "vs.h"

using namespace std;

vm::vm(){
    //DeclareHostFuncs();        
    //Vars["endl"] = var( TOK_STRING , "\n" );
    //Vars["tab"] = var( TOK_STRING , "\t" );
    DeclareHostFuncs();
	DeclareHostTypes();
    LoopInterrupt = 0;
}

vm::vm(parser& Parser){    
    *this = vm();         
}

vm::~vm(){
}

int vm::SizeOf(var Var){
    switch(Var.Type){
        case T_INT: return sizeof(int);
        case T_FLOAT: return sizeof(float);
        case T_CHAR: return sizeof(char);
        case T_BOOL: return sizeof(bool);
		case T_VOID: return 0;
        case T_OBJECT: return Types[Var.Class].Size;
    }
    
    return 1;    
}

int vm::SizeOf(int Type){
    switch(Type){
		case T_FUNC: return sizeof(ptr);
        case T_PTR: return sizeof(ptr);         
        case T_INT: return sizeof(int);
        case T_FLOAT: return sizeof(float);
        case T_CHAR: return sizeof(char);
        case T_BOOL: return sizeof(bool);
		case T_VOID: return 0;
        case T_INT64: return sizeof(__int64);
    }
    
    return 1;    
}

string vm::TypeId(var Var){
    string Type;
    switch(Var.Type){
        case T_INT:  Type="int"; break;
        case T_FLOAT: Type="float"; break;
        case T_CHAR: Type="char"; break;
        case T_BOOL: Type="bool"; break;
		case T_VOID: Type="void"; break;
		case T_FUNC: Type="fpointer"; break;
        case T_OBJECT: Type = ClassIdToName(Var.Class); break;
        case T_PTR:{ 
            Var.Type = Var.PtrType; 
            Type = TypeId(Var);
            for(int i=0; i<Var.Dim; i++) Type += "*";
        }
    }       
    return Type;
}

int vm::TokToType(int TokenType){
    switch( TokenType ){
        case TOK_ARRAY_SCOPE:
        case OP_ADDRESS:  
        case RES_TYPEID:
        case RES_SIZEOF:
		case OP_POINTER_MEMBER:
		case OP_OBJECT_MEMBER:
        case TOK_INT: return T_INT;
        case TOK_FLOAT: return T_FLOAT;
        case TOK_CHAR: return T_CHAR;
        case TOK_STRING: return T_STRING;
    }    
    return TokenType+100;
}

string vm::ClassIdToName( int Id ){
	for(map<string,int>::iterator i=TypeMap.begin(); i!=TypeMap.end(); i++){
		if( i->second == Id ) return i->first;
	}
	return "";
}
