#ifndef __MAIN__
#define __MAIN__
#include <winsock2.h>
#include <windows.h>
#include <wingdi.h>
#include <string.h>
#include "main.h"
#include "umfh.h"
#include <iostream>

using namespace std;

volatile bool bCommWorking = false;
volatile bool bNetWorking = false;
volatile bool bReadCard = false;
volatile int nTimeout = 0;
volatile int nStartCom = 1;
volatile unsigned char card_srn[9] = "00000000";
CLayout layout( MainProc );

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	
	HFONT hFont = CreateFont( 14, 6, 0, 0, 200, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "宋体" );
	layout.Init( hFont );
	SetTimer( 0, 1, 200, TimerProc );
	
	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}


LRESULT CALLBACK MainProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_SHOWWINDOW:
	case WM_MOVE:
	case WM_SIZE:
		break;
	case WM_CLOSE:
		PostQuitMessage( 0 );
		break;
	case WM_DEVICECHANGE:
		break;
	case WM_CTLCOLORSTATIC:
		return SelectLampColor( wParam, lParam );
	case WM_COMMAND: {
		layout.CallProc( (HWND)lParam, HIWORD(wParam) );
		break;
		}
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
}

LRESULT OnScanCom0( HWND hwnd, WORD code ) {
	volatile static bool bChecking = false;
	if( GetCheck( hwnd ) ) {
		if( nStartCom == 1 and !bChecking ) {
			bChecking = true;
			if( MessageBox( layout.GetHandle("主窗口"), "在某些电脑上COM1端口是常开端口,扫描COM1端口之前应保证COM1端口已经插入了设备", "重要提示", MB_ICONWARNING | MB_OKCANCEL ) == IDOK )
				nStartCom = 0;
			else
				SetCheck( hwnd, false );
			bChecking = false;
		} 
	} else {
		nStartCom = 1;
	}
}


LRESULT SelectLampColor( WPARAM wParam, LPARAM lParam ) {
	if( lParam == (LPARAM)layout.GetHandle( "画布_服务器状态" ) ) {
		if( !bNetWorking ) {
			SetTextColor( (HDC)wParam, RGB( 255, 0, 0 ) );
			SetBkColor( (HDC)wParam, RGB( 255, 0, 0 ) );
		} else {
			SetTextColor( (HDC)wParam, RGB( 0, 255, 0 ) );
			SetBkColor( (HDC)wParam, RGB( 0, 255, 0 ) );
		}
	} else if( lParam == (LPARAM)layout.GetHandle( "画布_设备状态" ) ) {
		if( !bCommWorking ) {
			SetTextColor( (HDC)wParam, RGB( 255, 0, 0 ) );
			SetBkColor( (HDC)wParam, RGB( 255, 0, 0 ) );
		} else {
			SetTextColor( (HDC)wParam, RGB( 0, 255, 0 ) );
			SetBkColor( (HDC)wParam, RGB( 0, 255, 0 ) );
		}
	} else {
		SetTextColor( (HDC)wParam, RGB( 0, 0, 0 ) );
		SetBkColor( (HDC)wParam, RGB( 240, 240, 240 ) );
	}
	return (LRESULT)GetStockObject(NULL_BRUSH);
}

void CALLBACK TimerProc ( HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime) {
	bool bCo = false;
	int port;
	int lDevice;
	for( port = nStartCom; port <= 10; port++ ) {//the first parameter, 100: USB,0:COM1,1:COM2,2:COM3 ...
		lDevice = fw_init( port, 115200 );
		if( lDevice > 0 ) {
			bCo = true;
			break;
		}
	}
	if( (bCo  and !bCommWorking) or (!bCo and bCommWorking ) ) {
		bCommWorking = bCo;
		hwnd = layout.GetHandle( "画布_设备状态" );
		HDC hdc = GetDC( hwnd );
		SelectLampColor( (WPARAM)hdc, (LPARAM)hwnd );
		InvalidateRect( hwnd, 0, TRUE );
		SendMessage( hwnd, WM_PAINT, (WPARAM)hdc, (LPARAM)hwnd );
		ReleaseDC( hwnd, hdc );
	}

	bCommWorking = bCo;

	if( !bCommWorking )
		return;

	memset( (void*)card_srn, '0', 8 );
	if( !fw_card_hex( lDevice, 1, (unsigned char*)card_srn ) ) {
		if( nTimeout >= 1 ) {
			fw_beep( lDevice, 10 );
			SetWindowText( layout.GetHandle( "值_当前卡SN" ), (char*)card_srn );
		}
		nTimeout = 0;
	} else {
		if( nTimeout == 0 )
			SetWindowText( layout.GetHandle( "值_当前卡SN" ), (char*)card_srn );
		nTimeout += 1;
	}
	fw_exit( lDevice );
}


bool GetCheck( HWND hwnd ) {
	return SendMessage( hwnd, BM_GETCHECK, 0, 0 );
}
bool GetCheck( string name ) {
	return SendMessage( layout.GetHandle(name), BM_GETCHECK, 0, 0 );
}
void SetCheck( HWND hwnd, bool checked ) {
	SendMessage( hwnd, BM_SETCHECK, (checked?BST_CHECKED:BST_UNCHECKED), 0 );
}
void SetCheck( string name, bool checked ) {
	SendMessage( layout.GetHandle( name ), BM_SETCHECK, (checked?BST_CHECKED:BST_UNCHECKED), 0 );
}
#endif
