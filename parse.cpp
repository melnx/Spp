#include "lex.h"
#include "parser.h"
#include "token_types.h"
#include "operators.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

//##############################################################################
//## parses tokens into a tree

void parser::LoadTree( vector<token> Tokens, tree& Tree )
{     
    //operator info
    op BestOp = {0, 0, "", 0,0, 0}; //lowest operator ever
    int BestOpIndex = -1;
    int Size = Tokens.size();
    int Depth = 0;
    
    //handle reserved words
    if( Size && Tokens[0].Type == TOK_RESERVED ){
        Tree.Category = TOK_RESERVED;
        ParseResWord( Tokens, Tree );   
        return;
	}else if( Size && Tokens[0].Type == TOK_USER_TYPE ){
	    ParseResWord( Tokens, Tree );
		return;
	}
    
    //handle leafs
    if( Tokens.size() == 1 ){
        Tree.Data = Tokens[0].Lexeme;
        Tree.Type = Tokens[0].Type;
        return;
    }
    
    //process LTR ops
    for(int i=Tokens.size()-1; i>=0; i--){
        if( Tokens[i].Type == TOK_OPEN_PARENTHESIS || Tokens[i].Type == TOK_OPEN_BRACE ) ++Depth;                   
        else if( Tokens[i].Type == TOK_CLOSE_PARENTHESIS || Tokens[i].Type == TOK_CLOSE_BRACE ) --Depth;
        else if( (Tokens[i].Type == TOK_COMMA || Tokens[i].Type == TOK_OPERATOR) && !Depth ) {
            op Curr = GetOp(Tokens[i].Lexeme); 
            if( Curr.Precedence > BestOp.Precedence && Curr.Assoc == LEFT_TO_RIGHT ){
                BestOp = Curr;
                BestOpIndex = i;    
            }
        }
    }
    
    //process RTL ops
    for(int i=0; i<Tokens.size(); i++){         
        if( Tokens[i].Type == TOK_OPEN_PARENTHESIS || Tokens[i].Type == TOK_OPEN_BRACE ) ++Depth;                   
        else if( Tokens[i].Type == TOK_CLOSE_PARENTHESIS || Tokens[i].Type == TOK_CLOSE_BRACE ) --Depth;
        else if( (Tokens[i].Type == TOK_COMMA || Tokens[i].Type == TOK_OPERATOR) && !Depth ) {
            op Curr = GetOp(Tokens[i].Lexeme);
            if( Curr.Precedence > BestOp.Precedence && Curr.Assoc == RIGHT_TO_LEFT ){
                BestOp = Curr;
                BestOpIndex = i;    
            }
        }
    }    
    
    //process the side(s) of the best operator
    if( BestOp.Precedence && BestOpIndex >= 0 ){
        //add data to this node
        Tree.Data = BestOp.Lexeme; 
        Tree.Category = TOK_OPERATOR;
        Tree.Type = BestOp.Type;
        Tree.Branch = vector<tree>(2);
        
        //recurse operands on LHS
        if( BestOpIndex && BestOp.Lhs ){
            Tree.Lhs = true;
            LoadTree( vector<token>(Tokens.begin(),Tokens.begin()+BestOpIndex), Tree.Branch[0] );
            
        }
         
        //recurse operands on RHS
        if( BestOpIndex < Size-1 && BestOp.Rhs ){
            Tree.Rhs = true;
            LoadTree( vector<token>(Tokens.begin()+BestOpIndex+1,Tokens.end()), Tree.Branch[1] );    
        }
        
    }
    
    //no operators found
    else{

		//parse parenthesis
        if( Tokens[0].Type == TOK_OPEN_PARENTHESIS ){ 
            
            int End = 0;                  
            vector<token> Paran = GetScope( Tokens, End, End, TOK_OPEN_PARENTHESIS, TOK_CLOSE_PARENTHESIS );
            
			//function call or cast parenthesis syntax: (add)(3,4) or (int)(4.5)
            if( End < Tokens.size()-1 ){

				vector<token> SecondParan( Tokens.begin()+End+2, Tokens.end()-1 ); 
				vector< vector<token> > Args = GetStatements( SecondParan, TOK_COMMA, false, true );
				
				Tree.Type = TOK_FUNC_CALL;

				Tree.Branch = vector<tree>(2);
				LoadTree( Paran, Tree.Branch[0] );

				Tree.Branch[1].Branch = vector<tree>(Args.size());
				for(int i=0; i<Args.size() && SecondParan.size(); ++i){
					LoadTree( Args[i], Tree.Branch[1].Branch[i] );        
				}

				//cout << "sorry, unsupported function call or casts" << endl;
				//exit(0);
            }
			
			//regular parenthesis syntax: (4+5)
			else{
				vector<token> Toks = vector<token>(Tokens.begin()+1, Tokens.begin()+End);
                LoadTree( Toks, Tree );
            }
        }

		//parse a list: {1,3,4,5,6};
        if( Tokens[0].Type == TOK_OPEN_BRACE ){
            int End = 0;         
            vector<token> Paran = GetScope( Tokens, 0, End, TOK_OPEN_BRACE, TOK_CLOSE_BRACE );
            vector<token> Stat(Tokens.begin()+1, Tokens.begin()+End);
            vector< vector<token> > Stats = GetStatements( Stat, TOK_COMMA, false, true );
            
            Tree.Type = TOK_ARRAY_SCOPE;
            Tree.Branch = vector<tree>(Stats.size());
            for(int i=0; i<Stats.size(); ++i){
                LoadTree( Stats[i],Tree.Branch[i] );        
            }
        }

    }
    
    return;
}
