#include "stdafx.h"
#include "win32specific.h"
#include "SDL_syswm.h"

using namespace PangoTUI;

void PangoTUI::concol(int c){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
	SetConsoleTextAttribute(hConsole, c);
}

void PangoTUI::strip_window(void){
	 SDL_SysWMinfo wmi;
     SDL_VERSION(&wmi.version);
	 if(!SDL_GetWMInfo(&wmi)) return;
	 
	HWND hwnd=wmi.window;
	LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
	SetWindowLongPtr(hwnd, GWL_STYLE, lStyle);
	//SetWindowLongPtr(hwnd, GWL_STYLE, 0);

	LONG lExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, lExStyle);

	SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	//SetWindowPos(hwnd, NULL, 0,0,0,0,0);
}