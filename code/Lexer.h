#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"        // encapsulates a pointer to a C string and its length
#include "llvm/Support/MemoryBuffer.h" // provides read-only access to a block of memory, filled
// with the content of a file

class Lexer;

class Token {
    friend class Lexer;
    public:
		enum TokenKind : unsigned short {
			semi_colon,            // ;
			unknown,        // unknown token
			ident,          // identifier like a, b, c, d, etc.
			number,         // number like 1, 2, 3, 4, etc.
			comma,          // ,
			colon,          // :
			plus,           // +
			minus,          // -
			star,           // *
			slash,          // /
			power,          // ^
			l_paren,        // (
			r_paren,        // )
			plus_equal,     // +=
			minus_equal,    // -=
			star_equal,     // *=
			slash_equal,    // /=
			mod_equal,		// %=
			equal,          // =
			equal_equal,    // ==
			not_equal,      // !=
			less,           // <
			less_equal,     // <=
			greater,        // >
			greater_equal,  // >=
			space,          // space
			new_line,       // \n
			KW_int,         // int
			KW_if,          // if
			KW_elif,        // elif
			KW_else,        // else
			KW_else_colon,  // else:
			KW_loopc,       // loopc
			KW_and,         // and
			KW_or,          // or
			KW_true,        // true
			KW_false,       // false
			eof,            // end of file
			mod,            // %
			KW_begin,		// begin
			KW_end			// end
		};


private:
	TokenKind Kind;         // <type of token,>
	llvm::StringRef Text;   // <,token context>
public:
	TokenKind getKind() const { return Kind; }
	llvm::StringRef getText() const { return Text; }

	bool is(TokenKind K) const { return Kind == K; }

	bool isOneOf(TokenKind K1, TokenKind K2) const {   // kind="+" isOneOf(plus, minus) -> true
		return is(K1) || is(K2);
	}

	template <typename... Ts>                           // variadic template
	bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks)  // can take several inputs
		const {
		return is(K1) || isOneOf(K2, Ks...);
	}
};

class Lexer {
	const char* BufferStart;
	const char* BufferPtr;

public:
	Lexer(const llvm::StringRef& Buffer) {    // constructor scans the whole context
		BufferStart = Buffer.begin();
		BufferPtr = BufferStart;
	}

	void next(Token& token);                       // gets next token

private:
	void formToken(Token& Result, const char* TokEnd,
		Token::TokenKind Kind);
};
#endif