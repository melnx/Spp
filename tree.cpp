#include "tree.h"

#include <iostream>
using namespace std;

void tree::Show(tree Tree, int Depth)
{
	for(int i=0; i<Depth; i++){
        cout << "  ";
    }
    cout << Tree.Data << " : " << Tree.Type << endl;
    for(int i=0; i<Tree.Branch.size(); i++){
        Show(Tree.Branch[i],Depth+2);  
    }
}

int tree::GetInt(){
	return *(int*)Address->Address;
}

ptr tree::GetPtr(){
	return *(ptr*)Address->Address;
}

float tree::GetFloat(){
	return *(float*)Address->Address;
}

char tree::GetChar(){
	return *(char*)Address->Address;
}

char* tree::GetCharPtr(){
	return (char*)(*(ptr*)Address->Address);
}