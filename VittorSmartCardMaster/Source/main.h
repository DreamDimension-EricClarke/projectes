#ifndef __main_h__
#define __main_h__

#include <windows.h>
#include "Layout.h"
#include <string>

using namespace std;

extern CLayout      layout;
extern HWND        	hwnd_main;
extern HDC         	hdc_main;
extern HGLRC       	hrc_main;


LRESULT CALLBACK MainProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
DWORD WINAPI RenderProc( LPVOID lpVoid );
DWORD WINAPI NetProc( LPVOID lpVoid );

void CALLBACK TimerProc ( HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);
LRESULT OnScanCom1( HWND hwnd, WORD code );

bool GetCheck( HWND hwnd );
bool GetCheck( string name );
void SetCheck( HWND hwnd, bool checked );
void SetCheck( string name, bool checked );

void DrawAnimi( void );
void DrawText( string tstr );
#endif
