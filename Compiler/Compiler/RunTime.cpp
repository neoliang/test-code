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
int TinyRunTime::EvalFunCall(Parser::FunCallPtr exp)
{
    FunStatmentPtr fun = _currentEnv->GetVar(exp->GetIdentifier()).fun;
    PushEnv();
    auto iterExp = exp->GetExpList().begin();
    auto iterParam = fun->GetParams().begin();
    
    while (iterExp != exp->GetExpList().end() && iterParam != fun->GetParams().end()) {
        
        _currentEnv->SetLocalVar(*iterParam, EvalNumberExp(*iterExp));
        ++iterExp;
        ++iterParam;
    }
    ExeStatmentSeq(fun->GetStmtSeq());
    
    int n = EvalNumberExp(fun->GetRetExp());
    
    PopEnv();
    return n;
}
void TinyRunTime::ExeStatmentFun(Parser::FunStatmentPtr stmt)
{
    _currentEnv->SetLocalVar(stmt->GetName(), stmt);
}
void TinyRunTime::ExeStatment(Parser::StatementNodePtr stmt)
{
    TryExe(ReadStatement, ExeStatmentRead)
    TryExe(WriteStatement, ExeStatmentWrite)
    TryExe(RepeatStatement, ExeStatmentRepeat)
    TryExe(IfStatement, ExeStatmentIf)
    TryExe(AssignStatement, ExeStatmentAssign)
    TryExe(FunStatment, ExeStatmentFun);
}
void TinyRunTime::ExeStatmentRepeat(Parser::RepeatStatementPtr stmt)
{
    auto stmtSeq = stmt->GetStmtSeq();
    PushEnv();
    do
    {
        ExeStatmentSeq(stmtSeq);
    }while (EvalNumberExp(stmt->GetExp()) == 0);
    PopEnv();
}

void TinyRunTime::ExeStatmentIf(Parser::IfStatementPtr stmt)
{
    int r = EvalNumberExp(stmt->GetExp());
    if (r != 0) {
        PushEnv();
        ExeStatmentSeq(stmt->GetThenStmt());
        PopEnv();
    }
    else if (stmt->GetElseStmt())
    {
        PushEnv();
        ExeStatmentSeq(stmt->GetElseStmt());
        PopEnv();
    }
}
void TinyRunTime::ExeStatmentRead(Parser::ReadStatementPtr stmt)
{
    const std::string& id = stmt->GetIdentifier();
    int x =0;
    std::cin >> x;
    _currentEnv->SetVar(id, x);
    
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
    return _currentEnv->GetVar(exp->GetIdentifier()).i;
}


int TinyRunTime::EvalNumberExp(Parser::ExpNodePtr exp)
{
#define EVAL_EXP(Exp,eval) if (std::dynamic_pointer_cast<Exp>(exp)) {\
        return eval(std::static_pointer_cast<Exp>(exp));\
    }
    EVAL_EXP(UnaryOpExp,EvalUnary)
    EVAL_EXP(ConstExp,EvalConst)
    EVAL_EXP(IdExp, EvalID)
    EVAL_EXP(FunCall, EvalFunCall);
    throw std::bad_exception();
}
void TinyRunTime::ExeStatmentAssign(Parser::AssignStatementPtr stmt)
{
    if (stmt->IsLocal()) {
        _currentEnv->SetLocalVar(stmt->GetIdentifier(), EvalNumberExp(stmt->GetExp()));
    }
    else
    {
        _currentEnv->SetVar(stmt->GetIdentifier(), EvalNumberExp(stmt->GetExp()));
    }
}