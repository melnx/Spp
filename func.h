#ifndef FUNC_H
#define FUNC_H

#include <string>
#include <vector>
#include "tree.h"

typedef ptr (*fpointer)(std::vector<tree>& Arg);

class arg
{
    int Size;
    int Type;
    std::string Class;  
    bool IsPtr;
    bool IsRef;
    std::string Default;
    
    //arg(){
    //    Size=Type=IsPtr=IsRef=0;
    //    Class=Default="":
    //}
};

class func
{
    public:
           
    std::map<std::string,var> Static;
    std::vector<arg> Args;
    fpointer Address;
    
    var Return;
    
    func();
    func( var ReturnVar, fpointer FuncAddress );
    
    ptr Call(tree& Args);
      
};

#endif
