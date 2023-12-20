#ifndef PARSER_H
#define PARSER_H
#include "Parser.h"
#include "Error.h"
#endif


/*
	parsing a number of statements. it is called two times.
	one at the start of program to get all the statements, 
	the other inside control and loop statetments because they 
	have a number of statements inside the bodies. it returns
	the base class that consists of all the statements inside the
	scope body
*/
Base* Parser::parseS()
{
	llvm::SmallVector<Statement*> statements;
	while (!Tok.is(Token::eof))
	{
		switch (Tok.getKind())
		{
		case Token::ident:
		{
			AssignStatement* state = parseAssign();

			statements.push_back(state);
			break;
		}
		case Token::KW_int:
		{
			llvm::SmallVector<DecStatement*> states = parseDefine();
			if (states.size() == 0)
			{
				return nullptr;
			}

			while (states.size() > 0)
			{
				statements.push_back(states.back());
				states.pop_back();
			}
			break;
		}
		case Token::KW_if:
		{
			IfStatement* statement = parseIf();
			statements.push_back(statement);
			break;

		}
		case Token::KW_loopc:
		{
			LoopStatement* statement = parseLoop();
			statements.push_back(statement);
			break;
		}

		default:
		{
			return new Base(statements);
		}

		}
	}
	return new Base(statements);
}

/*
	parses declaration statements like int a, b, c;
*/
llvm::SmallVector<DecStatement*> Parser::parseDefine()
{
	llvm::SmallVector<DecStatement*> assignments;

	llvm::SmallVector<Expression*> variables;
	llvm::SmallVector<Expression*> values;

	advance();
	bool SeenTokenVariable = true;
	while (SeenTokenVariable)
	{
		Expression* lhand = parseVar();
		variables.push_back(lhand);

		if (!Tok.is(Token::comma))
			SeenTokenVariable = false;
		else
			advance();
	}



	if (Tok.is(Token::semi_colon))
	{
		bool SeenTokenValue = true;
		for (int i = 0; i < variables.size(); i++)
		{
			Expression* rhand = new Expression(0);
			values.push_back(rhand);
		}
	}
	else if (!Tok.is(Token::equal))
	{
		Error::AssignmentEqualNotFound();
	}
	else
	{
		advance(); // pass equal

		bool SeenTokenValue = true;
		while (SeenTokenValue)
		{
			Expression* rhand = parseExpr();
			values.push_back(rhand);

			if (!Tok.is(Token::comma))
				SeenTokenValue = false;
			else
				advance();
		}
	}




	if (variables.size() < values.size())
	{
		Error::AssignmentSidesNotEqual();
	}
	else
	{
		while (variables.size() != 0)
		{
			assignments.push_back(new DecStatement(variables.front(), values.size() > 0 ? values.front() : new Expression(0)));
			variables.erase(variables.begin());
			
			if (values.size() > 0)
			{
				values.erase(values.begin());
			}
		}

		if (Tok.is(Token::semi_colon))
		{
			advance();
			return assignments;
		}
		else
		{
			Error::SemiColonNotFound();
		}
	}


}


/*
	parses the expression
*/
Expression* Parser::parseExpr()
{
	Expression* Left = parseTerm();
	while (Tok.isOneOf(Token::plus, Token::minus))
	{
		BinaryOp::Operator Op =
			Tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
		advance();
		Expression* Right = parseTerm();
		Left = new BinaryOp(Op, Left, Right);
	}
	return Left;
}


/*
	parses the term
*/
Expression* Parser::parseTerm()
{
	Expression* Left = parsePower();
	while (Tok.isOneOf(Token::star, Token::slash, Token::mod))
	{
		BinaryOp::Operator Op =
			Tok.is(Token::star) ? BinaryOp::Mul : Tok.is(Token::slash) ? BinaryOp::Div : BinaryOp::Mod;
		advance();
		Expression* Right = parsePower();
		Left = new BinaryOp(Op, Left, Right);
	}
	return Left;
}


/*
	parses the power inside expression.
*/
Expression* Parser::parsePower()
{
	Expression* Left = parseFactor();
	while (Tok.is(Token::power))
	{
		BinaryOp::Operator Op =
			BinaryOp::Pow;
		advance();
		Expression* Right = parseFactor();
		Left = new BinaryOp(Op, Left, Right);
	}
	return Left;
}

/*
	parses factor
*/
Expression* Parser::parseFactor()
{
	Expression* Res = nullptr;
	switch (Tok.getKind())
	{
	case Token::number:
	{
		int number;
		Tok.getText().getAsInteger(10, number);
		Res = new Expression(number);
		advance();
		break;
	}
	case Token::ident:
	{
		Res = new Expression(Tok.getText());
		advance();
		break;
	}
	case Token::l_paren:
	{
		advance();
		Res = parseExpr();
		if (!consume(Token::r_paren))
			break;
	}
	default: // error handling
	{
		Error::NumberVariableExpected();
	}

	}
	return Res;
}


/*
	parse assignment like a = 3;
*/
AssignStatement* Parser::parseAssign()
{
	Expression* variable;
	Expression* value;

	variable = parseVar();

	if (Tok.is(Token::minus_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Minus, variable, value);

	}
	else if (Tok.is(Token::plus_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Plus, variable, value);
	}
	else if (Tok.is(Token::star_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Mul, variable, value);
	}
	else if (Tok.is(Token::slash_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Div, variable, value);
	}
	else if (Tok.is(Token::mod_equal))
	{
		advance();
		value = parseExpr();
		value = new BinaryOp(BinaryOp::Mod, variable, value);
	}
	else if (Tok.is(Token::equal))
	{
		advance(); // pass equal
		value = parseExpr();
	}
	else
	{
		Error::AssignmentEqualNotFound();
	}

	if (!Tok.is(Token::semi_colon))
	{
		Error::SemiColonNotFound();
	}

	advance(); // pass semicolon
	return new AssignStatement(variable, value);

}


/*
	parses variable name, it returns an expression
	with type Identifier with variable name inside it.
	in order to get it you need to call getValue() on Expression object
*/
Expression* Parser::parseVar()
{
	if (!Tok.is(Token::ident))
	{
		Error::VariableNameNotFound();
	}

	Expression* variable = new Expression(Tok.getText());
	advance();
	return variable;
}


LoopStatement* Parser::parseLoop()
{
	advance();			// pass loop identifier

	Expression* condition = parseCondition();



	if (!Tok.is(Token::colon))
	{
		Error::ColonExpectedAfterCondition();
	}

	advance();

	if (Tok.is(Token::KW_begin))
	{
		advance();

		Base* AllStates = parseStatement();

		if (!consume(Token::KW_end))
		{

			return new LoopStatement(condition, AllStates->getStatements(), Statement::StateMentType::Loop);
		}
		else
		{
			Error::EndNotSeenForIf();
		}
	}
	else
	{
		Error::BeginExpectedAfterColon();
	}
}

/*
	parses condition like 3 > 5+1 and true
*/
Expression* Parser::parseCondition()
{
	Expression* lcondition;

	lcondition = parseSubCondition();
	if (Tok.is(Token::KW_and))
	{
		advance();
		Expression* rcondition = parseCondition();
		return new Expression(new BooleanOp(BooleanOp::And, lcondition, rcondition));
	}
	else if (Tok.is(Token::KW_or))
	{
		advance();
		Expression* rcondition = parseCondition();
		return new Expression(new BooleanOp(BooleanOp::Or, lcondition, rcondition));
	}
	else
	{
		return lcondition;
	}
}


/*
	parses a single subcondition like 3 > 5+1
	it does not deal with and, or
*/
Expression* Parser::parseSubCondition()
{


	if (Tok.is(Token::l_paren))
	{

		advance();
		Expression* expressionInside = parseCondition();
		if (!consume(Token::r_paren))
		{
			return expressionInside;
		}
		else
		{
			Error::RightParanthesisExpected();
		}
	}

	else if (Tok.is(Token::KW_true))
	{
		advance();

		return new Expression(true);
	}
	else if (Tok.is(Token::KW_false))
	{
		advance();
		return new Expression(false);
	}
	else
	{
		Expression* lhand = parseExpr();
		BooleanOp::Operator Op;

		if (Tok.is(Token::less))
		{
			Op = BooleanOp::Less;
		}
		else if (Tok.is(Token::less_equal))
		{
			Op = BooleanOp::LessEqual;
		}
		else if (Tok.is(Token::greater))
		{
			Op = BooleanOp::Greater;
		}
		else if (Tok.is(Token::greater_equal))
		{
			Op = BooleanOp::GreaterEqual;
		}
		else if (Tok.is(Token::equal_equal))
		{
			Op = BooleanOp::Equal;
		}
		else if (Tok.is(Token::not_equal))
		{
			Op = BooleanOp::NotEqual;
		}
		else
		{
			Error::BooleanValueExpected();
		}

		advance();
		Expression* rhand = parseExpr();
		return new BooleanOp(Op, lhand, rhand);

	}
}


/*
	parse if statement and return the control statement
	associated with it.
*/
IfStatement* Parser::parseIf()
{
	advance();			// pass if identifier

	Expression* condition = parseCondition();

	

	if (!Tok.is(Token::colon))
	{
		Error::ColonExpectedAfterCondition();
	}

	advance();

	if (Tok.is(Token::KW_begin))
	{
		advance();

		Base* AllStates = parseStatement();

		if (!consume(Token::KW_end))
		{
			llvm::SmallVector<ElifStatement*> ElifS;
			ElseStatement* ElseS;
			bool hasElif = false;
			bool hasElse = false;

			while (Tok.is(Token::KW_elif))
			{
				ElifStatement* statement = parseElif();
				ElifS.push_back(statement);
				hasElif = true;
			}
			if (Tok.is(Token::KW_else))
			{
				ElseS = parseElse();
				hasElse = true;
			}
			return new IfStatement(condition, AllStates->getStatements(),
									ElifS, ElseS, hasElif, hasElse,
									Statement::StateMentType::If);
		}
		else
		{
			Error::EndNotSeenForIf();
		}
	}
	else
	{
		Error::BeginExpectedAfterColon();
	}
}

ElifStatement* Parser::parseElif()
{
	advance();			// pass if identifier

	Expression* condition = parseCondition();

	

	if (!Tok.is(Token::colon))
	{
		Error::ColonExpectedAfterCondition();
	}

	advance();

	if (Tok.is(Token::KW_begin))
	{
		advance();

		Base* AllStates = parseStatement();

		if (!consume(Token::KW_end))
		{
	
			return new ElifStatement(condition, AllStates->getStatements(), Statement::StateMentType::Elif);
		}
		else
		{
			Error::EndNotSeenForIf();
		}
	}
	else
	{
		Error::BeginExpectedAfterColon();
	}
}

ElseStatement* Parser::parseElse()
{
	advance();			// pass if identifier

	if (!Tok.is(Token::colon))
	{
		Error::ColonExpectedAfterCondition();
	}

	advance();

	if (Tok.is(Token::KW_begin))
	{
		advance();

		Base* AllStates = parseStatement();

		if (!consume(Token::KW_end))
		{
	
			return new ElseStatement(AllStates->getStatements(), Statement::StateMentType::Else);
		}
		else
		{
			Error::EndNotSeenForIf();
		}
	}
	else
	{
		Error::BeginExpectedAfterColon();
	}
}

Base* Parser::parseStatement()
{
	llvm::SmallVector<Statement*> statements;
	while (!Tok.is(Token::KW_end))
	{
		switch (Tok.getKind())
		{
		case Token::ident:
		{
			AssignStatement* state = parseAssign();

			statements.push_back(state);
			break;
		}
		case Token::KW_int:
		{
			Error::DefineInsideScope();
		}
		case Token::KW_if:
		{
			IfStatement* statement = parseIf();
			statements.push_back(statement);
			break;

		}
		case Token::KW_loopc:
		{
			LoopStatement* statement = parseLoop();
			statements.push_back(statement);
			break;
		}

		default:
		{
			return new Base(statements);
		}

		}
	}
	return new Base(statements);
}

Base* Parser::parse()
{
	Base* Res = parseS();
	return Res;
}