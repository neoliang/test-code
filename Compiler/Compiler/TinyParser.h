//
//  TinyParser.h
//  Compiler
//
//  Created by neo on 15-10-26.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef __Compiler__TinyParser__
#define __Compiler__TinyParser__

#include "Lex.h"
#include "TinySyntax.h"
#include <memory>
namespace Parser {
   
    
    
    StatementSeqPtr Parser(const Lex::ParserStream& inp);
    ExpNodePtr Exp(const Lex::ParserStream& inp);
    
    //helper
    //exp
    Lex::ParserType<ExpNodePtr>::Result ParserExp(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserSimpleExp(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserTerm(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserFactor(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserFunCall(const Lex::ParserStream& inp);
    
    //statement
    Lex::ParserType<StatementNodePtr>::Result ParserRead(const Lex::ParserStream& inp);
    Lex::ParserType<StatementNodePtr>::Result ParserWrite(const Lex::ParserStream& inp);
    Lex::ParserType<StatementNodePtr>::Result ParserAssignment(const Lex::ParserStream& inp);
    Lex::ParserType<StatementNodePtr>::Result ParserIfStatment(const Lex::ParserStream& inp);
    Lex::ParserType<StatementNodePtr>::Result ParserRepeatStatement(const Lex::ParserStream& inp);
    Lex::ParserType<StatementNodePtr>::Result ParserFunStatement(const Lex::ParserStream& inp);
    
    Lex::ParserType<StatementNodePtr>::Result ParserStatement(const Lex::ParserStream& inp);
    Lex::ParserType<StatementSeqPtr>::Result ParserStatementSeq(const Lex::ParserStream& inp);
    Lex::ParserType<StatementSeqPtr>::Parser StmtListToSeq(const Lex::ParserType<std::list<StatementNodePtr>>::Parser& p);
}

#endif /* defined(__Compiler__TinyParser__) */
