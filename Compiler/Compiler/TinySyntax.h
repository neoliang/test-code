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
#include <functional>
#include <list>
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
    typedef std::shared_ptr<ExpNode> ExpNodePtr;
    typedef std::list<ExpNodePtr> ExpNodePtrList;
    
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
    
    class FunCall : public ExpNode
    {
        std::string _identifier;
        ExpNodePtrList _es;
    public:
        FunCall(unsigned int lineno,const std::string& id,const ExpNodePtrList& es)
        :ExpNode(lineno)
        ,_identifier(id)
        ,_es(es){}
        
        const std::string& GetIdentifier()const
        {
            return _identifier;
        }
        const ExpNodePtrList& GetExpList()const
        {
            return _es;
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
        void foreach(std::function<void(std::shared_ptr<StatementNode>)> v)
        {
            std::for_each(_statements.begin(), _statements.end(), v);
        }
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    //function-stmt -> identifier (idlist) stmt-seq [return exp] end
    class FunStatment : public StatementNode
    {
        ExpNodePtr _retExp;
        std::shared_ptr<StatementSeq> _stmtSeq;
        std::list<std::string> _params;
        std::string _name;
        
    public:
        FunStatment(unsigned int lineno, const std::string& name,const std::list<std::string>& params, std::shared_ptr<StatementSeq> stmtSeq,ExpNodePtr exp = nullptr)
        :StatementNode(lineno)
        ,_name(name)
        ,_params(params)
        ,_stmtSeq(stmtSeq)
        ,_retExp(exp)
        {
            
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
        std::shared_ptr<ExpNode> GetExp()const{return _exp;}
        std::shared_ptr<StatementSeq> GetThenStmt()const {return _then;}
        std::shared_ptr<StatementSeq> GetElseStmt()const {return _else;}
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
        
        std::shared_ptr<StatementSeq> GetStmtSeq()const {return _stateSeq;}
        std::shared_ptr<ExpNode> GetExp()const{return _exp;}
        void Visit(std::shared_ptr<NodeVisitor> vi);
    };
    
    class AssignStatement : public StatementNode
    {
        std::string _identifier;
        std::shared_ptr<ExpNode> _exp;
        bool _isLocal ;
    public:
        
        AssignStatement(unsigned int lineno,const std::string& id,std::shared_ptr<ExpNode> exp,bool isLocal = false)
        :StatementNode(lineno)
        ,_identifier(id)
        ,_exp(exp)
        ,_isLocal(isLocal)
        {}
        void Visit(std::shared_ptr<NodeVisitor> vi);
        std::shared_ptr<ExpNode> GetExp()const{return _exp;}
        const std::string& GetIdentifier()const{return _identifier;}
        const bool IsLocal()const{return _isLocal;}
    };
    
    class ReadStatement : public StatementNode
    {
        std::string _identifier;
    public:
        ReadStatement(unsigned int lineno,const std::string& id)
        :StatementNode(lineno)
        ,_identifier(id)
        {}
        const std::string& GetIdentifier()const{return _identifier;}
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
      
        std::shared_ptr<ExpNode> GetExp(){return _exp;}
    };
    
    class NodeVisitor
    {
    public:
        virtual ~NodeVisitor(){}
        virtual void Accept(const SyntaxNode& node) {};
    };
    
    typedef std::shared_ptr<UnaryOpExp> UnaryExpPtr;
    typedef std::shared_ptr<ConstExp> ConstExpPtr;
    typedef std::shared_ptr<IdExp> IdExpPtr;
    
    typedef std::shared_ptr<StatementNode> StatementNodePtr;
    typedef std::shared_ptr<StatementSeq> StatementSeqPtr;
    typedef std::shared_ptr<ReadStatement> ReadStatementPtr;
    typedef std::shared_ptr<WriteStatement> WriteStatementPtr;
    typedef std::shared_ptr<AssignStatement> AssignStatementPtr;
    typedef std::shared_ptr<IfStatement> IfStatementPtr;
    typedef std::shared_ptr<RepeatStatement> RepeatStatementPtr;
    typedef std::shared_ptr<FunStatment> FunStatmentPtr;
}


#endif /* defined(__Compiler__TinySyntax__) */
