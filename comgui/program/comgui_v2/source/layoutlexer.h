#ifndef __layout__
#define __layout__

#include <windows.h>
#include <string>
#include "dxml.h"
#include "dchain.cpp"

using namespace std;
using namespace dream;

struct widget {
	public:
		string	name;
		WORD	ID;
		HWND	hWnd;
};

typedef dchain<widget> widgets;

class layoutlexer {
	private:
		dXmlnode*	m_frame;		//主框架指针
		HWND		m_hwnd;			//主框架窗口
		widgets		m_widgets;		//参与互动的控件列表 
		HMENU		m_idcount;		//控件ID 
		
		static	WNDPROC	group_proc;
	private:
		bool	lexerAny( dXmlnode*, HWND );			//从xml文档解析一系列子控件 
		bool	lexerWindow( dXmlnode*, WNDPROC );		//从xml文档解析窗口
		bool	lexerText(  dXmlnode*, HWND );			//从xml文档解析文字
		bool	lexerButton( dXmlnode*, HWND );			//从xml文档解析按钮 
		bool	lexerGroup( dXmlnode*, HWND );			//从xml文档解析分组
		bool	lexerList( dXmlnode*, HWND );			//从xml文档解析列表 
		static LRESULT CALLBACK GroupProc( HWND, UINT, WPARAM, LPARAM );
	public:
		layoutlexer( void );		//默认构造函数
		~layoutlexer( void );		//默认析构函数
		
		bool	lexer( string, WNDPROC );	//解析布局文件并生成布局
		
		bool	isWidget( WORD );	//检查某窗体是否是互动控件
		string	getName( WORD );	//获取窗体对应控件的名字
		WORD	getID( string );	//获取名字对应的控件窗体
		HWND	getHWND( WORD );	//获取ID对应的控件窗体 
		HWND	getHWND( string );	//获取名字对应的窗体 
		HWND	getMain( void );	//获取主窗体 
};
#endif
