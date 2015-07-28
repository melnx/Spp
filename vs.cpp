#include "vs.h"
#include "var.h"
#include <iostream>
using namespace std;

ptr vs::StackBegin;
ptr vs::StackPointer;
std::stack<ptr> vs::Scopes;

ptr vs::push_int(int Int){
    StackPointer += sizeof(int); 
    *(int*)vs::StackPointer = Int;
    return StackPointer;
}  

ptr vs::push_int64(__int64 Int){
    StackPointer += sizeof(__int64); 
    *(__int64*)vs::StackPointer = Int;
    return StackPointer;
}  

ptr vs::push_char(char Char){
    StackPointer += sizeof(char); 
    *(char*)vs::StackPointer = Char;
    return StackPointer;
}  

ptr vs::push_float(float Float){
    StackPointer += sizeof(float); 
    *(float*)vs::StackPointer = Float;
    return StackPointer;
}

ptr vs::push_mem(ptr Address, int Size){
	StackPointer += Size;
	memcpy( (void*)(StackPointer), (const void*)(Address), Size );
	return StackPointer;
}

ptr vs::push_string(string String){
    ptr Temp = StackPointer; 
    int i;
    for(i=0; i<String.size(); ++i){
        *(char*)(StackPointer) = String[i];
        StackPointer++;
    }
    *(char*)(StackPointer) = char(0);
    //cout << "STRING: " << (char*)Temp << endl;
    return Temp;
}  



ptr vs::pop(int Bytes){
    StackPointer -= Bytes;       
    return StackPointer;
}    

ptr vs::alloc(int Bytes){
    StackPointer += Bytes;       
    return StackPointer;
}    

void vs::enter_scope(){
    Scopes.push( 0 );       
}

void vs::exit_scope(){
    StackPointer -= Scopes.top();
    Scopes.pop();
}

