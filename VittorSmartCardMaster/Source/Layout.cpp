#ifndef __Layout_CPP__
#define __Layout_CPP__
#include <iostream>
#include "Layout.h"
#include "main.h"

#define __INTERNAL_MODE__ 1

using namespace std;

volatile WNDPROC DefGroupProc = 0;
volatile WNDPROC DefButtonProc = 0;

SWidget::SWidget( HWND _hwnd, HMENU _ID, string _name, WDGPROC _proc ):
	hwnd(_hwnd),ID(_ID),name(_name), proc(_proc) {

}

SWidget::SWidget( const SWidget& another):
	hwnd(another.hwnd), ID(another.ID), name(another.name), proc(another.proc) {
}

CLayout::CLayout( WNDPROC proc ):
	m_proc( proc ) {
	WNDCLASS ws;
	HWND hwnd;
	memset( &ws, 0, sizeof(ws) );

	ws.hbrBackground = (HBRUSH)COLOR_WINDOW;
	ws.hInstance = GetModuleHandle( 0 );
	ws.hCursor = LoadCursor( 0, IDC_ARROW );
	ws.hIcon = LoadIcon( ws.hInstance, 0 );
	ws.lpfnWndProc = proc;
	ws.lpszClassName = (char*)"VittorSmartCardMaster";
	ws.style = CS_ENABLE | CS_OWNDC;

	if( !RegisterClass( &ws ) ) {

		throw runtime_error( "failed to register window class" );
	}

	hwnd = CreateWindowEx(
		0,"VittorSmartCardMaster", "微拓智能卡配置器", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX|WS_POPUP|WS_VISIBLE|WS_CLIPCHILDREN,
		32, 32, 1024, 640,
		0, 0, ws.hInstance, 0
	);

	if( hwnd == 0 )
		throw runtime_error( "failed to create main window" );

	m_widget.Push( SWidget( hwnd, 0, "主窗口" ) );
}
CLayout::~CLayout( void ) {
}

void CLayout::Init( HFONT hFont ) {
	HWND hwnd;
	CreateCheck( 20, 140, 65, 22, "扫描COM1", "复选框_扫描COM0", "主窗口", BS_TOP, OnScanCom1 );
	CreateStatic( 90, 140, 55, 22, "当前卡SN:", "标题_当前卡SN", "主窗口" );
	CreateStatic( 150, 140, 50, 22, "00000000", "值_当前卡SN", "主窗口" );
	CreateStatic( 20, 170, 65, 22, "绑定电话号", "标题_绑定电话号", "主窗口" );
	CreateEdit( 100, 170, 140, 22, "", "文本框_绑定电话号", "主窗口", ES_NUMBER );
	CreateButton( 250, 170, 60, 22, "绑定电话", "按钮_绑定电话", "主窗口" );
	CreateEdit( 440, 20, 550, 560, "", "文本框_运行日志", "主窗口", ES_READONLY|ES_MULTILINE );
	#if __INTERNAL_MODE__
	CreateStatic( 20, 210, 65, 22, "绑定卡编号", "标题_卡编号", "主窗口" );
	CreateEdit( 100, 210, 140, 22, "", "文本框_卡编号", "主窗口" );
	CreateButton( 250, 210, 60, 22, "绑定编号", "按钮_绑定编号", "主窗口" );
	CreateCheck( 20, 240, 75, 22, "自动绑卡", "复选框_自动绑卡", "主窗口" );
	CreateCheck( 100, 240, 75, 22, "编号自增", "复选框_编号自增", "主窗口" );
	#endif
	
	m_widget.ForEach( [&](SWidget& widget ) {
		SendMessage( widget.hwnd, WM_SETFONT, (WPARAM)hFont, TRUE );
		return true;
	});
	
	hwnd = GetHandle( "组合框_功能选择" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LRESULT)"绑定电话" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LRESULT)"批量发卡" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );
	
	Log(
	"基本操作流程:\r\n"
	"\t1.扫描智能卡,等待设备读取卡片信息\r\n"
	"\t2.输入绑定信息\r\n"
	"\t3.确认绑定信息,点击绑定按钮\r\n" );
}

HWND CLayout::GetHandle( HMENU ID ) {
	HWND hwnd = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			hwnd = widget.hwnd;
			return false;
		}
		return true;
	} );
	return hwnd;
}
HWND CLayout::GetHandle( string name ) {
	HWND hwnd = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			hwnd = widget.hwnd;
			return false;
		}
		return true;
	} );
	return hwnd;
}

HMENU CLayout::GetID( HWND handle ) {
	HMENU ID = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			ID = widget.ID;
			return false;
		}
		return true;
	} );
	return ID;
}
HMENU CLayout::GetID( string name ) {
	HMENU ID = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			ID = widget.ID;
			return false;
		}
		return true;
	} );
	return ID;
}

bool CLayout::GetName( HWND handle, string& name ) {
	bool bfound = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			name = widget.name;
			bfound = true;
			return false;
		}
		return true;
	} );
	return bfound;
}
bool CLayout::GetName( HMENU ID, string& name ) {
	bool bfound = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			name = widget.name;
			bfound = true;
			return false;
		}
		return true;
	} );
	return bfound;
}

bool CLayout::SetProc( HWND handle, WDGPROC proc ) {
	bool bSet = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			widget.proc = proc;
			bSet = true;
			return false;
		}
		return true;
	} );
	return bSet;
}
bool CLayout::SetProc( HMENU ID, WDGPROC proc ) {
	bool bSet = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			widget.proc = proc;
			bSet = true;
			return false;
		}
		return true;
	} );
	return bSet;
}
bool CLayout::SetProc( string name, WDGPROC proc ) {
	bool bSet = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			widget.proc = proc;
			bSet = true;
			return false;
		}
		return true;
	} );
	return bSet;
}

WDGPROC CLayout::GetProc( HWND handle ) {
	WDGPROC proc = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			proc = widget.proc;
			return false;
		}
		return true;
	} );
	return proc;
}
WDGPROC CLayout::GetProc( HMENU ID ) {
	WDGPROC proc = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			proc = widget.proc;
			return false;
		}
		return true;
	} );
	return proc;
}
WDGPROC CLayout::GetProc( string name ) {
	WDGPROC proc = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			proc = widget.proc;
			return false;
		}
		return true;
	} );
	return proc;
}

bool CLayout::CallProc( string name, WORD notify_code ) {
	WDGPROC proc = GetProc( name );
	if( proc == 0 )
		return false;
	proc( GetHandle(name), notify_code );
	return true;
}

bool CLayout::CallProc( HMENU name, WORD notify_code ) {
	WDGPROC proc = GetProc( name );
	if( proc == 0 )
		return false;
	proc( GetHandle(name), notify_code );
	return true;
}

bool CLayout::CallProc( HWND name, WORD notify_code ) {
	WDGPROC proc = GetProc( name );
	if( proc == 0 )
		return false;
	proc( name, notify_code );
	return true;
}

LRESULT CALLBACK GroupProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	case WM_COMMAND:
	case WM_NOTIFY:
		SendMessage( (HWND)GetWindowLongPtr( hwnd, GWLP_HWNDPARENT ), message, wParam, lParam );
		break;
	default:
		break;
	}
	if( DefGroupProc != 0 )
		return CallWindowProc(DefGroupProc, hwnd, message, wParam, lParam );
	else
		return DefWindowProc( hwnd, message, wParam, lParam );
}

LRESULT CALLBACK ButtonProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	default:
		break;
	}
	if( DefButtonProc != 0 )
		return CallWindowProc(DefButtonProc, hwnd, message, wParam, lParam );
	else
		return DefWindowProc( hwnd, message, wParam, lParam );
}


bool CLayout::CreateGroup( DWORD x, DWORD y, DWORD width, DWORD height, const string& caption,
                           const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption.data(),
	                WS_CHILD | WS_VISIBLE | BS_GROUPBOX | addition,
	                x, y, width, height,
	                GetHandle( parent ),
	                (HMENU)ID,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;
	DefGroupProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)GroupProc );
	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateStatic( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                            const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"STATIC",
	                caption,
	                WS_CHILD | WS_VISIBLE | addition,
	                x, y, width, height,
	                hparent,
	                (HMENU)ID,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateCombo( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                           const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"COMBOBOX",
	                caption,
	                WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | addition,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateButton( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                            const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption,
	                WS_CHILD | WS_VISIBLE | BS_NOTIFY | addition,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	DefButtonProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)ButtonProc );

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateRadio( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                           const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption,
	                WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_AUTORADIOBUTTON | addition,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	DefButtonProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)ButtonProc );

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateCheck( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                           const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption,
	                WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_AUTOCHECKBOX | addition,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	DefButtonProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)ButtonProc );

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateEdit( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                          const string& name, const string& parent, UINT addition,  WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"EDIT",
	                caption,
	                WS_CHILD | WS_VISIBLE | WS_BORDER | addition,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateScroll( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                            const string& name, const string& parent, UINT addition, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"SCROLLBOX",
	                caption,
	                WS_CHILD | WS_VISIBLE | SS_NOTIFY,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

void CLayout::Log( const string& value ) {
	HWND hWnd = GetHandle( "文本框_运行日志" );
	static string logstr;
	logstr += value + string("\r\n");
	SetWindowText( hWnd, logstr.data() );
	SendMessage( hWnd, EM_SCROLL, SB_BOTTOM, 0 );
}

#endif
