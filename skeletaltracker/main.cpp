/*
	Skeletal Tracker - main.cpp

	Written by Matthew Beckett. 2012.
*/

#include "tracker.h"

void debugwindow()
{
	AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
}

void keys()
{
	Tracker track2;

	// Output 'n'
	if(GetAsyncKeyState(VK_LEFT)) track2.ReturnKeys(VkKeyScan('n'));
		 
	// Output 'p'
	if(GetAsyncKeyState(VK_RIGHT)) track2.ReturnKeys(VkKeyScan('p'));
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	debugwindow();
	Tracker track;
	track.Go();	
	for(;; Sleep(30)) keys();
	//returnkeys(VK_LEFT); //powerpoint back a slide
	//returnkeys(VK_RIGHT); //powerpoint forward a slide
	return 0;
}

