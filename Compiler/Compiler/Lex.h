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
        T _value;
        std::string _remain;
        bool _isNone = true;
    public:
        LexResult()
        {}
        LexResult(const T& v,const std::string& r)
        :_value(v)
        ,_remain(r)
        ,_isNone(false)
        {}
        ~LexResult(){}
    public:
        bool isNone(){return _isNone;}
        T value(){
            if (isNone()) {
                throw std::bad_exception();
            }
            return _value;
        }
        const std::string& remain()
        {
            return _remain;
        }
        
    public:
        static std::shared_ptr<LexResult<T>> Some(const T& v,const std::string& r)
        {
            return std::shared_ptr<LexResult<T>>(new LexResult<T>(v,r));
        }
        static std::shared_ptr<LexResult<T>> None()
        {
            return std::shared_ptr<LexResult<T>>(new LexResult<T>());
        }
    };

    template<typename T>
    struct ParserType
    {
        typedef std::shared_ptr<LexResult<T>> Result;
        typedef std::function<Result(const std::string&)> Parser;
        static Parser ret(T t)
        {
            return [t](const std::string& inp)->Result
            {
                return LexResult<T>::Some(t,inp);
            };
        }
        static Parser failure()
        {
            return [](const std::string& inp)->Result
            {
                return LexResult<T>::None();
            };
        }
    };
    template<typename T1,typename T2>
    inline typename ParserType<T2>::Parser Bind(typename ParserType<T1>::Parser x,std::function<typename ParserType<T2>::Parser(T1)> y)
    {
        return [x,y](const std::string& inp)->typename ParserType<T2>::Result{
            auto r = x(inp);
            if (r->isNone()) {
                return LexResult<T2>::None();
            }
            else
            {
                return (y(r->value()))(r->remain());
            }
            
        };
    }
    
    ParserType<char>::Result item(const std::string& inp);
    ParserType<char>::Parser satParser(std::function<bool(char)> f);
    ParserType<char>::Parser charParser(char c);
    ParserType<std::string>::Parser charsParser(char c);
    ParserType<std::string>::Parser strParser(const std::string& str);
}
#endif /* defined(__Compiler__Lex__) */
