#include "vm.h"
#include "var.h"
#include "token_types.h"
#include "vs.h"
#include "agency/agency.h"

#include <map>
#include <sstream>
#include <iostream>

using namespace std;

//###############################################################################
//## if the token appears to be a var name, give the node the pointer to that var
//## if its a scalar value, make it a var, and give the node a pointer to that var

bool vm::PreEval(tree& Tree, map<string,var>& Vars, ptr& This, int TypeId, int Depth, int* DeepestClass )
{   
    static int TempVarCount = 0;
    static int DeclType = -1;
	static int Class = -1;
    static bool IsSigned = true;
    static bool IsConst = false;
    static bool IsPtr = false;
    static int Dim = 0;
    if( !Depth ){ IsPtr=false; DeclType=-1; }
    
    string Name = "." + agency::NtoS(TempVarCount);
    
	switch( Tree.Type ){
		case RES_INT  : IsPtr = false; DeclType = T_INT; break;
		case RES_INT64: IsPtr = false; DeclType = T_INT64; break;
		case RES_FLOAT: IsPtr = false; DeclType = T_FLOAT; break;
		case RES_CHAR : IsPtr = false; DeclType = T_CHAR; break;
		case RES_BOOL : IsPtr = false; DeclType = T_BOOL; break;
		case RES_VOID : IsPtr = false; DeclType = T_VOID; break;
		case TOK_USER_TYPE: IsPtr = false; DeclType = T_OBJECT; Class=TypeMap[ Tree.Data ]; break;
		case RES_CONST: IsConst = true; break;
		case RES_UNSIGNED: IsSigned = false; break;
	}

	//delete the reserved word from the tree
	if( DeclType >= 0 && !Depth && Tree.Branch.size() ){
		tree Tree2 = Tree.Branch[0];
		Tree = Tree2;
	}

    //count pointer dim
    while(Tree.Type == OP_DEREFERENCE && DeclType>=0){ 
		IsPtr = true; 
		Dim++; 
		tree Tree2 = Tree.Branch[1];
		Tree = Tree2; 
    }
    
	//pre-evaluate the left and right nodes of operators
	if( Tree.Type == OP_OBJECT_MEMBER || Tree.Type == OP_POINTER_MEMBER ){
		int LhsClass = -1;
		PreEval( Tree.Branch[0], Vars, This, -1, Depth+1, &LhsClass );
		//cout << "LHS Class: " << LhsClass << endl;
		PreEval( Tree.Branch[1], Vars, This, LhsClass , Depth+1 );     
	}else for(int i=0; i<Tree.Branch.size(); ++i){
        PreEval( Tree.Branch[i], Vars, This, -1, Depth+1, DeepestClass );     
    }    
    
    if( Tree.Type == TOK_IDENT ){

		//it's a member variable
		if( TypeId >= 0 ){
			if( Types[ TypeId ].Vars.find(Tree.Data) != Types[ TypeId ].Vars.end() ){
				cout << "known member variable " << Tree.Data << " of class " << ClassIdToName( TypeId ) << endl;
				Vars[ Name ] = Types[ TypeId ].Vars[ Tree.Data ]; 
			    Tree.Address = &Vars[ Name ];
				++TempVarCount;
			}else{
				cout << "unknown member variable " << Tree.Data << " of class " << ClassIdToName( TypeId ) << endl;
			}
			cout << "type:" << Vars[ Name ].Type << "  dimension:" 
				 << Vars[ Name ].Dim << "  pointer type:" << Vars[ Name ].PtrType  
				 << " class:" << Vars[ Name ].Class << endl;
		}

		//it's a regular variable 
        else if( Vars.find(Tree.Data) != Vars.end() ){
			cout << "known variable: " << Tree.Data << "  type:" << Vars[Tree.Data].Type << "  dimension:" 
				 << Vars[Tree.Data].Dim << "  pointer type:" << Vars[Tree.Data].PtrType  
				 << " class:" << Vars[Tree.Data].Class << endl;
			Tree.Address = &Vars[Tree.Data];
			Tree.Class = Vars[Tree.Data].Class;
        }
		
		//it's a function identifier
		else if( Funcs.find(Tree.Data) != Funcs.end() ){
			cout << "known function: " << Tree.Data << endl;
            
			Vars[Name].Address = ( This += sizeof(ptr) );
			*(ptr*)(Vars[Name].Address + vs::StackBegin) = (ptr)(&Funcs[Tree.Data]);
			
			Vars[Name].Type = T_PTR;
            Vars[Name].PtrType = T_FUNC;
            Vars[Name].Dim = 1;

			Tree.Address = &Vars[Name];

            ++TempVarCount;
        }
		
		//it's an unknown variable
		else{
            if( DeclType == -1 ) Error("Unknown type for variable: " + Tree.Data); 
            
            if( IsPtr){
				cout << "new pointer " << Tree.Data << " of dimension " << Dim << " and type " << DeclType 
					 << " of class " << Class << endl;
                Vars[Tree.Data].Type = T_PTR;
                Vars[Tree.Data].PtrType = DeclType;
                Vars[Tree.Data].Dim = Dim;
				Vars[Tree.Data].Class = Class;
				Vars[Tree.Data].Address = 0x0;
				Vars[Tree.Data].Size = sizeof(ptr);
                Dim = 0;
            }else{  
				cout << "new var " << Tree.Data << " of type " << DeclType << " of class " << Class << endl;
                Vars[Tree.Data].Type = DeclType;
				Vars[Tree.Data].Class = Class;
				Vars[Tree.Data].PtrType = -1;
				if( Class>=0 ) Vars[Tree.Data].Size = Types[ Class ].Size;
				else Vars[Tree.Data].Size = SizeOf(DeclType);
            }
            
            Vars[Tree.Data].Address = This;
			This += Vars[Tree.Data].Size;

            Vars[Tree.Data].IsConst = IsConst;
            Vars[Tree.Data].IsSigned = IsSigned;
            Tree.Address = &Vars[Tree.Data];

            IsPtr = false;
        }
    }
	
	//it's a literal value (i.e. 1 1.1 "text")
	else{
		string Name = "." + agency::NtoS(TempVarCount) + ":" + Tree.Data;
        Vars[Name] = var( TokToType(Tree.Type), Tree.Data, &This );
        //cout << "LITERAL: " << Tree.Data << " : " << Vars[Name].Address << " : " << Vars[Name].Type << endl;
        Tree.Address = &Vars[Name];
		Tree.Class = -1;
        ++TempVarCount;
    }


	if( DeepestClass && Tree.Class >= 0 ){ *DeepestClass = Tree.Class; }
	vs::StackPointer = This + vs::StackBegin;
	return true;
}
