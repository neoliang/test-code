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
    //idlist -> id {,id}
    Lex::ParserType<std::list<std::string>>::Result ParserIdList(const Lex::ParserStream& inp)
    {
        typedef std::list<std::string> StringListT;
        auto optionId = CONSF(std::string, TOKEN(","), _)
        CONS(std::string, Lex::idParser, id)
        RET((std::string)id);
        EndCONS;
        EndCONS;
        
        CONS(StringListT, Lex::idParser, x)
        CONS(StringListT, Lex::Many<std::string>(optionId), xs)
        auto all = std::list<std::string>();
        all.push_back(x);
        all.insert(all.end(), xs.begin(),xs.end());
        RET(all);
        EndCONS;
        EndCONS(inp);
    }
    //explist -> exp {,exp}
    Lex::ParserType<std::list<ExpNodePtr>>::Result ParserExpList(const Lex::ParserStream& inp)
    {
        typedef std::list<ExpNodePtr> EList;
        auto optionId = CONSF(ExpNodePtr, TOKEN(","), _)
        CONS(ExpNodePtr, ParserExp, exp)
        RET((ExpNodePtr)exp);
        EndCONS;
        EndCONS;
        
        CONS(EList, ParserExp, x)
        CONS(EList, Lex::Many<ExpNodePtr>(optionId), xs)
        auto all = std::list<ExpNodePtr>();
        all.push_back(x);
        all.insert(all.end(), xs.begin(),xs.end());
        RET(all);
        EndCONS;
        EndCONS(inp);
    }
    //functioncall -> identifier (explist)
    Lex::ParserType<ExpNodePtr>::Result ParserFunCall(const Lex::ParserStream& inp)
    {
        auto optionExpList = Lex::Option<std::list<ExpNodePtr>>(ParserExpList);
        CONS(ExpNodePtr, Lex::idParser, id)
        CONS(ExpNodePtr, TOKEN("("), _)
        CONS(ExpNodePtr, optionExpList, es)
        CONS(ExpNodePtr, TOKEN(")"), _)
        RET(ExpNodePtr(new FunCall(inp.lineNum(),id,es)));
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS(inp);
    }
#define ContinueWithSt(e1,ret) CONS(StatementNodePtr,e1,ret)
    //function-stmt -> identifier (idlist) stmt-seq [return exp] end
    Lex::ParserType<StatementNodePtr>::Result ParserFunStatement(const Lex::ParserStream& inp)
    {
        auto optionIdList = Lex::Option<std::list<std::string>>(ParserIdList);
        auto parserReturn = CONSF(ExpNodePtr, TOKEN("return"), _)
        CONS(ExpNodePtr, ParserExp, retExp);
        RET((ExpNodePtr)retExp);
        EndCONS;
        EndCONS;
        auto optionRet = Lex::Option<ExpNodePtr>(parserReturn);
        
        ContinueWithSt(TOKEN("fun"), _)
        ContinueWithSt(Lex::idParser, fname)
        ContinueWithSt(TOKEN("("), _)
        ContinueWithSt(optionIdList, params)
        ContinueWithSt(TOKEN(")"), _)
        ContinueWithSt(ParserStatementSeq, stmtSeq)
        ContinueWithSt(optionRet, retExp)
        ContinueWithSt(TOKEN("end"), _)
        RET(StatementNodePtr(new FunStatment(stmtSeq->LineNo(),fname,params,stmtSeq,retExp) ));
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS;
        EndCONS(inp);
    }
    


    /*
     if exp then stmt-seq {else stmt-sequence} end
     */
    Lex::ParserType<StatementNodePtr>::Result ParserIfStatment(const Lex::ParserStream& inp)
    {
        auto elsePart  = CONSF(StatementSeqPtr,TOKEN("else") , _) return ParserStatementSeq; EndCONS;
        auto optionElse = Lex::Option<StatementSeqPtr>(elsePart) ;
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
        
        auto optionPart = Lex::Option<std::string>(Lex::Token<std::string>(Lex::charsParser(';')));
        auto statmentParser = CONSF(StatementNodePtr, ParserStatement,stmt)
        CONS(StatementNodePtr, optionPart, _)
        RET((StatementNodePtr)stmt);
        EndCONS;
        EndCONS;
        auto statmentsParser =  Lex::Many<StatementNodePtr>(statmentParser);
        CONS(StatementSeqPtr, statmentsParser, statementsList)
        auto slist = std::vector<StatementNodePtr>(statementsList.begin(),statementsList.end());
        auto seq = StatementSeqPtr(new StatementSeq(inp.lineNum(),slist));
        RET(seq);
        EndCONS(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserStatement(const Lex::ParserStream& inp)
    {
        return Lex::ChooseN<StatementNodePtr>({ParserRead,ParserWrite,ParserAssignment,ParserIfStatment,ParserRepeatStatement,ParserFunStatement})(inp);
    }
    
    Lex::ParserType<StatementNodePtr>::Result ParserAssignment(const Lex::ParserStream& inp)
    {
        auto localId = Lex::Token<std::string>(Lex::strParser("local"));
        auto optionLocal = Lex::Option<std::string>(localId);
        ContinueWithSt(optionLocal, _l)
        ContinueWithSt(Lex::idParser , id)
        ContinueWithSt(TOKEN(":="), _)
        ContinueWithSt( ParserExp, exp)
        bool isLocal = _l == "local";
        RET(StatementNodePtr(new AssignStatement(inp.lineNum(),id,exp,isLocal)));
        EndCONS;
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
        return Lex::ChooseN<ExpNodePtr>({ParserFunCall, ParserConst,ParserIdentifier,fexp}) (inp);
    }
    
    Lex::ParserType<ExpNodePtr>::Parser _ParserUnary(const std::list<char>& ops,const Lex::ParserType<ExpNodePtr>::Parser& parser)
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
        EndCONS;
    }
    Lex::ParserType<ExpNodePtr>::Result ParserTerm(const Lex::ParserStream& inp)
    {
        return _ParserUnary({'*','/'}, ParserFactor)(inp);
    }
    
    Lex::ParserType<ExpNodePtr>::Result ParserSimpleExp(const Lex::ParserStream& inp)
    {
        return _ParserUnary({'+','-'}, ParserTerm)(inp);
    }
    
    Lex::ParserType<ExpNodePtr>::Result ParserExp(const Lex::ParserStream &inp)
    {
        return _ParserUnary({'=','<'}, ParserSimpleExp)(inp);
    }
}