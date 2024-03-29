
--	Embedding Lua in C++ Part #3
--	"2024 sucks" - javidx9
--
--	License (OLC-3)
--	~~~~~~~~~~~~~~~
--
--	Copyright 2018-2024 OneLoneCoder.com
--
--	Redistribution and use in source and binary forms, with or without
--	modification, are permitted provided that the following conditions
--	are met:
--
--	1. Redistributions or derivations of source code must retain the above
--	copyright notice, this list of conditions and the following disclaimer.
--
--	2. Redistributions or derivative works in binary form must reproduce
--	the above copyright notice. This list of conditions and the following
--	disclaimer must be reproduced in the documentation and/or other
--	materials provided with the distribution.
--
--	3. Neither the name of the copyright holder nor the names of its
--	contributors may be used to endorse or promote products derived
--	from this software without specific prior written permission.
--
--	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
--	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
--	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
--	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
--	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
--	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
--	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
--	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
--	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
--	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
--	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--
--	Relevant Video: https://youtu.be/ONywOoJC4TQ
--
--	Links
--	~~~~~
--	YouTube:	https://www.youtube.com/javidx9
--				https://www.youtube.com/javidx9extra
--	Discord:	https://discord.gg/WhwHUMV
--	Twitter:	https://www.twitter.com/javidx9
--	Twitch:		https://www.twitch.tv/javidx9
--	GitHub:		https://www.github.com/onelonecoder
--	Patreon:	https://www.patreon.com/javidx9
--	Homepage:	https://www.onelonecoder.com
--
--	Author
--	~~~~~~
--	David Barr, aka javidx9, ©OneLoneCoder 2024



-- Stage 2: Reading Simple Values
a = 5
b = 7
c = a + b
d = "OneLoneCoder.com"

-- Stage 3: Reading a simple object
player = {Title = "Squire", Name = "Ciaran", Family = "Wirral", Level = 20}

-- Stage 4: Calling a Lua Function
function CalledFromCPP1(r1, r2, r3)
	io.write("[LUA S4] CalledFromCPP1("..r1..", "..r2..", "..r3..")\n")
	return r1 * r2
end


-- Stage 5: Calling a C++ Function
function CalledFromCPP2(r1, r2)
	io.write("[LUA S5] CalledFromCPP2("..r1..", "..r2..")\n")
	io.write("[LUA S5] Calling HostFunction("..r1..", "..r2..")\n")
	r = HostFunction(r1, r2)
	io.write("[LUA S5] HostFunction returned "..r.."\n")
	return r * 15
end
	
-- Stage 7: New Stuff
function CreatePlayer1() 

	io.write("[LUA S7] CreatePlayer1() called\n")

	local p = Player:new()

	p.Name = "Saladin"
	p.Family = "Sheffield"
	p.Title = "King"
	p.Level = 35 

	return p
end

-- Stage 8: Create X players
function CreatePlayer2(n)

	io.write("[LUA S8] CreatePlayer2() called\n")

	allPlayers:clear()

	for i=1,n do
		allPlayers:add(CreatePlayer1())
	end
end