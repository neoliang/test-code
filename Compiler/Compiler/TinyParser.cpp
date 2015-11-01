//
//  TinyParser.cpp
//  Compiler
//
//  Created by neo on 15-10-26.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "TinyParser.h"
#include <sstream>
#include <iostream>
namespace Parser
{
    
    Lex::ParserType<ReadStatementPtr>::Result ParserRead(const Lex::ParserStream& inp)
    {
        auto _paser = Lex::Bind<std::string, ReadStatementPtr>(Lex::Token<std::string>(Lex::strParser("read")), [](const std::string&)->typename Lex::ParserType<ReadStatementPtr>::Parser{
            return [](const Lex::ParserStream& inp)->typename Lex::ParserType<ReadStatementPtr>::Result{
                auto id = Lex::idParser(inp);
                if (id->isNone()) {
                    return Lex::LexResult<ReadStatementPtr>::None();
                }
                else
                {
                    std::string idstr = std::string(id->value().begin(),id->value().end());
                    auto readState =  ReadStatementPtr(new ReadStatement(id->remain().lineNum(),idstr));
                    return Lex::LexResult<ReadStatementPtr>::Some(readState, id->remain());
                }
            };
        });
        return _paser(inp);
    }
    
    Lex::ParserType<WriteStatementPtr>::Result ParserWrite(const Lex::ParserStream& inp)
    {
        auto _paser = Lex::Bind<std::string, WriteStatementPtr>(Lex::Token<std::string>(Lex::strParser("write")), [](const std::string&)->typename Lex::ParserType<WriteStatementPtr>::Parser{
            return [](const Lex::ParserStream& inp)->typename Lex::ParserType<WriteStatementPtr>::Result{
                auto id = ParserExp(inp);
                if (id->isNone()) {
                    return Lex::LexResult<WriteStatementPtr>::None();
                }
                else
                {
                    auto writeState =  WriteStatementPtr(new WriteStatement(id->remain().lineNum(),id->value()));
                    return Lex::LexResult<WriteStatementPtr>::Some(writeState, id->remain());
                }
            };
        });
        return _paser(inp);
    }
    
    ExpNodePtr Exp(const Lex::ParserStream& inp)
    {
        auto r = ParserExp(inp);
        if (r->isNone()) {
            return nullptr;
        }
        return r->value();
    }
    Lex::ParserType<ExpNodePtr>::Result ParserConst(const Lex::ParserStream& inp)
    {
        auto dig = Lex::digitsParser(inp);
        if (dig->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            std::stringstream ss;
            for (std::list<char>::const_iterator iter  = dig->value().begin();iter != dig->value().end();++iter)
            {
                ss << *iter;
            }
            int vi =0;
            ss >> vi;
            return Lex::LexResult<ExpNodePtr>::Some(ExpNodePtr(new ConstExp(dig->remain().lineNum(),vi)), dig->remain());
        }
    }
    Lex::ParserType<ExpNodePtr>::Result ParserIdentifier(const Lex::ParserStream& inp)
    {
        auto dig = Lex::idParser(inp);
        if (dig->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            auto v = std::string(dig->value().begin(),dig->value().end());
            return Lex::LexResult<ExpNodePtr>::Some(ExpNodePtr(new IdExp(dig->remain().lineNum(),v)), dig->remain());
        }
    }
    
    
    Lex::ParserType<ExpNodePtr>::Result ParserFactor(const Lex::ParserStream& inp)
    {
        auto lexp = Lex::Bind<char, ExpNodePtr>(Lex::satParser([](char c){return c== '(';}),
            [](char )->typename Lex::ParserType<ExpNodePtr>::Parser{
                return ParserExp;
            }
        );
        auto fexp = Lex::Bind<ExpNodePtr, ExpNodePtr>(lexp, [](ExpNodePtr exp)->typename Lex::ParserType<ExpNodePtr>::Parser{
            return [exp](const Lex::ParserStream& inp)->typename Lex::ParserType<ExpNodePtr>::Result
            {
                auto rexp = Lex::satParser([](char c){return c== ')';})(inp);
                if (rexp->isNone()) {
                    return Lex::LexResult<ExpNodePtr>::None();
                }
                else
                {
                    return Lex::LexResult<ExpNodePtr>::Some(exp, rexp->remain());
                }
            };
        });
        return Lex::ChooseN<ExpNodePtr>({ParserConst,ParserIdentifier,fexp}) (inp);
    }
    Lex::ParserType<ExpNodePtr>::Result ParserTerm(const Lex::ParserStream& inp)
    {
        auto mulOpParser = Lex::satParser([](char c){return c == '*' || c == '/';});
        auto bMulOpExp = [](char c)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Parser{
            return [c](const Lex::ParserStream& inp)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Result
            {
                auto factor = ParserFactor(inp);
                if(factor->isNone())
                {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::None();
                }
                else
                {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::Some(std::make_pair(c, factor->value()),factor->remain());
                }
            };
        };
        auto mulExpPaser = Lex::Bind<char, std::pair<char, ExpNodePtr>>(mulOpParser,bMulOpExp);
        
        auto bMulExp =[mulExpPaser](ExpNodePtr exp)-> typename Lex::ParserType<ExpNodePtr>::Parser
        {
            return [mulExpPaser,exp](const Lex::ParserStream& inp)->typename Lex::ParserType<ExpNodePtr>::Result
            {
                auto comparisonR = mulExpPaser(inp);
                if (comparisonR->isNone()) {
                    return Lex::LexResult<ExpNodePtr>::Some(exp, inp);
                }
                else
                {
                    std::string op ;
                    op.push_back(comparisonR->value().first);
                    ExpNodePtr newExp = ExpNodePtr(new UnaryOpExp(comparisonR->remain().lineNum(),op, exp,comparisonR->value().second));
                    return Lex::LexResult<ExpNodePtr>::Some(newExp, comparisonR->remain());
                }
            };
            
        };
        auto x = Lex::Bind<ExpNodePtr, ExpNodePtr>(ParserFactor,bMulExp);
        auto y = x(inp);
        if (y->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            return Lex::LexResult<ExpNodePtr>::Some(y->value(), y->remain());
        }
    }
    
    Lex::ParserType<ExpNodePtr>::Result ParserSimpleExp(const Lex::ParserStream& inp)
    {
        auto addOpParser = Lex::satParser([](char c){return c == '+' || c == '-';});
        auto bOpExp = [](char c)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Parser{
            return [c](const Lex::ParserStream& inp)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Result
            {
                auto term = ParserTerm(inp);
                if(term->isNone())
                {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::None();
                }
                else
                {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::Some(std::make_pair(c, term->value()),term->remain());
                }
            };
        };
        auto addExpPaser = Lex::Bind<char, std::pair<char, ExpNodePtr>>(addOpParser,bOpExp);
        
        auto bAddExp =[addExpPaser](ExpNodePtr exp)-> typename Lex::ParserType<ExpNodePtr>::Parser
        {
            return [addExpPaser,exp](const Lex::ParserStream& inp)->typename Lex::ParserType<ExpNodePtr>::Result
            {
                auto comparisonR = addExpPaser(inp);
                if (comparisonR->isNone()) {
                    return Lex::LexResult<ExpNodePtr>::Some(exp, inp);
                }
                else
                {
                    std::string op ;
                    op.push_back(comparisonR->value().first);
                    ExpNodePtr newExp = ExpNodePtr(new UnaryOpExp(comparisonR->remain().lineNum(),op, exp,comparisonR->value().second));
                    return Lex::LexResult<ExpNodePtr>::Some(newExp, comparisonR->remain());
                }
            };
            
        };
        auto x = Lex::Bind<ExpNodePtr, ExpNodePtr>(ParserTerm,bAddExp);
        auto y = x(inp);
        if (y->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            return Lex::LexResult<ExpNodePtr>::Some(y->value(), y->remain());
        }
    }
    
    Lex::ParserType<ExpNodePtr>::Result ParserExp(const Lex::ParserStream &inp)
    {
        auto comparisonParser = Lex::satParser([](char c){return c == '=' || c == '<';});
        auto comparExp = [](char c)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Parser{
            return [c](const Lex::ParserStream& inp)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Result
            {
                auto simple = ParserSimpleExp(inp);
                if(simple->isNone())
                {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::None();
                }
                else
                {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::Some(std::make_pair(c, simple->value()),simple->remain());
                }
            };
        };
        auto comparisonExp = Lex::Bind<char, std::pair<char, ExpNodePtr>>(comparisonParser,comparExp);
        auto bComparsionExp =[comparisonExp](ExpNodePtr exp)-> typename Lex::ParserType<ExpNodePtr>::Parser
        {
            return [comparisonExp,exp](const Lex::ParserStream& inp)->typename Lex::ParserType<ExpNodePtr>::Result
            {
                auto comparisonR = comparisonExp(inp);
                if (comparisonR->isNone()) {
                    return Lex::LexResult<ExpNodePtr>::Some(exp, inp);
                }
                else
                {
                    std::string op ;
                    op.push_back(comparisonR->value().first);
                    ExpNodePtr newExp = ExpNodePtr(new UnaryOpExp(comparisonR->remain().lineNum(),op, exp,comparisonR->value().second));
                    return Lex::LexResult<ExpNodePtr>::Some(newExp, comparisonR->remain());
                }
            };
            
        };
        auto x = Lex::Bind<ExpNodePtr, ExpNodePtr>(ParserSimpleExp,bComparsionExp);
        auto y = x(inp);
        if (y->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            return Lex::LexResult<ExpNodePtr>::Some(y->value(), y->remain());
        }
    
    }
}