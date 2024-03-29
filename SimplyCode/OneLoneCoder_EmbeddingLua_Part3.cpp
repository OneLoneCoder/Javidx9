/*
	Embedding Lua in C++ Part #3
	"2024 sucks" - javidx9

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

	Relevant Video: https://youtu.be/ONywOoJC4TQ

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
	David Barr, aka javidx9, �OneLoneCoder 2024
*/

#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")


int main()
{
	// Step 0) Initialise Lua
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);



	// Step 1) Load & Parse File
	try
	{
		lua.safe_script_file("EmbeddingLua3.lua");
		std::cout << "[CPP S1] Lua File read OK!\n";
	}
	catch (const sol::error& e)
	{
		// Something went wrong with loading this script
		std::cout << std::string(e.what()) << "\n";
		return 0;
	}

	// Step 2) Read Simple Variables
	std::cout << "[CPP S2] a = " << lua["a"].get_or(0) << "\n";
	std::cout << "[CPP S2] b = " << lua["b"].get_or(0) << "\n";
	std::cout << "[CPP S2] c = " << lua["c"].get_or(0) << "\n";
	std::cout << "[CPP S2] d = " << lua["d"].get_or<std::string>("0") << "\n";

	// Explicit alternative
	auto a_double = lua["a"].get<double>();
	std::cout << "[CPP S2] a_double = " << a_double << "\n";

	// Existence Checking
	sol::optional<double> an_optional_double = lua["a"];
	if (an_optional_double)
		std::cout << "[CPP S2] an_optional_double = " << an_optional_double.value() << "\n";

	// ^ Workaround for
	auto an_optional_double_fix = lua["a"].get<std::optional<double>>();
	if (an_optional_double_fix)
		std::cout << "[CPP S2] an_optional_double_fix = " << an_optional_double_fix.value() << "\n";

	// Step 3) Read Table
	struct Player
	{
		std::string title;
		std::string name;
		std::string family;
		int level;
	} player;

	std::optional<sol::table> a_table = lua["player"];
	if (a_table)
	{
		for (const auto& entry : a_table.value())
		{
			sol::object key = entry.first;
			sol::object value = entry.second;

			std::string sKey = key.as<std::string>();

			if (sKey == "Name") player.name = value.as<std::string>();
			if (sKey == "Family") player.family = value.as<std::string>();
			if (sKey == "Title") player.title = value.as<std::string>();
			if (sKey == "Level") player.level = value.as<int>();

		}

		std::cout << "[CPP S3] a_table = " << player.title << " " << player.name << " of "
			<< player.family << " [Lvl: " << player.level << "]\n";
	}

	// Or even simpler
	sol::table another_table = lua["player"];
	player.name = another_table["Name"].get<std::string>();
	player.family = another_table["Family"].get<std::string>();
	player.title = another_table["Title"].get<std::string>();
	player.level = another_table["Level"].get<int>();

	std::cout << "[CPP S3] another_table = " << player.title << " " << player.name << " of "
		<< player.family << " [Lvl: " << player.level << "]\n";

	// Step 4) Calling a Lua Function
	lua["CalledFromCPP1"](5.0f, 6.0f, "Bwa Ha Ha!");

	// Function might return something
	float x1 = lua["CalledFromCPP1"](5.0f, 6.0f, "Bwa Ha Ha!");
	std::cout << "[CPP S4] 'CalledFromCPP1' returned " << x1 << "\n";


	// Safely call function
	sol::protected_function function_object = lua["CalledFromCPP1"];
	if (function_object)
	{
		auto x2 = function_object(5.0f, 6.0f, "Bwa Ha Ha!");
		if (x2.valid())
		{
			std::cout << "[CPP S4] 'CalledFromCPP1' returned " << x2.get<float>() << "\n";
		}
		else
		{
			std::cout << "[CPP S4] 'CalledFromCPP1' FAILED!!!\n";	
			sol::error e = x2;
			std::cout << e.what() << "\n";
		}
	}

	// Step 5) Call Lua Function, which calls C++ Function!
	lua["HostFunction"] = [](float a, float b)
		{
			std::cout << "[CPP S5] HostFunction(" << a << ", " << b << ") called from Lua\n";
			return a * b;
		};

	float x3 = lua["CalledFromCPP2"](5.0f, 6.0f);

	// Step 6) Inject variables into Lua state machine
	lua["entirely_new_variable"] = 42;

	lua.script("function entirely_new_function(a) "
				"    return entirely_new_variable * a "
				"end");

	int x4 = lua["entirely_new_function"](3);
	std::cout << "[CPP S6] x4 = " << x4 << "\n";

	// Step 7) Map C++ Types to Lua "Types"
	lua.new_usertype<Player>("Player",
		sol::constructors<Player()>(),
		"Name", sol::property(&Player::name),
		"Family", sol::property(&Player::family),
		"Title", sol::property(&Player::title),
		"Level", sol::property(&Player::level));


	Player player1 = lua["CreatePlayer1"]();

	std::cout << "[CPP S7] player1 = " << player1.title << " " << player1.name << " of "
		<< player1.family << " [Lvl: " << player1.level << "]\n";



	// Step 8) Use C++ Containers in Lua
	std::vector<Player> vecPlayers;
	lua["allPlayers"] = &vecPlayers;

	std::cout << "[CPP S9] vecPlayers.size() = " << vecPlayers.size() << "\n";
	lua["CreatePlayer2"](13);
	std::cout << "[CPP S9] vecPlayers.size() = " << vecPlayers.size() << "\n";

	for (const auto& p : vecPlayers)
		std::cout << "[CPP S9] player1 = " << p.title << " " << p.name << " of "
		<< p.family << " [Lvl: " << p.level << "]\n";

	return 0;
}