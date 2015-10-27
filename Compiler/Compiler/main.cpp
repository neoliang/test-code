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
#include <sstream>
#include "TinyParser.h"
int main(int argc, const char * argv[]) {
//    std::istringstream iss("hello word");
//    std::istream_iterator<char> issi(iss);
//    std::istream_iterator<char> end;
//    unsigned int i =0;
//    while (issi != end) {
//        std::cout<< *issi ;
//        ++issi;
//    }
//    std::cout << std::endl;
    // insert code here..
//    Lex::ParserStream s = Lex::ParserStream::fromString("hellow world");
//    Lex::ParserType<std::string>::Parser helloParser = Lex::strParser("hello");
//    auto r = helloParser(s);
//    std::cout << r->value() << std::endl;
//    auto digits = Lex::digitsParser(Lex::ParserStream::fromString("90+23*55 xxxx"));
//    auto l = digits->value();
//    std::string x(l.begin(),l.end());
//    std::cout << x << std::endl;
    
    auto stream = Lex::ParserStream::fromString("23");
    auto exp = Parser::Exp(stream);
    
    return 0;
}
