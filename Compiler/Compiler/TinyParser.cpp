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
#include "Util.h"
namespace Parser
{
    
    /*
     if exp then stmt-seq {else stmt-sequence} end
     */
#define TOKEN(t) Lex::Token<std::string>(Lex::strParser(t))

#define ContinueWithSt(e1,ret) CONS(StatementNodePtr,e1,ret)

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
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS(inp);
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
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS(inp);
    }
    
    Lex::ParserType<StatementSeqPtr>::Result ParserStatementSeq(const Lex::ParserStream& inp)
    {
        auto optionPart = CONSF(StatementNodePtr,Lex::Token<std::string>(Lex::charsParser(';')) , _)
            return ParserStatement;
        EndCONS;
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
        CONS(ExpNodePtr, Lex::digitsParser, dig)
        RET(ExpNodePtr(new ConstExp(inp.lineNum(),util::StrTot<int>(dig))));
        EndCONS(inp);
    }
    Lex::ParserType<ExpNodePtr>::Result ParserIdentifier(const Lex::ParserStream& inp)
    {
        CONS(ExpNodePtr, Lex::idParser, id)
        RET(ExpNodePtr(new IdExp(inp.lineNum(),id)));
        EndCONS(inp);
    }
    
    
    Lex::ParserType<ExpNodePtr>::Result ParserFactor(const Lex::ParserStream& inp)
    {
        auto fexp = CONSF(ExpNodePtr,Lex::Token<char>(Lex::charParser('(')) , _)
        CONS(ExpNodePtr,ParserExp,exp)
        CONS(ExpNodePtr, Lex::Token<char>(Lex::charParser(')')), _)
        RET((ExpNodePtr)exp);
        EndCONS;
        EndCONS;
        EndCONS;
        return Lex::ChooseN<ExpNodePtr>({ParserConst,ParserIdentifier,fexp}) (inp);
    }
    
    Lex::ParserType<ExpNodePtr>::Result _ParserUnary(const Lex::ParserStream& inp,const std::list<char>& ops,const Lex::ParserType<ExpNodePtr>::Parser& parser)
    {
        auto mulOpParser = Lex::satParser([&](char c)->bool{
            for (auto iter = ops.begin(); iter != ops.end(); ++iter) {
                if (*iter == c) {
                    return true;
                }
            }
            return false;
        });
        
        typedef std::pair<char, ExpNodePtr> opExpT;
        auto bMulOpExp = CONSF(opExpT, mulOpParser, op)
        CONS(opExpT, parser, exp)
        RET(std::make_pair(op,exp));
        EndCONS;
        EndCONS;
        auto manyMulExpParser = Lex::Many<opExpT>(bMulOpExp);
        CONS(ExpNodePtr, parser, factor)
        CONS(ExpNodePtr,manyMulExpParser, addfactorList)
        ExpNodePtr opExp = factor;
        for (auto iter = addfactorList.begin(); iter != addfactorList.end(); ++iter) {
            std::string op ;
            op.push_back(iter->first);
            opExp = ExpNodePtr(new UnaryOpExp(opExp->LineNo(),op, opExp,iter->second));
        }
        RET(opExp);
        EndCONS;
        EndCONS(inp);
    }
    Lex::ParserType<ExpNodePtr>::Result ParserTerm(const Lex::ParserStream& inp)
    {
        return _ParserUnary(inp, {'*','/'}, ParserFactor);
    }
    
    Lex::ParserType<ExpNodePtr>::Result ParserSimpleExp(const Lex::ParserStream& inp)
    {
        return _ParserUnary(inp, {'+','-'}, ParserTerm);
    }
    
    Lex::ParserType<ExpNodePtr>::Result ParserExp(const Lex::ParserStream &inp)
    {
        return _ParserUnary(inp, {'=','<'}, ParserSimpleExp);
    }
}