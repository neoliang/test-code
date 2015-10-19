//
//  main.cpp
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include <iostream>
#include "Lex.h"
#include <fstream>
int main(int argc, const char * argv[]) {
    // insert code here...
    Lex::ParserType<std::string>::Parser helloParser = Lex::strParser("hello");
    auto r = helloParser("hellow world");
    std::cout << r->value() << std::endl;
    auto digits = Lex::digitsParser("112345 xxxx");
    auto l = digits->value();
    std::string x(l.begin(),l.end());
    std::cout << x << std::endl;
    return 0;
}
