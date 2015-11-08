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
    int i = 0;
    Parser::FunStatmentPtr fun = nullptr;
    enum {
        INT,
        Fun,
        Nil,
    } t;
    Val()
    :t(Nil)
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
    EnvPtr _currentEnv = nullptr;
    
    void PushEnv()
    {
        _currentEnv = EnvPtr(new Env(_currentEnv));
    }
    void PopEnv()
    {
        _currentEnv = _currentEnv->PreEnv();
    }
    
    int EvalNumberExp(Parser::ExpNodePtr exp);
    int EvalUnary(Parser::UnaryExpPtr exp);
    int EvalConst(Parser::ConstExpPtr exp);
    int EvalID(Parser::IdExpPtr exp);
    int EvalFunCall(Parser::FunCallPtr exp);
    void ExeStatmentAssign(Parser::AssignStatementPtr stmt);
    void ExeStatmentIf(Parser::IfStatementPtr stmt);
    void ExeStatmentRepeat(Parser::RepeatStatementPtr stmt);
    void ExeStatmentWrite(Parser::WriteStatementPtr stmt);
    void ExeStatmentRead(Parser::ReadStatementPtr stmt);
    void ExeStatmentFun(Parser::FunStatmentPtr stmt);
    void ExeStatment(Parser::StatementNodePtr stmt);
    
    
public:
    
    TinyRunTime()
    {
        PushEnv();
    }
    ~TinyRunTime()
    {
        PopEnv();
    }
    void ExeStatmentSeq(Parser::StatementSeqPtr stmtSeq);
};

#endif /* defined(__Compiler__RunTime__) */
