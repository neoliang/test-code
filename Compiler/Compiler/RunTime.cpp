//
//  RunTime.cpp
//  Compiler
//
//  Created by neo on 15-11-3.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "RunTime.h"
#include <assert.h>
#include <iostream>
using namespace Parser;

void TinyRunTime::ExeStatmentSeq(Parser::StatementSeqPtr stmtSeq)
{
    stmtSeq->foreach([this](Parser::StatementNodePtr stmt){
        this->ExeStatment(stmt);
    });
}

#define TryExe(type,typeExe) if (std::dynamic_pointer_cast<type>(stmt)) {\
typeExe(std::static_pointer_cast<type>(stmt));\
return;\
}
void TinyRunTime::ExeStatment(Parser::StatementNodePtr stmt)
{
    TryExe(ReadStatement, ExeStatmentRead)
    TryExe(WriteStatement, ExeStatmentWrite)
    TryExe(RepeatStatement, ExeStatmentRepeat)
    TryExe(IfStatement, ExeStatmentIf)
    TryExe(AssignStatement, ExeStatmentAssign)
}
void TinyRunTime::ExeStatmentRepeat(Parser::RepeatStatementPtr stmt)
{
    auto stmtSeq = stmt->GetStmtSeq();
    do
    {
        ExeStatmentSeq(stmtSeq);
    }while (EvalNumberExp(stmt->GetExp()) == 0);
}

void TinyRunTime::ExeStatmentIf(Parser::IfStatementPtr stmt)
{
    int r = EvalNumberExp(stmt->GetExp());
    if (r != 0) {
        ExeStatmentSeq(stmt->GetThenStmt());
    }
    else if (stmt->GetElseStmt())
    {
        ExeStatmentSeq(stmt->GetElseStmt());
    }
}
void TinyRunTime::ExeStatmentRead(Parser::ReadStatementPtr stmt)
{
    const std::string& id = stmt->GetIdentifier();
    int x =0;
    std::cin >> x;
    _vars[id] = x;
    
}
void TinyRunTime::ExeStatmentWrite(Parser::WriteStatementPtr stmt)
{
    int v = EvalNumberExp(stmt->GetExp());
    std::cout << v << std::endl;
}
int TinyRunTime::EvalUnary(Parser::UnaryExpPtr exp)
{
    const std::string& op = exp->GetOp();
    int left = EvalNumberExp(exp->GetLeftExp());
    int right = EvalNumberExp(exp->GetRightExp());
    if (op == "+") {
        return left + right;
    }
    else if (op == "-")
    {
        return left - right;
    }
    else if (op == "*")
    {
        return left * right;
    }
    else if (op == "/")
    {
        return left / right;
    }
    else if (op == "=")
    {
        return left == right ? 1 : 0;
    }
    else if (op == "<")
    {
        return left < right ? 1 : 0;
    }
    assert(false);
}
int TinyRunTime::EvalConst(Parser::ConstExpPtr exp)
{
    return exp->GetNumber();
}

int TinyRunTime::EvalID(Parser::IdExpPtr exp)
{
    auto iter = _vars.find(exp->GetIdentifier());
    if (iter != _vars.end()) {
        return iter->second;
    }
    throw std::bad_exception();
}


int TinyRunTime::EvalNumberExp(Parser::ExpNodePtr exp)
{
#define EVAL_EXP(Exp,eval) if (std::dynamic_pointer_cast<Exp>(exp)) {\
        return eval(std::static_pointer_cast<Exp>(exp));\
    }
    EVAL_EXP(UnaryOpExp,EvalUnary)
    EVAL_EXP(ConstExp,EvalConst)
    EVAL_EXP(IdExp, EvalID)

    throw std::bad_exception();
}
void TinyRunTime::ExeStatmentAssign(Parser::AssignStatementPtr stmt)
{
    _vars[stmt->GetIdentifier()] = EvalNumberExp(stmt->GetExp());
}