#include "token_types.h"
#include "lex.h"
#include "operators.h"
#include <iostream>
#include <string>

struct op;
//NOTE: prefixing ' indicates its a unary operator

static const struct op Operators[] = {
    {0,0,"unknown",false,false,1},
    
    //group 1
    {OP_ARRAY_ACCESS,   1, "[]", true,true,  LEFT_TO_RIGHT},
    {OP_POINTER_MEMBER, 1, "->", true,true,  LEFT_TO_RIGHT},
    {OP_OBJECT_MEMBER,  1, ".",  true,true,  LEFT_TO_RIGHT},
    {OP_SCOPING_OP,     1, "::", true,true,  LEFT_TO_RIGHT},
    {OP_POST_INCREMENT, 1, "++", true,false, LEFT_TO_RIGHT},
    {OP_POST_DECREMENT, 1, "--", true,false, LEFT_TO_RIGHT},
    
    //group 2
    {OP_LOGIC_NOT,     2, "'!",  false,true, RIGHT_TO_LEFT},
    {OP_BITWISE_NOT,   2, "'~",  false,true, RIGHT_TO_LEFT},
    {OP_PRE_INCREMENT, 2, "'++", false,true, RIGHT_TO_LEFT},
    {OP_PRE_DECREMENT, 2, "'--", false,true, RIGHT_TO_LEFT},
    {OP_UNARY_MINUS,   2, "'-",  false,true, RIGHT_TO_LEFT},
    {OP_UNARY_PLUS,    2, "'+",  false,true, RIGHT_TO_LEFT},
    {OP_DEREFERENCE,   2, "'*",  false,true, RIGHT_TO_LEFT},
    {OP_ADDRESS,       2, "'&",  false,true, RIGHT_TO_LEFT},
    {OP_UNREFERENCE,   2, "'@",  false,true, RIGHT_TO_LEFT},
    {OP_CAST,          2, "",    false,true, RIGHT_TO_LEFT},
    
    //group 3
    {OP_POINTER_MEMBER_POINTER, 3, "->*", true,true, LEFT_TO_RIGHT},
    {OP_OBJECT_MEMBER_POINTER,  3, ".*",  true,true, LEFT_TO_RIGHT},
    {OP_EXPONENT,   3, "#",   true,true, RIGHT_TO_LEFT},
  
    //group 4
    {OP_MUL,  4, "*", true,true, LEFT_TO_RIGHT},
    {OP_DIV,  4, "/", true,true, LEFT_TO_RIGHT},
    {OP_MOD,  4, "%", true,true, LEFT_TO_RIGHT},
    
    //group 5
    {OP_ADD,  5, "+", true,true, LEFT_TO_RIGHT},
    {OP_SUB,  5, "-", true,true, LEFT_TO_RIGHT},
    
    //group 6
    {OP_LSH,  6, "<<",  true,true, LEFT_TO_RIGHT},
    {OP_RSH,  6, ">>",  true,true, LEFT_TO_RIGHT},
    {OP_ROL,  6, "<<<", true,true, LEFT_TO_RIGHT},
    {OP_ROR,  6, ">>>", true,true, LEFT_TO_RIGHT},
    
    //group 7
    {OP_CMP_B,  7, "<",  true,true, LEFT_TO_RIGHT},
    {OP_CMP_BE, 7, "<=", true,true, LEFT_TO_RIGHT},
    {OP_CMP_A,  7, ">",  true,true, LEFT_TO_RIGHT},
    {OP_CMP_AE, 7, ">=", true,true, LEFT_TO_RIGHT},
    
    //group 8
    {OP_CMP_E,   8, "==", true,true, LEFT_TO_RIGHT},
    {OP_CMP_NE,  8, "!=", true,true, LEFT_TO_RIGHT},
    {OP_CMP_E_ADDR,   8, "===", true,true, LEFT_TO_RIGHT},
    {OP_CMP_NE_ADDR,  8, "!==", true,true, LEFT_TO_RIGHT},
    
    //group 9
    {OP_BITWISE_AND,  9, "&", true,true, LEFT_TO_RIGHT},
    
    //group 10
    {OP_BITWISE_XOR, 10, "^", true,true, LEFT_TO_RIGHT},
    
    //group 11
    {OP_BITWISE_IOR, 11, "|", true,true, LEFT_TO_RIGHT},
    
    //group 12
    {OP_LOGIC_AND,  12, "&&", true,true, LEFT_TO_RIGHT},
    
    //group 13
    {OP_LOGIC_XOR,  13, "^^", true,true, LEFT_TO_RIGHT},
    
    //group 14
    {OP_LOGIC_IOR,  14, "||", true,true, LEFT_TO_RIGHT},
    
    //group 15
    {OP_TERNARY_THEN, 15, "?", true,true, RIGHT_TO_LEFT},
    {OP_TERNARY_ELSE, 15, ":", true,true, RIGHT_TO_LEFT},
    
    //group 16
    {OP_MOV,     16, "=",   true,true, RIGHT_TO_LEFT},
    {OP_ADD_MOV, 16, "+=",  true,true, RIGHT_TO_LEFT},
    {OP_SUB_MOV, 16, "-=",  true,true, RIGHT_TO_LEFT},
    {OP_MUL_MOV, 16, "*=",  true,true, RIGHT_TO_LEFT},
    {OP_POW_MOV, 16, "#=",  true,true, RIGHT_TO_LEFT},
    {OP_DIV_MOV, 16, "/=",  true,true, RIGHT_TO_LEFT},
    {OP_MOD_MOV, 16, "%=",  true,true, RIGHT_TO_LEFT},
    {OP_AND_MOV, 16, "&=",  true,true, RIGHT_TO_LEFT},
    {OP_XOR_MOV, 16, "^=",  true,true, RIGHT_TO_LEFT},
    {OP_IOR_MOV, 16, "|=",  true,true, RIGHT_TO_LEFT},
    {OP_LSH_MOV, 16, "<<=", true,true, RIGHT_TO_LEFT},
    {OP_RSH_MOV, 16, ">>=", true,true, RIGHT_TO_LEFT},
    {OP_ROL_MOV, 16, "<<<=",true,true, RIGHT_TO_LEFT},
    {OP_ROR_MOV, 16, ">>>=",true,true, RIGHT_TO_LEFT},
    {OP_LOGIC_AND_MOV, 16, "&&=",  true,true, RIGHT_TO_LEFT},
    {OP_LOGIC_XOR_MOV, 16, "^^=",  true,true, RIGHT_TO_LEFT},
    {OP_LOGIC_IOR_MOV, 16, "||=",  true,true, RIGHT_TO_LEFT},
    
    //group 17
    {OP_SEQUENCE, 17, ",", true,true, LEFT_TO_RIGHT}
};

op GetOp(std::string Lexeme){
    int NumOps = sizeof(Operators)/sizeof(op);
    //std::cout << "NumOps:" << NumOps << std::endl;
    for( int i=0; i<NumOps;  i++ ){
        //std::cout << i << " : " << Operators[i].Lexeme << std::endl; 
        if( Operators[i].Lexeme == Lexeme ){
            return Operators[i];    
        }
    }
    Error("undefined operator: '" + Lexeme + "'");
}

//##############################################################################

static const struct keyword Keywords[] = { 
    { RES_ASM, "asm"},
    { RES_AUTO, "auto"},
    { RES_BOOL, "bool"},
    { RES_BREAK, "break"},
    { RES_CASE, "case"},
    { RES_CATCH, "catch"},
    { RES_CHAR, "char"},
    { RES_CLASS, "class"},
    { RES_CONST, "const"},
    { RES_CONST_CAST, "const_cast"},
    { RES_CONTINUE, "continue"},
    { RES_DEFAULT, "default"},
    { RES_DELETE, "delete"},
    { RES_DO, "do"},
    { RES_DOUBLE, "double"},
    { RES_DYNAMIC_CAST, "dynamic_cast"},
    { RES_ELSE, "else"},
    { RES_ENUM, "enum"},
    { RES_EXPLICIT, "explicit"},
    { RES_EXTERN, "extern"},
    { RES_FALSE, "false"},
    { RES_FLOAT, "float"},
    { RES_FOR, "for"},
    { RES_FRIEND, "friend"},
    { RES_GOTO, "goto"},
    { RES_IF, "if"},
    { RES_INLINE, "inline"},
    { RES_INT, "int"},
    { RES_LONG, "long"},
    { RES_INT64, "__int64" },
    { RES_MUTABLE, "mutable"},
    { RES_NAMESPACE, "namespace"},
    { RES_NEW, "new"},
    { RES_OPERATOR, "operator"},
    { RES_PRIVATE, "private"},
    { RES_PROTECTED, "protected"},
    { RES_PUBLIC, "public"},
    { RES_REGISTER, "register"},
    { RES_REINTERPRET_CAST, "reinterpret_cast"},
    { RES_RETURN, "return"},
    { RES_SHORT, "short"},
    { RES_SIGNED, "signed"},
    { RES_RETURN, "return"},
    { RES_SIZEOF, "sizeof"},
    { RES_STATIC, "static"},
    { RES_STATIC_CAST, "static_cast"},
    { RES_STRUCT, "struct"},
    { RES_SWITCH, "switch"},
    { RES_TEMPLATE, "template"},
    { RES_THIS, "this"},
    { RES_THROW, "throw"},
    { RES_TRUE, "true"},
    { RES_TRY, "try"},
    { RES_TYPEDEF, "typedef"},
    { RES_TYPEID, "typeid"},
    { RES_TYPENAME, "typename"},
    { RES_UNION, "union"},
    { RES_UNSIGNED, "unsigned"},
    { RES_USING, "using"},
    { RES_VIRTUAL, "virtual"},
    { RES_VOID, "void"},
    { RES_VOLATILE, "volatile"},
    { RES_WCHAR_T, "wchar_t"},
    { RES_WHILE, "while"},
    
    { PRE_RES_ENDIF, "endif"},
    { PRE_RES_DEFINE, "define"},
    { PRE_RES_UNDEF, "undef"},
    { PRE_RES_IFDEF, "ifdef"},
    { PRE_RES_IFNDEF, "ifndef"},
    { PRE_RES_UNDEF, "undef"}
};

keyword GetKeyword(std::string Lexeme){
    int NumKeywords = sizeof(Keywords)/sizeof(keyword);
    for( int i=0; i<NumKeywords;  i++ ){
        if( Keywords[i].Lexeme == Lexeme ){
            return Keywords[i];    
        }
    }
    Error("undefined keyword: " + Lexeme);
}

//##############################################################################
