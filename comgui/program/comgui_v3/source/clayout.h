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
		HWND	hwnd;		//���ھ�� 
		WORD	ID;			//�ؼ�id 0����Ч��ID 
		string	name;		//�ؼ��� 
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
		HWND getHandle( WORD );		//ͨ���ؼ�ID��ȡ���ھ��,����0�������ȡ�����ھ��
		WORD getID( string );		//��ȡ�ؼ�ID 
};


#endif
