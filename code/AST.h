#ifndef AST_H
#define AST_H

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

class AST; // Abstract Syntax Tree
class Expression; // top level expression that is evaluated to boolean, int or variable name at last
class Base; // top level program
class Statement; // top level statement
class BinaryOp; // binary operation of numbers and identifiers
class AssignStatement; // assignment statement like a = 3;
class DecStatement; // declaration statement like int a;
class BooleanOp; // boolean operation like 3 > 6*2;
class LoopStatement;
class IfStatement;
class ElifStatement;
class ElseStatement;

class ASTVisitor
{
public:
	// Virtual visit functions for each AST node type
	virtual void visit(AST&) {}
	virtual void visit(Expression&) {}
	virtual void visit(Base&) = 0;
	virtual void visit(Statement&) = 0;
	virtual void visit(BinaryOp&) = 0;
	virtual void visit(DecStatement&) = 0;
	virtual void visit(AssignStatement&) = 0;
	virtual void visit(BooleanOp&) = 0;
	virtual void visit(IfStatement&) = 0;
	virtual void visit(ElifStatement&) = 0;
	virtual void visit(ElseStatement&) = 0;
	virtual void visit(LoopStatement&) = 0;
};




class AST {
public:
	virtual ~AST() {}
	virtual void accept(ASTVisitor& V) = 0;
};



// base Node that contains all the syntax nodes
class Base : public AST {
private:
	llvm::SmallVector<Statement*> statements;                          // Stores the list of expressions

public:
	Base(llvm::SmallVector<Statement*> Statements) : statements(Statements) {}
	llvm::SmallVector<Statement*> getStatements() { return statements; }

	llvm::SmallVector<Statement*>::const_iterator begin() { return statements.begin(); }

	llvm::SmallVector<Statement*>::const_iterator end() { return statements.end(); }
	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}

};


class TopLevelEntity : AST {
public:
	TopLevelEntity() {}
};



// Value Expression class that holds information about
// numbers or variable names. forexample a or 56 are 
// ValExpression and become part of the syntax tree
class Expression : public TopLevelEntity {
public:
	enum ExpressionType {
		Number,
		Identifier,
		Boolean,
		BinaryOpType,
		BooleanOpType
	};
private:
	ExpressionType Type; // can be number of variable name

	// if it holds a number NumberVal is used else Value is
	// used to store variable name
	llvm::StringRef Value;
	int NumberVal;
	bool BoolVal;
	BooleanOp* BOVal;

public:
	Expression() {}
	Expression(llvm::StringRef value) : Type(ExpressionType::Identifier), Value(value) {} // store string
	Expression(int value) : Type(ExpressionType::Number), NumberVal(value) {} // store number
	Expression(bool value) : Type(ExpressionType::Boolean), BoolVal(value) {} // store boolean
	Expression(BooleanOp* value) : Type(ExpressionType::BooleanOpType), BOVal(value) {} // store boolean
	Expression(ExpressionType type) : Type(type) {}

	bool isNumber() {
		if (Type == ExpressionType::Number)
			return true;
		return false;
	}

	bool isBoolean() {
		if (Type == ExpressionType::Boolean)
			return true;
		return false;
	}

	bool isVariable() {
		if (Type == ExpressionType::Identifier)
			return true;
		return false;
	}

	// returns identifier
	llvm::StringRef getValue() {
		return Value;
	}

	int getNumber() {
		return NumberVal;
	}

	BooleanOp* getBooleanOp() {
		return BOVal;
	}

	bool getBoolean() {
		return BoolVal;
	}


	// returns the kind of expression. can be identifier,
	// number, or an operation
	ExpressionType getKind()
	{
		return Type;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

// stores information of a statement. forexample x=56; is a statement
class Statement : public TopLevelEntity {
public:
	enum StateMentType {
		Declaration,
		Assignment,
		If,
		Elif,
		Else,
		Loop
	};

private:
	StateMentType Type;

public:

	StateMentType getKind()
	{
		return Type;
	}


	Statement(StateMentType type) : Type(type) {}
	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

class LoopStatement : public Statement {

private:
	Expression* Condition;
	llvm::SmallVector<Statement*> Statements;

public:
	LoopStatement(Expression* condition, llvm::SmallVector<Statement*> statements, StateMentType type) : Condition(condition), Statements(statements), Statement(type) { }

	Expression* getCondition()
	{
		return Condition;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};


// if statement
class IfStatement : public Statement {

private:
	Expression* Condition;
	llvm::SmallVector<Statement*> Statements;
	llvm::SmallVector<ElifStatement*> ElifsStatements;
	ElseStatement* ElseStatements;
	bool HasElif;
	bool HasElse;

public:
	IfStatement(Expression* condition,
	 llvm::SmallVector<Statement*> statements,
	 llvm::SmallVector<ElifStatement*> elifsStatements,
	 ElseStatement* elseStatement,
	 bool hasElif, bool hasElse,
	 StateMentType type): Condition(condition),
	 Statements(statements),
	 ElifsStatements(elifsStatements),
	 ElseStatements(elseStatement),
	 HasElif(hasElif),
	 HasElse(hasElse),
	 Statement(type) { }

	Expression* getCondition()
	{
		return Condition;
	}

	bool hasElif(){
		return HasElif;
	}

	bool hasElse(){
		return HasElse;
	}

	llvm::SmallVector<ElifStatement*> getElifsStatements()
	{
		return ElifsStatements;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	ElseStatement* getElseStatement()
	{
		return ElseStatements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

class ElifStatement : public Statement {

private:
	Expression* Condition;
	llvm::SmallVector<Statement*> Statements;

public:
	ElifStatement(Expression* condition, llvm::SmallVector<Statement*> statements, StateMentType type): Condition(condition), Statements(statements), Statement(type) { }

	Expression* getCondition()
	{
		return Condition;
	}

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

class ElseStatement : public Statement {

private:
	llvm::SmallVector<Statement*> Statements;

public:
	ElseStatement(llvm::SmallVector<Statement*> statements, StateMentType type): Statements(statements), Statement(type) { }

	llvm::SmallVector<Statement*> getStatements()
	{
		return Statements;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

/*
	Declaration statement like int x; or int a = 3;
*/
class DecStatement : public Statement {
private:

	Expression* lvalue;
	Expression* rvalue;
	Statement::StateMentType type;

public:
	DecStatement(Expression* lvalue, Expression* rvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::Declaration), Statement(type) { }
	DecStatement(Expression* lvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::Declaration), Statement(type) { rvalue = new Expression(0); }

	Expression* getLValue() {
		return lvalue;
	}

	Expression* getRValue() {
		return rvalue;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

// assignment statement. forexample x=56;
// right hand side is an expression
class AssignStatement : public Statement {
private:

	Expression* lvalue;
	Expression* rvalue;
	Statement::StateMentType type;

public:
	AssignStatement(Expression* lvalue, Expression* rvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::Assignment), Statement(type) { }
	Expression* getLValue() {
		return lvalue;
	}

	Expression* getRValue() {
		return rvalue;
	}

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

// Binary Operation for computation of numbers
// used in the syntax tree stage like 3*(56+a*2)/2
class BinaryOp : public Expression
{
public:
	enum Operator
	{
		Plus,
		Minus,
		Mul,
		Div,
		Mod,
		Pow
	};

private:
	Expression* Left;                               // Left-hand side expression
	Expression* Right;                              // Right-hand side expression
	Operator Op;                              // Operator of the binary operation

public:
	BinaryOp(Operator Op, Expression* L, Expression* R) : Op(Op), Left(L), Right(R), Expression(ExpressionType::BinaryOpType) {}

	Expression* getLeft() { return Left; }

	Expression* getRight() { return Right; }

	Operator getOperator() { return Op; }

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};


/*
	a boolean operation of form 3 > 5+1 that consists
	of 2 lefthand and righthand expressions. these expressions
	can be number, identifier or of type binaryop.
	it has an operator that can be +, -, *, /, % or ^
*/
class BooleanOp : public Expression
{
public:
	enum Operator
	{
		LessEqual,
		Less,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual,
		And,
		Or
	};

private:
	Expression* Left;                               // Left-hand side expression
	Expression* Right;                              // Right-hand side expression
	Operator Op;                              // Operator of the boolean operation

public:
	BooleanOp(Operator Op, Expression* L, Expression* R) : Op(Op), Left(L), Right(R), Expression(ExpressionType::BooleanOpType) { }

	Expression* getLeft() { return Left; }

	Expression* getRight() { return Right; }

	Operator getOperator() { return Op; }

	virtual void accept(ASTVisitor& V) override
	{
		V.visit(*this);
	}
};

#endif