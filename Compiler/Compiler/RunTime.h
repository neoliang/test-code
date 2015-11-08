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
        
    }
    
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
    
};
class TinyRunTime
{
    std::map<std::string, int> _vars;
    
    int EvalNumberExp(Parser::ExpNodePtr exp);
    int EvalUnary(Parser::UnaryExpPtr exp);
    int EvalConst(Parser::ConstExpPtr exp);
    int EvalID(Parser::IdExpPtr exp);
    
    void ExeStatmentAssign(Parser::AssignStatementPtr stmt);
    void ExeStatmentIf(Parser::IfStatementPtr stmt);
    void ExeStatmentRepeat(Parser::RepeatStatementPtr stmt);
    void ExeStatmentWrite(Parser::WriteStatementPtr stmt);
    void ExeStatmentRead(Parser::ReadStatementPtr stmt);
    void ExeStatment(Parser::StatementNodePtr stmt);
    
public:
    void ExeStatmentSeq(Parser::StatementSeqPtr stmtSeq);
};

#endif /* defined(__Compiler__RunTime__) */
