#ifndef OPERATORS_H
#define OPERATORS_H

enum operator_assoc{
     LEFT_TO_RIGHT,
     RIGHT_TO_LEFT   
};

//##############################################################################

//used during parsing for tree construction
struct op
{
    int Type;
    int Precedence;
    std::string Lexeme;
    bool Lhs;
    bool Rhs;  
    bool Assoc;   
};

//##############################################################################

//used during parsing for tree construction
struct keyword
{
    int Type;   
    std::string Lexeme;   
};

op GetOp(std::string Lexeme);
keyword GetKeyword(std::string Lexeme);

#endif
