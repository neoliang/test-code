//
//  Lex.cpp
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "Lex.h"
namespace Lex{
    
    ParserType<std::string>::Parser charListToString(const ParserType<std::list<char>>::Parser& charsP)
    {
        return [charsP](const Lex::ParserStream& inp)->typename ParserType<std::string>::Result{
            auto r = charsP(inp);
            if (r->isNone()) {
                return Lex::LexResult<std::string>::None();
            }
            else
            {
                auto str = std::string(r->value().begin(),r->value().end());
                return Lex::LexResult<std::string>::Some(str,r->remain());
            }
        };
    }
    
    ParserType<std::string>::Result digitsParser(const ParserStream& inp)
    {
        return Token<std::string>( charListToString(Many1<char>(satParser([](char c){
            return isnumber(c);
        }))))(inp);
    }
    ParserType<std::string>::Result idParser(const ParserStream& inp)
    {
        auto firstCharParser = Lex::satParser([](char c){return isalpha(c) || c == '_';});
        auto alnumsParser = Many<char>(Lex::satParser([](char c){return isalnum(c) ;}));
        auto _idParser = CONSF(std::string, firstCharParser, x)
        CONS(std::string, alnumsParser,xs )
        std::string id ;
        id.push_back(x);
        for (auto iter = xs.begin(); iter != xs.end(); ++iter) {
            id.push_back(*iter);
        }
        RET(id);
        EndCONS;
        EndCONS;
        return Token<std::string>(_idParser)(inp);
    }
    
    ParserType<std::list<char>>::Result whiteParser(const ParserStream& inp)
    {
        return  Many<char>(satParser([](char c){
            return c == ' ' || c == '\n' || c == '\t';
        }))(inp);
    }
    ParserType<char>::Result item(const  ParserStream& inp)
    {
        if (inp.empty()) {
            return LexResult<char>::None();
        }
        else
        {
            return LexResult<char>::Some(inp.get(), inp.next());
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
        return Bind<char,char>(item, funx);
    }
    
    ParserType<char>::Parser charParser(char c)
    {
        return satParser([c](char x)->bool {return c == x;});
    }
    
    ParserType<std::string>::Parser charsParser(char c)
    {
        CONS(std::string, charParser(c), ch)
        std::string cs;
        cs.push_back(ch);
        RET(cs);
        EndCONS;
    }
    
    ParserType<std::string>::Parser strParser(const std::string& str)
    {
        if (str.empty()) {
            RET((std::string)"");
        }
        else
        {
            auto x = charParser(str.front());
            auto xs = strParser(str.substr(1));
            CONS(std::string, x, _)
            CONS(std::string, xs, _)
            RET((std::string)str);
            EndCONS;
            EndCONS;
        }
    }
}