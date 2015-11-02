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
    
    /*
     if exp then stmt-seq {else stmt-sequence} end
     */
#define TOKEN(t) Lex::Token<std::string>(Lex::strParser(t))
#define Continues(T1,e1,r1) return Lex::Bind<T1, StatementNodePtr>(e1,[&](const T1& r1)->typename Lex::ParserType<StatementNodePtr>::Parser{
#define EndContinues })
    Lex::ParserType<StatementNodePtr>::Result ParserIfStatment(const Lex::ParserStream& inp)
    {
        Continues(std::string,TOKEN("if"),_)
        Continues(ExpNodePtr,ParserExp,exp)
        Continues(std::string,TOKEN("then"),_)
        Continues(StatementSeqPtr,ParserStatementSeq,stmtSeq)
        
        auto optionElse = Lex::Choice<StatementSeqPtr>(Lex::Bind<std::string, StatementSeqPtr>(TOKEN("else"), [](const std::string&)->typename Lex::ParserType<StatementSeqPtr>::Parser{
            return ParserStatementSeq;
        }), Lex::ParserType<StatementSeqPtr>::ret(nullptr)) ;
        Continues(StatementSeqPtr, optionElse, elseExp)
        return [&](const Lex::ParserStream& inp)->Lex::ParserType<StatementNodePtr>::Result{
            auto rend = TOKEN("end")(inp);
            if(rend->isNone())
            {
                return Lex::LexResult<StatementNodePtr>::None();
            }
            else
            {
                auto ifStmt = IfStatementPtr(new IfStatement(rend->remain().lineNum(), exp,stmtSeq,elseExp));
                return Lex::LexResult<StatementNodePtr>::Some(ifStmt, rend->remain());
            }
        };
        EndContinues;
        EndContinues;
        EndContinues;
        EndContinues;
        EndContinues(inp);
    }
    
    /*
     repeate stmt-seq until exp
     */


    Lex::ParserType<StatementNodePtr>::Result ParserRepeatStatement(const Lex::ParserStream& inp)
    {
        Continues(std::string, TOKEN("repeat"), _)
        Continues(StatementSeqPtr, ParserStatementSeq, stmtSeq)
        Continues(std::string, TOKEN("until"), _)
        return [&](const Lex::ParserStream& inp)->Lex::ParserType<StatementNodePtr>::Result{
            auto rExp = ParserExp(inp);
            if (rExp->isNone()) {
                return Lex::LexResult<StatementNodePtr>::None();
            }
            else
            {
                auto repStmt = RepeatStatementPtr(new RepeatStatement(rExp->remain().lineNum(),rExp->value(),stmtSeq));
                return Lex::LexResult<StatementNodePtr>::Some(repStmt, rExp->remain());
            }
        };
        EndContinues;
        EndContinues;
        EndContinues(inp);
    }
    
    Lex::ParserType<StatementSeqPtr>::Result ParserStatementSeq(const Lex::ParserStream& inp)
    {
        return Lex::Bind<StatementNodePtr, StatementSeqPtr>(ParserStatement, [](StatementNodePtr stmt)->typename Lex::ParserType<StatementSeqPtr>::Parser{
            
            auto optionPart = Lex::Bind<std::string, StatementNodePtr>(Lex::Token<std::string>(Lex::charsParser(';')), [](const std::string&)->typename Lex::ParserType<StatementNodePtr>::Parser{
                return ParserStatement;
            });
            auto optioPartList = Lex::Many<StatementNodePtr>(optionPart);
            
            return [optioPartList,stmt](const Lex::ParserStream& inp)->typename Lex::ParserType<StatementSeqPtr>::Result{
                auto r = optioPartList(inp);
                auto stmtseq = StatementSeqPtr(new StatementSeq(inp.lineNum()));
                stmtseq->AddStatement(stmt);
                if (r->isNone()) {
                    return Lex::LexResult<StatementSeqPtr>::Some(stmtseq, inp);
                }
                else
                {
                    for (auto iter = r->value().begin(); iter != r->value().end(); ++iter) {
                        stmtseq->AddStatement(*iter);
                    }
                    return Lex::LexResult<StatementSeqPtr>::Some(stmtseq, r->remain());
                }
            };
            
        })(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserStatement(const Lex::ParserStream& inp)
    {
        return Lex::ChooseN<StatementNodePtr>({ParserRead,ParserWrite,ParserAssignment,ParserIfStatment,ParserRepeatStatement})(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserAssignment(const Lex::ParserStream& inp)
    {
        return Lex::Bind<std::string, StatementNodePtr>(Lex::idParser, [](const std::string& id)->typename Lex::ParserType<StatementNodePtr>::Parser{
            return Lex::Bind<std::string, StatementNodePtr>(TOKEN(":="),[id](const std::string& )->typename Lex::ParserType<StatementNodePtr>::Parser{
                return [id](const Lex::ParserStream& inp)->typename Lex::ParserType<StatementNodePtr>::Result{
                    auto r = ParserExp(inp);
                    if (r->isNone()) {
                        return Lex::LexResult<StatementNodePtr>::None();
                    }
                    else
                    {
                        auto assinState = AssignStatementPtr(new AssignStatement(r->remain().lineNum(),id,r->value()));
                        return Lex::LexResult<StatementNodePtr>::Some(assinState, r->remain());
                    }
                };
            });

            
        })(inp);
    }

    Lex::ParserType<StatementNodePtr>::Result ParserRead(const Lex::ParserStream& inp)
    {
        auto _paser = Lex::Bind<std::string, StatementNodePtr>(Lex::Token<std::string>(Lex::strParser("read")), [](const std::string&)->typename Lex::ParserType<StatementNodePtr>::Parser{
            return [](const Lex::ParserStream& inp)->typename Lex::ParserType<StatementNodePtr>::Result{
                auto id = Lex::idParser(inp);
                if (id->isNone()) {
                    return Lex::LexResult<StatementNodePtr>::None();
                }
                else
                {
                    auto readState =  ReadStatementPtr(new ReadStatement(id->remain().lineNum(),id->value()));
                    return Lex::LexResult<StatementNodePtr>::Some(readState, id->remain());
                }
            };
        });
        return _paser(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserWrite(const Lex::ParserStream& inp)
    {
        auto _paser = Lex::Bind<std::string, StatementNodePtr>(Lex::Token<std::string>(Lex::strParser("write")), [](const std::string&)->typename Lex::ParserType<StatementNodePtr>::Parser{
            return [](const Lex::ParserStream& inp)->typename Lex::ParserType<StatementNodePtr>::Result{
                auto id = ParserExp(inp);
                if (id->isNone()) {
                    return Lex::LexResult<StatementNodePtr>::None();
                }
                else
                {
                    auto writeState =  WriteStatementPtr(new WriteStatement(id->remain().lineNum(),id->value()));
                    return Lex::LexResult<StatementNodePtr>::Some(writeState, id->remain());
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
            ss << dig->value();
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
            return Lex::LexResult<ExpNodePtr>::Some(ExpNodePtr(new IdExp(dig->remain().lineNum(),dig->value())), dig->remain());
        }
    }
    
    
    Lex::ParserType<ExpNodePtr>::Result ParserFactor(const Lex::ParserStream& inp)
    {
        auto lexp = Lex::Bind<char, ExpNodePtr>(Lex::Token<char>(Lex::satParser([](char c){return c== '(';})),
            [](char )->typename Lex::ParserType<ExpNodePtr>::Parser{
                return ParserExp;
            }
        );
        auto fexp = Lex::Bind<ExpNodePtr, ExpNodePtr>(lexp, [](ExpNodePtr exp)->typename Lex::ParserType<ExpNodePtr>::Parser{
            return [exp](const Lex::ParserStream& inp)->typename Lex::ParserType<ExpNodePtr>::Result
            {
                auto rexp = Lex::Token<char>(Lex::satParser([](char c){return c== ')';}))(inp);
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