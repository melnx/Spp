#include "lex.h"
#include "parser.h"
#include "token_types.h"
#include <vector>

using namespace std;
//##############################################################################
//## gets one scope from the point you told it to

std::vector<token> parser::
GetScope(vector<token> Tokens, int Start, int &End, int Open, int Close){
    
    int Depth=0;
    vector<token> ScopeTokens;
    
    if( Start+2 >= Tokens.size() || Tokens[Start+1].Type == TOK_CLOSE_BRACE ) return ScopeTokens;
    
    for(int i=Start; i<Tokens.size(); ++i){
        if( Tokens[i].Type == Open  ){
            //if( Tokens[0].Type == TOK_FUNCTION_CALL && !Depth ){ ++i; }
            if( Depth ) ScopeTokens.push_back(Tokens[i]);
            Depth++;
        }
        else if( Tokens[i].Type == Close ){
            Depth--;
            if( Depth ){ ScopeTokens.push_back(Tokens[i]); }
            else{
                End = i;
                return ScopeTokens;    
            }     
        }
        else if( Depth ){
            ScopeTokens.push_back(Tokens[i]);     
        }
    }
    End=Tokens.size()-1;
    return ScopeTokens;                                  
}

//##############################################################################
//## separates tokens into statements

vector< vector<token> > parser::
GetStatements( vector<token> Tokens, int Div, bool AllowBraceTerm, bool AddTail ){
    vector< vector<token> > Statements;
    int Begin = 0;
    int Depth = 0;
    
    for( int i=0; i<Tokens.size(); ++i ){
         
        if( Tokens[i].Type == TOK_OPEN_BRACE || Tokens[i].Type == TOK_OPEN_PARENTHESIS 
            || Tokens[i].Type == TOK_OPEN_BRACKET )
        {
            ++Depth;    
        }
        else if( Tokens[i].Type == TOK_CLOSE_BRACE || Tokens[i].Type == TOK_CLOSE_PARENTHESIS 
            || Tokens[i].Type == TOK_CLOSE_BRACKET )
        {
            --Depth;
            if( AllowBraceTerm && !Depth && Tokens[i].Type == TOK_CLOSE_BRACE ){
                Statements.push_back( vector<token>(Tokens.begin()+Begin,Tokens.begin()+i+1) );
                Begin = i+1;
            }
        }
        else if( Tokens[i].Type == Div ){
            if( !Depth ){
                Statements.push_back( vector<token>(Tokens.begin()+Begin,Tokens.begin()+i) );
                Begin = i+1;
            }     
        }    
    }
    if( AddTail && Begin <= Tokens.size() ){
        Statements.push_back( vector<token>(Tokens.begin()+Begin,Tokens.end()) );
    }
               
    return Statements;           
}
