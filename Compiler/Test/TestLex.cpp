//
//  TestLex.cpp
//  Test
//
//  Created by neo on 15-10-28.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "Lex.h"
#include "rapidcheck.h"
#include <sstream>
using namespace std;
using namespace Lex;

void TestLex()
{
    //parser stream
    rc::check("empty",
              []() {
                  auto stream = Lex::ParserStream::fromString("");
                  RC_ASSERT(stream.empty());
              });
    rc::check("get",[](const std::string& str){
        auto stream1 = Lex::ParserStream::fromString(str);
        auto iter = str.begin();
        while (iter != str.end()) {
            RC_ASSERT(*iter == stream1.get());
            stream1 = stream1.next();
            ++iter;
        }
    });
    
    //item
    rc::check("item",[](const std::string& str){
        auto stream = Lex::ParserStream::fromString(str);
        auto itemResult = item(stream);
        if(str.empty())
        {
            RC_ASSERT(itemResult->isNone());
        }
        else
        {
            RC_ASSERT(*str.begin() == itemResult->value());
        }
    });
    
    //sat
    rc::check("sat & char & chars",[](const char& c){
        auto str = std::string();
        str.push_back(c);
        auto stream = Lex::ParserStream::fromString(str);
        auto p = satParser([c](char x)->bool{return c == x;});
        auto r = p(stream);
        RC_ASSERT(r->value() == c);
        RC_ASSERT(r->remain().empty());
        p = charParser(c);
        r = p(stream);
        RC_ASSERT(r->value() == c);
        RC_ASSERT(r->remain().empty());
        auto ps = charsParser(c);
        auto rs = ps(stream);
        RC_ASSERT(rs->value().front() == c);
        RC_ASSERT(rs->value().size() == 1);
        RC_ASSERT(r->remain().empty());
        
    });
    
    //str
    rc::check("string",[](const std::string& str){
        auto stream = Lex::ParserStream::fromString(str);
        auto sp = Lex::strParser(str);
        auto r = sp(stream);
        RC_ASSERT(r->value() == str);
        RC_ASSERT(r->remain().empty());
    });
    
    
    //wihte space
    auto stream = ParserStream::fromString("0");
    auto r = Lex::whiteParser(stream);
    RC_ASSERT(!r->isNone());
    //digit
    rc::check("digits",[](const unsigned int& number){
        stringstream ss;
        ss << number;
        auto stream = ParserStream::fromString(ss.str());
        auto r = digitsParser(stream);
        std::string str(r->value().begin(),r->value().end());
        RC_ASSERT(str == ss.str());
    });
}

