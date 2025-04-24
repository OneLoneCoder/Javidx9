/*
	Code-It-Yourself: Remote Home Energy Monitor
	"C'mon Mum, Keep going!" - javidx9

	Video: https://youtu.be/NJXEzL3peIA

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2025 OneLoneCoder.com

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

	Video:
	~~~~~~
	https://youtu.be/NJXEzL3peIA

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	X:			https://www.x.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, (c) OneLoneCoder 2025
*/

#include "olcWebSocketServer.h"

class Server : public olc::WebSocketServer
{
public:
    Server() : olc::WebSocketServer(
        12345, 
        "fullchain.pem", 
        "privkey.pem", 0.1f)
    {

    }

    bool OnClientConnect(const ws_connection& client)
    {
        std::cout << "[" + client.address->first + ":" + std::to_string(client.uid) + "] Connected\n";
        return true;
    }
    
    void OnClientDisconnect(const ws_connection& client)
    {
        std::cout << "[" + client.address->first + ":" + std::to_string(client.uid) + "] Disconnected\n";
    }

    bool OnClientIncoming(const ws_connection& client, const std::string& data, bool text_mode)
    {
        if(text_mode)
        {
            if(std::string(data.c_str() + 5) != "15")
            {
                std::string s = "{\"x\":" + std::to_string(nTick) + ",\"y\":" + std::string(data.c_str() + 5) + "}";
                nTick++;
                WriteToAll(s);
                std::cout << s << std::endl;
            }
        }
        return true;
    }


    bool OnUpdate(const float fElapsedTime)
    {
        return true;
    }

    bool OnValidateIncoming(const std::string& data)
    {   
        if(data.size() > 5)
        {
            if(std::strncmp(data.c_str(), "drb42", 5) == 0)
                return true;
        }     

        return false;
    }

private:
    int nTick = 0;
};

int main()
{
    Server server;
    server.Start();
    return 0;
}