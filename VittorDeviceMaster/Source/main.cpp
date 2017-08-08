#ifndef __main_cpp__
#define __main_cpp__
#include "main.h"
#include <iostream>
/**
���뻷��:
	DevC++ 5.11 MinGW
��̬���ӿ�:
	opengl32
	glu32
	gdi32
���ӵı���ָ��: 
	-std=gnu++14
*/

using namespace std;
CLayout		layout(MainProc);
CCabinet 	cab;
int			curcabnum = 1;
HDC			hdc_main;
HFONT		hFont_small;
MSG 		global_msg;
POINTS		pts; 
HWND		hedit_num;
HWND		hwnd_sub = 0;

int main( int argc, char **argv ) {
	hdc_main = GetWindowDC( layout.GetHandle( "������" ) );
	hFont_small = CreateFont( 14, 6, 0, 0, 200, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "����" );
	layout.Init( hFont_small );
	cab.x = 20;
	cab.y = 50;
	cab.width = 370;
	cab.height = 480;
	cab.list_x = 392;
	cab.list_y = 20;
	cab.list_width = 80;
	cab.list_height = 555;
	cab.list_offset = 0;
	EnumCommPort();
	CreateThread( 0, 0, RenderProc, 0, 0, 0 );
	layout.Log( "[��Ϣ]Ӧ�ó������� v20170808" );
	
	//ע�ᵯ�������� 
	WNDCLASS wcls;
	memset( &wcls, 0, sizeof(wcls) );
	wcls.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcls.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcls.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wcls.hInstance = GetModuleHandle( 0 );
	wcls.lpfnWndProc = SubWindowProc;
	wcls.style = CS_NOCLOSE | CS_OWNDC;
	wcls.lpszClassName = (char*)"sub_window_class";
	if( !RegisterClass( &wcls ) ) {
		MessageBox( layout.GetHandle("������"), "ע���Ӵ�����ʧ��", "����", MB_OK );
		return 0;
	}
	
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
			if( hwnd_sub == 0 ) 
				layout.CallProc( (HWND)lParam, HIWORD(wParam) ) ;
			break;
		case WM_DEVICECHANGE:
			EnumCommPort();
			break;
		case WM_MOUSEMOVE:
			pts = MAKEPOINTS( lParam );
			break;
		case WM_LBUTTONUP: {
			if( hwnd_sub != 0 )
				break;
			pts = MAKEPOINTS( lParam );
			int gate = cab.CatchGate( curcabnum, pts );
			int num = cab.CatchList( pts );
			if( gate != 0 ) {
				char buf[64];
				sprintf( buf, "[��־]��%d�Ź�%d����", curcabnum, gate );
				layout.Log( string( buf ) );
				cab.OpenGate( curcabnum, gate );
				//cab.QueryInfo();
			} else if ( num != 0 ) {
				curcabnum = num;
			} else if( pts.x >= cab.list_x and pts.x <= cab.list_x+cab.list_width ) {
				if( pts.y>=cab.list_y and pts.y <= cab.list_y+25 ) {
					if( cab.list_offset >= 4 ) {
						cab.list_offset -= 4;
					}else {
						cab.list_offset = 0;
					}
				} else if( pts.y >= cab.list_y + cab.list_height - 25 and pts.y <= cab.list_y + cab.list_height ) {
					if( cab.list_offset < cab.GetCabCount()-4 ) {
						cab.list_offset += 4;
					}
				}
			}
			break;
		}
		default:
			break;
	}
	return DefWindowProc( hwnd, message, wParam, lParam );
}

LRESULT CALLBACK SubWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	char buffer[1024];
	memset( buffer, 0, 1024 );
	switch( message ) {
		case WM_COMMAND: {
			GetWindowText( (HWND)lParam, buffer, 1024 );
			if( !strcmp( buffer, "ȡ��" ) and HIWORD(wParam) == BN_CLICKED ) {
				DestroyWindow( hwnd_sub );
				PostQuitMessage( 0 );
				break;
			} else if( !strcmp( buffer, "ȷ��" ) and HIWORD(wParam) == BN_CLICKED ) {
				memset( buffer, 0, 1024 );
				GetWindowText( hedit_num, buffer, 1024 );
				int count;
				if( sscanf( buffer, "%d", &count ) < 1 ) {
					MessageBox(  layout.GetHandle("������"),  "��������ȷ�Ĺ�������", "����", MB_OK );
					break;
				}
				cab.SetGateCount( curcabnum, count );
				DestroyWindow( hwnd_sub );
				PostQuitMessage( 0 );
			}
			break;
		}
		default :
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
			curcabnum = 1;
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
	if( !cab.IsCommOpen() ) {
		return 0;
	}
	int tag = cab.GetLastCab( curcabnum );
	if( tag > 0 ) {
		curcabnum = tag;
	}
}
LRESULT OnNextCab( HWND hwnd, WORD code ) {
	if( code != BN_CLICKED ) {
		return 0;
	}
	if( !cab.IsCommOpen() ) {
		return 0;
	}
	int tag = cab.GetNextCab( curcabnum );
	if( tag > 0 ) {
		curcabnum = tag;
	}
}
LRESULT OnSetCab( HWND hwnd, WORD code ) {
	MSG msg;
	if( code != BN_CLICKED ) {
		return 0;
	}
	if( !cab.IsCommOpen() ) {
		return 0;
	}
	layout.Log( "[��־]���ù��Ӳ���" );
	hwnd_sub = CreateWindowEx( 
		0, "sub_window_class", "���ù�������", 
		WS_OVERLAPPEDWINDOW & (~ WS_MINIMIZEBOX) & (~ WS_MAXIMIZEBOX) & (~ WS_SIZEBOX) | WS_POPUP | WS_VISIBLE,
		256, 128, 512, 280, layout.GetHandle("������"), 0, GetModuleHandle(0), 0  );
	if( hwnd_sub == 0 ) {
		return 0;
	}
	CreateWindow( "BUTTON", "ȷ��", WS_CHILD | BS_NOTIFY| WS_VISIBLE, 32, 220, 80, 22, hwnd_sub, 0, GetModuleHandle(0), 0 );
	CreateWindow( "BUTTON", "ȡ��", WS_CHILD | BS_NOTIFY| WS_VISIBLE, 400, 220, 80, 22, hwnd_sub, 0, GetModuleHandle(0), 0 );
	hedit_num = CreateWindow( "EDIT", "12", WS_CHILD | ES_CENTER | ES_NUMBER| WS_VISIBLE, 80, 80, 128, 22, hwnd_sub, 0, GetModuleHandle(0), 0 );
	CreateWindow( "STATIC", "��������", WS_CHILD| WS_VISIBLE, 20, 80, 60, 22, hwnd_sub, 0, GetModuleHandle(0), 0 );
	
	while( GetMessage( &msg, 0, 0, 0 ) > 0 ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	cab.QueryInfo( curcabnum );
	hwnd_sub = 0;
	return 0;
}
LRESULT OnRefresh( HWND hwnd, WORD code ) {
	if( code != BN_CLICKED ) {
		return 0;
	}
	if( !cab.IsCommOpen() ) {
		return 0;
	}
	//layout.Log( "[��־]��ȡ������Ϣ" );
	cab.QueryInfo( curcabnum );
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
	
	glEnable( GL_POINT_SMOOTH );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor( 1.0, 1.0, 1.0, 0.0 );
	glTranslatef( -1.0f, 1.0f, 0.0f );
	glScalef( 2.0/(rect.right-rect.left), -2.0/(rect.bottom-rect.top), 1.0 );
	while( true ) {
		glClear( GL_COLOR_BUFFER_BIT );
		cab.DrawCab( curcabnum );
		cab.DrawList();
		
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
