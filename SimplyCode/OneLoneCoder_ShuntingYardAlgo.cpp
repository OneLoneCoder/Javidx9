/*
	DIY Programming Language #1
	Expressions To Solutions
	The Shunting Yard Algorithm

	"Francis, have a rest buddy, then get your guitar" - javidx9

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

	Relevant Video: https://youtu.be/unh6aK8WMwM

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
#include <vector>
#include <string>
#include <unordered_map>
#include <deque>

int main()
{
	struct sOperator
	{
		uint8_t precedence = 0;
		uint8_t arguments = 0;
	};

	std::unordered_map<char, sOperator> mapOps;
	mapOps['/'] = { 4, 2 };
	mapOps['*'] = { 3, 2 };
	mapOps['+'] = { 2, 2 };
	mapOps['-'] = { 1, 2 };

	std::string sExpression = "-((1+2)/((6*-7)+(7*-4)/2)-3)";

	struct sSymbol
	{
		std::string symbol = "";

		enum class Type : uint8_t
		{
			Unknown,
			Literal_Numeric,
			Operator,
			Parenthesis_Open,
			Parenthesis_Close
		} type = Type::Unknown;

		sOperator op;
	};

	std::deque<sSymbol> stkHolding;
	std::deque<sSymbol> stkOutput;

	sSymbol symPrevious = { "0", sSymbol::Type::Literal_Numeric, 0, 0 };
	int pass = 0;

	for (const char c : sExpression)
	{
		if (std::isdigit(c))
		{
			// Push literals straight to output, they are already in order
			stkOutput.push_back({ std::string(1, c), sSymbol::Type::Literal_Numeric });
			symPrevious = stkOutput.back();
		}
		else if (c == '(')
		{
			// Push to holding stack, it acts as a stopper when we back track
			stkHolding.push_front({ std::string(1, c), sSymbol::Type::Parenthesis_Open });
			symPrevious = stkHolding.front();
		}
		else if (c == ')')
		{
			// Backflush holding stack into output until open parenthesis
			while (!stkHolding.empty() && stkHolding.front().type != sSymbol::Type::Parenthesis_Open)
			{
				stkOutput.push_back(stkHolding.front());
				stkHolding.pop_front();
			}

			if (stkHolding.empty())
			{
				std::cout << "!!!!     ERROR! Unexpected parenthesis '" << std::string(1, c) << "'\n";
				return 0;
			}

			// Remove corresponding open parenthesis from holding stack
			if (!stkHolding.empty() && stkHolding.front().type == sSymbol::Type::Parenthesis_Open)
			{
				stkHolding.pop_front();
			}

			symPrevious = { std::string(1, c), sSymbol::Type::Parenthesis_Close };

		}
		else if (mapOps.contains(c))
		{
			// Symbol is operator
			sOperator new_op = mapOps[c];

			if (c == '-' || c == '+')
			{
				if ((symPrevious.type != sSymbol::Type::Literal_Numeric
					&& symPrevious.type != sSymbol::Type::Parenthesis_Close) || pass == 0)
				{
					new_op.arguments = 1;
					new_op.precedence = 100;
				}
			}

			while (!stkHolding.empty() && stkHolding.front().type != sSymbol::Type::Parenthesis_Open)
			{
				// Ensure holding stack front is an operator (it might not be later...)
				if (stkHolding.front().type == sSymbol::Type::Operator)
				{
					const auto& holding_stack_op = stkHolding.front().op;

					if (holding_stack_op.precedence >= new_op.precedence)
					{
						stkOutput.push_back(stkHolding.front());
						stkHolding.pop_front();
					}
					else
						break;
				}
			}

			// Push the new operator onto the holding stack
			stkHolding.push_front({ std::string(1, c), sSymbol::Type::Operator, new_op });
			symPrevious = stkHolding.front();
		}
		else
		{
			std::cout << "Bad Symbol: '" << std::string(1, c) << "'\n";
			return 0;
		}

		pass++;
	}


	// Drain the holding stack
	while (!stkHolding.empty())
	{
		stkOutput.push_back(stkHolding.front());
		stkHolding.pop_front();
	}

	std::cout << "Expression:= " << sExpression << "\n";
	std::cout << "RPN       := ";
	for (const auto& s : stkOutput)
	{
		std::cout << s.symbol;
	}
	std::cout << "\n";

	// Solver
	std::deque<double> stkSolve;

	for (const auto& inst : stkOutput)
	{
		switch (inst.type)
		{
		case sSymbol::Type::Literal_Numeric:
		{
			stkSolve.push_front(std::stod(inst.symbol));
		}
		break;

		case sSymbol::Type::Operator:
		{
			std::vector<double> mem(inst.op.arguments);
			for (uint8_t a = 0; a < inst.op.arguments; a++)
			{
				if (stkSolve.empty())
				{
					std::cout << "!!!     ERROR! Bad Expression\n";
				}
				else
				{
					mem[a] = stkSolve[0];
					stkSolve.pop_front();
				}
			}

			double result = 0.0;
			if (inst.op.arguments == 2)
			{
				if (inst.symbol[0] == '/') result = mem[1] / mem[0];
				if (inst.symbol[0] == '*') result = mem[1] * mem[0];
				if (inst.symbol[0] == '+') result = mem[1] + mem[0];
				if (inst.symbol[0] == '-') result = mem[1] - mem[0];
			}

			if (inst.op.arguments == 1)
			{
				if (inst.symbol[0] == '+') result = +mem[0];
				if (inst.symbol[0] == '-') result = -mem[0];
			}


			stkSolve.push_front(result);
		}
		break;

		}
	}

	std::cout << "Result    := " << std::to_string(stkSolve[0]) << "\n";
	return 0;
}