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
#include <list>
#include "ParserStream.h"
namespace Lex {
    
    
    template<typename T>
    class LexResult
    {
        T _value;
        ParserStream _remain;
        bool _isNone = true;
    public:
        typedef T LexType;
        LexResult()
        {}
        LexResult(const T& v,const ParserStream& r)
        :_value(v)
        ,_remain(r)
        ,_isNone(false)
        {}
        ~LexResult(){}
    public:
        bool isNone(){return _isNone;}
        const T& value(){
            if (isNone()) {
                throw std::bad_exception();
            }
            return _value;
        }
        const ParserStream& remain()
        {
            return _remain;
        }
        
    public:
        static std::shared_ptr<LexResult<T>> Some(const T& v,const ParserStream& r)
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
        typedef std::function<Result(const ParserStream&)> Parser;
        typedef T ElemntT;
        static Parser ret(const T& t)
        {
            return [t](const ParserStream& inp)->Result
            {
                return LexResult<T>::Some(t,inp);
            };
        }
        
        static Parser failure()
        {
            return [](const ParserStream& inp)->Result
            {
                return LexResult<T>::None();
            };
        }
    };
    
    //same as regular expression cocatnation but more powerfull
    
    template<typename T1,typename T2>
    inline typename ParserType<T2>::Parser Bind(const typename ParserType<T1>::Parser& x,const std::function<typename ParserType<T2>::Parser(const T1&)>& y)
    {
        return [x,y](const ParserStream& inp)->typename ParserType<T2>::Result{
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
    
//    template<typename T1,typename T2>
//    inline auto _Bind(T1 x,T2 y)->decltype(y(x))
//    {
//        typedef typename decltype(x(ParserStream()))::element_type::LexType T11;
//        typedef typename decltype((y(T11()))(ParserStream()))::element_type::LexType T3;
//        return [x,y](const ParserStream& inp)->typename ParserType<T3>::Result{
//            auto r = x(inp);
//            if (r->isNone()) {
//                return LexResult<T2>::None();
//            }
//            else
//            {
//                return (y(r->value()))(r->remain());
//            }
//            
//        };
//    }
#define CONS(ct,e1,r) return Lex::Bind<decltype(e1(Lex::ParserStream()))::element_type::LexType,ct>(e1, [=](const decltype(e1(Lex::ParserStream()))::element_type::LexType& r )->typename Lex::ParserType<ct>::Parser{

#define CONSF(ct,e1,r) Lex::Bind<decltype(e1(Lex::ParserStream()))::element_type::LexType,ct>(e1, [=](const decltype(e1(Lex::ParserStream()))::element_type::LexType& r )->typename Lex::ParserType<ct>::Parser{


#define EndCONS })

#define RET(e) return Lex::ParserType<decltype(e)>::ret(e)
#define RETF(e) Lex::ParserType<decltype(e)>::ret(e)    
    template<typename T>
    inline typename ParserType<T>::Parser ChooseN(const std::list<typename ParserType<T>::Parser>& ps)
    {
        return [ps](const ParserStream& inp)->typename ParserType<T>::Result{
            for (auto iter = ps.begin(); iter != ps.end(); ++iter) {
                auto r = (*iter)(inp);
                if (!r->isNone()) {
                    return r;
                }
            }
            return LexResult<T>::None();
        };
    }
#define CHOOSEN(xs) Lex::ChooseN<decltype(xs)::value_type::ElemntT>(xs)
    
    
    //regular expression Choice
    template<typename T>
    inline typename ParserType<T>::Parser Choice(const typename ParserType<T>::Parser& x,const typename ParserType<T>::Parser& y)
    {
        return [x,y](const ParserStream& inp)->typename ParserType<T>::Result{
            auto r = x(inp);
            if (!r->isNone()) {
                return r;
            }
            else
            {
                return y(inp);
            }
        };
    }
    template<typename T>
    inline typename ParserType<T>::Parser Option(const typename ParserType<T>::Parser& x)
    {
        return Choice<T>(x, ParserType<T>::ret(T()));
    }
    
    //regular expression +
    template<typename T>
    typename ParserType<std::list<T>>::Parser Many1(const typename ParserType<T>::Parser& f);
    
    //regular expression *
    template<typename T>
    inline typename ParserType<std::list<T>>::Parser Many(const typename ParserType<T>::Parser& f)
    {
        return Option<std::list<T>>(Many1<T>(f));
    }
    
    template<typename T>
    inline typename ParserType<std::list<T>>::Parser Many1(const typename ParserType<T>::Parser& f)
    {
        return Bind<T, std::list<T>>(f,[f](T v){
            return Bind<std::list<T>, std::list<T>>(Many<T>(f), [v](std::list<T> vs)
                                                        {
                                                            vs.push_front (v);
                                                            return ParserType<std::list<T>>::ret(vs);
                                                        });
        });
        
    }
    
    ParserType<std::list<char>>::Result whiteParser(const ParserStream& inp);
    
    template<typename T>
    inline typename ParserType<T>::Parser Token(const typename ParserType<T>::Parser& f)
    {
        return Bind<std::list<char>, T>(whiteParser, [f](const std::list<char>&)->typename ParserType<T>::Parser{
            return f;
        });

    }
    
    ParserType<std::string>::Parser charListToString(const ParserType<std::list<char>>::Parser&);
    ParserType<char>::Parser satParser(std::function<bool(char)> f);
    ParserType<char>::Parser charParser(char c);
    ParserType<std::string>::Parser charsParser(char c);
    ParserType<std::string>::Parser strParser(const std::string& str);
    ParserType<char>::Result item(const ParserStream& inp);
    ParserType<std::string>::Result digitsParser(const ParserStream& inp);
    ParserType<std::string>::Result idParser(const ParserStream& inp);
    
    #define TOKEN(t) Lex::Token<std::string>(Lex::strParser(t))
}
#endif /* defined(__Compiler__Lex__) */
