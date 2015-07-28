#ifndef TOKEN_H
#define TOKEN_H

#include "token_types.h"
#include <vector>
#include <string>

enum lex_states
{
	LEX_STATE_SPACE,
	LEX_STATE_INT,
	LEX_STATE_DELIM,
	LEX_STATE_FLOAT,
	LEX_STATE_STRING,
	LEX_STATE_CHAR,
	LEX_STATE_ESCAPE,
	LEX_STATE_IDENT,
	LEX_STATE_OPERATOR,
};

enum
{
	INT_TYPE_BIN,
	INT_TYPE_OCT,
	INT_TYPE_DEC,
	INT_TYPE_HEX,
};

typedef int tokenType;

class token
{
    public:  
    token_category Category;
    tokenType Type;
	std::string Lexeme;

	token(tokenType t, std::string s);
	token(){}
      
};

typedef std::vector<token> tokenStream;

tokenStream Lex( std::string s );


void Error( std::string X );

#endif
