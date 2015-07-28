#include "var.h"
#include "lex.h"
#include "token_types.h"
#include "vs.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

using namespace std;

var::var()
{
    IsRef = false;
    Type = T_NONE;
    IsPtr = false;
    IsConst = false;                 
    IsSigned = true;
    Dim = 0;
	Class = -1;
}

var::var(int _Type, string Lexeme, ptr* sp)
{   
    //cout << "TYPE: " << _Type << " LEXEME: " << Lexeme;
    //cout << " ADDRESS: " << Address  << endl;
    
    IsRef = false;
    IsPtr = false;
    IsConst = false;                 
    IsSigned = true;
    Dim = 0;
    
    switch( _Type )
    {
        case T_INT : 
            Type = T_INT;
            Size = sizeof(int);
            Address = vs::push_int( atoi(Lexeme.c_str()) );
			if( sp ) (*sp) += Size;
            break;
        
        case T_INT64 : 
            Type = T_INT;
            Size = sizeof(__int64);
            Address = vs::push_int64( (__int64)atoi(Lexeme.c_str()) );
			if( sp ) (*sp) += Size;
            break;
            
        case T_CHAR:
            Type = T_CHAR;
            Size = sizeof(char);
            Address = vs::push_char( Lexeme[0] );
			if( sp ) (*sp) += Size;
            break; 
        
        case T_FLOAT : 
            Type = T_FLOAT;
            Size = sizeof(float);
            Address = vs::push_float( atof(Lexeme.c_str()) ) ;
			if( sp ) (*sp) += Size;
            break;
            
        case T_STRING :
            PtrType = T_CHAR;
            Type = T_PTR;
            Dim = 1;
            Size = sizeof(char) * Lexeme.size()+1;

			char* Ptr = new char[ Size ];
			strcpy( Ptr, Lexeme.c_str() );
			Ptr[ Lexeme.size() ] = 0x0;
            Address = vs::push_int( (int)Ptr ); 
			if( sp ) (*sp) += sizeof(ptr);
            break;
    }

}

var::var(int Int)
{
    *this = var();         
    Address = vs::push_int(Int);
    Type = T_INT;
}

string var::str()
{
    stringstream str;
    switch(Type){                   
        case T_INT:
            str << *((int*)(Address));
            break;
        case T_INT64:
            str << *((__int64*)(Address));
            break;
        case T_BOOL:    
            str <<  *((bool*)(Address)) << endl;
            break;
        case T_CHAR:
            str << *((char*)(Address));
            break;
        case T_FLOAT:
            str << *((float*)(Address));
            break;
        case T_PTR:
            if( PtrType == T_CHAR && Dim == 1 ){
                for(char* i=(char*)(*(int*)Address); *i != char(0); ++i){
                    str << *i;
                }
            }else{
                //str << (int*)(*((ptr*)(Address)));
                str << "0x" << hex << *((ptr*)(Address));
				//str << "@" << *((ptr*)(Address));
            }
            break;
    }
    return str.str();
}

std::ostream& operator<<(std::ostream& stm, var& v)
{
    stm << v.str(); 
    return stm;
}

var* var::cast(var* R, int CastType)
{
    //cout << "L Type: " << CastType << endl;
    switch(CastType){      
        
        case T_PTR:
            switch(R->Type){            
		        case T_PTR: *(ptr*)(Address) = *(ptr*)(R->Address); break;
                case T_INT: *(ptr*)(Address) = *(int*)(R->Address); break;
                case T_INT64: *(ptr*)Address = *(__int64*)R->Address; break;
                case T_FLOAT: *(ptr*)Address = (int)(*(float*)R->Address); break;
                case T_CHAR: *(ptr*)Address = (int)(*(char*)R->Address); break;
				default: Error("cannot convert to pointer");
            }     
            break;
        case T_INT: 
            switch(R->Type){ 
                case T_PTR: 
                case T_INT: *(int*)(Address) = *(int*)(R->Address); break;
                case T_INT64: *(int*)Address = *(__int64*)R->Address; break;
                case T_FLOAT: *(int*)Address = (int)(*(float*)R->Address); break;
                case T_CHAR: *(int*)Address = (int)(*(char*)R->Address); break;
				default: Error("cannot convert to int");
            }
            break;
        case T_INT64: 
            switch(R->Type){ 
                case T_PTR:             
                case T_INT: *(__int64*)Address = *(int*)R->Address; break;
                case T_INT64: *(__int64*)Address = *(__int64*)R->Address; break;
                case T_FLOAT: *(__int64*)Address = (__int64)(*(float*)R->Address); break;
                case T_CHAR: *(__int64*)Address = (__int64)(*(char*)R->Address); break;
				default: Error("cannot convert to int64");
            }
            break;
        case T_FLOAT:
            switch(R->Type){ 
                case T_PTR:             
                case T_INT: *(float*)Address = (float)(*(int*)R->Address); break;
                case T_INT64: *(float*)Address = (float)(*(__int64*)R->Address); break;
                case T_FLOAT: *(float*)Address = *(float*)R->Address; break;
                case T_CHAR: *(float*)Address = (float)(*(char*)R->Address); break;
				default: Error("cannot convert to float");
            }
            break;
        case T_CHAR:
            switch(R->Type){ 
                case T_PTR:             
                case T_INT: *(char*)Address = (char)(*(int*)R->Address); break;
                case T_INT64: *(char*)Address = (char)(*(__int64*)R->Address); break;
                case T_FLOAT: *(char*)Address = (char)(*(float*)R->Address); break;
                case T_CHAR: *(char*)Address = *(char*)R->Address; break;
				default: Error("cannot convert to char");
            }    
            break;
    }
    
    //Dim = R->Dim;
    Type = CastType;
    Size = R->Size;
    IsRef = R->IsRef;
    IsPtr = R->IsPtr;
    IsConst = R->IsConst;
    PtrType = R->PtrType;
	Class = R->Class;
    return this;          
}

var* var::copy(var* R)
{
    
    switch(R->Type){
        case T_PTR:                         
        case T_INT:
            *(int*)Address = *(int*)R->Address;
            break;
        case T_INT64:
            *(__int64*)Address = *(__int64*)R->Address;
            break;
        case T_FLOAT:
            *(float*)Address = *(float*)R->Address;
            break; 
        case T_CHAR:
            *(char*)Address = *(char*)R->Address; 
            break;
        default:
            *(int*)Address = *(int*)R->Address;
    }
    
    Dim = R->Dim;
    Type = R->Type;
    Size = R->Size;
    IsRef = R->IsRef;
    IsPtr = R->IsPtr;
    IsConst = R->IsConst;
    PtrType = R->PtrType;
    Class = R->Class;
    return this;     
}

var::var(ptr Pointer, int _Type, int _Size, int _PtrType, int _Dim, int _Class, bool Const)
{
    *this = var();         
    Address = Pointer;
    Type = _Type;
    Dim = _Dim;
	Class = _Class;
    Size = _Size;
    IsRef = 0;
    IsPtr = 0;
    IsConst = Const;
    PtrType = _PtrType;    
}

//##############################################################################

var& var::operator == (var& Rhs){ 
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) == *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator !=(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) != *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator <(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) < *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;     
}

var& var::operator >(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) > *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;     
}

var& var::operator <=(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) <= *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator >=(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) >= *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;     
}

var& var::operator + (var& Rhs){ 
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) + *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator - (var& Rhs){ 
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) - *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator / (var& Rhs){ 
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) / *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator * (var& Rhs){ 
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) * *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator % (var& Rhs){ 
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) % *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::exponent(var Rhs){
    static var Ret;     
    //Ret.Address = vs::push( (int)pow(*(int*)(Address), (*(int*)(Rhs.Address))) );
	Ret.Address = vs::push_int( *(int*)(Address) * *(int*)(Rhs.Address) );
    Ret.Type = T_INT; Ret.Size = sizeof(int);
    return Ret;
}

var& var::operator +=(var& Rhs){
    return *this = *this + Rhs;
}

var& var::operator -=(var& Rhs){
    return *this = *this - Rhs;
}

var& var::operator *=(var& Rhs){
    return *this = *this * Rhs;
}

var& var::operator /=(var& Rhs){
    return *this = *this / Rhs;
}

var& var::operator %=(var& Rhs){
    return *this = *this % Rhs;
}
var& var::operator ++(int){
    static var Temp;
    Temp = var(*(int*)Address);
    ++(*(int*)Address); 
    return Temp;
}

var& var::operator ++(){
    ++(*(int*)Address); 
    return *this;     
}

var& var::operator --(int){
    static var Temp;
    Temp = var(*(int*)Address);
    --(*(int*)Address); 
    return Temp;
}

var& var::operator --(){
    --(*(int*)Address); 
    return *this;     
}

var& var::operator -(){
    static var Temp;
    Temp.Address = vs::push_int( 1 );
    Temp.copy(this);   
    switch(Type){
        case T_INT: (*(int*)Temp.Address) = -(*(int*)Temp.Address); break;
        case T_FLOAT: (*(float*)Temp.Address) = -(*(float*)Temp.Address); break;
    }
    return Temp;
}
var& var::operator +(){
    static var Temp;
    Temp.Address = vs::push_int( 1 );
    Temp.copy(this); 
    return Temp;
}

var& var::operator &&(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) && *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;     
}

var& var::operator ||(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) || *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;
}

var& var::operator !(){
    static var Ret; 
    Ret.Address = vs::push_int( ! *(int*)(Address) );
    Ret.Type = T_INT;
    return Ret;
}

var& var::operator &(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) & *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;     
}

var& var::operator |(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) | *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;
}

var& var::operator ^(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) ^ *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;
}

var& var::operator ~(){
    static var Ret; 
    Ret.Address = vs::push_int( ~ *(int*)(Address) );
    Ret.Type = T_INT;
    return Ret;
}

var& var::operator <<(var& Rhs){
    static var Ret; 
	if( Type == T_INT ){
        Ret.Address = vs::push_int( *(int*)(Address) << *(int*)(Rhs.Address) );
		Ret.Type = T_INT;
	}else if( Type == T_OBJECT ){
	    cout << Rhs.str();
		Ret.Type = T_OBJECT;
		Ret.Address = Address;
	}
    return Ret;
}

var& var::operator >>(var& Rhs){
    static var Ret; 
	if( Type == T_INT ){
        Ret.Address = vs::push_int( *(int*)(Address) >> *(int*)(Rhs.Address) );
	    Ret.Type = T_INT;
	}else if( Type == T_OBJECT ){
		switch( Rhs.Type ){
			case T_INT: cin >> *(int*)(Rhs.Address); break;
		}
	    Ret.Address = Address;
		Ret.Type = T_OBJECT;
	}
    return Ret;     
}

var& var::operator <<=(var& Rhs){
    static var Ret;
    Ret.Address = vs::push_int( *(int*)(Address) <<= *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;     
}

var& var::operator >>=(var& Rhs){
    static var Ret; 
    Ret.Address = vs::push_int( *(int*)(Address) >>= *(int*)(Rhs.Address) );
    Ret.Type = T_INT;
    return Ret;     
}

