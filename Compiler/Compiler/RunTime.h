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
