#ifndef __MAIN_H__
#define __MAIN_H__
#include "Layout.h"
#include "CChain.h"
#include <windows.h>
#define RES_BMP_VITTOR 0
#define RES_BMP_RED 1
#define RES_BMP_GREEN 2

extern CLayout layout;
extern volatile bool bCommWorking;
extern volatile bool bNetWorking;
extern SOCKET server;

LRESULT CALLBACK MainProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT SelectLampColor( WPARAM wParam, LPARAM lParam );
void CALLBACK TimerProc ( HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);
LRESULT OnScanCom0( HWND hwnd, WORD code );

bool GetCheck( HWND hwnd );
bool GetCheck( string name );
void SetCheck( HWND hwnd, bool checked );
void SetCheck( string name, bool checked );
#endif
