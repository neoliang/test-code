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

std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentSeq(Parser::StatementSeqPtr stmtSeq)
{
    const auto& stmts = stmtSeq->getStatments();
    std::tuple<Val,ReturnState> r;
    for (auto iter = stmts.begin(); iter != stmts.end(); ++iter) {
        if (std::dynamic_pointer_cast<ReturnStatement>(*iter)) {
            return ExeStatmentReturn(std::static_pointer_cast<ReturnStatement>(*iter));
        }
        else
        {
            r = ExeStatment(*iter);
            if (std::get<1>(r) == Return_Early) {
                return r;
            }
        }
    }
    std::cout << "seq return state" << std::get<1>(r) << std::endl;
    return r;
}
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentReturn(Parser::ReturnStatementPtr stmt)
{
    auto exp = stmt->GetRetExp();
    if (exp != nullptr) {
        return std::make_tuple(EvalExp(exp),Return_Early);
    }
    return std::make_tuple(Val(),Return_Early);
}

#define TryExe(type,typeExe) if (std::dynamic_pointer_cast<type>(stmt)) {\
return typeExe(std::static_pointer_cast<type>(stmt));\
}
Val TinyRunTime::EvalFunCall(Parser::FunCallPtr exp)
{
    FunStatmentPtr fun = _currentEnv->GetVar(exp->GetIdentifier()).fun;
    PushEnv();
    auto iterExp = exp->GetExpList().begin();
    auto iterParam = fun->GetParams().begin();
    
    while (iterExp != exp->GetExpList().end() && iterParam != fun->GetParams().end()) {
        
        _currentEnv->SetLocalVar(*iterParam, EvalExp(*iterExp));
        ++iterExp;
        ++iterParam;
    }
    auto v = ExeStatmentSeq(fun->GetStmtSeq());
    
    
    PopEnv();
    return std::get<0>(v);
}
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentFun(Parser::FunStatmentPtr stmt)
{
    _currentEnv->SetLocalVar(stmt->GetName(), stmt);
    return (Val(stmt),Return_normal);
}
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatment(Parser::StatementNodePtr stmt)
{
    TryExe(ReadStatement, ExeStatmentRead)
    TryExe(WriteStatement, ExeStatmentWrite)
    TryExe(RepeatStatement, ExeStatmentRepeat)
    TryExe(IfStatement, ExeStatmentIf)
    TryExe(AssignStatement, ExeStatmentAssign)
    TryExe(FunStatment, ExeStatmentFun);
    throw std::bad_exception();
}
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentRepeat(Parser::RepeatStatementPtr stmt)
{
    auto stmtSeq = stmt->GetStmtSeq();
    std::tuple<Val,ReturnState> r;
    PushEnv();
    do
    {
        r = ExeStatmentSeq(stmtSeq);
        auto rs = std::get<1>(r);
        if (rs == Return_Early) {
            return r;
        }
        else if (rs ==  Return_break)
        {
            break;
        }
    }while (!EvalExp(stmt->GetExp()).b);
    PopEnv();
    return r;
}

std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentIf(Parser::IfStatementPtr stmt)
{
    Val e = EvalExp(stmt->GetExp());
    std::tuple<Val,ReturnState> r;
    if (e.b) {
        PushEnv();
        r = ExeStatmentSeq(stmt->GetThenStmt());
        PopEnv();
    }
    else if (stmt->GetElseStmt())
    {
        PushEnv();
        r = ExeStatmentSeq(stmt->GetElseStmt());
        PopEnv();
    }
    return r;
}
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentRead(Parser::ReadStatementPtr stmt)
{
    const std::string& id = stmt->GetIdentifier();
    int x =0;
    std::cin >> x;
    _currentEnv->SetVar(id, x);
    return std::make_tuple(Val(),Return_normal);
}
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentWrite(Parser::WriteStatementPtr stmt)
{
    Val v = EvalExp(stmt->GetExp());
    std::cout << v.i << std::endl;
    return std::make_tuple(Val(),Return_normal);
}
Val TinyRunTime::EvalUnary(Parser::UnaryExpPtr exp)
{
    const std::string& op = exp->GetOp();
    Val left = EvalExp(exp->GetLeftExp());
    Val right = EvalExp(exp->GetRightExp());
    if (op == "+") {
        return left.i + right.i;
    }
    else if (op == "-")
    {
        return left.i - right.i;
    }
    else if (op == "*")
    {
        return left.i * right.i;
    }
    else if (op == "/")
    {
        return left.i / right.i;
    }
    else if (op == "=")
    {
        return left.i == right.i ? 1 : 0;
    }
    else if (op == "<")
    {
        return left.i < right.i ? 1 : 0;
    }
    assert(false);
}
Val TinyRunTime::EvalConst(Parser::ConstExpPtr exp)
{
    return Val(exp->GetNumber());
}

Val TinyRunTime::EvalID(Parser::IdExpPtr exp)
{
    return _currentEnv->GetVar(exp->GetIdentifier());
}


Val TinyRunTime::EvalExp(Parser::ExpNodePtr exp)
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
std::tuple<Val,TinyRunTime::ReturnState> TinyRunTime::ExeStatmentAssign(Parser::AssignStatementPtr stmt)
{
    if (stmt->IsLocal()) {
        _currentEnv->SetLocalVar(stmt->GetIdentifier(), EvalExp(stmt->GetExp()));
    }
    else
    {
        _currentEnv->SetVar(stmt->GetIdentifier(), EvalExp(stmt->GetExp()));
    }
    return std::make_tuple(Val(),Return_normal);
}