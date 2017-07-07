#ifndef __clayout_cpp__
#define __clayout_cpp__

#include "clayout.h"
#include <stdio.h>
#include <stdlib.h>

int clayout::m_class_count = 0;

LRESULT CALLBACK clayout::GroupProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static HWND hMother = NULL;
	static WNDPROC group_proc = 0;
	switch( msg )
	{
		case MSG_SETMOTHER:
			hMother = (HWND)lParam;
			break;
		case WM_COMMAND:
			PostMessage( hMother, msg, wParam, lParam );
			break;
		case MSG_LOADPROC:
			group_proc = (WNDPROC)lParam;
			break;
	}
	if( group_proc != 0 )
		return group_proc( hwnd, msg, wParam, lParam );
	else
		return DefWindowProc( hwnd, msg, wParam, lParam );
}

bool	clayout::lexer( dXmlnode* node, HWND hFather )
{
	if( node == 0 )
		return true;
	
	DWORD x,y;
	DWORD width, height;
	DWORD style = WS_VISIBLE;
	string attr;
	string caption;
	HWND hwnd = 0;
	
	/***通用属性***/ 
	if( node->getAttr( "x", attr ) )
		sscanf( attr.data(), "%d", &x );
	else
		x = 0;
	
	if( node->getAttr( "y", attr ) )
		sscanf( attr.data(), "%d", &y );
	else
		y = 0;
	
	if( node->getAttr( "width", attr ) )
		sscanf( attr.data(), "%d", &width );
	else
		width = 0;
	
	if( node->getAttr( "height", attr ) )
		sscanf( attr.data(), "%d", &height );
	else
		height = 0;
	
	if( node->getAttr( "caption", attr ) )
		caption = attr;
	
	if( node->getName() == "layout" )
	{
		if( hFather != 0 )
			return false;
		
		char clsname[256];
		memset( clsname, 0, 256 );
		sprintf( clsname, "wndclass%d", m_class_count++ );
		
		WNDCLASS wcls;
		memset( &wcls, 0, sizeof( wcls ) );
		
		wcls.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcls.lpfnWndProc	= this->m_proc;
		wcls.hInstance	 	= GetModuleHandle( 0 );
		wcls.lpszClassName 	= clsname;
		wcls.hCursor		= LoadCursor(NULL, IDC_ARROW);
		
		if( node->getAttr( "icon", attr ) )
			wcls.hIcon		= LoadIcon( GetModuleHandle( 0 ), attr.data() );
		else
			wcls.hIcon		= LoadIcon(NULL, IDI_APPLICATION);
			
		if( node->getAttr( "close", attr ) )
		{
			if( attr == "false" )
				wcls.style |= CS_NOCLOSE;
		}
		
		if( !RegisterClass( &wcls ) )
			return false;
			
		style |= WS_CAPTION | WS_SYSMENU;
		
		if( node->getAttr( "minimizebox", attr ) )
		{
			if( attr == "true" )
				style |= WS_MINIMIZEBOX;
		}
		
		if( node->getAttr( "maximizebox", attr ) )
		{
			if( attr == "true" )
				style |= WS_MAXIMIZEBOX;
		}
		
		this->m_hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, clsname, caption.data(), 
		style, x, y, width, height, 0, 0, 
		GetModuleHandle( 0 ), 0 );
		
		if( this->m_hwnd == NULL )
			return false;
		
		return this->lexer( node->getChild(), this->m_hwnd );
	}
	else if( hFather == 0 )
		return false;
	else if( node->getName() == "text" )
	{
		dXmlnode* value = node->getChild();
		if( value == 0 or !value->isText() )
			return false;
		caption = value->getText();
		
		if( node->getAttr( "align", attr ) )
		{
			if( attr == "left" )
				style |= ES_LEFT;
			else if( attr == "right" )
				style |= ES_RIGHT;
			else if( attr == "center" )
				style |= ES_CENTER;
		}
		
		if( node->getAttr( "multiline", attr ) )
		{
			if( attr == "true" )
				style |= ES_MULTILINE;
		}
		
		if( node->getAttr( "readonly", attr ) )
		{
			if( attr == "true" )
				style |= ES_READONLY;
		}
		else
		{
			style |= WS_BORDER;
		}
		
		hwnd = CreateWindow( "EDIT", caption.data(), style | WS_CHILD,
			x, y, width, height,
			hFather, this->m_widgets.count() + (HMENU)1, GetModuleHandle( 0 ), 0 );
		if( hwnd == NULL )
			return false;
			
		if( node->getAttr( "text-size", attr ) )
		{
			int size;
			sscanf( attr.data(), "%d", &size );
			HFONT hFont = CreateFont( size, size>>1, 0, 0, 400, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, NULL  );
			PostMessage( hwnd, WM_SETFONT, (WPARAM)hFont, TRUE );
		}
	}
	else if( node->getName() == "button" )
	{
		hwnd = CreateWindow( "BUTTON", caption.data(), style | WS_CHILD,
			x, y, width, height,
			hFather, this->m_widgets.count() + (HMENU)1, GetModuleHandle( 0 ), 0 );
		if( hwnd == NULL )
			return false;
	}
	else if( node->getName() == "group" )
	{
		hwnd = CreateWindow( "BUTTON", caption.data(), style | WS_CHILD | BS_GROUPBOX,
			x, y, width, height,
			hFather, this->m_widgets.count() + (HMENU)1, GetModuleHandle( 0 ), 0 );
		if( hwnd == NULL )
			return false;
		
		WNDPROC proc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
		SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)clayout::GroupProc );
		SendMessage( hwnd, MSG_LOADPROC, 0, (LPARAM)proc );
		
		if( !this->lexer( node->getChild(), hwnd ) )
			return false;
			
		SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)clayout::GroupProc );
		PostMessage( hwnd, WM_USER + 1, 0, (LPARAM)this->m_hwnd );
	}
	else if( node->getName() == "list" )
	{
		
		if( node->getAttr( "border", attr ) )
		{
			if( attr == "true" )
				style |= WS_BORDER;
		}
		
		if( node->getAttr( "vscroll", attr ) )
		{
			if( attr == "true" )
				style |= WS_VSCROLL;
		}
		
		hwnd = CreateWindow( "LISTBOX", caption.data(), style | WS_CHILD | LBS_NOTIFY,
			x, y, width, height,
			hFather, this->m_widgets.count() + (HMENU)1, GetModuleHandle( 0 ), 0 );
		if( hwnd == NULL )
			return false;
		
		if( !this->lexer( node->getChild(), hwnd ) )
			return false;
		
		SendMessage( hwnd, LB_SETCURSEL, 0, 0 );
	}
	else if( node->getName() == "item" )
	{
		node->getAttr( "value", attr );
		SendMessage( hFather, LB_ADDSTRING, 0, (LPARAM)attr.data() );
	}
	else if( node->getName() == "combobox" )
	{
		hwnd = CreateWindow( "COMBOBOX", "", style | WS_CHILD | CBS_DROPDOWNLIST,
			x, y, width, height,
			hFather, this->m_widgets.count() + (HMENU)1, GetModuleHandle( 0 ), 0 );
		if( hwnd == NULL )
			return false;
		
		if( !this->lexer( node->getChild(), hwnd ) )
			return false;
		
		SendMessage( hwnd, CB_SETCURSEL, 0, 0 );
	}
	else if( node->getName() == "option" )
	{
		node->getAttr( "value", attr );
		SendMessage( hFather, CB_ADDSTRING, 0, (LPARAM)attr.data() );
	}
	
	
	if( node->getAttr( "name", attr ) and 0 == this->getHandle( attr ) )
	{
		element eletmp;
		eletmp.hwnd = hwnd;
		eletmp.ID = this->m_widgets.count() + 1;
		eletmp.name = attr;
		this->m_widgets.push( eletmp );
	}
	
	return this->lexer( node->getNext(), hFather );
}

clayout::clayout( void ):
m_hwnd(0) 
{
	
}
clayout::~clayout()
{
	this->clear();
}

bool clayout::create( string filename, WNDPROC proc )
{
	HANDLE hFile = CreateFile( filename.data(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
	if( hFile == INVALID_HANDLE_VALUE )
		return false;
		
	DWORD dwLength = GetFileSize( hFile, 0 );
	char *pBuf = new char[dwLength + 1];
	DWORD dwRead;
	ReadFile( hFile, pBuf, dwLength, &dwRead, 0 );
	CloseHandle( hFile );
	pBuf[dwLength] = '\0';
	
	dXmlanal anal;
	dXmlnode* node;
	try {
		node = anal.analysisCode( pBuf );
	}catch( ... ) {
		return false;
	}
	
	if( proc == 0 )
		this->m_proc = DefWindowProc;
	else
		this->m_proc = proc;
	
	return this->lexer( node, NULL );
}
bool clayout::clear( void )
{
	if( this->m_hwnd != 0 )
	{	
		DestroyWindow( this->m_hwnd );
		this->m_hwnd = 0;
		this->m_widgets.clear();
		return true;
	}
	return false;
}

HWND clayout::getHandle( string name )
{
	int i = 0;
	for( element* tag = this->m_widgets.at( i++ ); tag != 0; tag = this->m_widgets.at( i++ ) )
		if( tag->name == name )
			return tag->hwnd;
	return NULL;
}
HWND clayout::getHandle( WORD ID )
{
	int i = 0;
	if( ID == 0 )
		return this->m_hwnd;
	for( element* tag = this->m_widgets.at( i++ ); tag != 0; tag = this->m_widgets.at( i++ ) )
		if( tag->ID == ID )
			return tag->hwnd;
	return NULL;
}
WORD clayout::getID( string name )	//获取控件ID
{
	int i = 0;
	for( element* tag = this->m_widgets.at( i++ ); tag != 0; tag = this->m_widgets.at( i++ ) )
		if( tag->name == name )
			return tag->ID;
	return 0;
}
#endif
