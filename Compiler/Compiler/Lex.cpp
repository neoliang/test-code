//
//  Lex.cpp
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "Lex.h"
namespace Lex{
    
    ParserType<std::list<char>>::Parser digitsParser = Many1<char>(satParser([](char c){
            return isnumber(c);
        }));
    ParserType<char>::Result item(const std::string& inp)
    {
        if (inp.empty()) {
            return LexResult<char>::None();
        }
        else
        {
            return LexResult<char>::Some(inp.front(), inp.substr(1));
        }
    }
    
    ParserType<char>::Parser satParser(std::function<bool(char)> f)
    {
        auto funx = [f](char x)->typename ParserType<char>::Parser{
            if (f(x)) {
                return ParserType<char>::ret(x);
            }
            else
            {
                return ParserType<char>::failure();
            }
        };
        auto x = Bind<char,char>(item, funx);
        return x;
    }
    
    ParserType<char>::Parser charParser(char c)
    {
        return satParser([c](char x)->bool {return c == x;});
    }
    
    ParserType<std::string>::Parser charsParser(char c)
    {
        return [c](const std::string& inp)->typename ParserType<std::string>::Result{
            auto r = (charParser(c))(inp);
            if (r->isNone()) {
                return LexResult<std::string>::None();
            }
            else
            {
                std::string cs = "";
                cs.push_back(r->value());
                return LexResult<std::string>::Some(cs, r->remain());
            }
        };
    }
    
    ParserType<std::string>::Parser strParser(const std::string& str)
    {
        if (str.empty()) {
            return ParserType<std::string>::ret("");
        }
        else
        {
            auto x = str.front();
            auto xs = str.substr(1);
            return Bind<char, std::string>(charParser(x),
                [str,xs](char)->typename ParserType<std::string>::Parser
                {
                    return Bind<std::string,std::string>(strParser(xs),
                        [str](std::string)->typename ParserType<std::string>::Parser
                    {
                        return ParserType<std::string>::ret(str);
                    });
                });
        }
    }
}