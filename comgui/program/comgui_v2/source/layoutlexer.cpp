#ifndef __layout_cpp__
#define __layout_cpp__

#include "layoutlexer.h"
#include <stdio.h>
#include <windows.h>
#include <wingdi.h>

WNDPROC layoutlexer::group_proc = 0;
static const string label_window	= "window";
static const string label_button	= "button";
static const string label_group		= "group";
static const string label_text		= "text";
static const string label_list		= "list";
static const string label_item		= "item";
LRESULT CALLBACK layoutlexer::GroupProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( msg == WM_COMMAND )	//使按钮消息透明 
		PostMessage( GetParent( hwnd ), WM_COMMAND, wParam, lParam );
	return group_proc( hwnd, msg, wParam, lParam );
}

bool	layoutlexer::lexerAny( dXmlnode* node, HWND h_father )			//从xml文档解析一系列子控件
{
	for( dXmlnode* tag = node->getChild(); tag != 0; tag = tag->getNext() )
	{
		if( tag->isText() )
			continue;
		
		string name = tag->getName();
		
		if( name == label_text )
		{
			if( !this->lexerText( tag, h_father ) )
				return false;
		}
		else if( name == label_button )
		{
			if( !this->lexerButton( tag, h_father ) )
				return false;
		}
		else if( name == label_group )
		{
			if( !this->lexerGroup( tag, h_father ) )
				return false;
		}
		else if( name == label_list )
		{
			if( !this->lexerList( tag, h_father ) )
				return false;
		}
	}
}

bool	layoutlexer::lexerWindow( dXmlnode* node, WNDPROC proc )	//从xml文档解析窗口
{
	WNDCLASSEX	wc;
	string		attr;
	string		caption;
	DWORD		left;
	DWORD		top;
	DWORD		width;
	DWORD		height;
	
	/***从文档节点中获取窗口的参数***/ 
	try {
		attr = this->m_frame->getAttr( "left" );
		sscanf( attr.data(), "%d", &left );
	}catch( ... ) {
		left = 0;
	}
	
	try {
		attr = this->m_frame->getAttr( "top" );
		sscanf( attr.data(), "%d", &top );
	}catch( ... ) {
		top = 0;
	}
	
	try {
		attr = this->m_frame->getAttr( "width" );
		sscanf( attr.data(), "%d", &width );
	}catch( ... ) {
		width = 0;
	}
	
	try {
		attr = this->m_frame->getAttr( "height" );
		sscanf( attr.data(), "%d", &height );
	}catch( ... ) {
		height = 0;
	}
	
	try {
		caption = this->m_frame->getAttr( "caption" );
	}catch( ... ) {
		caption = "caption";
	}

	/***设置窗口类参数***/ 
	memset(&wc,0,sizeof(wc));
	wc.cbSize			= sizeof(WNDCLASSEX);
	
	if( proc == 0 )
	
		wc.lpfnWndProc	= DefWindowProc; 
	else
		wc.lpfnWndProc	= proc;
		
	wc.hInstance		= GetModuleHandle( 0 );
	
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	
	wc.lpszClassName	= "WindowClass";
	
	try {
		attr = node->getAttr( "icon" ); 
		wc.hIcon		= LoadIcon( wc.hInstance, attr.data() );
		wc.hIconSm		= LoadIcon( wc.hInstance, attr.data() ); /* use the name "A" to use the project icon */
	}catch( ... ) {
		wc.hIcon		= LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
		wc.hIconSm		= LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */
	}
	
	/***尝试注册窗口类***/ 
	if(!RegisterClassEx(&wc))
		return false;

	/***尝试创建窗口***/ 
	this->m_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass", caption.data(),WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		left, /* x */
		top, /* y */
		width, /* width */
		height, /* height */
		NULL,NULL,wc.hInstance,NULL);

	if(this->m_hwnd == NULL)
		return false;
}
bool	layoutlexer::lexerText(  dXmlnode* node, HWND h_father )	//从xml文档解析文字
{	//创建文本
	string	attr;
	string	caption;
	DWORD	left;
	DWORD	top;
	DWORD	width;
	DWORD	height;
	DWORD	style = WS_CHILD|WS_VISIBLE|ES_READONLY;
	
	/***获取文本控件的数据***/ 
	try {
		attr = node->getAttr( "left" );
		sscanf( attr.data(), "%d", &left );
	}catch( ... ) {
		left = 0;
	}
	
	try {
		attr = node->getAttr( "top" );
		sscanf( attr.data(), "%d", &top );
	}catch( ... ) {
		top = 0;
	}
	
	try {
		attr = node->getAttr( "width" );
		sscanf( attr.data(), "%d", &width );
	}catch( ... ) {
		width = 0;
	}
	
	try {
		attr = node->getAttr( "height" );
		sscanf( attr.data(), "%d", &height );
	}catch( ... ) {
		height = 0;
	}
	
	try {
		attr = node->getAttr( "multiline" );
		if( attr == "true" )
			style |= ES_MULTILINE | WS_VSCROLL;
	}catch( ... ) {
		
	}
	
	try {
		attr = node->getAttr( "align" );
		if( attr == "left" )
			style |= ES_LEFT;
		else if( attr == "center" )
			style |= ES_CENTER;
		else if( attr ==  "right" )
			style |= ES_RIGHT;
		else
			return false;
	}catch( ... ) {
		style |= ES_LEFT;
	}
	
	if( node->getChild() != 0 )
		caption = node->getChild()->getText();
	
	HWND taw = CreateWindow( "edit", caption.data(), style,
				left, /* x */
				top, /* y */
				width, /* width */
				height, /* height */
				h_father,this->m_idcount++,GetModuleHandle( 0 ),NULL);
	if( taw == NULL )
		return false;
	
	/***设置文字大小***/
	try {
		int size;
		attr = node->getAttr( "text-size" );
		sscanf( attr.data(), "%d", &size );
		HFONT hf = CreateFont( size, size>>1, 0, 0, 400, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, NULL  );
		SendMessage( taw, WM_SETFONT, (WPARAM)hf, TRUE );
	}catch( ... ) {
		
	}
	
	/***试图将文本控件列为互动控件***/ 
	try{
		widget wid;
		wid.name = node->getAttr( "name" );
		wid.ID = LOWORD(this->m_idcount-1);
		wid.hWnd = taw;
		
		/***对互动控件查重***/ 
		if( this->getID( wid.name ) != 0 )
			return false;
	
		this->m_widgets.push( wid );
	}catch( ... ){
	}
	
	return true;
}
bool	layoutlexer::lexerButton( dXmlnode* node, HWND h_father )	//从xml文档解析按钮 
{
	string attr;
	string caption;
	string image;
	DWORD left;
	DWORD top;
	DWORD width;
	DWORD height;
	DWORD style;
	
	/***获取按钮控件的数据***/ 
	try {
		attr = node->getAttr( "left" );
		sscanf( attr.data(), "%d", &left );
	}catch( ... ) {
		left = 0;
	}
	
	try {
		attr = node->getAttr( "top" );
		sscanf( attr.data(), "%d", &top );
	}catch( ... ) {
		top = 0;
	}
	
	try {
		attr = node->getAttr( "width" );
		sscanf( attr.data(), "%d", &width );
	}catch( ... ) {
		width = 0;
	}
	
	try {
		attr = node->getAttr( "height" );
		sscanf( attr.data(), "%d", &height );
	}catch( ... ) {
		height = 0;
	}
	
	try {
		image = node->getAttr( "img" );
		style = WS_CHILD | WS_VISIBLE | BS_BITMAP;
	}catch( ... ) {
		style = WS_CHILD | WS_VISIBLE;
		try {
			caption = node->getAttr( "caption" );
		}catch( ... ) {
			caption = "button";
		}
	}
	
	HWND taw = CreateWindowEx( WS_EX_CLIENTEDGE, "BUTTON", caption.data(),style,
				left, /* x */
				top, /* y */
				width, /* width */
				height, /* height */
				h_father,(HMENU)this->m_idcount++,GetModuleHandle( 0 ),NULL);
	if( style & BS_BITMAP != 0 )
		SendMessage( taw, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadImage( GetModuleHandle( 0 ), image.data(), IMAGE_BITMAP, width, height, 0 ) );
	if( taw == NULL )
		return false;
		
	/***试图将文本控件列为互动控件***/ 
	try{
		widget wid;
		wid.name = node->getAttr( "name" );
		wid.ID = LOWORD(this->m_idcount-1);
		wid.hWnd = taw;
		
		/***对互动控件查重***/ 
		if( this->getID( wid.name ) != 0 )
			return false;
	
		this->m_widgets.push( wid );
	}catch( ... ){
	}
	
	return true;
}

bool	layoutlexer::lexerGroup( dXmlnode* node, HWND h_father )
{
	string attr;
	string caption;
	DWORD left;
	DWORD top;
	DWORD width;
	DWORD height;
	
	/***获取按钮控件的数据***/ 
	try {
		attr = node->getAttr( "left" );
		sscanf( attr.data(), "%d", &left );
	}catch( ... ) {
		left = 0;
	}
	
	try {
		attr = node->getAttr( "top" );
		sscanf( attr.data(), "%d", &top );
	}catch( ... ) {
		top = 0;
	}
	
	try {
		attr = node->getAttr( "width" );
		sscanf( attr.data(), "%d", &width );
	}catch( ... ) {
		width = 0;
	}
	
	try {
		attr = node->getAttr( "height" );
		sscanf( attr.data(), "%d", &height );
	}catch( ... ) {
		height = 0;
	}
	
	try {
		caption = node->getAttr( "caption" );
	}catch( ... ) {
		caption = "button";
	}
	
	HWND taw = CreateWindow( "BUTTON", caption.data(),WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
				left, /* x */
				top, /* y */
				width, /* width */
				height, /* height */
				h_father,(HMENU)this->m_idcount++,GetModuleHandle( 0 ),NULL);
	if( taw == NULL )
		return false;
		
	
	this->lexerAny( node, taw );
	
	if( group_proc == 0 )
		group_proc = (WNDPROC)GetWindowLongPtr( taw, GWLP_WNDPROC );
	SetWindowLongPtr( taw, GWLP_WNDPROC, (LONG_PTR)GroupProc );
	
	/***试图将文本控件列为互动控件***/ 
	try{
		widget wid;
		wid.name = node->getAttr( "name" );
		wid.ID = LOWORD(this->m_idcount-1);
		wid.hWnd = taw;
		
		/***对互动控件查重***/ 
		if( this->getID( wid.name ) != 0 )
			return false;
	
		this->m_widgets.push( wid );
	}catch( ... ){
	}
	
	return true;
}

bool	layoutlexer::lexerList( dXmlnode* node, HWND h_father )			//从xml文档解析列表
{
	string attr;
	string caption;
	DWORD left;
	DWORD top;
	DWORD width;
	DWORD height;
	DWORD style = WS_VISIBLE | WS_CHILD | LBS_NOTIFY;
	
	/***获取按钮控件的数据***/ 
	try {
		attr = node->getAttr( "left" );
		sscanf( attr.data(), "%d", &left );
	}catch( ... ) {
		left = 0;
	}
	
	try {
		attr = node->getAttr( "top" );
		sscanf( attr.data(), "%d", &top );
	}catch( ... ) {
		top = 0;
	}
	
	try {
		attr = node->getAttr( "width" );
		sscanf( attr.data(), "%d", &width );
	}catch( ... ) {
		width = 0;
	}
	
	try {
		attr = node->getAttr( "height" );
		sscanf( attr.data(), "%d", &height );
	}catch( ... ) {
		height = 0;
	}
	
	try {
		attr = node->getAttr( "border" );
		if( attr == "true" )
			style |= WS_BORDER;
	}catch( ... ) {
		
	}
	
	try {
		attr = node->getAttr( "vscroll" );
		if( attr == "true" )
			style |= WS_VSCROLL;
	}catch( ... ) {
		
	}
	
	HWND taw = CreateWindowEx( WS_EX_CLIENTEDGE, "LISTBOX", caption.data(), style,
				left, /* x */
				top, /* y */
				width, /* width */
				height, /* height */
				h_father,(HMENU)this->m_idcount++,GetModuleHandle( 0 ),NULL);
	if( taw == NULL )
		return false;
		
	for( dXmlnode* tag = node->getChild(); tag != 0; tag = tag->getNext() )
	{
		if( tag->isText() )
			continue;
		
		string name = tag->getName();
		
		if( name == label_item )
		{
			name = tag->getAttr( "value" );
			SendMessage( taw, LB_ADDSTRING, 0, (LPARAM)name.data() );
		}
	}
	
	SendMessage( taw, LB_SETCURSEL, 0, 0 );
	
	/***试图将文本控件列为互动控件***/ 
	try{
		widget wid;
		wid.name = node->getAttr( "name" );
		wid.ID = LOWORD(this->m_idcount-1);
		wid.hWnd = taw;
		
		/***对互动控件查重***/ 
		if( this->getID( wid.name ) != 0 )
			return false;
	
		this->m_widgets.push( wid );
	}catch( ... ){
	}
	
	return true;
}

layoutlexer::layoutlexer( void ):		//默认构造函数
m_frame(0),m_hwnd(0),m_idcount((HMENU)0x01)
{
}
layoutlexer::~layoutlexer( void )		//默认析构函数
{
	if( this->m_frame != 0 )
		delete this->m_frame;
	if( m_hwnd != NULL )
		DestroyWindow( this->m_hwnd );
}

bool	layoutlexer::lexer( string filename, WNDPROC proc )	//解析布局文件并生成布局
{
	//打开文件 
	HANDLE hf = CreateFile( filename.data(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
	if( hf == INVALID_HANDLE_VALUE )
		return false;	//若未打开文件,则失败
	
	//读取文件内容 
	DWORD dwFileSize = GetFileSize( hf, 0 );
	DWORD dwRead;
	char* buffer = new char[dwFileSize + 1];
	ReadFile( hf, buffer, dwFileSize, &dwRead, 0 );
	buffer[dwFileSize] = '\0';
	
	/***解析dxml文档***/ 
	dXmlanal cXml;
	try{
		this->m_frame = cXml.analysisCode( buffer );
	}catch( invalid_argument& e ){
		char buf[256];
		sprintf( buf, " layout document error in %s offset %d :%s", filename.data(), cXml.getOffs(), e.what() );
		MessageBox( NULL, buf, "error", MB_OK );
		return false;
	}
	
	/***检查首节点是否为窗口节点***/ 
	if( this->m_frame->getName() != label_window )
		return false;
	else if( !this->lexerWindow( this->m_frame, proc ) )
		return false;
	
	this->lexerAny( this->m_frame, this->m_hwnd );
	return true;
}

bool	layoutlexer::isWidget( WORD ID )	//检查某窗体是否是互动控件
{
	int i = 0;
	for( widget* tag = this->m_widgets.at(i++); tag != 0; tag = this->m_widgets.at(i++) )
		if( tag->ID == ID )
			return true;
	return false;
}
string	layoutlexer::getName( WORD ID )	//获取窗体对应控件的名字
{
	int i = 0;
	for( widget* tag = this->m_widgets.at(i++); tag != 0; tag = this->m_widgets.at(i++) )
		if( tag->ID == ID )
			return tag->name;
	throw invalid_argument( "widget not found" );
}
WORD	layoutlexer::getID( string name )	//获取名字对应的控件窗体
{
	int i = 0;
	for( widget* tag = this->m_widgets.at(i++); tag != 0; tag = this->m_widgets.at(i++) )
		if( tag->name == name )
			return tag->ID;
	return 0;
}

HWND	layoutlexer::getHWND( string name )
{
	int i = 0;
	for( widget* tag = this->m_widgets.at(i++); tag != 0; tag = this->m_widgets.at(i++) )
		if( tag->name == name )
			return tag->hWnd;
	return NULL;
}
HWND	layoutlexer::getHWND( WORD ID )	//获取ID对应的控件窗体 
{
	int i = 0;
	for( widget* tag = this->m_widgets.at(i++); tag != 0; tag = this->m_widgets.at(i++) )
		if( tag->ID == ID )
			return tag->hWnd;
	return 0;
}
HWND	layoutlexer::getMain( void )	//获取主窗体 
{
	return this->m_hwnd;
}

#endif
