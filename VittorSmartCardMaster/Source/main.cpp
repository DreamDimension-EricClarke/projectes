#ifndef __main_cpp__
#define __main_cpp__

#include <winsock2.h>
#include <windows.h>
#include <wingdi.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "umfh.h"
#include "main.h"
#include <iostream>

using namespace std;

CLayout      layout(MainProc);      //主布局实例
HWND        hwnd_main;              //主窗口句柄
HDC         hdc_main;               //主设备描述句柄
HGLRC       hrc_main;               //主渲染句柄
MSG         msg_main;               //主消息体
int         animi_frame;            //动画帧
string      net_word;               //网络状态描述

SOCKET  sock = INVALID_SOCKET;

volatile bool bCommWorking = false;
volatile bool bNetWorking = false;
volatile bool bNetBusy = false;
volatile bool bReadCard = false;
volatile int nTimeout = 0;
volatile int nStartCom = 1;
volatile unsigned char card_srn[9] = "00000000";

int main( int argc, char **argv ) {
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	
	HFONT hFont = CreateFont( 14, 6, 0, 0, 200, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "宋体" );
	hwnd_main = layout.GetHandle( "主窗口" );
	layout.Init( hFont );
	
	if( hwnd_main == 0 ) {
		return 2;
	}
	
	hdc_main = GetDC( hwnd_main );
	if( hdc_main == 0 ) {
		return 3;
	}
	
	memset( &pfd, 0, sizeof(pfd) );
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	iFormat = ChoosePixelFormat( hdc_main, &pfd );
	if( !SetPixelFormat( hdc_main, iFormat, &pfd ) ) {
		return 4;
	}
	
	hrc_main = wglCreateContext( hdc_main );
	if( hrc_main == 0 ) {
		return 5;
	}
	
	CreateThread( 0, 0, RenderProc, 0, 0, 0 );
	CreateThread( 0, 0, NetProc, 0, 0, 0 );
	SetTimer( NULL, 0, 200, TimerProc );
	
	while( GetMessage( &msg_main, NULL, 0, 0 ) ) {
		TranslateMessage( &msg_main );
		DispatchMessage( &msg_main );
	}
	return msg_main.wParam;
}
LRESULT CALLBACK MainProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch( msg ) {
		case WM_CTLCOLORSTATIC:
			SetBkColor( (HDC)wParam, RGB( 255, 255, 255 ) );
			SetTextColor( (HDC)wParam, RGB( 0, 0, 0 ) );
			return (LRESULT)COLOR_WINDOWFRAME;
		case WM_COMMAND:
			layout.CallProc( (HWND)lParam, HIWORD(wParam) );
			break;
		case WM_CLOSE:
			PostQuitMessage( 0 );
			break;
	}
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

DWORD WINAPI RenderProc( LPVOID lpVoid ) {
	wglMakeCurrent( hdc_main, hrc_main );
	
	glEnable( GL_POINT_SMOOTH );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glEnable( GL_BLEND );
	
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glClearColor( 1.0, 1.0, 1.0, 0.0 );
	while( true ) {
		glClear( GL_COLOR_BUFFER_BIT );
		DrawAnimi();
		SwapBuffers( hdc_main );
	}
}


DWORD WINAPI NetProc( LPVOID lpVoid ) {
	WSADATA data;
	WSAStartup( 0x22, &data );
	LPHOSTENT hostent = 0;
	SOCKADDR_IN addr;
	
	while( true ) {
		net_word = "正在尝试连接服务器";

		sock = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
		if( sock == INVALID_SOCKET ) {
			net_word = "套接字建立失败";
			Sleep(2000);
			continue;
		}
		//cout << '.';
		while( hostent == 0 ) {
			hostent = gethostbyname( "www.dn-ezr.cn" );
			if( hostent == 0 ) {
				net_word = "域名解析失败";
				Sleep( 2000 );
			}
		}
		//cout << '.';
		memset( &addr, 0, sizeof(addr) );
		addr.sin_addr = *(IN_ADDR*)hostent->h_addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons( 20024 );
		//cout << endl << inet_ntoa( addr.sin_addr ) << endl;
		
		while( !bNetWorking ) {
			if( connect( sock, (SOCKADDR*)&addr, sizeof(addr) ) ) {
				net_word = "服务器连接失败";
				Sleep( 2000 );
			} else {
				bNetWorking = true;
			}
		}
		//cout << '.';
		net_word = "服务器连通";
		
		while( true );
	}
}

void DrawAnimi( void ) {
	static int des = 1;
	glLineWidth( 1 );
	glColor3f( 0.9, 0.9, 0.9 );
	glBegin( GL_LINES );
	glVertex2f( -1, -0.96 );
	glVertex2f( 1, -0.96 );
	glEnd();
	
	glLineWidth( 4 );
	glBegin( GL_LINE_STRIP );
	if( des == 1 ) {
		for( int i = 0; i <= animi_frame; i++ ) {
			double cl = (animi_frame-i)/(double)animi_frame;
			glColor3f( cl, cl, 1.0 );
			glVertex2f( -1 + 0.0004*i, -0.96 );
		}
	} else {
		for( int i = 5000; i >= animi_frame; i-- ) {
			double cl = (i-animi_frame)/(5000.0 - animi_frame);
				glColor3f( cl, cl, 1.0 );
			glVertex2f( -1 + 0.0004*i, -0.96 );
		}
	}
	glEnd();
	
	if( des == 1 ) {
		animi_frame += 1;
		if( animi_frame > 5000 ) {
			des = 0;
		}
	} else {
		animi_frame -= 1;
		if( animi_frame < 0 ) {
			des = 1;
		}
	}

	glBegin( GL_QUADS );
	if( bCommWorking )
		glColor3f( 0.2, 0.7, 0.2 );
	else
		glColor3f( 0.7, 0.2, 0.2 );
	glVertex2f( -0.95, 0.93 );
	glVertex2f( -0.15, 0.93 );
	glVertex2f( -0.15, 0.75 );
	glVertex2f( -0.95, 0.75 );
	glEnd();
	
	glColor3f( 1.0f, 1.0f, 1.0f );
	glRasterPos2f( -0.65, 0.85 );
	if( bCommWorking )
		DrawText( "设备正在工作" );
	else
		DrawText( "未找到设备" );
		
	glBegin( GL_QUADS );
	if( bNetWorking )
		glColor3f( 0.2, 0.7, 0.2 );
	else
		glColor3f( 0.7, 0.2, 0.2 );
	glVertex2f( -0.95, 0.73 );
	glVertex2f( -0.15, 0.73 );
	glVertex2f( -0.15, 0.55 );
	glVertex2f( -0.95, 0.55 );
	glEnd();

	glColor3f( 1.0f, 1.0f, 1.0f );
	glRasterPos2f( -0.65, 0.65 );
	DrawText( net_word );
}


LRESULT OnScanCom1( HWND hwnd, WORD code ) {
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


void CALLBACK TimerProc ( HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime) {
	int port;
	int lDevice;
	for( port = nStartCom; port <= 10; port++ ) {//the first parameter, 100: USB,0:COM1,1:COM2,2:COM3 ...
		lDevice = fw_init( port, 115200 );
		if( lDevice > 0 ) {
			bCommWorking = true;
			break;
		}
	}
	if( port > 10 ) {
		bCommWorking = false;
	}
	
	memset( (void*)card_srn, '0', 8 );
	if( !bCommWorking ) {
		SetWindowText( layout.GetHandle( "值_当前卡SN" ), (char*)card_srn );
		return;
	}

	if( !fw_card_hex( lDevice, 1, (unsigned char*)card_srn ) ) {
		if( nTimeout >= 1 ) {
			fw_beep( lDevice, 10 );
			SetWindowText( layout.GetHandle( "值_当前卡SN" ), (char*)card_srn );
		}
		nTimeout = 0;
	} else {
		nTimeout += 1;
	}
	fw_exit( lDevice );
}

void DrawText( string tstr ) {
	/***创建字体***/
//	HFONT hFont = CreateFont(
//		this->m_Height, this->m_Height/2, 0, 0, this->m_Wieght,
//		this->m_Italic, this->m_Underline, this->m_StrikeOut,
//		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
//		DEFAULT_PITCH | FF_DONTCARE, NULL  );
	/***选择新建的字体***/
//	hFont = (HFONT)SelectObject( hDC, (HGDIOBJ)hFont );
	int leng = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, 0, 0 );
	wchar_t* text = new wchar_t[leng];	//为结束符保存位置
	int retn = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, text, leng );
	/***申请并创建显示列表***/
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
