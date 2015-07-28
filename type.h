#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <map>

class type
{
    public:  
    std::map<std::string,var> Vars;
	std::map<std::string,func> Funcs;
    int Size;
};

#endif
