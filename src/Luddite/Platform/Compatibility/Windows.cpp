#ifdef LD_PLATFORM_WINDOWS
#include "Luddite/Platform/Compatibility/Windows.hpp"
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif
// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;
#ifdef LD_DEBUG
void LUDDITE_API RedirectIOToConsole()
{
        int hConHandle;
        long lStdHandle;
        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        FILE *fp;

        // allocate a console for this app
        AllocConsole();

        // set the screen buffer to be big enough to let us scroll text
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
        coninfo.dwSize.Y = MAX_CONSOLE_LINES;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

        // redirect unbuffered STDOUT to the console
        lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
        hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "w");
        *stdout = *fp;
        setvbuf(stdout, NULL, _IONBF, 0);

        // redirect unbuffered STDIN to the console
        lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
        hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "r");
        *stdin = *fp;
        setvbuf(stdin, NULL, _IONBF, 0);

        // redirect unbuffered STDERR to the console
        lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
        hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "w");
        *stderr = *fp;
        setvbuf(stderr, NULL, _IONBF, 0);

        // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        ios::sync_with_stdio();

        //TODO: Add Error checking.

        // AllocConsole();
        // // Redirect the CRT standard input, output, and error handles to the console
        // freopen("CONIN$", "r", stdin);
        // freopen("CONOUT$", "w", stderr);
        // freopen("CONOUT$", "w", stdout);

        // // Note that there is no CONERR$ file
        // HANDLE hStdout = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        //         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        // HANDLE hStdin = CreateFile(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        //         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        // SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
        // SetStdHandle(STD_ERROR_HANDLE, hStdout);
        // SetStdHandle(STD_INPUT_HANDLE, hStdin);

        // //Clear the error state for each of the C++ standard stream objects.
        // std::wclog.clear();
        // std::clog.clear();
        // std::wcout.clear();
        // std::cout.clear();
        // std::wcerr.clear();
        // std::cerr.clear();
        // std::wcin.clear();
        // std::cin.clear();
}

#endif //LD_DEBUG
#endif //LD_PLATFORM_WINDOWS