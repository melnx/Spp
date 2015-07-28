#include <cstdlib>
#include <iostream>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <time.h>

#include "parser.h"
#include "lex.h"
#include "vm.h"
#include "var.h"
#include "vs.h"

#define SPP_STACK_SIZE 100000000

using namespace std;

int main(int argc, char** argv)
{       
    cout << "ÄãºÃ£¬ÕâÊÇS++" << endl; 

    vs::StackBegin = (ptr)( malloc(SPP_STACK_SIZE) );
    vs::StackPointer = vs::StackBegin;
	ptr This = 0x0;
    
    vm VM;
    
    VM.Vars["_sp"] = var( (ptr)(&vs::StackPointer), T_PTR, sizeof(ptr), T_INT, 1 );
    VM.Vars["_ds"] = var( (ptr)(&vs::StackBegin), T_PTR, sizeof(ptr), T_INT, 1);
    VM.Vars["argc"] = var( (ptr)(&argc), T_INT, sizeof(int) );
    VM.Vars["argv"] = var( (ptr)(&argv), T_PTR, sizeof(char), T_CHAR, 2);
	VM.Vars["cout"] = var( (ptr)(&cout), T_OBJECT, sizeof(cout) );
	VM.Vars["cin"] = var( (ptr)(&cin), T_OBJECT, sizeof(cin), 0, 0, 0 );

    while(1){
		
        cout << "\n@" << vs::StackPointer-vs::StackBegin << "> "; 
		string Line; 
		getline(cin,Line);        

        try{
			vector<token> Tokens = Lex(Line);
			if( !Tokens.size() ) continue;
			tree Tree;

            VM.PreParse( Tokens );
            VM.LoadTree( Tokens, Tree );
			VM.PreEval( Tree, VM.Vars, This );
            Tree.Show();
            
            int Start=clock();
			var* Temp = VM.Eval( Tree, VM.Vars, vs::StackBegin );
			This = vs::StackPointer - vs::StackBegin;

            cout << "T: " << clock() - Start << endl;
			cout << "(" << Temp->Type << " @ " << Temp->Address << ") >> " << *Temp << endl;
        }
        catch(string){}
    }
    
    return EXIT_SUCCESS;
}

void Error( string X )
{
	cout << "ERROR: " << X << endl;
	cin.get();
	throw string("");
}
