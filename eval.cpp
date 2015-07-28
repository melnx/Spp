#include "vm.h"
#include "var.h"
#include "token_types.h"
#include "vs.h"

#include <map>
#include <iostream>
using namespace std;

var* vm::Eval(tree& Tree, map<string,var>& Var, ptr This)
{
    register var* L; //pointer to lhs  DO NOT MAKE
    register var* R; //pointer to rhs  THESE STATIC
    register var* Tmp = Tree.Address; //points to the node's var
    static int LoopInterrupt = 0;
    
    if( Tree.Type >= OP_NONE ){//if true its a regular operator
        if( Tree.Lhs ){
            L = Eval( Tree.Branch[0], Var, This );
            //if( L->IsRef && L->Val.Int ) L = (var*)L->Val.Int; //autodereference
        }
        if( Tree.Rhs ){
            R = Eval( Tree.Branch[1], Var, This );
            //if( R->IsRef && R->Val.Int ) R = (var*)R->Val.Int; //autodereference
        }
    }
    
    //handle the operators
    switch(Tree.Type){
        
        case OP_PRE_INCREMENT: // ++x
            return &(++(*R));
        
        case OP_POST_INCREMENT: // x++
            return &((*L)++);
        
        case OP_PRE_DECREMENT: // --x
            return &(--(*R));
        
        case OP_POST_DECREMENT: // x--
            return &((*L)--);
        
        case OP_ADD: // +
            return &(*Tmp = *L + *R);
        
        case OP_SUB: // -
            return &(*Tmp = *L - *R);
            
        case OP_MUL: // *
            return &(*Tmp = *L * *R);
            
        case OP_DIV: // /
            return &(*Tmp = *L / *R);
            
        case OP_MOD: // %
            return &(*Tmp = *L % *R);
        
        case OP_EXPONENT: // x#y
            return &(*Tmp = L->exponent(*R));
        
        case OP_POW_MOV: // x#=y
            return L->copy(&L->exponent(*R));
            
        case OP_ADD_MOV: // +=
            return &(*L += *R);
            
        case OP_SUB_MOV: // -=
            return &(*L -= *R);
            
        case OP_MUL_MOV: // *=
            return &(*L *= *R);
        
        case OP_DIV_MOV: // /=
            return &(*L /= *R);
        
        case OP_MOD_MOV: // %=
            return &(*L %= *R);
        
        case OP_MOV: // =   
			L->cast(R,L->Type);
            return L;
        
        case OP_UNARY_MINUS: // -x
            return &(-(*R));
            
        case OP_UNARY_PLUS: // +x
            return &(+(*R));
                    
        case OP_LOGIC_AND: // &&
            return &(*L && *R);
            
        case OP_LOGIC_IOR: // ||
            return &(*L || *R);
            
        case OP_LOGIC_NOT: // !
            return &(!*R);
            
        case OP_BITWISE_IOR: // |
            return &(*L | *R);
          
        case OP_BITWISE_AND: // &
            return &(*L & *R);  
          
        case OP_BITWISE_XOR: // ^
            return &(*L ^ *R);
            
        case OP_BITWISE_NOT: // ~
            return &(~*R);
        
        case OP_SEQUENCE: // x,y,z
            return R;
            
        case OP_CMP_B: // <
            return &(*L < *R);
            
        case OP_CMP_A: // >
            return &(*L > *R);
            
        case OP_CMP_BE: // <=
            return &(*L <= *R);
            
        case OP_CMP_AE: // >=
            return &(*L >= *R);
            
        case OP_RSH: // >>
            return &(*L >> *R);
            
        case OP_LSH: // <<
            return &(*L << *R);
        
        case OP_RSH_MOV: // >>=
            return &(*L >>= *R);
        
        case OP_LSH_MOV: // <<=
            return &(*L <<= *R);
            
        case OP_CMP_E: // ==
            return &(*L == *R);
        
        case OP_CMP_NE: // !=
            return &(*L !=*R);
            
        case( OP_TERNARY_THEN ):{ // ?
            bool Res = *(int*)(Eval(Tree.Branch[0], Var, This)->Address);
            if( Res )
                return Eval(Tree.Branch[1].Branch[0], Var, This);
            else
                return Eval(Tree.Branch[1].Branch[1], Var, This);
        }
            
        case OP_TERNARY_ELSE : // :
            return Tmp;
        
        case OP_OBJECT_MEMBER : // a.x() a.x
            Tmp->Address = L->Address + (R->Address - This);
			Tmp->Size = R->Size;
			Tmp->Type = R->Type;
			Tmp->Dim = R->Dim;
			Tmp->PtrType = R->PtrType;
			Tmp->Class = R->Class;
			//cout << "type:" << Tmp->Type << " ptrtype:" << Tmp->PtrType << " dim:" << Tmp->Dim << endl;
			//cout << "L->Address:" << L->Address << " R->Address:" << R->Address-This << " Tmp->Address:" << Tmp->Address << endl;
            return Tmp;

		case OP_POINTER_MEMBER :
			Tmp->Address = *(ptr*)(L->Address) + ( R->Address - This );
			//cout << "L->Address:" << *(ptr*)(L->Address) << " R->Address:" << R->Address-This << " Tmp->Address:" << Tmp->Address << endl;
			Tmp->Size = R->Size;
			Tmp->Type = R->Type;
			Tmp->Dim = R->Dim;
			Tmp->PtrType = R->PtrType;
			Tmp->Class = R->Class;
			return Tmp;
        
        case RES_FOR :{ //for(a;b;c){} 
            int Start = vs::StackPointer;
            for( Eval(Tree.Branch[0],Var, This); *(bool*)(Eval(Tree.Branch[1], Var, This)->Address); Eval(Tree.Branch[2],Var, This) ){ 
                CONTINUE_FOR:
                for(vector<tree>::iterator i = Tree.Branch.begin()+3; i!=Tree.Branch.end(); ++i ){
                    Eval(*i,Var,This);
                    if( LoopInterrupt ){
                        LoopInterrupt = 0;
                        if( LoopInterrupt == RES_CONTINUE ) goto CONTINUE_FOR;    
                        else goto BREAK_FOR;
                    }
                }
                vs::StackPointer = Start;
            }
            BREAK_FOR:
            vs::StackPointer = Start;
            return Tmp;
        }
        
        case RES_WHILE:{ //while(a){b}
            int Start = vs::StackPointer;
            while( *(bool*)(Eval(Tree.Branch[0], Var, This)->Address) ){ 
                CONTINUE_WHILE:
                for(vector<tree>::iterator i = Tree.Branch.begin()+1; i!=Tree.Branch.end(); ++i ){
                    Eval(*i, Var, This);
                    if( LoopInterrupt ){
                        LoopInterrupt = 0;
                        if( LoopInterrupt == RES_CONTINUE ) goto CONTINUE_WHILE;    
                        else goto BREAK_WHILE;
                    }
                }
                vs::StackPointer = Start;
            }
            BREAK_WHILE:
            vs::StackPointer = Start;
            return Tmp;
        }
        
        case RES_IF :{ //if(a){b}
            int Start = vs::StackPointer;
            if( *(bool*)(Eval(Tree.Branch[0], Var, This)->Address) ){ 
                for(vector<tree>::iterator i = Tree.Branch.begin()+1; i!=Tree.Branch.end(); ++i ){
                    Eval(*i, Var, This);
                }
            }
            vs::StackPointer = Start;
            return Tmp;
        }
        
        case RES_ELSE : // else{} 
            
        
        case RES_BREAK: //  break;
            LoopInterrupt = RES_BREAK;
            return Tmp;
            
        case RES_CONTINUE: // continue;
            LoopInterrupt = RES_CONTINUE;
            return Tmp;
        
        case OP_ADDRESS: // &x
            *(ptr*)Tmp->Address = (ptr)(R->Address);
            Tmp->Type = T_PTR;
            Tmp->Dim = R->Dim+1;
			Tmp->Class = R->Class;
            if( R->Type != T_PTR) Tmp->PtrType = R->Type;
            else Tmp->PtrType = R->PtrType;
            return Tmp; 
        
        case OP_DEREFERENCE: // *x
			R = Eval( Tree.Branch[1], Var, This);
            if( R->PtrType != T_FUNC ){
                Tmp->Address = *(ptr*)R->Address;
                if( R->Dim == 1 ){ Tmp->Type = R->PtrType; }
                else{ Tmp->Type = R->Type; }
                Tmp->PtrType = R->PtrType;
                Tmp->Dim = R->Dim-1;
				Tmp->Class = R->Class;
            }else{
                for(int i=0; i<Tree.Branch.size(); i++)
                    Tree.Branch[i].Address = Eval(Tree.Branch[i], Var, This); 
                
                Tmp->Address = ((func*)(Tree.Address))->Call(Tree);
                Tmp->Type = ((func*)(Tree.Address))->Return.Type;
                Tmp->PtrType = ((func*)(Tree.Address))->Return.PtrType;
                Tmp->Dim = ((func*)(Tree.Address))->Return.Dim;
            }
            return Tmp;
        
        case RES_SIZEOF :{
            L = Eval(Tree.Branch[0], Var, This);
            int Size;
                
            switch(Tree.Branch[0].Type){    
                case RES_INT: Size = SizeOf(T_INT); break;
                case RES_INT64: Size = SizeOf(T_INT64); break;
                case RES_CHAR: Size = SizeOf(T_CHAR); break;
                case RES_BOOL: Size = SizeOf(T_BOOL); break;
                case RES_FLOAT: Size = SizeOf(T_FLOAT); break;
                case TOK_USER_TYPE: Types[ TypeMap[Tree.Branch[0].Data] ].Size; break;
                default: Size = L->Size;     
            }
            *(int*)(Tmp->Address) = Size;
            Tmp->Type = T_INT;
            return Tmp;
        }
        
        case RES_TYPEID:{ //typeid
            L = Eval(Tree.Branch[0], Var, This);

            string Id = TypeId(*L);
            
            char* TypeName = new char[Id.size()];
            strcpy( TypeName, Id.c_str() );
            
            *(ptr*)(Tmp->Address) = (ptr)(TypeName);
            Tmp->Type = T_PTR;
            Tmp->PtrType = T_CHAR;
			Tmp->Dim = 1;
            Tmp->Size = Id.size();
            
            return Tmp; 
        }
        
        case RES_NEW:{ //new a[10]
            int Size = *(int*)( Eval(Tree.Branch[0].Branch[1], Var, This)->Address );
			cout << TOK_INT << ":" << Tree.Branch[0].Branch[0].Type << endl;
            Tmp->Address = (ptr)( malloc(Size) );
            return Tmp;
        }
        
        case RES_DELETE: //delete a
			R = Eval(Tree.Branch[0], Var, This);
            free( (void*)R->Address );
            return Tmp;
        
        case OP_ARRAY_ACCESS:{ // []
            int Size;
            if( L->Dim == 1 ) Size = SizeOf(L->PtrType);
            else Size = SizeOf(L->Type);
                
            Tmp->Address = *(ptr*)L->Address + ( (*(int*)(R->Address)) *  Size );
            //cout << "ADDR: " <<<< Tmp->Address << endl;
            cout << "SIZE: " << Size << endl;
            
            if( L->Dim == 1 ){ Tmp->Type = L->PtrType; }
            else{ Tmp->Type = L->Type; }
            Tmp->PtrType = L->PtrType;
            Tmp->Dim = L->Dim-1;
            
            //Tmp->Type = L->PtrType;
            Tmp->Size = SizeOf(L->PtrType);
            return Tmp;
        }
        
		case TOK_ARRAY_SCOPE:{ //{x,y,z}
           
            *(ptr*)Tmp->Address = (Tree.Branch[0].Address->Address);
                 
            Tmp->Type = T_PTR;
            Tmp->Dim = Tree.Branch[0].Address->Dim+1;
            
            if( Tmp->Dim == 1 )
                Tmp->PtrType = Tree.Branch[0].Address->Type;
            else
                Tmp->PtrType = Tree.Branch[0].Address->PtrType;
            
            Tmp->Size = Tree.Branch[0].Address->Size * Tree.Branch.size();
            return Tmp;
		}
            
        case RES_RETURN: //return x; return;
            cout << "return" << endl;
            return Tmp;
        
        case TOK_FUNC_CALL:{

			//evaluate arguments
            for(int i=0; i<Tree.Branch[1].Branch.size(); i++){
                Tree.Branch[1].Branch[i].Address = Eval( Tree.Branch[1].Branch[i], Var, This);
            } 

			//get the function location
			ptr FunctionAddress = *(ptr*)( Eval( Tree.Branch[0], Var, This )->Address );
			
			ptr ReturnAddress = ((func*)(FunctionAddress))->Call(Tree.Branch[1]); 
			
			//set the return pointer if it's not a void-return function
			if( ((func*)(FunctionAddress))->Return.Type != T_VOID ) Tmp->Address = ReturnAddress;
            
			Tmp->Type = ((func*)(FunctionAddress))->Return.Type;
            Tmp->PtrType = ((func*)(FunctionAddress))->Return.PtrType;
            Tmp->Dim = ((func*)(FunctionAddress))->Return.Dim;
            return Tmp;
        }
        
        case OP_NONE: return L;
        
        break;
    } 
    
    //it it wasn't an operator it must be a leaf
	if( Tree.Address->Address < 500000 ) Tree.Address->Address += This;
	return Tree.Address;
}
