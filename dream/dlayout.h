#ifndef __dlayout__
#define __dlayout__

#include <windows.h>
#include <string>

#include "dxml.h"
#include "dchain.h"

using namespace std;

namespace dream {

class dLayout {
	private:
		struct widget {
			DWORD	hID;		//id of widget
			HWND	hWnd;		//handle of widget
			string	name;		//name of widget
		};
		static LRESULT CALLBACK GroupProc
	private:
		dchain<widget>	widget_list;	//list of widget
	public:
		dLayout( char* filename );		//���ļ����ж�ȡ���� 
		dLayout( dXmlnode* layout );	//����״�ṹ�ж�ȡ���� 
		~dLayout();
		
		HWND getHandle( const DWORD& );
		HWND getHandle( const string& );
		
		DWORD getID( const HWND& );
		DWORD getID( const string& );
		
		string getName( const DWORD& );
		string getName( const HWND& );
		
		bool	setProc( const string&, WNDPROC );
		WNDPROC	getProc( const string& );
};

//end of file
}
#endif
