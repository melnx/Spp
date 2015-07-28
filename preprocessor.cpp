#include "lex.h"
#include "tree.h"
#include "operators.h"
#include "token_types.h"
#include "preprocessor.h"
#include "agency/agency.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stack>

using namespace std;
using namespace agency;

//##############################################################################
//## preprocess takes a file path and returns a processed string

string preprocessor::PreProcessFile(string Path){
    int LineNum=0;
    
    ifstream In(Path.c_str());
    if( !In ) Error("cannot read file: " + Path);
    
    string Line;
    string Script;
    string IncludePath = "whatever";
    bool InSingleQuote=false;
    bool InDoubleQuote=false;
    bool InComment=false;
    
    //set the file macro (tells the current file)
    Macros["__FILE__"].Data = Lex("\"\\\"" + Path + "\\\"\"");
    
    while(getline(In,Line)){
        ++LineNum; //the Line number
        
        Line=Replace(Line,'\t',' ');
        Line=Trim(Line,' ');
        string Finline; //temp storage for the curent Line
        
        //illiminate the comments from the Line
        for( int i=0; i<Line.size(); i++ ){
            if( Line[i] == '"' && Line[i-1] != '\\' ){ //handle a double quote
                if( !InSingleQuote ){
                    if( !InDoubleQuote ){ InDoubleQuote=true; } else{ InDoubleQuote=false; }    
                }
            }
            else if( Line[i] == '\'' && Line[i-1] != '\\' ){ //handle a single quote
                if( !InDoubleQuote ){    
                    if( !InSingleQuote ){ InSingleQuote=true; } else{ InSingleQuote=false; }
                }
            }  
            else if( !InDoubleQuote && !InSingleQuote && Line[i] == '/' ){ //handle comments
                if( !InComment && Line[i+1] == '*' ){
                    InComment=true; continue;    
                }
                else if( !InComment && Line[i+1] == '/' || Line[0]=='#' ){
                    break;     
                }     
                else if( InComment && Line[i-1] == '*' && Line[i+1]!='/' ){
                    InComment=false; continue;    
                }
            }
            if( !InComment || Line[0]=='#' ){ //add the char to the Line
                Finline += Line[i];
            }
        }
        
        Line = (Finline += "\n");
        
        //handle the preprocessor functions
        if( Line.at(0) == '#' ){
            Finline[0] = ' '; // trim the '#'
            vector<token> Tokens = Lex(Finline);
            
            if( Tokens[0].Type == TOK_PPRES ){
                Tokens[0].Type = GetKeyword(Tokens[0].Lexeme).Type;    
            }        
            
            switch( Tokens.at(0).Type ){
                
                case( PRE_RES_INCLUDE ):{
                    string FilePath;
                    if( Tokens.at(1).Lexeme == "<" ){ 
                        FilePath = IncludePath + Tokens.at(2).Lexeme; 
                    }else{
                        FilePath = Tokens.at(1).Lexeme;
                    }
                    Script += PreProcessFile(FilePath);
                    break;
                }
                
                case( PRE_RES_DEFINE ):{
                    int i=2;
                    
                    //add parameters to a macro function  
                    if( Tokens.at(1).Type == TOK_OPEN_PARENTHESIS ){
                        for(i=3; i<Tokens.size(); i++){
                            if( Tokens[i].Type == TOK_COMMA ) continue;
                            if( Tokens[i].Type == TOK_CLOSE_PARENTHESIS ) break;
                            Macros[Tokens.at(1).Lexeme].Args.push_back(Tokens.at(i).Lexeme);
                        }    
                    }
                                        
                    //define the macro body  
                    Macros[Tokens.at(1).Lexeme].Data = 
                    vector<token>(Tokens.begin()+i,Tokens.end());
                    
                    break;
                }
                
                case( PRE_RES_UNDEF ):{
                    Macros.erase(Tokens[1].Lexeme);      
                    break;
                }
                
                case( PRE_RES_IFDEF ):{
                    if( Macros.find(Tokens[1].Lexeme) == Macros.end() ){
                        InComment = true;    
                    }
                    break;
                }
                
                case( PRE_RES_IFNDEF ):{
                    if( Macros.find(Tokens[1].Lexeme) != Macros.end() )
                        InComment = true;      
                    break;
                }
                
                case( PRE_RES_ENDIF ):{
                    InComment = false;
                    break;
                }
                
            }
        
            Line="";
        }
        
        //set the line macro (tells the current line)
        Macros["__LINE__"].Data = Lex(NtoS(LineNum));
        
        //replace tokens with their contents
        vector<token> Tokens = Lex(Line);
        
        for(int i=0; i<Tokens.size(); i++){
            if( Macros.find(Tokens[i].Lexeme) != Macros.end() ){        
                //add tokens before macro
                vector<token> Before(Tokens.begin(),Tokens.begin()+i);
                //add macro
                for(int j=0; j<Macros[Tokens[i].Lexeme].Data.size(); j++)
                    Before.push_back( Macros[Tokens[i].Lexeme].Data[j] );
                //add tokens after macro
                for(int j=i+1; j<Tokens.size(); j++){    
                    Before.push_back( Tokens[j] );
                }
                //se the original to the new value
                Tokens = Before;
            }
        }
        
        Line = "";
        for(int i=0; i<Tokens.size(); i++) Line += (Tokens[i].Lexeme + " ");
        
        //add the Line to the script
        if( Line.size() )
            Script += Line + "\n";
    }
    
    //cout << "loaded: " << path << endl; 
    //if( InComment ) cout << "WARNING: unterminated comment";    
    //if( InDoubleQuote || InSingleQuote ) cout << "WARNING: unterminated quotation";  
    
    return Script;
}
