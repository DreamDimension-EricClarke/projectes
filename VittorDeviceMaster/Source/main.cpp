#ifndef __main_cpp__
#define __main_cpp__
#include "main.h"
#include <iostream>
using namespace std;
CLayout		layout(MainProc);
CCabinet 	cab;
int			curcabnum = 1;
HDC			hdc_main;
HFONT		hFont_small;
MSG 		global_msg;
POINTS		pts; 

int main( int argc, char **argv ) {
	hdc_main = GetWindowDC( layout.GetHandle( "������" ) );
	hFont_small = CreateFont( 14, 6, 0, 0, 200, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "����" );
	layout.Init( hFont_small );
	cab.x = 20;
	cab.y = 50;
	cab.width = 370;
	cab.height = 480;
	EnumCommPort();
	CreateThread( 0, 0, RenderProc, 0, 0, 0 );
	layout.Log( "[��Ϣ]Ӧ�ó�������" );
	while( GetMessage( &global_msg, 0, 0, 0 ) ) {
		TranslateMessage( &global_msg );
		DispatchMessage( &global_msg );
	}
	
	return global_msg.wParam;
}

LRESULT CALLBACK MainProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
		case WM_CREATE:
			break;
		case WM_CTLCOLORSTATIC:
			SetBkColor( (HDC)wParam, RGB( 255, 255, 255 ) );
			SetTextColor( (HDC)wParam, RGB( 0, 0, 0 ) );
			return (LRESULT)COLOR_WINDOWFRAME;
		case WM_CLOSE:
			PostQuitMessage( 0 );
			break;
		case WM_COMMAND:
			layout.CallProc( (HWND)lParam, HIWORD(wParam) ) ;
			break;
		case WM_DEVICECHANGE:
			EnumCommPort();
			break;
		case WM_MOUSEMOVE:
			pts = MAKEPOINTS( lParam );
			break;
		case WM_LBUTTONUP: {
			pts = MAKEPOINTS( lParam );
			int gate = cab.CatchGate( curcabnum, pts );
			if( gate != 0 ) {
				char buf[64];
				sprintf( buf, "[��־]��%d�Ź�%d����", curcabnum, gate );
				layout.Log( string( buf ) );
				cab.OpenGate( curcabnum, gate );
				//cab.QueryInfo();
			}
			break;
		}
		default:
			break;
	}
	return DefWindowProc( hwnd, message, wParam, lParam );
}

LRESULT OnCommSwitch( HWND hwnd, WORD code ) {
	if( code != BN_CLICKED ) {
		return 0;
	}
	
	if( cab.IsCommOpen() ) {
		layout.Log( "[��־]���ڹرմ���" );
		if( cab.CloseComm() ) {
			layout.Log( "[��־]�����ѹر�" );
			SetWindowText( hwnd, "�򿪴���" );
		}
		layout.Log( "[��־]�ر�ʧ��" );
	} else {
		char buf[32];
		DWORD baudrate;
		memset( buf, 0, 32 );
		GetWindowText( layout.GetHandle( "��Ͽ�_���ڲ�����" ), buf, 32 );
		if( sscanf( buf, "%u", &baudrate ) < 1 ) {
			layout.Log( "[����]��ѡ�񴮿ڲ�����" );
			return 0;
		}
		memset( buf, 0, 32 );
		GetWindowText( layout.GetHandle( "��Ͽ�_���ں���" ), buf, 32 );
		if( strlen( buf ) == 0 ) {
			layout.Log( "[����]��ѡ�񴮿ں���" );
			return 0; 
		}
		layout.Log( "[��־]���ڴ򿪴���" );
		if( cab.OpenComm( buf, baudrate ) ) {
			SetWindowText( hwnd, "�رմ���" );
			layout.Log( "[��־]�����Ѵ�" );
			if( !cab.QueryInfo() ) {
				layout.Log( "[����]��ȡ����״̬ʧ��" );
			}
		} else {
			layout.Log( "[����]���ڴ�ʧ��" );
		}
	}
}
LRESULT OnLastCab( HWND hwnd, WORD code ) {
	if( code != BN_CLICKED ) {
		return 0;
	}
	if( curcabnum > 0 ) {
		curcabnum -= 1;
	}
}
LRESULT OnNextCab( HWND hwnd, WORD code ) {
		if( code != BN_CLICKED ) {
		return 0;
	}
	curcabnum += 1;
}

DWORD WINAPI RenderProc( LPVOID lpVoid ) {
	HWND hwnd = layout.GetHandle( "������" );
	HDC hdc = GetDC( hwnd );
	RECT rect;
	HGLRC hrc = 0;
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	
	memset( &pfd, 0, sizeof(pfd) );
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	
	iFormat = ChoosePixelFormat( hdc, &pfd );
	SetPixelFormat( hdc, iFormat, &pfd );
	
	GetClientRect( hwnd, &rect );
	
	hrc = wglCreateContext( hdc );
	wglMakeCurrent( hdc, hrc );
	SelectObject( hdc, (HGDIOBJ)hFont_small );
	
	glClearColor( 1.0, 1.0, 1.0, 0.0 );
	glTranslatef( -1.0f, 1.0f, 0.0f );
	glScalef( 2.0/(rect.right-rect.left), -2.0/(rect.bottom-rect.top), 1.0 );
	while( true ) {
		glClear( GL_COLOR_BUFFER_BIT );
		cab.DrawCab( curcabnum );
		
		SwapBuffers( hdc );
	}
}

void EnumCommPort( void ) {
	HWND hWnd = layout.GetHandle( "��Ͽ�_���ں���" );
	SendMessage( hWnd, CB_RESETCONTENT, 0, 0 );
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM\\", 0, KEY_READ,
	                 &hKey) ) {
		return;
	}
	CHAR Name[25];
	UCHAR szPortName[25];
	LONG Status;
	DWORD dwIndex = 0;
	DWORD dwName;
	DWORD dwSizeofPortName;
	DWORD Type;
	dwName = sizeof(Name);
	dwSizeofPortName = sizeof(szPortName);
	do {
		memset( szPortName, 0, 25 );
		dwName = dwSizeofPortName = 25;
		Status = RegEnumValue( hKey, dwIndex++, Name, &dwName, NULL, &Type, szPortName, &dwSizeofPortName );
		if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)) {
			SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)szPortName );       // �����ַ�������
		}
	} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA));
	RegCloseKey(hKey);
	SendMessage( hWnd, CB_SETCURSEL, 0, 0 );
}

void DrawText( string tstr ) {
	int leng = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, 0, 0 );
	wchar_t* text = new wchar_t[leng];	//Ϊ����������λ��
	int retn = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, text, leng );
	/***���벢������ʾ�б�***/
	GLuint caller = glGenLists( 1 );
	for( int i = 0; i < retn-1; i++ )
	{
		while(!wglUseFontBitmapsW( hdc_main, text[i], 1, caller ));
		glCallList( caller );
	}
	glDeleteLists( caller, 1 );
	delete[] text;
	return;
}

#endif
