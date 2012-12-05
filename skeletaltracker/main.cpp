/*
	Skeletal Tracker - main.cpp

	Written by Matthew Beckett. 2012.
*/

#include "tracker.h"


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
	Tracker track;
	track.Go();	
	for(;; Sleep(30)) keys();
	//returnkeys(VK_LEFT); //powerpoint back a slide
	//returnkeys(VK_RIGHT); //powerpoint forward a slide
	return 0;
}

