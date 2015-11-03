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
#define EndContinues })

#define ContinueWithSt(e1,ret) CONS(StatementNodePtr,e1,ret)
    /*
    let _ <- Token("if")
    let exp <- ParserExp
    let _ <- Token("then")
    let stmtSeq <- ParserStatementSeq
    let elseExp <- optionElse
    let _ <- Token("end")
      return IfStatement(exp->LineNo(), exp,stmtSeq,elseExp))
    */
    Lex::ParserType<StatementNodePtr>::Result ParserIfStatment(const Lex::ParserStream& inp)
    {
        auto elsePart  = CONSF(StatementSeqPtr,TOKEN("else") , _) return ParserStatementSeq; EndCONS;
        auto optionElse = CHOICE(elsePart, RETF(StatementSeqPtr(nullptr))) ;
        ContinueWithSt(TOKEN("if"),_)
        ContinueWithSt(ParserExp,exp)
        ContinueWithSt(TOKEN("then"),_)
        ContinueWithSt(ParserStatementSeq,stmtSeq)
        ContinueWithSt( optionElse, elseExp)
        ContinueWithSt(TOKEN("end") , _)
        RET(StatementNodePtr(new IfStatement(exp->LineNo(), exp,stmtSeq,elseExp)));
        EndContinues;
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
        ContinueWithSt(TOKEN("repeat"), _)
        ContinueWithSt(Parser::ParserStatementSeq, stmtSeq)
        ContinueWithSt(TOKEN("until"), _)
        ContinueWithSt(Parser::ParserExp, rExp)
        RET(StatementNodePtr(new RepeatStatement(stmtSeq->LineNo(),rExp,stmtSeq)));
        EndContinues;
        EndContinues;
        EndContinues;
        EndContinues(inp);
    }
    
    Lex::ParserType<StatementSeqPtr>::Result ParserStatementSeq(const Lex::ParserStream& inp)
    {
        auto optionPart = CONSF(StatementNodePtr,Lex::Token<std::string>(Lex::charsParser(';')) , _)
        return ParserStatement; EndCONS;
        auto optioPartList =  Lex::Many<StatementNodePtr>(optionPart);
        CONS(StatementSeqPtr, ParserStatement, stmt)
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
            
        EndCONS(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserStatement(const Lex::ParserStream& inp)
    {
        return Lex::ChooseN<StatementNodePtr>({ParserRead,ParserWrite,ParserAssignment,ParserIfStatment,ParserRepeatStatement})(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserAssignment(const Lex::ParserStream& inp)
    {
        ContinueWithSt(Lex::idParser , id)
        ContinueWithSt(TOKEN(":="), _)
        ContinueWithSt( ParserExp, exp)
        RET(StatementNodePtr(new AssignStatement(inp.lineNum(),id,exp)));
        EndCONS;
        EndCONS;
        EndCONS(inp);
    }

    Lex::ParserType<StatementNodePtr>::Result ParserRead(const Lex::ParserStream& inp)
    {
        ContinueWithSt(Lex::Token<std::string>(Lex::strParser("read")) , _)
        ContinueWithSt( Lex::idParser, id)
        RET(StatementNodePtr(new ReadStatement(inp.lineNum(),id)));
        EndCONS;
        EndCONS(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserWrite(const Lex::ParserStream& inp)
    {
        ContinueWithSt( Lex::Token<std::string>(Lex::strParser("write")), _)
        ContinueWithSt( ParserExp, exp)
        RET(StatementNodePtr(new WriteStatement(inp.lineNum(),exp)));
        EndCONS;
        EndCONS(inp);
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