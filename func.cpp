#include "func.h"
#include <iostream>

using namespace std;

ptr func::Call(tree& Args)
{
    
    return (*Address)(Args.Branch);

}

func::func( var ReturnVar, fpointer FuncAddress )
{
    
    Return = ReturnVar;
    Address = FuncAddress;
          
}

func::func()
{
    Address = 0;   
}