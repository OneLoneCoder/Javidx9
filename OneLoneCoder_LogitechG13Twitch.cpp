/*
	OneLoneCoder.com - Program a Logitech Keyboard To Display Twitch Chat
	"Put Your Money Where Your Mouth Is" - @Javidx9
	
	License
	~~~~~~~
	Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details. 
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE

	From Javidx9 :)
	~~~~~~~~~~~~~~~
	Hello! Ultimately I don't care what you use this for. It's intended to be 
	educational, and perhaps to the oddly minded - a little bit of fun. 
	Please hack this, change it and use it in any way you see fit. You acknowledge 
	that I am not responsible for anything bad that happens as a result of 
	your actions. However this code is protected by GNU GPLv3, see the license in the
	github repo. This means you must attribute me if you use it. You can view this
	license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	Cheers!
	
	Background
	~~~~~~~~~~
	A very minimal example of using the Logitech SDK to interface with a keyboard display,
	then uses a really sloppy implementation of sockets, to connect to a twitch chat
	session, displaying the chat on the keybaord screen.
	
	Future Modifications
	~~~~~~~~~~~~~~~~~~~~
	1) Wrap around text display on screen
	
	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com
	
	Video:
	~~~~~~
	https://youtu.be/8UXCo-GhiF0
	
	Last Updated: 23/05/2017
*/

#include <iostream>
#include <list>
using namespace std;

// Include Winsock
#include <winsock2.h>
#include <ws2tcpip.h>

// Include Logitech library
#include "LogitechLCDLib.h"

int main()
{
	// Load the driver
	LogiLcdInit(L"OneLoneCoder Test", LOGI_LCD_TYPE_MONO);

	// Check hardware is connected
	if (!LogiLcdIsConnected(LOGI_LCD_TYPE_MONO))
		wcout << "Hardware not found" << endl;

	// Load WinSock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		wcout << "Could not start WinSock" << endl;

	// Get address to twitch server
	struct addrinfo *addr = nullptr;
	if (getaddrinfo("irc.chat.twitch.tv", "6667", nullptr, &addr) != 0)
		wcout << "Failed to get address info" << endl;

	// Create socket
	SOCKET sock = INVALID_SOCKET;
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

	// Connect to server via socket
	int i = connect(sock, addr->ai_addr, (int)addr->ai_addrlen);
	if (i != SOCKET_ERROR)
	{
		// Handshake with twitch
		string s;

		// Authenticate
		s = "PASS oauth:###INSERT YOUR OAUTH HERE###\r\n";
		send(sock, s.c_str(), s.length(), 0);

		// Register your twitch name
		s = "NICK javidx9\r\n";
		send(sock, s.c_str(), s.length(), 0);

		// Join a twitch chat
		s = "JOIN #javidx9\r\n";
		send(sock, s.c_str(), s.length(), 0);
	}
	
	char buffer[100];
	string s;
	list<string> sLines = { "","","","" };

	while (1)
	{
		// Get Twitch Server Message
		int i = recv(sock, buffer, 100, 0);
		for (int j = 0; j < i; j++)
		{
			s.append(1, buffer[j]);
			if (buffer[j] == '\n')
			{
				// User name is between first ':' and '!'. 
				// User chat is after second ':'
				size_t m = s.find('!');
				string sUserName = s.substr(1, m - 1);
				size_t n = s.find(':', m);
				if (n != string::npos)
				{
					string chat = s.substr(n + 1);
					cout << sUserName.c_str() << ": " << chat.c_str() << endl;

					sLines.pop_front();
					sLines.push_back(sUserName + ": " + chat);
				}
				s.clear();
			}
		}

		// Display 4 lines of text from list
		int p = 0;
		for (auto k : sLines)
		{
			wstring ws;
			ws.assign(k.begin(), k.end());
			LogiLcdMonoSetText(p, (wchar_t*)ws.c_str()); // yuck...
			p++;

		}

		// Update display
		LogiLcdUpdate();
	}

	LogiLcdShutdown();
	return 0;
}
