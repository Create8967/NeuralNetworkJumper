/* B I R D F U L  G R A P H I C S
   Console Graphics by Uniprism
*/
#pragma once

#include <iostream>
#include <math.h>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <chrono>

using namespace std;

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

int ScrH = 32;
int ScrW = 32;

float pi = 3.141592653f;

CHAR_INFO* buf = new CHAR_INFO[ScrW * ScrH];

void hidecursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void PxlSize(int w, int h)
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = w;                   // Width of each character in the font
	cfi.dwFontSize.Y = h;                  // Height
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(hStdOut, FALSE, &cfi);

}

void NoSelect()
{
	HANDLE hInput;
	DWORD prev_mode;
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, prev_mode & ENABLE_EXTENDED_FLAGS);
}
bool IsBound(int x, int y)
{
	bool val = false;
	if (x >= 0 && x < ScrW)
	{
		if (y >= 0 && y < ScrH)
		{
			val = true;
		}
	}
	return val;
}
void Draw(int x, int y, int col, int shade)
{
	if (IsBound(x, y))
	{
		short ch = 0x2593;
		switch (shade)
		{
		case 0:
			ch = 0x2593;
		case 1:
			ch = 0x2592;
		case 2:
			ch = 0x2591;
		case 3:
			ch = 0x2588;
		}
		//Full block = 0x2588, Dark shade = 0x2593, Medium shade = 0x2592, Light shade = 0x2591
		int pos = y * ScrW + x;
		buf[pos].Char.UnicodeChar = ch;
		buf[pos].Attributes = col * 16 + col;
	}
}
void DrawC(int x, int y, int col, int colB, int shade)
{
	if (IsBound(x, y))
	{
		short ch = 0x2591;
		switch (shade)
		{
		case 0:
			ch = 0x2593;
		case 1:
			ch = 0x2592;
		case 2:
			ch = 0x2591;
		case 3:
			ch = 0x2588;
		}
		//Full block = 0x2588, Dark shade = 0x2593, Medium shade = 0x2592, Light shade = 0x2591
		int pos = y * ScrW + x;
		buf[pos].Char.UnicodeChar = ch;
		buf[pos].Attributes = colB * 16 + col;
	}
}
void Line(int x1, int y1, int x2, int y2, int c)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1; dy = y2 - y1;
	dx1 = abs(dx); dy1 = abs(dy);
	px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1; y = y1; xe = x2;
		}
		else
		{
			x = x2; y = y2; xe = x1;
		}

		Draw(x, y, c, 3);

		for (i = 0; x < xe; i++)
		{
			x = x + 1;
			if (px < 0)
				px = px + 2 * dy1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			Draw(x, y, c, 3);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1; y = y1; ye = y2;
		}
		else
		{
			x = x2; y = y2; ye = y1;
		}

		Draw(x, y, c, 3);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			Draw(x, y, c, 3);
		}
	}
}
void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int col)
{
	Line(x1, y1, x2, y2, col);
	Line(x2, y2, x3, y3, col);
	Line(x3, y3, x1, y1, col);
}
char GetDrawP(int x, int y)
{
	return buf[y * ScrW + x].Char.UnicodeChar;
}
int GetDrawC(int x, int y)
{
	return buf[y * ScrW + x].Attributes;
}
void setBuf()
{
	COORD sumCoords = { ScrW, ScrH };
	SetConsoleScreenBufferSize(hStdOut, sumCoords);
}
void Fill(int x1, int y1, int x2, int y2, int col)
{
	for (int i = y1; i < y2; i++)
	{
		for (int j = x1; j < x2; j++)
			Draw(j, i, col, 3);
	}
}
void FillB(int x1, int y1, int x2, int y2, int col)
{
	for (int i = y1; i < y2; i++)
	{
		for (int j = x1; j < x2; j++)
		{
			if (i == y1 || i == y2 - 1)
				Draw(j, i, col, 3);
			if (j == x1 || j == x2 - 1)
				Draw(j, i, col, 3);
		}
	}
}
void clear()
{
	for (int i = 0; i < ScrW * ScrH; i++)
	{
		buf[i].Char.UnicodeChar = 0x2588;
		buf[i].Attributes = 15;
	}
}

void DrawCirc(int x, int y, int r, int col)
{
	for (int i = y - r; i < y + r; i++)
	{
		for (int j = x - r; j < x + r; j++)
		{
			if (((i - y) * (i - y)) + ((j - x) * (j - x)) < r * r)
			{
				Draw(j, i, col, 3);
			}
		}
	}
}
void DrawCircLP(int x, int y, int r, int thk, int col)
{
	float r2 = (r - thk) * (r - thk);
	for (int i = y - r; i < y + r; i++)
	{
		for (int j = x - r; j < x + r; j++)
		{
			if (((i - y) * (i - y)) + ((j - x) * (j - x)) < r * r && ((i - y) * (i - y)) + ((j - x) * (j - x)) >= r2)
			{
				Draw(j, i, col, 3);
			}
		}
	}
}
void setup(int w, int h, int wid, int hig)
{
	HWND wind = GetConsoleWindow();
	RECT r;
	GetWindowRect(wind, &r);
	ScrW = wid;
	ScrH = hig;
	buf = new CHAR_INFO[ScrW * ScrH];
	PxlSize(w, h);
	NoSelect();
	hidecursor();
	MoveWindow(wind, r.left, r.top, 1920,1080, TRUE);
	setBuf();
}

void update()
{
	SMALL_RECT window = { 0,0 ,ScrW, ScrH };
	COORD BufSize = { ScrW, ScrH };
	WriteConsoleOutputW(hStdOut, buf, BufSize, { 0,0 }, &window);
}
bool GetKey(char Key)
{
	return GetAsyncKeyState((unsigned short)Key) & 0x8000;
}