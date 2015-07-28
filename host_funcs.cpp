#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include "vm.h"
#include "token_types.h"
#include "vs.h"

using namespace std;

//##############################################################################

ptr HOST_ADD( vector<tree>& Args ){ 
	int Sum=0;
	for(int i=0; i<Args.size(); ++i){
		Sum += Args[i].GetInt();
	}

	return vs::push_int( Sum );;    
}

ptr HOST_EXIT( vector<tree>& Args ){
	int a=0;
	if(Args.size()) a = Args[0].GetInt();
	exit(a);
	return 0;
}

ptr HOST_SYSTEM( vector<tree>& Args ){
	char* a = Args[0].GetCharPtr(); 
	ptr RetPointer = vs::push_int( system( ("\"" + string(a) + "\"").c_str() ) );
	return RetPointer;    
}

ptr HOST_ATOI( vector<tree>& Args ){
	char* a = Args[0].GetCharPtr(); 
	return vs::push_int( atoi(a) );;    
}

ptr HOST_ATOF( vector<tree>& Args ){
	char* a = Args[0].GetCharPtr(); 
	ptr RetPointer = vs::push_float( atof(a) );
	return RetPointer;    
}

ptr HOST_MALLOC( vector<tree>& Args ){
	return (ptr)( malloc(Args[0].GetInt()) );
}

ptr HOST_FREE( vector<tree>& Args ){
	free( (void*)Args[0].GetPtr() );
	return 0;
}

ptr HOST_MEMCPY( vector<tree>& Args ){
	memcpy( (void*)(Args[0].GetPtr()), (const void*)(Args[1].GetPtr()), Args[2].GetInt() );
	return 0;
}

ptr HOST_MEMSET( vector<tree>& Args ){
	memset( (void*)(Args[0].GetPtr()), Args[1].GetInt(), Args[2].GetInt() );
	return 0;
}

ptr HOST_STRCPY( vector<tree>& Args ){
	strcpy( (char*)(Args[0].GetPtr()), (const char*)(Args[1].GetPtr()) );
	return 0;
}

ptr HOST_STRLEN( vector<tree>& Args ){
	return vs::push_int( strlen( (char*)(Args[0].GetPtr()) ) );
}

ptr HOST_STRCMP( vector<tree>& Args ){
	return vs::push_int( strcmp( (const char*)(Args[0].GetPtr()), (const char*)(Args[1].GetPtr()) ) );
}

ptr HOST_GETKEY( vector<tree>& Args ){
	return vs::push_int( GetAsyncKeyState( Args[0].GetChar() ) );
}

ptr HOST_TIME( vector<tree>& Args ){
	int a = 0;
	if( Args.size() ) a = Args[0].GetPtr();
	return vs::push_int( time( (time_t*)(a) ) );
}

ptr HOST_CLOCK( vector<tree>& Args ){
	return vs::push_int( clock() );
}

//##############################################################################

int vm::DeclareHostFuncs()
{    
	Funcs["add"] = func( var( 0, T_INT, sizeof(int)), HOST_ADD );
	Funcs["exit"] = func( 0, HOST_EXIT );
	Funcs["system"] = func( var( 0, T_INT, sizeof(int)), HOST_SYSTEM );
	Funcs["atoi"] = func( var( 0, T_INT, sizeof(int)), HOST_ATOI );
	Funcs["atof"] = func( var( 0, T_FLOAT, sizeof(float)), HOST_ATOF );
	Funcs["malloc"] = func( var( 0, T_PTR, sizeof(ptr)), HOST_MALLOC );
	Funcs["free"] = func( var( 0, T_VOID, 0), HOST_FREE );
	Funcs["memcpy"] = func( var( 0, T_VOID, 0), HOST_MEMCPY );
	Funcs["memset"] = func( var( 0, T_VOID, 0), HOST_MEMSET );
	Funcs["strcpy"] = func( var( 0, T_VOID, 0), HOST_STRCPY );
	Funcs["strlen"] = func( var( 0, T_INT, sizeof(ptr)), HOST_STRLEN );
	Funcs["strcmp"] = func( var( 0, T_INT, sizeof(ptr)), HOST_STRCMP );
	Funcs["time"] = func( var( 0, T_INT, sizeof(int)), HOST_TIME );
	Funcs["clock"] = func( var( 0, T_INT, sizeof(int)), HOST_CLOCK );
	// sin, cos, tan, asin, acos, atan, atan2, exp, ln, log, sqrt, PoRusskiy, InEnglish

	return 0;                          
}
