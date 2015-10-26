//
//  TinyParser.cpp
//  Compiler
//
//  Created by neo on 15-10-26.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "TinyParser.h"
#include <sstream>
namespace Parser
{
     typedef std::shared_ptr<ExpNode> ExpNodePtr;
    Lex::ParserType<ExpNodePtr>::Result ParserExp(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserSimpleExp(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserTerm(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserFactor(const Lex::ParserStream& inp);
    
    Lex::ParserType<ExpNodePtr>::Result ParserConst(const Lex::ParserStream& inp)
    {
        auto dig = Lex::digitsParser(inp);
        if (dig->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            auto v = std::string(dig->value().begin(),dig->value().end());
            std::stringstream ss;
            ss >> v;
            int vi ;
            ss << vi;
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
        return Lex::Choice<ExpNodePtr>(ParserIdentifier,ParserConst) (inp);
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
        
        auto bMulExp =[mulExpPaser](ExpNodePtr exp)-> typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Parser
        {
            return [mulExpPaser,exp](const Lex::ParserStream& inp)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Result
            {
                auto comparisonR = mulExpPaser(inp);
                if (comparisonR->isNone()) {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::None();
                }
                else
                {
                    std::string op ;
                    op.push_back(comparisonR->value().first);
                    ExpNodePtr newExp = ExpNodePtr(new UnaryOpExp(comparisonR->remain().lineNum(),op, exp,comparisonR->value().second));
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::Some(std::make_pair(op.front(), newExp), comparisonR->remain());
                }
            };
            
        };
        auto x = Lex::Bind<ExpNodePtr, std::pair<char, ExpNodePtr>>(ParserFactor,bMulExp);
        auto y = x(inp);
        if (y->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            return Lex::LexResult<ExpNodePtr>::Some(y->value().second, y->remain());
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
        
        auto bAddExp =[addExpPaser](ExpNodePtr exp)-> typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Parser
        {
            return [addExpPaser,exp](const Lex::ParserStream& inp)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Result
            {
                auto comparisonR = addExpPaser(inp);
                if (comparisonR->isNone()) {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::None();
                }
                else
                {
                    std::string op ;
                    op.push_back(comparisonR->value().first);
                    ExpNodePtr newExp = ExpNodePtr(new UnaryOpExp(comparisonR->remain().lineNum(),op, exp,comparisonR->value().second));
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::Some(std::make_pair(op.front(), newExp), comparisonR->remain());
                }
            };
            
        };
        auto x = Lex::Bind<ExpNodePtr, std::pair<char, ExpNodePtr>>(ParserTerm,bAddExp);
        auto y = x(inp);
        if (y->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            return Lex::LexResult<ExpNodePtr>::Some(y->value().second, y->remain());
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
        auto bComparsionExp =[comparisonExp](ExpNodePtr exp)-> typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Parser
        {
            return [comparisonExp,exp](const Lex::ParserStream& inp)->typename Lex::ParserType<std::pair<char, ExpNodePtr>>::Result
            {
                auto comparisonR = comparisonExp(inp);
                if (comparisonR->isNone()) {
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::None();
                }
                else
                {
                    std::string op ;
                    op.push_back(comparisonR->value().first);
                    ExpNodePtr newExp = ExpNodePtr(new UnaryOpExp(comparisonR->remain().lineNum(),op, exp,comparisonR->value().second));
                    return Lex::LexResult<std::pair<char, ExpNodePtr>>::Some(std::make_pair(op.front(), newExp), comparisonR->remain());
                }
            };
            
        };
        auto x = Lex::Bind<ExpNodePtr, std::pair<char, ExpNodePtr>>(ParserSimpleExp,bComparsionExp);
        auto y = x(inp);
        if (y->isNone()) {
            return Lex::LexResult<ExpNodePtr>::None();
        }
        else
        {
            return Lex::LexResult<ExpNodePtr>::Some(y->value().second, y->remain());
        }
    
    }
}