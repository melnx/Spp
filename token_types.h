#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

#include <string>

//##############################################################################
//##

enum token_category
{
    TOK_INT=0,
	TOK_FLOAT,
	TOK_STRING,
	TOK_CHAR,

	TOK_COMMA,
	TOK_SEMICOLON,
	TOK_COLON,
	TOK_QUESTION_MARK,
	TOK_OPEN_PARENTHESIS,
	TOK_CLOSE_PARENTHESIS,
	TOK_OPEN_BRACKET,
	TOK_CLOSE_BRACKET,
	TOK_OPEN_BRACE,
	TOK_CLOSE_BRACE,

	TOK_PPRES,
	TOK_RESERVED,
	
    TOK_NUMERIC,
    TOK_OPERATOR,
    TOK_DELIMITER,
    TOK_IDENT_FUNC,
    TOK_FUNC_CALL,
    TOK_ARRAY_SCOPE,
    TOK_USER_TYPE,
    TOK_FUNC_RETURN,
    TOK_FPOINTER,
    
    TOK_IDENT,
};

//##############################################################################
//## if a token has a category TOK_RESWORD, then this is its Type

enum reswords
{
    RES_ASM=500,
    RES_AUTO,
    RES_BOOL,
    RES_BREAK,
    RES_CASE,
    RES_CATCH,
    RES_CHAR,
    RES_CLASS,
    RES_CONST,
    RES_CONST_CAST,
    RES_CONTINUE,
    RES_DEFAULT,
    RES_DELETE,
    RES_DO,
    RES_DOUBLE,
    RES_DYNAMIC_CAST,
    RES_ELSE,
    RES_ENUM,
    RES_EXPLICIT,
    RES_EXTERN,
    RES_FALSE,
    RES_FLOAT,
    RES_FOR,
    RES_FRIEND,
    RES_GOTO,
    RES_IF,
    RES_INLINE,
    RES_INT,
    RES_LONG,
    RES_PTR,
    RES_INT64,
    RES_MUTABLE,
    RES_NAMESPACE,
    RES_NEW,
    RES_OPERATOR,
    RES_PRIVATE,
    RES_PROTECTED,
    RES_PUBLIC,
    RES_REGISTER,
    RES_REINTERPRET_CAST,
    RES_RETURN,
    RES_SHORT,
    RES_SIGNED,
    RES_SIZEOF,
    RES_STATIC,
    RES_STATIC_CAST,
    RES_STRUCT,
    RES_SWITCH,
    RES_TEMPLATE,
    RES_THIS,
    RES_THROW,
    RES_TRUE,
    RES_TRY,
    RES_TYPEDEF,
    RES_TYPEID,
    RES_TYPENAME,
    RES_UNION,
    RES_UNSIGNED,
    RES_USING,
    RES_VIRTUAL,
    RES_VOID,
    RES_VOLATILE,
    RES_WCHAR_T,
    RES_WHILE,
    
    PRE_RES_DEFINE,
    PRE_RES_INCLUDE,
    PRE_RES_UNDEF,
    PRE_RES_IFDEF,
    PRE_RES_IFNDEF,
    PRE_RES_ENDIF,
};

//##############################################################################
//## if a token has a category TOK_OPERATOR than this is its Type

enum operators
{
    
    OP_NONE = 1000,
     
    //group 1,
    OP_ARRAY_ACCESS,
    OP_POINTER_MEMBER,
    OP_OBJECT_MEMBER,
    OP_SCOPING_OP,
    OP_POST_INCREMENT,
    OP_POST_DECREMENT,
    
    //group 2,
    OP_LOGIC_NOT,
    OP_BITWISE_NOT,
    OP_PRE_INCREMENT,
    OP_PRE_DECREMENT,
    OP_UNARY_MINUS,
    OP_UNARY_PLUS,
    OP_DEREFERENCE,
    OP_ADDRESS,
    OP_UNREFERENCE,
    OP_CAST,
    
    //group 3,
    OP_POINTER_MEMBER_POINTER,
    OP_OBJECT_MEMBER_POINTER,
    OP_EXPONENT,
    
    //group 4,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    
    //group 5,
    OP_ADD,
    OP_SUB,
    
    //group 6,
    OP_LSH,
    OP_RSH,
    OP_ROL,
    OP_ROR,
    
    //group 7,
    OP_CMP_B,
    OP_CMP_BE,
    OP_CMP_A,
    OP_CMP_AE,
    
    //group 8,
    OP_CMP_E,
    OP_CMP_NE,
    OP_CMP_E_ADDR,
    OP_CMP_NE_ADDR,
    
    //group 9,
    OP_BITWISE_AND,
    
    //group 10,
    OP_BITWISE_XOR,
    
    //group 11,
    OP_BITWISE_IOR,
    
    //group 12,
    OP_LOGIC_AND,
    
    //group 13,
    OP_LOGIC_XOR,
    
    //group 14,
    OP_LOGIC_IOR,
    
    //group 15,
    OP_TERNARY_THEN,
    OP_TERNARY_ELSE,
    
    //group 16,
    OP_MOV,
    OP_ADD_MOV,
    OP_SUB_MOV,
    OP_MUL_MOV,
    OP_DIV_MOV,
    OP_MOD_MOV,
    OP_POW_MOV,
    OP_AND_MOV,
    OP_XOR_MOV,
    OP_IOR_MOV,
    OP_LSH_MOV,
    OP_RSH_MOV,
    OP_ROL_MOV,
    OP_ROR_MOV,
    OP_LOGIC_AND_MOV,
    OP_LOGIC_XOR_MOV,
    OP_LOGIC_IOR_MOV,
    
    //group 17,
    OP_SEQUENCE,
};



#endif
