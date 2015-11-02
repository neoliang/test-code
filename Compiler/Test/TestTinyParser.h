//
//  TestTinyParser.h
//  Compiler
//
//  Created by neo on 15-10-30.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef Compiler_TestTinyParser_h
#define Compiler_TestTinyParser_h
#include "TinyParser.h"
#include <sstream>

template<typename T>
std::string toString(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}


inline void TestTinyParser()
{
    //number
    
    rc::check("number",[](const int& n){
        int number = abs(n);
        auto str = toString(number);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::Exp(stream);
        auto uexp = std::dynamic_pointer_cast<Parser::ConstExp>(exp);
        RC_ASSERT(uexp != nullptr);
        RC_ASSERT(uexp->GetNumber() == number);
    });
    
    rc::check("(number)",[](const int& n){
        int number = abs(n);
        auto str = "(" + toString(number) + ")";
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::Exp(stream);
        auto uexp = std::dynamic_pointer_cast<Parser::ConstExp>(exp);
        RC_ASSERT(uexp != nullptr);
        RC_ASSERT(uexp->GetNumber() == number);
    });
    
    rc::check("(number)*number",[](const int& n,const int & n1){
        int number = abs(n);
        int number1 = abs(n1);
        auto str = "(" + toString(number) + ")" + "*" + toString(number1);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::Exp(stream);
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(exp);
        RC_ASSERT(uexp != nullptr);
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetLeftExp())->GetNumber() == number);
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetRightExp())->GetNumber() == number1);
    });
    
    rc::check("(number+number)*number",[](const int& n,const int & n1,const int& n2){
        int number2 = abs(n2);
        int number = abs(n);
        int number1 = abs(n1);
        auto str = "(" + toString(number) + "+" + toString(number2) + ")" + "*" + toString(number1);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::Exp(stream);
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(exp);
        RC_ASSERT(uexp != nullptr);
        auto leftExp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(uexp->GetLeftExp());
        RC_ASSERT( leftExp != nullptr);
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(leftExp->GetLeftExp())->GetNumber() == number );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(leftExp->GetRightExp())->GetNumber() == number2 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetRightExp())->GetNumber() == number1);
    });
    
    rc::check("number+number*number",[](const int& n,const int & n1,const int& n2){
        int number2 = abs(n2);
        int number = abs(n);
        int number1 = abs(n1);
        auto str = toString(number) + "+" + toString(number2) + "*" + toString(number1);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::Exp(stream);
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(exp);
        RC_ASSERT(uexp != nullptr);
        auto rightExp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(uexp->GetRightExp());
        RC_ASSERT( rightExp != nullptr);
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(rightExp->GetLeftExp())->GetNumber() == number2 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(rightExp->GetRightExp())->GetNumber() == number1 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetLeftExp())->GetNumber() == number);
    });
    
    //write state
    rc::check("write number+number*number",[](const int& n,const int & n1,const int& n2){
        int number2 = abs(n2);
        int number = abs(n);
        int number1 = abs(n1);
        auto str = std::string("write \t") + toString(number) + "+" + toString(number2) + "*" + toString(number1);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::ParserWrite(stream);
        RC_ASSERT(!exp->isNone());
        auto wr = std::dynamic_pointer_cast<Parser::WriteStatement>(exp->value());
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(wr->GetExp());
        RC_ASSERT(uexp != nullptr);
        auto rightExp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(uexp->GetRightExp());
        RC_ASSERT( rightExp != nullptr);
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(rightExp->GetLeftExp())->GetNumber() == number2 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(rightExp->GetRightExp())->GetNumber() == number1 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetLeftExp())->GetNumber() == number);
    });
            

    
    //assignment
    rc::check("hello := number+number*number",[](const int& n,const int & n1,const int& n2){
        int number2 = abs(n2);
        int number = abs(n);
        int number1 = abs(n1);
        auto str = std::string("hello := ") + toString(number) + "+" + toString(number2) + "*" + toString(number1);
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::ParserAssignment(stream);
        RC_ASSERT(!exp->isNone());
        auto wr = std::dynamic_pointer_cast<Parser::AssignStatement>(exp->value());
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(wr->GetExp());
        RC_ASSERT(uexp != nullptr);
        auto rightExp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(uexp->GetRightExp());
        RC_ASSERT( rightExp != nullptr);
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(rightExp->GetLeftExp())->GetNumber() == number2 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(rightExp->GetRightExp())->GetNumber() == number1 );
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetLeftExp())->GetNumber() == number);
    });

    //ifstate
    rc::check("if number < number then word:= 123 end",[](const int& n,const int & n1){
        int number = abs(n);
        int number1 = abs(n1);
        auto str = std::string("if ") + toString(number) + " < " + toString(number1) + " then \n word:= 1\n end";
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::ParserIfStatment(stream);
        RC_ASSERT(!exp->isNone());
        auto wr = std::dynamic_pointer_cast<Parser::IfStatement>(exp->value());
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(wr->GetExp());
        auto thenStmt = wr->GetThenStmt();
        RC_ASSERT(thenStmt != nullptr);
        RC_ASSERT(wr->GetElseStmt() == nullptr);
        RC_ASSERT(uexp != nullptr);
        auto rightExp = std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetRightExp());
        RC_ASSERT( rightExp != nullptr);
        RC_ASSERT(rightExp->GetNumber() == number1 );
        RC_ASSERT(uexp->GetOp() == "<");
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetLeftExp())->GetNumber() == number);
    });
    
    rc::check("if number < number then word:= 123 else word := word - 1; \n hello := 8 end",[](const int& n,const int & n1){
        int number = abs(n);
        int number1 = abs(n1);
        auto str = std::string("if ") + toString(number) + " < " + toString(number1) + " then \n word:= word -1\n else \nword := 47; \n hello := 8 end";
        auto stream = Lex::ParserStream::fromString(str);
        auto exp = Parser::ParserIfStatment(stream);
        RC_ASSERT(!exp->isNone());
        auto wr = std::dynamic_pointer_cast<Parser::IfStatement>(exp->value());
        auto uexp = std::dynamic_pointer_cast<Parser::UnaryOpExp>(wr->GetExp());
        auto thenStmt = wr->GetThenStmt();
        RC_ASSERT(thenStmt != nullptr);
        RC_ASSERT(wr->GetElseStmt() != nullptr);
        RC_ASSERT(uexp != nullptr);
        auto rightExp = std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetRightExp());
        RC_ASSERT( rightExp != nullptr);
        RC_ASSERT(rightExp->GetNumber() == number1 );
        RC_ASSERT(uexp->GetOp() == "<");
        RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(uexp->GetLeftExp())->GetNumber() == number);
    });
    
    
    //repeate statement
    
    auto str = std::string("repeat\n\tx := x - 1\nuntil x = 0\n");
    auto stream = Lex::ParserStream::fromString(str);
    auto r = Parser::ParserRepeatStatement(stream);
    RC_ASSERT(!r->isNone());
    auto rs = std::dynamic_pointer_cast<Parser::RepeatStatement>(r->value());
    auto exprep = std::dynamic_pointer_cast<Parser::UnaryOpExp>(rs->GetExp());
    RC_ASSERT(exprep != nullptr);
    RC_ASSERT(exprep->GetOp() == "=");
    RC_ASSERT(std::dynamic_pointer_cast<Parser::ConstExp>(exprep->GetRightExp())->GetNumber() == 0);
    RC_ASSERT(rs->GetStmtSeq() != nullptr);
    RC_ASSERT(rs != nullptr);
}

#endif
