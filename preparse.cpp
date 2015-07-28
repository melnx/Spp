#include "lex.h"
#include "parser.h"
#include "token_types.h"
#include "operators.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

void parser::PreParseFuncCalls( vector<token>& Tokens )
{

	for(int i=0; i<Tokens.size()-1; i++){
		bool IsCast = false;

		//if this sees something like add(4,5) it makes it into (add)(4,5)
		if( Tokens[i].Type == TOK_IDENT ){

			//if the token next to an identifier is a parenthesis then it is a function call
			if( Tokens[ i+1 ].Type == TOK_OPEN_PARENTHESIS ){
				Tokens.insert( Tokens.begin()+i, token( TOK_OPEN_PARENTHESIS, "(" ) );
                Tokens.insert( Tokens.begin()+i+2, token( TOK_CLOSE_PARENTHESIS, ")" ) );
				i+=2;
			}

		}

		//process casts
		if( Tokens[i].Type == TOK_RESERVED || Tokens[i].Type == TOK_USER_TYPE ){

			for(int j=i+1; j<Tokens.size(); j++ ){
			}
  
		}

	}

	//for(int i=0; i<Tokens.size(); i++) cout << Tokens[i].Lexeme << " "; cout << endl;
}

//##############################################################################

void parser::ParseResWord( vector<token> Tokens, tree& Tree )
{
    
    keyword Keyword;
    
    if( Tokens[0].Type == TOK_RESERVED ){
        Keyword = GetKeyword( Tokens[0].Lexeme ); 
    }else{
        Keyword.Type = Tokens[0].Type;      
        Keyword.Lexeme = Tokens[0].Lexeme;
    }
    
    Tree.Data = Keyword.Lexeme;
    Tree.Type = Keyword.Type; 
    
    int End=0;
    
    //contains everything inside ()
    vector<token> Arg;
    vector< vector<token> > Args;
    
    //contains everything inside {}
    vector<token> Proc;
    vector< vector<token> > Procs;
     
    switch( Keyword.Type ){
        //keyword(){}         
        case RES_IF :
        case RES_FOR :
        case RES_WHILE :
        case RES_SWITCH :
        case RES_CATCH :
              
            //get content of ()  
            Arg = GetScope( Tokens, End, End, TOK_OPEN_PARENTHESIS, TOK_CLOSE_PARENTHESIS );
            
            //split the instructions inside the ()
			if( Keyword.Type == RES_FOR ){//(a;b;c)
                Args = GetStatements( Arg, TOK_SEMICOLON, false, true );
            }else{//(a,b,c)
                Args = GetStatements( Arg, TOK_COMMA, false, true );
            }
            
            ++End;
        
        //keyword{}
        case RES_ELSE :
        case RES_DO :    
        case RES_ASM :{
            
			//get the block
            if( !End ) ++End;
            
            if( End >= Tokens.size() ){
            }else if( Tokens[End].Type == TOK_OPEN_BRACE ){ //multi instruction
                Proc = GetScope(Tokens, End, End, TOK_OPEN_BRACE, TOK_CLOSE_BRACE);
                Procs = GetStatements( Proc, TOK_SEMICOLON, true, false );
            }else{ //single instruction
                Procs.push_back( vector<token>(Tokens.begin()+End,Tokens.end()) );  
            }
            
            //size the branch properly
            Tree.Branch = vector<tree>( Args.size()+Procs.size() );
    
            //add the parameters
            int i=0;
            for(; i<Args.size(); i++ ){
                LoadTree( Args[i], Tree.Branch[i] );
            }
            
            //no body if its NOT a function call, add the block
            for(int j=0; j<Procs.size(); j++,i++ ){        
                LoadTree( Procs[j], Tree.Branch[i] );
            }
            
			//the tree type is the type of the key word
            Tree.Type = Keyword.Type;
            
            return;
        }
        
        //keyword
        case RES_TRUE :
        case RES_FALSE :
        case RES_BREAK :    
        case RES_CONTINUE :
            Tree.Type = Keyword.Type;
            return;
        
        //keyword x;
        case RES_NEW :
            Tokens[1].Type = 0;

        case RES_DELETE :
             
        case RES_VOID :
        case RES_BOOL :
        case RES_CHAR :
        case RES_SHORT :
        case RES_INT :
		case TOK_USER_TYPE :
        case RES_INT64 :
        case RES_LONG :
        case RES_FLOAT :
        case RES_DOUBLE :
        
        case RES_GOTO :
        case RES_RETURN :
        case RES_THROW :
        case RES_CONST :
        case RES_MUTABLE :
        case RES_STATIC :
        case RES_AUTO :
        case RES_UNSIGNED :
        case RES_SIGNED :
        case RES_SIZEOF :
        case RES_TYPEID :
        case TOK_FUNC_RETURN:  
            
			if( Tokens.size() > 1 ){
				Tree.Branch = vector<tree>(1);
                LoadTree( vector<token>(Tokens.begin()+1,Tokens.end()), Tree.Branch[0] );
			}

            return;
            
    }
     
}

//##############################################################################

void parser::PreParse(vector<token>& Tokens){
    int Size = Tokens.size();
    
	//convert user defined types to TOK_USER_TYPE
	for(int i=0; i<Size; i++){
		if( TypeMap.find(Tokens[i].Lexeme) != TypeMap.end() ){
			cout << "known type " << Tokens[i].Lexeme << endl;
			Tokens[i].Type = TOK_USER_TYPE;
		}
	}

    //pre-process the index operator from a[b] to a[](b) NOTE, 
    //the () aren't needed, [] has a high precedence anyway
    
    stack<int> OpenIndex;
    for(int i=0; i<Tokens.size(); i++){
        if( Tokens[i].Type == TOK_OPEN_BRACKET ) OpenIndex.push(i); 
        else if( Tokens[i].Type == TOK_CLOSE_BRACKET ){
            Tokens[OpenIndex.top()] = token( TOK_OPERATOR, "[]" );
            Tokens[i] = token( TOK_CLOSE_PARENTHESIS, ")" );
            Tokens.insert( Tokens.begin() + OpenIndex.top()+1 , token(TOK_OPEN_PARENTHESIS,"(") );
            OpenIndex.pop();
        }else if( Tokens[i].Type == TOK_RESERVED ){
            if( Tokens[i].Lexeme == "true" ){ Tokens[i].Lexeme = "1"; Tokens[i].Type = TOK_INT; }   
            if( Tokens[i].Lexeme == "false" ){ Tokens[i].Lexeme = "0"; Tokens[i].Type = TOK_INT; }
        }
    }
    
    //modify unary operators
    for(int i=0; i<Size; i++){
        if( Tokens[i].Type == TOK_OPERATOR && 
			(!i || 
			  (    Tokens[i-1].Type != TOK_IDENT 
	           &&  Tokens[i].Lexeme[0] != '\'' 
               &&  Tokens[i-1].Type != TOK_INT 
			   &&  Tokens[i-1].Type != TOK_FLOAT 
               &&  Tokens[i-1].Type != TOK_STRING
               &&  Tokens[i-1].Type != TOK_CLOSE_PARENTHESIS 
               &&  Tokens[i-1].Type != TOK_CLOSE_BRACKET
               &&  Tokens[i-1].Type != TOK_CLOSE_BRACE
              ) 
            ) 
        ){
            string Lexeme = Tokens[i].Lexeme;
            if( Lexeme == "&" || Lexeme == "*" || Lexeme == "-" || 
                Lexeme == "+"  || Lexeme == "--" || Lexeme == "++" ||
                Lexeme == "~" || Lexeme == "!" )
            Tokens[i].Lexeme = "'" + Tokens[i].Lexeme;
        }
        //cout << "'" << Tokens[i].Lexeme << "' ";
    }//cout << endl;
    
    PreParseFuncCalls(Tokens);
            
    return;
}
