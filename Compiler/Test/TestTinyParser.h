//
//  TestTinyParser.h
//  Compiler
//
//  Created by neo on 15-10-30.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef Compiler_TestTinyParser_h
#define Compiler_TestTinyParser_h
#include "TinyParser.h"
#include <sstream>

template<typename T>
std::string toString(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}


inline void TestTinyParser()
{
    //number
    //digit
    rc::check("number",[](const int& n,const int & n1){
        int number = abs(n);
        int number1 = abs(n1);
        auto str = "(" + toString(number) + ")" + "*" + toString(number1);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::Exp(stream);
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(exp);
        RC_ASSERT(uexp != nullptr);
        RC_ASSERT(uexp->GetLeftExp() != nullptr);
        RC_ASSERT(uexp->GetRightExp() != nullptr);
    });
    
}

#endif
