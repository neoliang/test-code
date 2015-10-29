//
//  TestLex.cpp
//  Test
//
//  Created by neo on 15-10-28.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "Lex.h"
#include "rapidcheck.h"
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
}


//TEST_CASE( "[lex]", "[lex]" ) {
//    SECTION("normal")
//    {
//        auto stream = Lex::ParserStream::fromString("hellow word");
//        auto itemResult = item(stream);
//        REQUIRE(itemResult->value() == 'h');
//        REQUIRE(itemResult->remain().get() == 'e');
//    }
//    SECTION("empty and 1"){
//        auto r = item(ParserStream::fromString(""));
//        REQUIRE(r->isNone());
//        auto r1 = item(ParserStream::fromString("1"));
//        REQUIRE(r1->value() == '1');
//        REQUIRE(r1->remain().empty());
//    }
//}
