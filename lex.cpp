#include <iostream>
#include <string>
#include "lex.h"
#include "token_types.h"

using std::cout;
using std::string;


//cast, sizeof, comma
typedef int lexState;
typedef int intType;

std::string ValidOperators[] = { "->", ".", "::", "++", "--", "!", "~", "-", "+", "*", "&", "/", "%", "<<", ">>", "<<<", ">>>", "<", "<=", ">", ">=", "==", "!=", "&", "^" , "|", "&&", "||", "=", "+=", "-=", "*=", "/=", "%=", "&=", "^=", "|=", "<<=", ">>=", "<<<=", ">>>=", "===", "!==", "@", "#", "#=", "!", "?", ":"};
const std::string Resword_Literals[] = { "asm", "auto", "bool", "break", "case", "catch", "char", "class", "const", "const_cast", "continue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace", "new", "operator", "private", "protected", "public", "register", "reinterpret_cast", "return", "short", "signed", "sizeof", "static", "static_cast", "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "__int64"};
const std::string PreprocessorKeywords[] = { "include", "define", "ifdef", "ifndef", "undef", "endif" };

bool CanAddCharToOp(string CurrLexeme, char c);
tokenType LexStateToTokenType(lexState ls, string Lexeme);
string TokenTypeToString( tokenType t );

bool IsOperator( char c );
bool IsNumeric( char c, intType type );
bool IsSpace( char c );
bool IsDelim( char c );
bool IsIdent( char c );

token::token(tokenType t, string s)
{
	Type = t;
	Lexeme = s;
}

tokenStream Lex ( string s )
{
	//add a newline to the end
	s+="\n";

	//Initialize the return value
	tokenStream r;

	//Initialize a pointer to the beginning of the file
	char* File = (char*) s.c_str();
	
	lexState State = LEX_STATE_SPACE;
	lexState Prev = LEX_STATE_SPACE;
	lexState StateBeforeEscape;

	intType IntType = INT_TYPE_DEC;

	//Declare vars
	char c;
	bool AddChar, LexemeDone, FirstTokenOnLine = true;
	string CurrLexeme = "";
	LexemeDone = false;

	//Loop through each character until we find the null-terminator
	while( 1 )
	{
		c = *(File++);

		AddChar = true;

		//Take the appropriate action based on our current state
		switch(State)
		{			
			case LEX_STATE_SPACE:
				AddChar = false;
				//If we find a numeric char, assume we have an integer
				if( IsNumeric(c, IntType) )
				{
					State = LEX_STATE_INT;
					AddChar = true;
				}

				else if( IsDelim(c) )
				{
					State = LEX_STATE_DELIM;
					AddChar = true;
				}

				else if( IsIdent(c) )
				{
					State = LEX_STATE_IDENT;
					AddChar = true;
				}

				else if( c == '\"' )
				{
					State = LEX_STATE_STRING;
					AddChar = false;
				}

				else if( c == '\'')
				{
					State = LEX_STATE_CHAR;
					AddChar = false;
				}

				else if( IsOperator(c) )
				{
					State = LEX_STATE_OPERATOR;
					AddChar = true;
				}
				break;

			case LEX_STATE_INT:

				//If we find a space, the lexeme is over
				if( IsSpace(c) )
				{
					AddChar = false;
					State = LEX_STATE_SPACE;
					LexemeDone = true;
				}

				else if( IsDelim(c) )
				{
					State = LEX_STATE_DELIM;
					LexemeDone = true;
				}

				else if( c == '\"' )
				{
					State = LEX_STATE_STRING;
					LexemeDone = true;
				}

				else if( c == '.' )
				{
					State = LEX_STATE_FLOAT;
				}

				else if( IsNumeric(c, IntType) )
				{
				}
				
				else if( (c == 'b' || c == 'B') && (CurrLexeme == "0" || CurrLexeme == "-0") )
				{
					IntType = INT_TYPE_BIN;
				}

				else if( (c == 'x' || c == 'X') && (CurrLexeme == "0" || CurrLexeme == "-0") )
				{
					IntType = INT_TYPE_HEX;
				}

				else if( (c == 'o' || c == 'O') && (CurrLexeme == "0" || CurrLexeme == "-0") )
				{
					IntType = INT_TYPE_OCT;
				}

				else if( IsIdent(c) )
				{
					State = LEX_STATE_IDENT;
					LexemeDone = true;
				}

				else if( IsOperator(c) )
				{
					State = LEX_STATE_OPERATOR;
					AddChar = true;
					LexemeDone = true;
				}

				else
					LexemeDone = true;

				break;

			case LEX_STATE_DELIM:
				if(c == ':')
					State = LEX_STATE_OPERATOR;
				else
				{
					LexemeDone = true;
					State = LEX_STATE_SPACE;
				}
				break;

			case LEX_STATE_FLOAT:

				if( IsSpace(c) )
				{
					AddChar = false;
					State = LEX_STATE_SPACE;
					LexemeDone = true;
				}

				else if( IsDelim(c) )
				{
					State = LEX_STATE_DELIM;
					LexemeDone = true;
				}

				else if( c == '\"' )
				{
					State = LEX_STATE_STRING;
					AddChar = false;
					LexemeDone = true;
				}

				else if( IsNumeric(c, IntType) )
				{
				}

				else if( IsIdent(c) )
				{
					State = LEX_STATE_IDENT;
					LexemeDone = true;
				}

				else if( IsOperator(c) )
				{
					State = LEX_STATE_OPERATOR;
					AddChar = true;
					LexemeDone = true;
				}
				break;

			case LEX_STATE_STRING:
				if( c == '\\')
				{
					StateBeforeEscape = LEX_STATE_STRING;
					State = LEX_STATE_ESCAPE;
					AddChar = false;
				}
				if( c == '\"' )
				{
					AddChar = false;
					LexemeDone = true;
				}
				break;
			case LEX_STATE_CHAR:
				if(c == '\\')
				{
					StateBeforeEscape = LEX_STATE_CHAR;
					State = LEX_STATE_ESCAPE;
					AddChar = false;
				}
				if(c == '\'')
				{
					AddChar = false;
					LexemeDone = true;
				}
				break;

			case LEX_STATE_ESCAPE:
				if( c == 't' )
					c = '\t';
				else if( c == 'b' )
					c = '\b';
				else if( c == '0' )
					c = '\0';
				else if( c == 'n' )
					c = '\n';
				else if( c == 'r' )
					c = '\r';
				else if( c == 'a' )
					c = '\a';
				else if( c == 'v' )
					c = '\v';
				else if( c == 'f' )
					c = '\f';
				State = StateBeforeEscape;
				break;

			case LEX_STATE_IDENT:
				if( IsSpace(c) )
				{
					AddChar = false;
					State = LEX_STATE_SPACE;
					LexemeDone = true;
				}

				else if( IsDelim(c) )
				{
					State = LEX_STATE_DELIM;
					LexemeDone = true;
				}

				else if( c == '\"' )
				{
					State = LEX_STATE_STRING;
					AddChar = false;
					LexemeDone = true;
				}

				else if( IsOperator(c) )
				{
					State = LEX_STATE_OPERATOR;
					AddChar = true;
					LexemeDone = true;
				}
				break;

			case LEX_STATE_OPERATOR:
				if( IsSpace(c) )
				{
					AddChar = false;
					State = LEX_STATE_SPACE;
					LexemeDone = true;
				}

				else if( IsDelim(c) )
				{
					State = LEX_STATE_DELIM;
					LexemeDone = true;
				}

				else if( c == '\"' )
				{
					State = LEX_STATE_STRING;
					LexemeDone = true;
				}

				else if( IsNumeric(c, IntType) )
				{
					if(CurrLexeme == ".")
						State = LEX_STATE_FLOAT;
					else
					{
						if(FirstTokenOnLine)
						{
							State = LEX_STATE_INT;
							LexemeDone = true;
						}
						else
						{
							if( r.size() > 0 )
							{
								tokenType last = r[r.size() - 1].Type;
								if(
									(CurrLexeme != "+" && CurrLexeme != "-" ) ||
									(last == TOK_FLOAT || last == TOK_INT || last == TOK_STRING || last == TOK_IDENT || last == TOK_CLOSE_PARENTHESIS || last == TOK_CLOSE_BRACKET)
								)
									LexemeDone = true;
								else
									State = LEX_STATE_INT;
							}
							else
								State = LEX_STATE_INT;
						}
					}
				}

				else if( IsIdent(c) )
				{
					State = LEX_STATE_IDENT;
					LexemeDone = true;
				}

				else if( CurrLexeme == "&")
				{
					if( r.size() > 0 )
					{
						tokenType last = r[r.size() - 1].Type;
						if( !(last == TOK_FLOAT || last == TOK_INT || last == TOK_STRING || last == TOK_IDENT || last == TOK_CLOSE_PARENTHESIS || last == TOK_CLOSE_BRACKET) )
							LexemeDone = true;
					}
					else
						LexemeDone = true;

				}

				else if( !CanAddCharToOp(CurrLexeme, c) )
					LexemeDone = true;
				break;

		};

		if( AddChar )
			CurrLexeme += c;

		if( LexemeDone )
		{
			if( AddChar )
			{
				File--;
				CurrLexeme = CurrLexeme.substr(0, CurrLexeme.length() - 1);
			}

			State = LEX_STATE_SPACE;
			tokenType type = LexStateToTokenType( Prev, CurrLexeme );
			if(type == TOK_CHAR && CurrLexeme.length() != 1)
				Error("Multiple characters in character literal.");
			if(type == TOK_INT && (CurrLexeme == "0b" || CurrLexeme == "0B" || CurrLexeme == "0o" || CurrLexeme == "0O" || CurrLexeme == "0x" || CurrLexeme == "0X"))
				Error("Numerical prefix without a following number.");
			r.push_back( token ( type, CurrLexeme ) );
			CurrLexeme = "";
			LexemeDone = false;
			FirstTokenOnLine = false;
			IntType = INT_TYPE_DEC;
		}

		//Save the last loop's state
		Prev = State;

		if( c == '\n' )
			FirstTokenOnLine = true;

		if( c == '\0' )
			break;
	}

	//Clear our pointer
	File = NULL;

	return r;
}


bool IsNumeric( char c, intType type )
{
	if( type == INT_TYPE_BIN)
		return ( c == '0' || c == '1');
	if(type == INT_TYPE_OCT)
		return ( c >= '0' && c <= '7');
	if( type == INT_TYPE_DEC )
		return ( c >= '0' && c <= '9');
	if( type == INT_TYPE_HEX )
		return ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') );

	return false;
}

bool IsSpace( char c )
{
	return ( c == ' ' || c == '\t' || c == '\n' || c == '\0');
}

bool IsDelim( char c )
{
	return ( c == ',' || c == ';' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}');
}

bool IsIdent( char c )
{
	return ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' );
}

tokenType LexStateToTokenType(lexState ls, string Lexeme)
{
	switch(ls)
	{
		case LEX_STATE_INT:
			return TOK_INT;
		case LEX_STATE_DELIM:
		{
			if( Lexeme == "," )
				return TOK_COMMA;
			if( Lexeme == ";")
				return TOK_SEMICOLON;
			if( Lexeme == ":")
				return TOK_COLON;
			if( Lexeme == "?")
				return TOK_QUESTION_MARK;
			if( Lexeme == "(")
				return TOK_OPEN_PARENTHESIS;
			if( Lexeme == ")")
				return TOK_CLOSE_PARENTHESIS;
			if( Lexeme == "[")
				return TOK_OPEN_BRACKET;
			if( Lexeme == "]")
				return TOK_CLOSE_BRACKET;
			if( Lexeme == "{")
				return TOK_OPEN_BRACE;
			if( Lexeme == "}")
				return TOK_CLOSE_BRACE;
			Error("Unknown Delimiter");
		}
		case LEX_STATE_FLOAT:
			return TOK_FLOAT;
		case LEX_STATE_STRING:
			return TOK_STRING;
		case LEX_STATE_CHAR:
			return TOK_CHAR;
		case LEX_STATE_IDENT:
			for(int i = 0; i < sizeof(Resword_Literals) / sizeof(string); i++)
				if(Lexeme == Resword_Literals[i])
					return TOK_RESERVED;
			for(int i = 0; i < sizeof(PreprocessorKeywords) / sizeof(string); i++)
				if(Lexeme == PreprocessorKeywords[i])
					return TOK_PPRES;
			return TOK_IDENT;
		case LEX_STATE_OPERATOR:
			return TOK_OPERATOR;
	}
	Error("Unknown Lex State");
	return 0;
}

bool IsOperator( char c )
{
	int NumOperators = sizeof(ValidOperators) / sizeof(string);
	for(int i = 0; i < NumOperators; i++)
	{
		if(ValidOperators[i].find(c) != string::npos)
			return true;
	}

	return false;
}

bool CanAddCharToOp(string CurrLexeme, char c)
{
	int l = CurrLexeme.length() + 1;
	string NextLexeme = CurrLexeme + c;
	for(int i = 0; i < sizeof(ValidOperators) / sizeof(string); i++)
	{
		if(ValidOperators[i].substr(0, l) == NextLexeme)
			return true;
	}
	return false;
}
