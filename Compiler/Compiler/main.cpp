//
//  main.cpp
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include <iostream>
#include "Lex.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    Lex::ParserType<std::string>::Parser helloParser = Lex::strParser("hello");
    auto r = helloParser("hellow world");
    std::cout << r->value() << std::endl;
    return 0;
}
