#include "Lexer.h"


namespace charinfo {

	LLVM_READNONE inline bool isWhitespace(char c) {
		return c == ' ' || c == '\t' || c == '\f' ||
			c == '\v' || c == '\r' || c == '\n';
	}

	LLVM_READNONE inline bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	LLVM_READNONE inline bool isLetter(char c) {
		return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z');
	}

	LLVM_READNONE inline bool isOperator(char c) {
		return c == '+' || c == '-' || c == '*' ||
			c == '/' || c == '^' || c == '=' ||
			c == '<' || c == '>' || c == '!' ||
			c == ':' || c == ',';
	}
}
void Lexer::next(Token& token) {

	while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {      // Skips whitespace like " "
		++BufferPtr;
	}

	if (!*BufferPtr) {              // since end of context is 0 -> !0 = true -> end of context
		token.Kind = Token::eof;
		return;
	}

	if (charinfo::isLetter(*BufferPtr)) {   // looking for keywords or identifiers like "int", a123 , ...

		const char* end = BufferPtr + 1;

		while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
			++end;                          // until reaches the end of lexeme
		// example: ".int " -> "i.nt " -> "in.t " -> "int. "


		llvm::StringRef Context(BufferPtr, end - BufferPtr);  // start of lexeme, length of lexeme

		Token::TokenKind kind;

		if (Context == "int") {
			kind = Token::KW_int;
		}
		else if (Context == "if") {
			kind = Token::KW_if;
		}
		else if (Context == "elif") {
			kind = Token::KW_elif;
		}
		else if (Context == "else") {
			kind = Token::KW_else;
		}
		else if (Context == "loopc") {
			kind = Token::KW_loopc;
		}
		else if (Context == "and") {
			kind = Token::KW_and;
		}
		else if (Context == "or") {
			kind = Token::KW_or;
		}
		else if (Context == "true") {
			kind = Token::KW_true;
		}
		else if (Context == "false") {
			kind = Token::KW_false;
		}
		else if (Context == "begin")
		{
			kind = Token::KW_begin;
		}
		else if (Context == "end")
		{
			kind = Token::KW_end;
		}
		else {
			kind = Token::ident;
		}


		formToken(token, end, kind);
		return;
	}

	else if (charinfo::isDigit(*BufferPtr)) {

		const char* end = BufferPtr + 1;

		while (charinfo::isDigit(*end))
			++end;

		formToken(token, end, Token::number);
		return;
	}

	else {

		if (*BufferPtr == '=' && *(BufferPtr + 1) == '=') {      // ==
			formToken(token, BufferPtr + 2, Token::equal_equal);
		}
		else if (*BufferPtr == '+' && *(BufferPtr + 1) == '=') {      // +=
			formToken(token, BufferPtr + 2, Token::plus_equal);

		}
		else if (*BufferPtr == '-' && *(BufferPtr + 1) == '=') {      // -=
			formToken(token, BufferPtr + 2, Token::minus_equal);
		}

		else if (*BufferPtr == '*' && *(BufferPtr + 1) == '=') {      // *=
			formToken(token, BufferPtr + 2, Token::star_equal);
		}

		else if (*BufferPtr == '/' && *(BufferPtr + 1) == '=') {      // /=
			formToken(token, BufferPtr + 2, Token::slash_equal);
		}

		else if (*BufferPtr == '%' && *(BufferPtr + 1) == '=') {      // %=
			formToken(token, BufferPtr + 2, Token::mod_equal);
		}

		else if (*BufferPtr == '!' && *(BufferPtr + 1) == '=') {      // !=
			formToken(token, BufferPtr + 2, Token::not_equal);
		}

		else if (*BufferPtr == '<' && *(BufferPtr + 1) == '=') {      // <=
			formToken(token, BufferPtr + 2, Token::less_equal);
		}

		else if (*BufferPtr == '>' && *(BufferPtr + 1) == '=') {      // >=
			formToken(token, BufferPtr + 2, Token::greater_equal);
		}
		else if (*BufferPtr == '=') {
			formToken(token, BufferPtr + 1, Token::equal);
		}
		else {
			switch (*BufferPtr) {

#define CASE(ch, tok) \
            case ch: formToken(token, BufferPtr + 1, tok); break

				CASE('+', Token::plus);
				CASE('-', Token::minus);
				CASE('*', Token::star);
				CASE('/', Token::slash);
				CASE('(', Token::l_paren);
				CASE(')', Token::r_paren);
				CASE(':', Token::colon);
				CASE(',', Token::comma);
				CASE('^', Token::power);
				CASE('>', Token::greater);
				CASE('<', Token::less);
				CASE(';', Token::semi_colon);
				CASE('%', Token::mod);

#undef CASE

			default:
				formToken(token, BufferPtr + 1, Token::unknown);
			}
			return;
		}
	}


}

void Lexer::formToken(Token& Tok, const char* TokEnd, Token::TokenKind Kind) {

	Tok.Kind = Kind;
	Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
	BufferPtr = TokEnd;
}