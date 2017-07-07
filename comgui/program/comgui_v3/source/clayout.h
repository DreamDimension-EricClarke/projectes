#ifndef __clayout__
#define __clayout__

#include "dxml.h"
#include <windows.h>

using namespace std;
using namespace dream;

#define MSG_SETLAYOUT (WM_USER + 0)
#define MSG_SETMOTHER (WM_USER + 1)
#define MSG_LOADPROC  (WM_USER + 2)

struct element {
	public:
		HWND	hwnd;		//窗口句柄 
		WORD	ID;			//控件id 0是无效的ID 
		string	name;		//控件名 
};

class clayout {
	private:
		static LRESULT CALLBACK GroupProc( HWND, UINT, WPARAM, LPARAM ); 
	
	private:
		static	int		m_class_count;
		
		HWND			m_hwnd;
		dchain<element>	m_widgets;
		WNDPROC			m_proc;
	private:
		bool	lexer( dXmlnode*, HWND );
	
	public:
		clayout( void );
		~clayout( void );
		
		bool create( string, WNDPROC );
		bool clear( void );
		
		HWND getHandle( string );
		HWND getHandle( WORD );		//通过控件ID获取窗口句柄,传递0参数则获取主窗口句柄
		WORD getID( string );		//获取控件ID 
};


#endif
