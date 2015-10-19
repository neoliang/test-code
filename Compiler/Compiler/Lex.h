//
//  Lex.h
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef __Compiler__Lex__
#define __Compiler__Lex__

#include <stdio.h>
#include <exception>
#include <memory>
#include <string>
#include <functional>
namespace Lex {
    template<typename T>
    class LexResult
    {
    public:
        virtual ~LexResult(){}
    public:
        virtual bool IsFailed(){return true;}
        virtual T value(){throw std::bad_exception();}
    };
    template<typename T>
    class SuccLexResult : public LexResult<T>
    {
        T _value;
    public:
        SuccLexResult(const T& v)
        :_value(v){}
    public:
        bool IsFailed(){return false;}
        virtual T value(){
            return _value;
        }
    };
    

    
    template<typename T>
    inline static   std::shared_ptr<LexResult<T>> LexResultSucc(const T& v)
    {
        return std::shared_ptr<LexResult<T>>(new SuccLexResult<T>(v));
    }
    template<typename T>
    inline static std::shared_ptr<LexResult<T>> LexResultNone()
    {
        return std::shared_ptr<LexResult<T>>(new LexResult<T>());
    }

    template<typename T>
    struct ParserType
    {
        typedef std::pair<T, std::string> Pair;
        typedef std::shared_ptr<LexResult<Pair>> Result;
        typedef std::function<Result(const std::string&)> Parser;
        static Parser ret(T t)
        {
            return [t](const std::string& inp)->Result
            {
                return LexResultSucc(std::make_pair(t,inp));
            };
        }
        static Parser failure()
        {
            return [](const std::string& inp)->Result
            {
                return LexResultNone<std::pair<T, std::string>>();
            };
        }
    };
    typedef ParserType<char>::Pair ItemPair;
    typedef ParserType<char>::Result ItemResult;
    typedef ParserType<char>::Parser ItemParser;
    inline ItemResult item(const std::string& inp)
    {
        if (inp.empty()) {
            return LexResultNone<ItemPair>();
        }
        else
        {
            return LexResultSucc(std::make_pair(inp.front(), inp.substr(1)));
        }
    }
    template<typename T1,typename T2>
    inline typename ParserType<T2>::Parser Bind(typename ParserType<T1>::Parser x,std::function<typename ParserType<T2>::Parser(T1)> y)
    {
        return [x,y](const std::string& inp)->typename ParserType<T2>::Result{
            auto r = x(inp);
            if (r->IsFailed()) {
                return LexResultNone<std::pair<T2, std::string>>();
            }
            else
            {
                return (y(r->value().first))(r->value().second);
            }
            
        };
    }
    
    inline typename ParserType<char>::Parser satParser(std::function<bool(char)> f)
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
    
    inline typename ParserType<char>::Parser charParser(char c)
    {
        return satParser([c](char x)->bool {return c == x;});
    }
    inline typename ParserType<std::string>::Parser charsParser(char c)
    {
        return [c](const std::string& inp)->typename ParserType<std::string>::Result{
            auto r = (charParser(c))(inp);
            if (r->IsFailed()) {
                return LexResultNone<std::pair<std::string, std::string>>();
            }
            else
            {
                std::string cs = "";
                cs.push_back(r->value().first);
                return LexResultSucc(std::make_pair(cs, r->value().second));
            }
        };
    }
    ParserType<std::string>::Parser strParser(const std::string& str);
}
#endif /* defined(__Compiler__Lex__) */
