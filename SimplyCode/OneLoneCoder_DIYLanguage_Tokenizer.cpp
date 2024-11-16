/*
	DIY Programming Language #2
	Parsing Text To Tokens with a Finite State Machine
	
	"Say it properly, say it properly, say it properly" - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2024 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Relevant Video: https://youtu.be/wrj3iuRdA-M

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2024
*/

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <variant>
#include <deque>

namespace olc::lang
{
	// Create fast look-up tables for identifying if a given input character
	// is allowed, given the context at the time. Tough to beat a look-up table
	// for speed, but we sacrifice some scaleability, limiting our input character
	// set to effectively ASCII
	namespace lut
	{
		static constexpr auto MakeLUT(const std::string_view sAcceptedCharacters)
		{
			std::array<bool, 256> lut{ 0 };
			for (const auto c : sAcceptedCharacters)
				lut.at(uint8_t(c)) = true;
			return lut;
		};

		constexpr auto WhitespaceDigits =
			MakeLUT(" \t\n\r\v\f");
		constexpr auto NumericDigits =
			MakeLUT("0123456789");
		constexpr auto RealNumericDigits =
			MakeLUT(".0123456789");
		constexpr auto OperatorDigits =
			MakeLUT("!$%^&*+-=#@?|`/\\<>~");
		constexpr auto SymbolNameDigits =
			MakeLUT("abcdefghijklmnopqrstuvwxyz.ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789");
		constexpr auto AllowedHexDigits =
			MakeLUT("0123456789abcdefABCDEF");
		constexpr auto AllowedBinaryDigits =
			MakeLUT("01");
	}



	struct Token
	{
		enum class Type : uint8_t
		{
			Unknown,			// ??
			Literal_Numeric,	// 0.1 100 100.463 0b101010 0xABCD
			Literal_String,		// "something"
			Parenthesis_Open,	// (
			Parenthesis_Close,	// )
			Scope_Open,			// {
			Scope_Close,		// }
			EndOfStatement,		// ;
			Separator,			// ,
			Operator,			// +, -, == etc
			Symbol,				// A_named_thing69
			Keyword,			// uses, for, while etc
		};

		Type				type = Type::Unknown;
		std::string			text = "";
		double				value = 0.0;

		std::string str() const
		{
			std::string o;
			switch (type)
			{
			case Token::Type::Unknown:
				o += "[Unknown           ]"; break;
			case Token::Type::Literal_Numeric:
				o += "[Literal, Numeric  ]"; break;
			case Token::Type::Literal_String:
				o += "[Literal, String   ]"; break;
			case Token::Type::Parenthesis_Open:
				o += "[Parenthesis, Open ]"; break;
			case Token::Type::Parenthesis_Close:
				o += "[Parenthesis, Close]"; break;
			case Token::Type::Scope_Open:
				o += "[Scope, Open       ]"; break;
			case Token::Type::Scope_Close:
				o += "[Scope, Close      ]"; break;
			case Token::Type::EndOfStatement:
				o += "[End of Statement  ]"; break;
			case Token::Type::Separator:
				o += "[Separator         ]"; break;
			case Token::Type::Operator:
				o += "[Operator          ]"; break;
			case Token::Type::Symbol:
				o += "[Symbol            ]"; break;
			case Token::Type::Keyword:
				o += "[Keyword           ]"; break;
			}
			

			o += " : " + text;
			if (type == Token::Type::Literal_Numeric)
				o += " [" + std::to_string(value) + "]";
			return o;
		}
	};

	struct Operator
	{
		uint8_t precedence = 0;
		uint8_t arguments = 0;
	};

	class CompileError : public std::exception
	{
	public:
		CompileError(const std::string& sMsg)
		{
			message = sMsg;
		}

	public:
		const char* what() const override
		{
			return message.c_str();
		}

	private:
		std::string message;
	};

	struct Keyword
	{
		uint8_t placeholder_for_now = 0;
	};

	class Compiler
	{
	protected:
		std::unordered_map<std::string, Operator> mapOperators;
		std::unordered_map<std::string, Keyword> mapKeywords;

	public:
		Compiler()
		{
			// Standard Binary Operators
			mapOperators["*"] = { 3, 2 };
			mapOperators["/"] = { 3, 2 };
			mapOperators["+"] = { 1, 2 };
			mapOperators["-"] = { 1, 2 };

			// Unary Operators - special user-invalid signatures, which makes
			// them unique for processing later. This is more convenient than
			// "solving" based on argument count.
			mapOperators["u+"] = { 100, 1 };
			mapOperators["u-"] = { 100, 1 };
		}

	public:
		// Take text input, and form a vector of classified tokens
		std::vector<Token> Parse(const std::string& sInput)
		{
			if (sInput.empty())
				throw CompileError("[PARSER] No input provided");

			std::vector<Token> vecOutputTokens;

			// Finite State Machine Go!
			enum class TokeniserState : uint8_t
			{
				NewToken,
				StringLiteral,
				NumericLiteral,
				FancyNumericLiteral,
				HexNumericLiteral,
				BinNumericLiteral,
				SymbolName,
				Parenthesis_Open,
				Parenthesis_Close,
				Scope_Open,
				Scope_Close,
				EndOfStatement,
				Comma,
				Operator,
				CompleteToken,
			};

			// FSM Variables
			TokeniserState stateNow = TokeniserState::NewToken;
			TokeniserState stateNext = TokeniserState::NewToken;

			// The accumulating "token"
			std::string sCurrentToken = "";
			Token tokCurrent;

			// During parsing, sometimes we just need some ancilliary variables
			// like these...
			bool bDecimalPointFound = false;		// Prevent more than one '.' in a numeric literal
			std::string sFancyNumericLiteralToken;	// We want both "0xABCD" and "ABCD"

			// Quick and dirty way to make sure we have complete syntax
			size_t nParenthesisBalanceChecker = 0;
			size_t nScopeBalanceChecker = 0;

			// NOTE: I'm an iterator...
			auto charNow = sInput.begin();

			// Iterate through input text
			while (charNow != sInput.end())
			{
				// The FSM Implementation
				switch (stateNow)
				{

				// Start here... do First Digit Analysis
				case TokeniserState::NewToken:
				{
					// Reset FSM status
					sCurrentToken.clear();
					tokCurrent = { Token::Type::Unknown, "" };
					bDecimalPointFound = false;

					// First Digit Analysis - use character to hint at what state
					// to go into next. Most of the time its obvious, but literals
					// can be probloemantic and need additional states

					// Check for whitespace				
					if (lut::WhitespaceDigits.at(charNow[0]))
					{
						// Consume it, do nothing
						++charNow;
						stateNext = TokeniserState::NewToken;
					}

					// Check for numeric literals
					//if(std::isdigit(charNow[0])) <- Best choice, but a little slow
					//if(std::string("0123456789").find(charNow[0]) != std::string::npos) <- Easy to use choice, but slow
					//if(charNow[0] >= '0' && charNow[0] <= '9' || charNow[0] == '.') <- Fast choice, but unclear and fiddly
					else if (lut::RealNumericDigits.at(charNow[0])) // <- Fast choice, clear, but not universal
					{
						// A numeric literal has been found
						sCurrentToken = charNow[0];
						

						// If first digit is '0' then it may be a non-base10 literal
						if (charNow[0] == '0')
						{
							// Base16 or Base2
							stateNext = TokeniserState::FancyNumericLiteral;
						}
						else
						{
							// Base10
							stateNext = TokeniserState::NumericLiteral;
						}

						++charNow;
					}

					// Check for operators
					else if (lut::OperatorDigits.at(charNow[0]))
					{
						stateNext = TokeniserState::Operator;
					}

					// Check for parenthesis
					else if (charNow[0] == '(')
					{
						stateNext = TokeniserState::Parenthesis_Open;
					}

					else if (charNow[0] == ')')
					{
						stateNext = TokeniserState::Parenthesis_Close;
					}

					// Check for scope
					else if (charNow[0] == '{')
					{
						stateNext = TokeniserState::Scope_Open;
					}

					else if (charNow[0] == '}')
					{
						stateNext = TokeniserState::Scope_Close;
					}

					// Check for other syntax features
					else if (charNow[0] == ',')
					{
						stateNext = TokeniserState::Comma;
					}

					else if (charNow[0] == ';')
					{
						stateNext = TokeniserState::EndOfStatement;
					}

					// Check for string literal				
					else if (charNow[0] == '\"')
					{
						// Consume opening "quotes", then read in
						++charNow;
						stateNext = TokeniserState::StringLiteral;
					}

					else // Anything left "could" be a symbol, so assume as much
					{
						sCurrentToken = charNow[0];
						++charNow;
						stateNext = TokeniserState::SymbolName;
					}					
				}
				break;

				// Read in a string literal. Currently does not support escape characters. Strings
				// are surrounded with ", eg "hello"
				case TokeniserState::StringLiteral:
				{
					// For now string literals read everything as string characters
					// until a close quotes is found. If EOF is found first then
					// we have a mismatched quotes error
					if (charNow[0] == '\"')
					{
						// Matching close "quotes" found, consume it, and define token
						++charNow;
						stateNext = TokeniserState::CompleteToken;
						tokCurrent = { Token::Type::Literal_String, sCurrentToken };
					}
					else
					{
						sCurrentToken += charNow[0];
						++charNow;
					}
				}
				break;


				// Read in numeric literal. This is a REAL number for now
				case TokeniserState::NumericLiteral:
				{
					// Check for numeric literals
					if (lut::RealNumericDigits.at(charNow[0]))
					{
						// Decimal points are allowed (and are a useful discriminator for real identification)
						if (charNow[0] == '.')
						{
							// But... if this flag is already set, we've more than one in literal - bad
							if (bDecimalPointFound)
							{
								// Error! More than one decimal point in this numeric literal
								throw CompileError("[PARSER] Bad numeric construction");
							}
							else
							{
								// And if it wasn't set, this is the first one (yay!), so set the flag
								bDecimalPointFound = true;
							}
						}

						sCurrentToken += charNow[0];
						++charNow;
						stateNext = TokeniserState::NumericLiteral;
					}
					else
					{
						// Numbers have to be terminated with anything other than a symbol character
						if (lut::SymbolNameDigits.at(charNow[0]))
						{
							// Situation is 12345func - which is never valid
							throw CompileError("[PARSER] Invalid number/symbol");
						}

						// Anything else found indicates the end of this numeric literal
						// Dont consume it, as something else may want it, but time to finish
						stateNext = TokeniserState::CompleteToken;
						tokCurrent = { Token::Type::Literal_Numeric, sCurrentToken };

						// For convenience, store numeric value too
						tokCurrent.value = std::stod(sCurrentToken);
					}
				}
				break;


				// Read in numeric literal. This is a REAL number for now, but could be represented as decimal, binary or hex
				case TokeniserState::FancyNumericLiteral:
				{
					if (charNow[0] == 'x')
					{
						// 0x...... indicates hex (Base16)
						sCurrentToken += charNow[0];
						sFancyNumericLiteralToken.clear();
						++charNow;
						stateNext = TokeniserState::HexNumericLiteral;
					}

					else if (charNow[0] == 'b')
					{
						// 0b...... indicates binary (Base2)						
						sCurrentToken += charNow[0];
						sFancyNumericLiteralToken.clear();
						++charNow;
						stateNext = TokeniserState::BinNumericLiteral;
					}

					else if (lut::RealNumericDigits.at(charNow[0]))
					{
						// Just a regular number so go back to that state
						stateNext = TokeniserState::NumericLiteral;
					}
					else
						throw CompileError("[PARSER] Bad numeric literal");
				}
				break;


				// Read in numeric literal as hex
				case TokeniserState::HexNumericLiteral:
				{
					// Check for numeric literals
					if (lut::AllowedHexDigits.at(charNow[0]))
					{
						sCurrentToken += charNow[0];
						sFancyNumericLiteralToken += charNow[0];
						++charNow;
						stateNext = TokeniserState::HexNumericLiteral;
					}
					else
					{
						// Our rules say a numeric literal cannot contain symbol name digits
						if (lut::SymbolNameDigits.at(charNow[0]))
							throw CompileError("[PARSER] Hexadecimal literal is malformed");

						// Dont consume it, as something else may want it, but time to finish
						stateNext = TokeniserState::CompleteToken;
						tokCurrent = { Token::Type::Literal_Numeric, sCurrentToken };

						// For convenience, store numeric value too
						tokCurrent.value = double(std::stoll(sFancyNumericLiteralToken, nullptr, 16));
					}
				}
				break;



				// Read in numeric literal as binary
				case TokeniserState::BinNumericLiteral:
				{
					// Check for numeric literals
					if (lut::AllowedBinaryDigits.at(charNow[0]))
					{
						sCurrentToken += charNow[0];
						sFancyNumericLiteralToken += charNow[0];
						++charNow;
						stateNext = TokeniserState::BinNumericLiteral;
					}
					else
					{
						// Our rules say a numeric literal cannot contain symbol name digits
						if (lut::SymbolNameDigits.at(charNow[0]))
							throw CompileError("[PARSER] Binary literal is malformed");

						// Dont consume it, as something else may want it, but time to finish
						stateNext = TokeniserState::CompleteToken;
						tokCurrent = { Token::Type::Literal_Numeric, sCurrentToken };

						// For convenience, store numeric value too
						tokCurrent.value = double(std::stoll(sFancyNumericLiteralToken, nullptr, 2));
					}
				}
				break;



				// Read in operator
				case TokeniserState::Operator:
				{
					// Operators need to be greedy. for example, = is valid and == is valid, so what's
					// the largest valid operator we can determine?
					if (lut::OperatorDigits.at(charNow[0]))
					{
						// If we continue to grow the operator, is it valid?
						if (mapOperators.contains(sCurrentToken + charNow[0]))
						{
							// YES - so keep going
							sCurrentToken += charNow[0];
							++charNow;
						}
						else
						{
							// NO - so is the current token a valid operator?
							if (mapOperators.contains(sCurrentToken))
							{
								// YES - bank it, move on								
								tokCurrent = { Token::Type::Operator, sCurrentToken };
								stateNext = TokeniserState::CompleteToken;
							}
							else
							{
								// NO - Current operator token is invalid, but it might be
								// as we accumulate more operator digits
								sCurrentToken += charNow[0];
								++charNow;
							}
						}
					}
					else
					{
						// We've moved into non-operator territory, confirm if
						// current token is an operator an emit, or throw error
						// Confirm final token is an operator
						if (mapOperators.contains(sCurrentToken))
						{
							tokCurrent = { Token::Type::Operator, sCurrentToken };
							stateNext = TokeniserState::CompleteToken;
						}
						else
						{
							throw CompileError("[PARSER] Unrecognised operator: " + sCurrentToken);
						}
					}

				}
				break;


				// Special single characters
				case TokeniserState::Parenthesis_Open:
				{
					sCurrentToken += charNow[0];
					++charNow;
					nParenthesisBalanceChecker++;
					tokCurrent = { Token::Type::Parenthesis_Open, sCurrentToken };
					stateNext = TokeniserState::CompleteToken;
				}
				break;

				case TokeniserState::Parenthesis_Close:
				{
					sCurrentToken += charNow[0];
					++charNow;
					nParenthesisBalanceChecker--;
					tokCurrent = { Token::Type::Parenthesis_Close, sCurrentToken };
					stateNext = TokeniserState::CompleteToken;
				}
				break;

				case TokeniserState::Scope_Open:
				{
					sCurrentToken += charNow[0];
					++charNow;
					nScopeBalanceChecker++;
					tokCurrent = { Token::Type::Scope_Open, sCurrentToken };
					stateNext = TokeniserState::CompleteToken;
				}
				break;

				case TokeniserState::Scope_Close:
				{
					sCurrentToken += charNow[0];
					++charNow;
					nScopeBalanceChecker--;
					tokCurrent = { Token::Type::Scope_Close, sCurrentToken };
					stateNext = TokeniserState::CompleteToken;
				}
				break;

				case TokeniserState::Comma:
				{
					sCurrentToken += charNow[0];
					++charNow;
					tokCurrent = { Token::Type::Separator, sCurrentToken };
					stateNext = TokeniserState::CompleteToken;
				}
				break;

				case TokeniserState::EndOfStatement:
				{
					sCurrentToken += charNow[0];
					++charNow;
					tokCurrent = { Token::Type::EndOfStatement, sCurrentToken };
					stateNext = TokeniserState::CompleteToken;
				}
				break;



				// Read in a symbol
				case TokeniserState::SymbolName:
				{
					if (lut::SymbolNameDigits.at(charNow[0]))
					{
						sCurrentToken += charNow[0];
						++charNow;
					}
					else
					{
						// May as well identify a keyword here, it's free to do
						// and could be used to configure the parser in real time
						if (mapKeywords.contains(sCurrentToken))
							tokCurrent = { Token::Type::Keyword, sCurrentToken };
						else
							tokCurrent = { Token::Type::Symbol, sCurrentToken };

						stateNext = TokeniserState::CompleteToken;
					}
				}
				break;



				// Store currently accumulating token
				case TokeniserState::CompleteToken:
				{
					// Emit composited token into container
					vecOutputTokens.push_back(tokCurrent);
					stateNext = TokeniserState::NewToken;
				}
				break;
				}

				// Update FSM only here, this ensures no state changes during 
				// implementation.
				stateNow = stateNext;
			}

			// Sanity Checks 
			if (nParenthesisBalanceChecker != 0)
				throw CompileError("[PARSER] Parenthesis '(' & ')' not balanced");

			if (nScopeBalanceChecker != 0)
				throw CompileError("[PARSER] Scope Brackets '{' & '}' not balanced");

			// We reached end of the input string, but we may still be in the middle
			// of parsing a token
			if (stateNow == TokeniserState::StringLiteral)
			{
				// Mismatched quotation error
				throw CompileError("[PARSER] Missing quotation mark - \"");
			}

			return vecOutputTokens;
		}

		// Solve like a calculator the stream of parsed tokens, using the Shunting Yard Algorithm
		double Solve(const std::vector<Token>& vTokens)
		{
			std::deque<Token> stkHolding;
			std::deque<Token> stkOutput;

			Token tokPrevious = { Token::Type::Literal_Numeric };
			int pass = 0;

			for (const auto& token : vTokens)
			{
				if (token.type == Token::Type::Literal_Numeric)
				{
					// Push literals straight to output, they are already in order
					stkOutput.push_back(token);
					tokPrevious = stkOutput.back();
				}
				else if (token.type == Token::Type::Parenthesis_Open)
				{
					// Push to holding stack, it acts as a stopper when we back track
					stkHolding.push_front(token);
					tokPrevious = stkHolding.front();
				}
				else if (token.type == Token::Type::Parenthesis_Close)
				{
					// Check something is actually wrapped by parenthesis
					if (stkHolding.empty())
					{
						throw CompileError("[SOLVER] Unexpected close parenthesis");
					}

					// Backflush holding stack into output until open parenthesis
					while (!stkHolding.empty() && stkHolding.front().type != Token::Type::Parenthesis_Open)
					{
						stkOutput.push_back(stkHolding.front());
						stkHolding.pop_front();
					}

					// Check if open parenthesis was actually found
					if (stkHolding.empty())
					{
						throw CompileError("[SOLVER] No open parenthesis found");
					}

					// Remove corresponding open parenthesis from holding stack
					if (!stkHolding.empty() && stkHolding.front().type == Token::Type::Parenthesis_Open)
					{
						stkHolding.pop_front();
					}

					tokPrevious = { Token::Type::Parenthesis_Close };
				}

				else if (token.type == Token::Type::Operator)
				{
					// Symbol is operator
					std::string op_text = token.text;

					// Unary Operator check
					if (token.text == "-" || token.text == "+")
					{
						if ((tokPrevious.type != Token::Type::Literal_Numeric
							&& tokPrevious.type != Token::Type::Parenthesis_Close) || pass == 0)
						{
							// "Upgrade" operator
							op_text = "u" + token.text;
						}
					}

					while (!stkHolding.empty() && stkHolding.front().type != Token::Type::Parenthesis_Open)
					{
						// Ensure holding stack front is an operator (it might not be later...)
						if (stkHolding.front().type == Token::Type::Operator)
						{
							const auto& holding_stack_op = mapOperators[stkHolding.front().text];

							if (holding_stack_op.precedence >= mapOperators[op_text].precedence)
							{
								stkOutput.push_back(stkHolding.front());
								stkHolding.pop_front();
							}
							else
								break;
						}
					}

					// Push the new operator onto the holding stack
					stkHolding.push_front({ Token::Type::Operator, op_text });
					tokPrevious = stkHolding.front();
				}
				else
				{
					throw CompileError("[SOLVER] Expression contains elements that can't be solved (yet...)");
				}

				pass++;
			}

			// Drain the holding stack
			while (!stkHolding.empty())
			{
				stkOutput.push_back(stkHolding.front());
				stkHolding.pop_front();
			}

			// Solver (Almost identical to video 1)
			std::deque<double> stkSolve;

			for (const auto& inst : stkOutput)
			{
				switch (inst.type)
				{
				case Token::Type::Literal_Numeric:
				{
					stkSolve.push_front(inst.value);
				}
				break;

				case Token::Type::Operator:
				{
					const auto& op = mapOperators[inst.text];

					std::vector<double> mem(op.arguments);
					for (uint8_t a = 0; a < op.arguments; a++)
					{
						if (stkSolve.empty())
						{
							throw CompileError("[SOLVER] Expression is malformed");
						}
						else
						{
							mem[a] = stkSolve[0];
							stkSolve.pop_front();
						}
					}

					double result = 0.0;
					if (op.arguments == 2)
					{
						if (inst.text == "/") result = mem[1] / mem[0];
						if (inst.text == "*") result = mem[1] * mem[0];
						if (inst.text == "+") result = mem[1] + mem[0];
						if (inst.text == "-") result = mem[1] - mem[0];
					}

					if (op.arguments == 1)
					{
						if (inst.text == "u+") result = +mem[0];
						if (inst.text == "u-") result = -mem[0];
					}

					stkSolve.push_front(result);
				}
				break;

				default:
					throw CompileError("[SOLVER] Unexpected Token");
				}
			}

			if (stkSolve.empty())
				return 0;
			else
				return stkSolve.front();

		}
	};
}


int main()
{
	std::string sExpression;

	std::cout << "Type 'quit' to exit\n\n";
	do
	{
		// Read in expression
		std::cout << "\nSolve: ";
		std::getline(std::cin, sExpression);

		// Append "end-of-statement" character. This serves two purposes:
		// 1) It allows the penultimate token to be fully parsed before the FSM loop exits
		// 2) It indicates end-of-statement, which has utility later on
		sExpression += ";";

		// Our compiler could throw exceptions. This is convenient as it 
		// removes the need to check error states all the time
		try
		{
			// Create compiler instance
			olc::lang::Compiler compiler;
			// Parse the input expression
			auto vTokens = compiler.Parse(sExpression);
			// Display the parsing result
			for (const auto& token : vTokens)
				std::cout << token.str() << "\n";
			// Solve the expression
			auto result = compiler.Solve(vTokens);
			// Display result
			std::cout << "Result: " << std::to_string(result) << "\n";
		}
		catch (olc::lang::CompileError& e)
		{
			// Oops, something happened so display what
			std::cout << e.what() << "\n";
		}
	} 
	while (sExpression != "quit;"); // Continue until user types quit	
	return 0;
}
