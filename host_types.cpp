#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include "vm.h"
#include "token_types.h"
#include "vs.h"

int vm::DeclareHostTypes()
{
	TypeMap["dog"] = 0; Types.push_back( type() );
	Types[0].Vars["age"]  = var( 0, T_INT, sizeof(int) );
	Types[0].Vars["name"] = var( 4, T_PTR, sizeof(ptr), T_CHAR,	1 );
	Types[0].Vars["size"] = var( 8, T_INT, sizeof(int) );
	Types[0].Size = 12;

	return 0;
}