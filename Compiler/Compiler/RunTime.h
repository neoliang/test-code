//
//  RunTime.h
//  Compiler
//
//  Created by neo on 15-11-3.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef __Compiler__RunTime__
#define __Compiler__RunTime__

#include <map>
#include <iostream>
#include "TinySyntax.h"

struct Val{
    union{
        int i = 0;
        bool b ;
    };
    Parser::FunStatmentPtr fun = nullptr;
    enum {
        INT,
        Fun,
        BOOL,
        Nil,
    } t;
    Val()
    :t(Nil)
    {}
    Val(bool v)
    :b(v)
    ,t(BOOL)
    {}
    Val(int v)
    :i(v)
    ,t(INT){}
    Val(const Parser::FunStatmentPtr& f)
    :fun(f),t(Fun){}
    
    
};
class Env
{
    std::map<std::string,Val> _vars;
    std::shared_ptr<Env> _pre;
    
public:
    Env(std::shared_ptr<Env> pre = nullptr)
    :_pre(pre){
        //std::cout << "Env()" << std::endl;
    }
    Env(const Env&) = delete;
    const Env& operator = (const Env&) = delete;
    
    void SetLocalVar(const std::string& key,Val v)
    {
        _vars[key] = v;
    }
    void SetVar(const std::string& key,Val v)
    {
        if (_pre == nullptr) {
            SetLocalVar(key, v);
        }
        
        auto iter = _vars.find(key);
        if (iter != _vars.end()) {
            iter->second = v;
        }
        else
        {
            _pre->SetVar(key, v);
        }
    }
    Val GetVar(const std::string& key)
    {
        auto iter = _vars.find(key);
        if (iter != _vars.end()) {
            return iter->second;
        }
        else if (_pre != nullptr)
        {
            return _pre->GetVar(key);
        }
        throw std::bad_exception();
    }
    
    std::shared_ptr<Env> PreEnv()
    {
        return _pre;
    }
    ~Env()
    {
        //std::cout << "~Env()" << std::endl;
    }
};

typedef std::shared_ptr<Env> EnvPtr;
class TinyRunTime
{
public:
    enum ReturnState{
        Return_normal,
        Return_Early,
        Return_break,
    };
private:
    EnvPtr _currentEnv = nullptr;
    
    void PushEnv()
    {
        _currentEnv = EnvPtr(new Env(_currentEnv));
    }
    void PopEnv()
    {
        _currentEnv = _currentEnv->PreEnv();
    }
    
    Val EvalExp(Parser::ExpNodePtr exp);
    Val EvalUnary(Parser::UnaryExpPtr exp);
    Val EvalConst(Parser::ConstExpPtr exp);
    Val EvalID(Parser::IdExpPtr exp);
    Val EvalFunCall(Parser::FunCallPtr exp);
    
    
    std::tuple<Val,ReturnState> ExeStatmentAssign(Parser::AssignStatementPtr stmt);
    std::tuple<Val,ReturnState> ExeStatmentIf(Parser::IfStatementPtr stmt);
    std::tuple<Val,ReturnState> ExeStatmentRepeat(Parser::RepeatStatementPtr stmt);
    std::tuple<Val,ReturnState> ExeStatmentWrite(Parser::WriteStatementPtr stmt);
    std::tuple<Val,ReturnState> ExeStatmentRead(Parser::ReadStatementPtr stmt);
    std::tuple<Val,ReturnState> ExeStatmentFun(Parser::FunStatmentPtr stmt);
    std::tuple<Val,ReturnState> ExeStatment(Parser::StatementNodePtr stmt);
    std::tuple<Val,ReturnState> ExeStatmentReturn(Parser::ReturnStatementPtr stmt);
    
public:

    TinyRunTime()
    {
        PushEnv();
    }
    ~TinyRunTime()
    {
        PopEnv();
    }
    std::tuple<Val,TinyRunTime::ReturnState> ExeStatmentSeq(Parser::StatementSeqPtr stmtSeq);
};

#endif /* defined(__Compiler__RunTime__) */
