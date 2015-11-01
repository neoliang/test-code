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
    Lex::ParserType<ExpNodePtr>::Result ParserExp(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserSimpleExp(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserTerm(const Lex::ParserStream& inp);
    Lex::ParserType<ExpNodePtr>::Result ParserFactor(const Lex::ParserStream& inp);
}

#endif /* defined(__Compiler__TinyParser__) */
