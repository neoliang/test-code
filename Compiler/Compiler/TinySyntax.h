//
//  TinySyntax.h
//  Compiler
//
//  Created by neo on 15-10-26.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef __Compiler__TinySyntax__
#define __Compiler__TinySyntax__

#include <vector>
#include <memory>
#include <string>

namespace Parser {
    class NodeVisitor;
    
    class SyntaxNode
    {

        unsigned int _lineno;
    public:
        virtual ~SyntaxNode(){}
        SyntaxNode(unsigned int lineno)
        :_lineno(lineno){}
        SyntaxNode(const SyntaxNode&) = delete;
        const SyntaxNode& operator = (const SyntaxNode&) = delete;
        
    public:

        unsigned int LineNo()const{return _lineno;}
        virtual void Visit(std::shared_ptr<NodeVisitor> vi) = 0;
    };
    
    
    class StatementNode : public SyntaxNode
    {
        using SyntaxNode::SyntaxNode;
    };
    
    class ExpNode : public SyntaxNode
    {
        using SyntaxNode::SyntaxNode;
    };
    
    class ConstExp : public ExpNode
    {
        int _number;
        
    public:
        ConstExp(unsigned int lineno,int n)
        :ExpNode(lineno)
        ,_number(n){}
        void Visit(std::shared_ptr<NodeVisitor> vi);
        int GetNumber()const{return _number;}
    };
    class IdExp : public ExpNode
    {
        std::string _identifier;
    public:
        IdExp(unsigned int lineno,const std::string& id)
        :ExpNode(lineno)
        ,_identifier(id){}
        
        const std::string& GetIdentifier()const
        {
            return _identifier;
        }
        
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    class UnaryOpExp : public ExpNode
    {
        std::shared_ptr<ExpNode> _left;
        std::shared_ptr<ExpNode> _right;
        std::string _op;
    public:
        UnaryOpExp(unsigned int lineno, const std::string& op,std::shared_ptr<ExpNode> l, std::shared_ptr<ExpNode> r)
        :ExpNode(lineno)
        ,_op(op)
        ,_left(l)
        ,_right(r)
        {}
        
        std::shared_ptr<ExpNode> GetLeftExp()const
        {
            return _left;
        }
        std::shared_ptr<ExpNode> GetRightExp()const
        {
            return _right;
        }
        const std::string& GetOp()const
        {
            return _op;
        }
        void Visit(std::shared_ptr<NodeVisitor> vi);
        
    };
    
    class StatementSeq : public SyntaxNode
    {
        std::vector<std::shared_ptr<StatementNode>> _statements;
    public:
        StatementSeq(unsigned int lineno,const std::vector<std::shared_ptr<StatementNode>>& s = {} )
        :SyntaxNode(lineno)
        ,_statements(s)
        {}
        void AddStatement(std::shared_ptr<StatementNode> statement)
        {
            _statements.push_back(statement);
        }
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    
    class IfStatement : public StatementNode
    {
        std::shared_ptr<ExpNode> _exp;
        std::shared_ptr<StatementSeq> _then;
        std::shared_ptr<StatementSeq> _else;
        
    public:
        
        IfStatement(unsigned int lineno,std::shared_ptr<ExpNode> exp,std::shared_ptr<StatementSeq> thenStmtSeq,std::shared_ptr<StatementSeq> elseStmtSeq)
        :StatementNode(lineno)
        ,_exp(exp)
        ,_else(elseStmtSeq)
        ,_then(thenStmtSeq)
        {}
        
        void Visit(std::shared_ptr<NodeVisitor> vi);
        
    };
    
    class RepeatStatement : public StatementNode
    {
        std::shared_ptr<StatementSeq> _stateSeq;
        std::shared_ptr<ExpNode> _exp;
    public:
        
        RepeatStatement(unsigned int lineno,std::shared_ptr<ExpNode> exp,std::shared_ptr<StatementSeq> stmtSeq)
        :StatementNode(lineno)
        ,_exp(exp)
        ,_stateSeq(stmtSeq)
        {}
        
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    
    class AssignStatement : public StatementNode
    {
        std::string _identifier;
        std::shared_ptr<ExpNode> _exp;
    public:
        
        AssignStatement(unsigned int lineno,const std::string& id,std::shared_ptr<ExpNode> exp)
        :StatementNode(lineno)
        ,_identifier(id)
        ,_exp(exp)
        {}
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    
    class ReadStatement : public StatementNode
    {
        std::string _identifier;
    public:
        ReadStatement(unsigned int lineno,const std::string& id)
        :StatementNode(lineno)
        ,_identifier(id)
        {}
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    
    class WriteStatement : public StatementNode
    {
        std::shared_ptr<ExpNode> _exp;
    public:
        WriteStatement(unsigned int lineno, std::shared_ptr<ExpNode> exp)
        :StatementNode(lineno)
        ,_exp(exp)
        {
            
        }
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    
    class NodeVisitor
    {
    public:
        virtual ~NodeVisitor(){}
        virtual void Accept(const SyntaxNode& node) {};
    };
}


#endif /* defined(__Compiler__TinySyntax__) */
