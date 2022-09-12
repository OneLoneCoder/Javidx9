/*
OneLoneCoder.com - MicroGame1 - Cave Diver
"Err..." - @Javidx9

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
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
Often as an exercise, I'll try to write code where every byte of source code
should count. I feel that this is an excellent way to practice logic, code
comprehension and smarmy arrogance.



Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/oJGx8cqaJLc

Last Updated: 16/10/2017
*/

#include <iostream>
#include <thread>
using namespace std;
using namespace chrono;

// NOTE!! WILL ONLY WORK WITH UPDATED WINDOWS 10
#include <Windows.h>

int main()
{
	DWORD d = 0;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &d);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), d | 0x0004); // ??? Mystery Console Mode Feature!
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &d);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), d | 0x0200);

	auto tp1 = system_clock::now();	auto tp2 = tp1;
	char s[80 * 30] = {'\0'};
	char n[10];
	
	auto pfxy = [](const char* b, int x, int y)	{printf("\x1B[%d;%dH%s", y, x, b);};
	auto sbxy = [&](int x, int y, char c){s[y * 80 + x] = c; };

	float t = 0.0f, f = 0.0f, tt = 0.0f; // Timing Variables
	float w = 20.0f, m = 40.0f, tw = 20.0f, tm = 40.0f; // World Shape Variables
	float e = 1.0f, c = 40.0f; // Difficulty, Player Position
	int r = 0, h = 1000; // Player Score
	
	while (h>0)
	{
		// Timing
		this_thread::sleep_for(10ms);
		tp2 = system_clock::now();
		duration<float> elapsedTime = tp2 - tp1; tp1 = tp2;
		t+=(f=elapsedTime.count());	tt += f;
		
		// Get and Handle Input
		for (int i = 0; i < 2; i++)	if (GetAsyncKeyState("\x25\x27"[i])!= 0) c += (i?1:-1) * 40.0f * f;

		// Update World
		if (t >= 1.5f){	t -= 1.5f;	tw = rand() % 10 + 10;	tm = rand() % (76 - ((int)tw >> 1)) + 4; }
		e += f*0.001f;	w += (tw - w) * e * f; m += (tm - m) * e * f;
		float p = sinf(tt * e) * m / 2.0f + 40.0f;

		// Fill Row
		for (int x = 0; x < 79; x++)
			if (x+1 < p-w/2 || x> p+w/2) sbxy(x,r,'.');
			else if (x == (int)(p - (w / 2)) || x== (int)(p + (w / 2)))	sbxy(x, r, '#');
			else sbxy(x,r,' ');
	
		// Scrolling Effect
		sbxy(79, r, '\0'); (r += 1) %= 28;

		// Draw To Screen
		for (int y = 1; y < 28; y++) pfxy(&s[((y+28+r)%28) * 80], 0, y);
		pfxy("O-V-O", c - 2, 2); pfxy(" O-O ", c - 2, 3); pfxy("  V  ", c - 2, 4);
		
		// Collision Checking & Health Update
		h -= s[((4 + r) % 28) * 80 + ((int)c - 2)] == '.';
		
		// Draw HUD	
		pfxy("===============================================================================", 0, 28);
		pfxy("Cave Diver - www.onelonecoder.com - Left Arrow / Right Arrow - Survive!", 2, 29);
		_itoa_s((int)(tt * 100.0f), n, 10, 10);
		pfxy("Distance Fallen:            ", 2, 30); pfxy(n, 19, 30);

		_itoa_s(h, n, 10, 10);
		pfxy("Health:             ", 40, 30); pfxy(n, 48, 30);
	}

	pfxy("Dead...\n", 70, 30);
	system("pause"); // :-P
	return 0;
}
