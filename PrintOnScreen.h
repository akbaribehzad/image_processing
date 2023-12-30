#pragma once
#include <iostream>
#include <windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")


void GotoXY(int x, int y)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    COORD xy = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hStdOut, xy);
}


void ClearScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count)) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count)) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}


COORD GetConsoleWindowSize()
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return { 0, 0 };

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return { 0, 0 };

    return
    {
      (short)(csbi.srWindow.Right - csbi.srWindow.Left + 1),
      (short)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1)
    };
}


int display(const char* path)
{

    // First we'll clear the console and position our cursor for future update
    // (We do this NOW since anything we do will likely clobber some of the image we draw)

    ClearScreen();
    COORD size = GetConsoleWindowSize();
    GotoXY(0, size.Y - 3);
    Sleep(150); // wait a moment for the above commands to update to the console

    // Here we draw on the console.
    // Remember, the console does not care about what we draw on it.
    // Anything we ask the console to do later may clobber all or part of it.

    HWND hconsole = GetConsoleWindow();
    if (!hconsole) return 2;

    HBITMAP hbitmap = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hbitmap) return 3;

    BITMAP bm;
    GetObject(hbitmap, sizeof(bm), &bm);

    HDC hdc = GetDC(hconsole);
    {
        HDC hdc_bitmap = CreateCompatibleDC(hdc);
        SelectObject(hdc_bitmap, hbitmap);
        {
            BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdc_bitmap, 0, 0, SRCCOPY);
        }
        DeleteDC(hdc_bitmap);
    }


    // Write information. Wait for user to press ENTER.

    //std::cout
    //    << "image width  = " << bm.bmWidth
    //    << "\nimage height = " << bm.bmHeight
    //    << "\nPress ENTER...";

    std::cin.ignore(~0UL, '\n');
}
