#ifndef __dframe_cpp__
#define __dframe_cpp__

#include "dframe.h"
#include "dchain.cpp"
#include <iostream>
#include <string>
#include <windows.h>
#include <wingdi.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <winuser.h>

using namespace std;

namespace dream {

/***********dColor结构体成员函数***********/
dColor::dColor( void )
{
	memset( this->value, 0, 4 );
}
dColor::dColor( GLubyte* v )
{
	if( v != 0 )
		memcpy( this->value, v, 4 );
}
dColor::dColor( const dColor& cl )
{
	memcpy( this->value, cl.value, 4 );
}
dColor::dColor( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	this->red	=	r;
	this->green	=	g;
	this->blue	=	b;
	this->alpha	=	a;
}
dColor& dColor::operator= ( const dColor& c )
{
	memcpy( this->value, c.value, 4 );
}


/***********dQuad结构体成员函数***********/
dQuad::dQuad( void )
{
	this->x = 0;
	this->y = 0;
	this->width = 0;
	this->height = 0;
}
dQuad::dQuad( long x, long y, long w, long h )
{
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
}
dQuad& dQuad::operator= ( const dQuad& q )
{
	memcpy( this, &q, sizeof( dQuad ) );
}
dQuad& dQuad::operator= ( const dRect& r )
{
	this->x = r.left;
	this->y = r.top;
	this->width = r.right - r.left;
	this->height = r.bottom - r.top;
}
bool	dQuad::contain( long x, long y )
{
	return (x >= this->x and x <= this->x + this->width and y >= this->y and y <= this->y + this->height);
}
bool	dQuad::contain(  dQuad& q )
{
	return (this->contain( q.x, q.y ) and this->contain( q.x + q.width, q.y + q.height ) );
}
bool	dQuad::belongto(  dQuad& q )
{
	return q.contain( *this );
}
bool	dQuad::overlap(  dQuad& q )
{
	return !(
		q.x > this->x + this->width or
		q.x + q.width < this->x or
		q.y > this->y + this->height or
		q.y + q.height < this->y );
}

/***********dRect结构体的成员函数***********/
dRect::dRect( void )
{
	this->left = 0;
	this->right = 0;
	this->top = 0;
	this->bottom = 0;
}
dRect::dRect( long l, long r, long t, long b )
{
	this->left = l;
	this->right = r;
	this->top = t;
	this->bottom = b;
}
dRect& dRect::operator= ( const dRect& r )
{
	memcpy( this, &r, sizeof( dRect ) );
}
dRect& dRect::operator= ( const dQuad& q )
{
	this->left = q.x;
	this->right = q.x + q.width;
	this->top = q.y;
	this->bottom = q.y + q.height;
}
bool	dRect::contain( long x, long y )		//是否包含了一个点 
{
	return (x >= this->left and x <= this->right and y >= this->top and y <= this->bottom);
}
bool	dRect::contain( dRect& r )	//是否包含了一个rect 
{
	return ( this->contain( r.left, r.top ) and this->contain( r.right, r.bottom ) );
}
bool	dRect::belongto( dRect& r )	//是否被一个rect包含 
{
	return r.contain( *this );
}
bool	dRect::overlap( dRect& r )	//是否与一个rect重叠
{
	return !( 
		r.left > this->right or
		r.right < this->left or
		r.top > this->bottom or
		r.bottom < this->top );
}

/********************dWidget类的成员函数********************/
/***需要任何继承本类的类重载的重要函数***/
duiClass dWidget::toClass( void )	//用以迅速分辨子类的具体类型
{
	return dCwidget;
}
void dWidget::onEvent( dTask task )		//用于处理事件的函数 
{
	/***此函数暂时为空***/ 
}
void dWidget::onDraw( dFrame& frame )		//用于处理绘制过程的函数
{
	/***次函数暂时为空***/ 
}

/***构造函数会规定父控件的存在***/
dWidget::dWidget( dField* field )					//同时会设定一个全树领域内不重复的id
{
	this->m_field = 0;
	/***生成默认id***/ 
	
//	this->m_id = 0;	//第一次设置默认id 
//	/***如果指定了域***/ 
//	if( field != 0 )
//	{
//		for( int i = 0; !field->installChild( this ); this->m_id = ++i );	//不断用新的id尝试进入域 
//	}
	char 	tmpbuf[64];
	int		i = 0;
	bool	bRepeat;
	do
	{
		bRepeat = false;
		sprintf( tmpbuf, "default-id-%d", i++ );
		if( field != 0 )
			if( field->searchChild( tmpbuf ) != 0 )
				bRepeat = true;
	}while( bRepeat );
	
	this->m_id = tmpbuf;
	if( field != 0 )
		field->installChild( this );
	return;
}
dWidget::~dWidget( void )					//析构函数会自动将自己从父控件中除名 
{
	if( this->m_field != 0 )
		this->m_field->kickChild( this );	//如果域能在子控件列表中找到这个控件则除名 
}

/***基本操作函数***/ 
bool	dWidget::setId( const string& id )
{
	if( 0 == searchFrame( id ) )
	{
		this->m_id = id;
		return true;
	}
	return false;
}
string	dWidget::getId( void )	const						//获取Id
{
	return this->m_id;
}
bool	dWidget::setShape( const dQuad& quad )
{
	if( quad.width < 0 or quad.height < 0 )
		return false;
	this->m_shape = quad;
	return true;
}
dQuad dWidget::getShape( void )	const			//获取位置及形状
{
	return this->m_shape;
}
dField* dWidget::getField( void ) const			//获取当前控件所在域
{
	return this->m_field;
}
dField*	dWidget::getTopField( void )		//获取当前控件所在的顶层域 
{
	dField* t = dynamic_cast< dField*>(this);
	if( t == 0 )
		t = this->m_field;
	for( ; t != 0 and t->m_field != 0; t = t->m_field );
	return t;
}
bool	dWidget::catchFocus( void )			//为自己抓获键盘焦点,将调用父控件的catchFocus函数  
{
	if( this->m_field == 0 )
		return false;
	return this->m_field->catchFocus( this );
}
dWidget*	dWidget::searchFrame( const string& id )
{
	dField* top = this->getTopField();
	if( top == 0 )
		return 0;
	else if( top->getId() == id )
		return top;
	else
		return top->searchChild( id );
}
dWidget*	dWidget::searchFrame( const dWidget* ptr )	//在整颗控件树内搜索控件
{
	dField* top = this->getTopField();
	if( top == 0 )
		return 0;
	else if( top == ptr )
		return top;
	else
		return top->searchChild( ptr );
}
void	dWidget::setRender( dRender render )			//设置绘制器 
{
	this->m_render = render;
}
dRender	dWidget::getRender( void )					//获取绘制器 
{
	return this->m_render;
}

/********************dField类的成员函数********************/
/***构造函数***/ 
dField::dField( dField* field ):dWidget( field )
{
	this->m_key_focus = 0;
	this->m_render = 0;
}
dField::~dField( void )	//析构函数会在自己被销毁之前解散子控件树
{
	while( !this->m_child.bEmpty() )
		delete this->m_child[0];	//直接删除子控件,子控件会自动将自己除名 
}

/***基本操作函数***/
bool	dField::catchFocus( dWidget* tag )			//为子控件抓取键盘焦点,若传递0则表示为自己抓获键盘焦点
{
	if( tag == 0 )
	{
		if( this->getField() != 0 )
			return this->getField()->catchFocus( this );
		return false;
	}
	if( this->searchChild( tag ) != 0 )	//确认要求的控件是自己的子控件
	{
		this->clearFocus();
		this->m_key_focus = tag;
		if( this->getField() != 0 )
			return this->getField()->catchFocus( this );
	}
	return false;
}
void	dField::clearFocus( void )					//清除本区域内的键盘焦点
{
	this->m_key_focus = 0;	
}
void	dField::setBorderColor( const dColor& cl )	//设置边框颜色
{
	this->m_cl_board = cl;
}
dColor	dField::getBorderColor( void )				//获取边框颜色 
{
	return this->m_cl_board;
}
void	dField::setBackgColor( const dColor& cl )	//设置背景颜色
{
	this->m_cl_backg = cl;
}
dColor	dField::getBackgColor( void )				//获取背景颜色
{
	return this->m_cl_backg;
}
/***有关子控件的操作函数***/
dWidget*	dField::searchChild( const string& id )
{
	dWidget* tag;
	dField* tcd;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		/***测试当前目标是否符合要求***/ 
		tag = this->m_child[i];
		if( id == tag->getId() )
			return tag;
		/***如果当前目标拥有子控件,则应该测试其子控件***/ 
		tcd = dynamic_cast<dField*>(tag);
		if( tcd != 0 )
		{
			tag = tcd->searchChild( id );
			if( tag != 0 )
				return tag;
		}
	}
	return 0;
}
dWidget*	dField::searchChild( const dWidget* tg )	//在子控件树中搜索控件
{
	if( tg == 0 )
		return 0;
	this->m_child.focus( 0 );
	dWidget* tag;
	dField* tcd;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		/***测试当前目标是否符合要求***/ 
		tag = this->m_child[i];
		if( tag == tg )
			return tag;
		/***如果当前目标拥有子控件,则应该测试其子控件***/ 
		tcd = dynamic_cast<dField*>(tag);
		if( tcd != 0 )
		{
			tag = tcd->searchChild( tg );
			if( tag != 0 )
				return tag;
		}
	};
	return 0;
}
dWidget*	dField::enumChild( int n )				//按照安装次序列举子控件
{
		try {
			return this->m_child[ n ];
		}catch( ... )
		{
			return 0;
		}
}
bool		dField::removeChild( const string& id )	//将子节点直接删除 
{
	dWidget* tag = this->searchChild( id );
	if( tag != 0 )
	{
		//删除时控件会自动除名 
		delete tag;
		return true;
	}
	return false;	
}

dWidget*	dField::pickChild( long x, long y )		//在子控件链中寻找被像素点 
{
	dWidget* t = 0;
	dQuad tqd;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		t = this->m_child[i];
		tqd = t->getShape();
		if( tqd.contain( x, y ) )
			return t;
	};
	return 0; 
}
bool		dField::kickChild( const dWidget* tag )		//将子控件除名 
{
	if( tag == 0 )
		return false;
	dField* tfd = tag->getField();
	if( tfd == 0 )
		return false;
	else if( tfd == this )
	{
		for( int i = 0; this->m_child.at(i) != 0; i++ )
		{		
			dWidget* temp = this->m_child[i];
			if( temp == tag )
			{
				this->m_child.remove();
				return true;
			}
		};
		return false;
	}
	else
		return tfd->kickChild( tag );
}
void		dField::clearChild( void )				//清空子控件树,删除所有子节点 
{
	while( !this->m_child.bEmpty() )
		delete this->m_child[0];
}
bool		dField::installChild( dWidget* tag )		//将控件安装到子链中,不能重名 
{
	if( tag == 0 )
		return false;
	if( tag->getField() != 0 )
		return false;
	if( this->searchFrame( tag ) == 0 and this->searchFrame( tag->getId() ) == 0 )
	{
		this->m_child.push( tag );
		tag->m_field = this;
		return true;
	}
	return false;
}

/***需要任何继承本类的类重载的重要函数***/
duiClass dField::toClass( void )	//用以迅速分辨子类的具体类型
{
	return dCfield;
}
void dField::onEvent( dTask task )		//用于处理事件的函数 
{
	/***field自己要检索的消息***/ 
	switch( task.task_mask )
	{
		case WM_LBUTTONDOWN:		//如果在field内单击左键会抢夺键盘焦点 
			if( this->m_shape.contain( task.mouse_x + this->m_shape.x, task.mouse_y + this->m_shape.y ) )
				this->catchFocus();
			break;
		case WM_RBUTTONDOWN:		//在field内空白处单击右键会触发右键事件 
			if( this->m_shape.contain( task.mouse_x + this->m_shape.x, task.mouse_y + this->m_shape.y ) )
				if( this->pickChild( task.mouse_x, task.mouse_y ) == 0 )
				{
					dFrame* frame = dynamic_cast<dFrame*>(this->getTopField());
					dEvent dev;
					dev.event_mask = dVclickR;
					dev.event_param = 0;
					dev.widget = this;
					if( frame != 0 )
						frame->pushEvent( dev );
				}
			break;	
	}
	/***毫无保留的传递所有的task***/
	dQuad tqd;
	dWidget* tag;
	dTask ttk;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		tag = this->m_child[i];
		tqd = tag->getShape();
		ttk = task;
		ttk.mouse_x -= tqd.x;
		ttk.mouse_y -= tqd.y;
		tag->onEvent( ttk );
	}
}
void dField::onDraw( dFrame& frame )		//用于处理绘制过程的函数
{
	/***绘制自己***/
	dRect rect;
	rect = this->m_shape;
	glColor4ubv( this->m_cl_backg.value );
	glBegin( GL_QUADS );
	glVertex2i( rect.left, rect.top );
	glVertex2i( rect.right, rect.top );
	glVertex2i( rect.right, rect.bottom );
	glVertex2i( rect.left, rect.bottom );
	glEnd();
	glColor4ubv( this->m_cl_board.value );
	glBegin( GL_LINES );
	glVertex2i( rect.left, rect.top );
	glVertex2i( rect.right, rect.top );
	
	glVertex2i( rect.right, rect.top );
	glVertex2i( rect.right, rect.bottom );
	
	glVertex2i( rect.right, rect.bottom );
	glVertex2i( rect.left, rect.bottom );
	
	glVertex2i( rect.left, rect.bottom );
	glVertex2i( rect.left, rect.top );
	glEnd();
	
	/***为子控件调用绘制过程***/ 
	glPushMatrix();
	glTranslatef( this->m_shape.x, this->m_shape.y, 0.0f );
	/***绘制函数被当成内置的画布来处理***/ 
	if( this->m_render != 0 )
		this->m_render( frame, *this );
	this->m_child.focus( 0 );
	dWidget* tag;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		tag = this->m_child[i];
		tag->onDraw( frame );
	};
	glPopMatrix();
}

/*********************dFrame类的成员函数********************/
/***构造函数***/
dFrame::dFrame( void ):dField( 0 )		//frame不可以作为子控件出现
{
	/***配置没有默认构造函数的成员变量***/ 
	this->m_BorderWidth = 0;
	/***尚待开放的功能***/ 
	//this->m_b_clsbx = true;
	//this->m_b_draft = true;
	//this->m_b_maxbx = true;
	//this->m_b_minibx = true;
	this->m_b_run = false;		//run口令由绘制线程设置,由destroyWindows函数清除 
	this->m_b_pause = false;	//绘制线程暂停口令
	this->m_b_size = false;
	this->m_cursor = LoadCursor( NULL, IDC_ARROW );
	this->m_hDC = 0;
	this->m_hDrawThread = 0;
	this->m_hInstance = GetModuleHandle( 0 );
	this->m_HintbarHeight = 0;
	this->m_hRC = 0;
	this->m_hWnd = 0;
	this->m_icon = LoadIcon( NULL, IDI_APPLICATION );
	this->m_proc = DefWindowProc;
	this->m_SysbarHeight = 0;
	/***开始设置id***/
	//for( int i = 0; !this->setId( i ); i++ );		//不断用新的id尝试进入域 
	char 	tmpbuf[64];
	int		i = 0;
	bool	bRepeat;
	do
	{
		bRepeat = false;
		sprintf( tmpbuf, "default-id-%d", i++ );
		if( dFrame::findFrame( tmpbuf ) != 0 )
			bRepeat = true;
	}while( bRepeat );
	dFrame::global_frame_list.push( this );
}
dFrame::~dFrame( void )	//frame销毁时会解散子控件树,销毁窗口
{
	this->destroyWindow();
	return;
}

/***常规操作函数***/
//边框宽度相关
void	dFrame::setBorderWidth( long w )
{
	if( 
		w < 0 or
		(w > this->m_SysbarHeight and this->m_SysbarHeight != 0 ) or 
		(w << 1) > this->m_shape.width or 
		(w > this->m_HintbarHeight and this->m_HintbarHeight != 0 ) )
		return ;
	this->m_BorderWidth = w;
}
long	dFrame::getBorderWidth( void )
{
	return this->m_BorderWidth;
}
//标题框高度相关 
void	dFrame::setSysbarHeight( long h )
{
	if( h < this->m_BorderWidth or h > this->getShape().height - this->m_HintbarHeight )
		return;
	this->m_SysbarHeight = h;
	this->m_tx_Sysbar.setShape( dQuad( 0, 0, this->m_shape.width, h ) );
	this->m_tx_Sysbar.setFont( this->m_SysbarHeight*0.75, 700, false, false, false );
}
long	dFrame::getSysbarHeight( void )
{
	return this->m_SysbarHeight;
}
//提示框高度相关 
void	dFrame::setHintbarHeight( long h )
{
	if( h < this->m_BorderWidth or h > this->getShape().height - this->m_SysbarHeight )
		return;
	this->m_HintbarHeight = h;
	this->m_tx_Hintbar.setShape( dQuad( 0, this->m_shape.height - h, this->m_shape.width, h ) );
	this->m_tx_Hintbar.setFont( this->m_HintbarHeight*0.75, 700, false, false, false );
}
long	dFrame::getHintbarHeight( void )
{
	return this->m_HintbarHeight;
}
//标题框文字相关 
void	dFrame::setSysbarText( const string& s )
{
	this->m_tx_Sysbar = s;
	this->m_tx_Sysbar.setFont( this->m_SysbarHeight*0.75, 700, false, false, false );
}
string	dFrame::getSysbarText( void )
{
	return this->m_tx_Sysbar;
}
//提示框文字相关 
void	dFrame::setHintbarText( const string& s )
{
	this->m_tx_Hintbar = s;
	this->m_tx_Hintbar.setFont( this->m_HintbarHeight*0.75, 700, false, false, false );
}
string	dFrame::getHintbarText( void )
{
	return this->m_tx_Hintbar;
}
//标题框颜色相关 
void	dFrame::setSysbarColor( const dColor& cl )
{
	this->m_cl_Sysbar = cl;
} 
dColor	dFrame::getSysbarColor( void )
{
	return this->m_cl_Sysbar;
}
//标题框边框颜色相关 
void	dFrame::setSysbarBorderColor( const dColor& cl )
{
	this->m_cl_SysbarBorder = cl;
}
dColor	dFrame::getSysbarBorderColor( void )
{
	return this->m_cl_SysbarBorder;
}
//标题框文字颜色相关 
void	dFrame::setSysbarTextColor( const dColor& cl )
{
	this->m_tx_Sysbar.setColor( cl );
}
dColor	dFrame::getSysbarTextColor( void )
{
	return m_tx_Sysbar.getColor();
}
//提示框颜色相关 
void	dFrame::setHintbarColor( const dColor& cl )
{
	this->m_cl_Hintbar = cl;
}
dColor	dFrame::getHintbarColor( void )
{
	return this->m_cl_Hintbar;
}
//提示框边框颜色相关 
void	dFrame::setHintbarBorderColor( const dColor& cl )
{
	this->m_cl_HintbarBorder = cl;
}
dColor	dFrame::getHintbarBorderColor( void )
{
	return this->m_cl_HintbarBorder;
}
//提示框边框颜色相关 
void	dFrame::setHintbarTextColor( const dColor& cl )
{
	this->m_tx_Hintbar.setColor( cl );
}
dColor	dFrame::getHintbarTextColor( void )
{
	return this->m_tx_Hintbar.getColor();
}
//应用程序图标相关

void	dFrame::setIcon( HICON ic )
{
	if( ic == 0 )
	{
		this->m_icon = LoadIcon( NULL, IDI_APPLICATION );
	}
	if( this->m_hWnd != 0 )
		SetClassLongPtr( this->m_hWnd, GCLP_HICON, (LONG_PTR)ic );
}
HICON	dFrame::getIcon( void )
{
	return this->m_icon;
}
//应用程序鼠标相关
void	dFrame::setCursor( HCURSOR cr )
{
	if( cr == 0 )
	{
		this->m_cursor = LoadCursor( NULL, IDC_ARROW );
	}
	if( this->m_hWnd != 0 )
		SetClassLongPtr( this->m_hWnd, GCLP_HCURSOR, (LONG_PTR)cr );
}
HCURSOR	dFrame::getCursor( void )
{
	return this->m_cursor;
}
//拖拽功能相关
void	dFrame::setDraftFunc( bool b )
{
	this->m_b_draft = b;
}
bool	dFrame::getDraftFunc( void )
{
	return this->m_b_draft;
}
//缩放功能相关
void	dFrame::setSizeFunc( bool b )
{
	this->m_b_size = b;
}
bool	dFrame::getSizeFunc( void )
{
	return this->m_b_size;
}
//位置及形状函数
bool 	dFrame::setShape( const dQuad& q )
{
	if( q.width < this->m_BorderWidth or q.height < this->m_SysbarHeight + this->m_HintbarHeight )
		return false;
	if( this->m_hWnd != 0 )
	{
		dQuad dqd = this->getShape();
		MoveWindow( this->m_hWnd, q.x, q.y, q.width, q.height, 0 );
		if( q.x != dqd.x or q.y != dqd.y )
		{
			LPARAM l = ((q.y bitand 0x0ffff) << 16) bitor ( q.x bitand 0x0ffff );
			PostMessage( this->m_hWnd, WM_MOVE, 0, l );
		}
		if( q.width != dqd.width or q.height != dqd.height )
		{
			LPARAM l = ( (q.height bitand 0x0ffff) << 16) bitor ( q.width bitand 0x0ffff );
			PostMessage( this->m_hWnd, WM_SIZE, 0, l );
		}
	}
	this->m_shape = q;
	return true;
}
//Id相关
bool	dFrame::setId( const string& id )		//重载的会在frame链中进行查重
{
	if( 0 == searchFrame( id ) and 0 == findFrame( id ) )
	{
		this->m_id = id;
		return true;
	}
	return false;
}
//获取客户区位置及形状
dRect	dFrame::getClientRect( void )		//子控件坐标的坐标原点是客户区左上角
{
	return dRect( 
	this->m_BorderWidth,
	this->m_shape.width - this->m_BorderWidth,
	this->m_SysbarHeight,
	this->m_shape.height - this->m_BorderWidth );
}
//获取客户区设备描述表句柄
HDC		dFrame::getHDC( void )
{
	return this->m_hDC;
}
/***功能性函数***/
dFrame* dFrame::findFrame( const string& id )	//通过id寻找frame 
{
	dFrame* tag = 0;
	for( int i = 0; global_frame_list.at(i) != 0; i++ )
	{
		tag = global_frame_list[i];
		if( id == tag->m_id )
			return tag;
	};
	return 0;
}
dFrame*	dFrame::findFrame( HWND hwnd )			//通过窗口寻找frame
{
	dFrame* tag = 0;
	for( int i = 0; global_frame_list.at(i) != 0; i++ )
	{
		tag = global_frame_list[i];
		if( hwnd == tag->m_hWnd )
			return tag;
	};
	return 0;
}
dFrame* dFrame::enumFrame( int n )			//按照安装次序列举frame
{
	try {
		return dFrame::global_frame_list[n];
	}catch( ... ) {
		return 0;
	}
}
LRESULT	dFrame::nchitTest( long x, long y )	//测试像素坐标所在区域 
{
	dRect rect;
	dRect client = this->getClientRect();
	rect = this->m_shape;
	long bw = this->m_BorderWidth;
	long sw = this->m_SysbarHeight;
	if( x>= rect.left and x <= rect.right and y >= rect.top and y <= rect.bottom )
	{
		if( y <= rect.top + bw and this->m_b_size )	//上边框
			if( x <= bw )
				return HTTOPLEFT;
			else if( x >= rect.right - bw )
				return HTTOPRIGHT;
			else
				return HTTOP; 
		else if( y <= rect.top + sw )	//标题栏
			if( this->pickChild( x - rect.left - client.left, y - rect.top - client.top ) == 0 )
				return HTCAPTION;
			else return HTCLIENT;
		else if( y >= rect.bottom - bw and this->m_b_size )	//下边框 
			if( x <= bw )
				return HTBOTTOMLEFT;
			else if( x >= rect.right - bw )
				return HTBOTTOMRIGHT;
			else
				return HTBOTTOM; 
		else if( this->m_b_size )	//左右边框 
			if( x <= rect.left + bw)
				return HTLEFT;
			else if( x >= rect.right - bw )
				return HTRIGHT;
			else
				return HTCLIENT;
		else
			return HTCLIENT;
	}
	else return HTNOWHERE;
}
bool	dFrame::createWindow( void )		//创建窗口
{
	WNDCLASS cls;
	int iFormat; 
	/***不能创建多个窗口***/ 
	if( this->m_hWnd != 0 )
		return false;
	memset( &cls, 0, sizeof(cls) );

	cls.hCursor = this->m_cursor;
	cls.hIcon = this->m_icon;
	cls.hInstance = this->m_hInstance;
	cls.lpfnWndProc = frameWndProc;
	cls.lpszClassName = "dream_frame_wnd_class";
	cls.style = CS_OWNDC;
	/***注册窗口类***/ 
	if( !RegisterClass( &cls ) )
		return false;
	/***创建窗口***/  
	this->m_hWnd = CreateWindowEx( 
		0, "dream_frame_wnd_class", this->m_tx_Sysbar.getText().data(),
	 	WS_POPUP | WS_VISIBLE, this->m_shape.x, this->m_shape.y,
		this->m_shape.width, this->m_shape.height,
		NULL, NULL, this->m_hInstance, NULL );
	if( 0 == this->m_hWnd )
		return false;
	
	/*	
	if( !SetLayeredWindowAttributes( this->m_hWnd, RGB( 0, 0, 0 ), 255, LWA_COLORKEY ) )
		return false;
	*/
		
	this->m_hDC = GetWindowDC( this->m_hWnd );
	//初始化像素格式
	memset( &this->m_pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
	this->m_pfd.cColorBits = 32;
	this->m_pfd.cDepthBits = 16;
	this->m_pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	this->m_pfd.nSize = sizeof( this->m_pfd );
	this->m_pfd.nVersion = 1;
	
	iFormat = ChoosePixelFormat( this->m_hDC, &this->m_pfd );
	if( !SetPixelFormat( this->m_hDC, iFormat, &this->m_pfd ) )
		return false;
	
	this->m_hRC = wglCreateContext( this->m_hDC );
	DWORD thread_id;
	this->m_hDrawThread = CreateThread( 0, 0, frameDrawThreadProc, this, 0, &thread_id );
	while( !this->m_b_run );	//等待线程设置运行标记 
	return true;
}
bool	dFrame::destroyWindow( void )	//销毁窗口,当关闭按钮被点击时会触发这个函数 
{
	//告诉绘制线程该停止工作了
	this->m_b_run = false;	
	this->m_hDrawThread = 0;
	if( this->m_hWnd != 0 )
	{
		wglDeleteContext( this->m_hRC );
		this->m_hRC = 0;
		ReleaseDC( this->m_hWnd, this->m_hDC );
		this->m_hDC = 0;
		DestroyWindow( this->m_hWnd );
		this->m_hWnd = 0;
		return true;
	}
	return false;
}
HWND	dFrame::getWindow( void )			//获取当前窗口句柄
{
	return this->m_hWnd;
}
void	dFrame::setWndProc( WNDPROC proc )		//设置窗口回调函数 
{
	if( proc == 0 )
		this->m_proc = DefWindowProc;
	else
		this->m_proc = proc;
	return;
}
WNDPROC	dFrame::getWndProc( void )			//获取窗口回调函数
{
	return this->m_proc;
}
void	dFrame::pushEvent( dEvent ev )		//上传一个event将触发窗口消息
{
	this->m_ev_list.push( ev );
	PostMessage( this->m_hWnd, DM_UPDATA, 0, 0 );
}
bool	dFrame::pickEvent( dEvent* pev )		//获取最先被上传的event 
{
	try {
		*pev = this->m_ev_list[0];
		this->m_ev_list.remove( 0 );
		return true;
	}catch( ... ) {
		return false;
	}
}

/***需要任何继承本类的类重载的重要函数***/
duiClass dFrame::toClass( void )	//用以迅速分辨子类的具体类型
{
	return dCframe;
}
void dFrame::onEvent( dTask task )		//用于处理事件的函数 
{
	switch( task.task_mask )
	{
	case WM_MOVING:
	case WM_MOVE:
	{
		RECT rect;
		GetWindowRect( this->m_hWnd, &rect );
		this->m_shape.x = rect.left;
		this->m_shape.y = rect.top;
		this->m_shape.width = rect.right - rect.left;
		this->m_shape.height = rect.bottom - rect.top;
		break;
	}
	case WM_SIZING:
	case WM_SIZE:	//有必要的话应该设计pause标记让绘制线程暂停下来 
	{
		this->m_b_pause = true;
		RECT rect;
		GetWindowRect( this->m_hWnd, &rect );
		this->m_shape.x = rect.left;
		this->m_shape.y = rect.top;
		this->m_shape.width = rect.right - rect.left;
		this->m_shape.height = rect.bottom - rect.top;
		/***重置渲染句柄***/ 
		wglDeleteContext( this->m_hRC );
		ReleaseDC( this->m_hWnd, this->m_hDC );
		this->m_hDC = GetDC( this->m_hWnd );
		this->m_hRC = wglCreateContext( this->m_hDC );
		this->m_b_pause = false;
		break;
	}
	}
	/***毫无保留的传递所有的task***/
	dQuad tqd;
	dWidget* tag;
	dTask ttk;
	for( int i = 0; this->m_child.at( i ) != 0; i++ )
	{
		tag = this->m_child[i];
		ttk = task;
		/***将鼠标坐标放入客户区坐标系***/ 
		tqd = this->getClientRect();
		ttk.mouse_x -= tqd.x;
		ttk.mouse_y -= tqd.y;
		/***将鼠标坐标放入目标控件坐标系***/ 
		tqd = tag->getShape();
		ttk.mouse_x -= tqd.x;
		ttk.mouse_y -= tqd.y;
		/***触发目标事件***/ 
		tag->onEvent( ttk );
	}
	this->m_tx_Hintbar.onEvent( task );
	this->m_tx_Sysbar.onEvent( task );
}
void dFrame::onDraw( dFrame& frame )	//用于处理绘制过程的函数 
{
	/***此时坐标原点与窗口原点重合***/ 
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glEnable( GL_BLEND );
	glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA );
	//glEnable( GL_DEPTH_TEST );
	//glDepthFunc( GL_GEQUAL );
	glPushMatrix();
	glTranslatef( -1.0f, 1.0f, 0.0f );
	glScalef( 2.0/this->m_shape.width, -2.0/this->m_shape.height, 1.0f );
	//glOrtho( 0, this->m_shape.width, this->m_shape.height, 0, 0, 100 );
	
	glClear( GL_COLOR_BUFFER_BIT );
	glClear( GL_DEPTH_BUFFER_BIT );
	/***绘制背景***/ 
	glBegin( GL_QUADS );
	glColor4ubv( this->m_cl_backg.value );
	glVertex2i( 0, 0 );
	glVertex2i( this->m_shape.width, 0 );
	glVertex2i( this->m_shape.width, this->m_shape.height );
	glVertex2i( 0, this->m_shape.height );
	glEnd();
	if( this->m_BorderWidth != 0 )
	{	//如果设置了边框则绘制出来 
		glLineWidth( (float)this->m_BorderWidth );
		glBegin( GL_LINES );
		glColor4ubv( this->m_cl_board.value );
		glVertex2i( 0, 0 );
		glVertex2i( this->m_shape.width, 0 );
		glVertex2i( this->m_shape.width, 0 );
		glVertex2i( this->m_shape.width, this->m_shape.height );
		glVertex2i( this->m_shape.width, this->m_shape.height );
		glVertex2i( 0, this->m_shape.height );
		glVertex2i( 0, this->m_shape.height );
		glVertex2i( 0, 0 );
		glEnd();
	}
	/***绘制标题栏***/
	if( this->m_SysbarHeight != 0 )
	{
		glColor4ubv( this->m_cl_Sysbar.value );
		glBegin( GL_QUADS );
		glVertex2i( 0, 0 );
		glVertex2i( this->m_shape.width, 0 );
		glVertex2i( this->m_shape.width, this->m_SysbarHeight );
		glVertex2i( 0, this->m_SysbarHeight );
		glEnd();
		glLineWidth( 1 );
		glBegin( GL_LINES );
		glColor4ubv( this->m_cl_SysbarBorder.value );
		glVertex2i( 0, this->m_SysbarHeight );
		glVertex2i( this->m_shape.width, this->m_SysbarHeight );
		glEnd();
		//glColor4ub( 0, 0, 0, 0 );
		this->m_tx_Sysbar.onDraw( frame );
	}
	/***绘制提示栏***/ 
	if( this->m_HintbarHeight != 0 )
	{
		glBegin( GL_QUADS );
		glColor4ubv( this->m_cl_Hintbar.value );
		glVertex2i( this->m_shape.width, this->m_shape.height );
		glVertex2i( this->m_shape.width, this->m_shape.height - this->m_HintbarHeight );
		glVertex2i( 0, this->m_shape.height - this->m_HintbarHeight );
		glVertex2i( 0, this->m_shape.height );
		glEnd();
		glLineWidth( 1 );
		glBegin( GL_LINES );
		glColor4ubv( this->m_cl_HintbarBorder.value );
		glVertex2i( 0, this->m_shape.height - this->m_HintbarHeight );
		glVertex2i( this->m_shape.width, this->m_shape.height - this->m_HintbarHeight );
		glEnd();
		this->m_tx_Hintbar.onDraw( frame );
	}
	if( this->m_render != 0 )
		this->m_render( frame, frame );
	
	/***为所有子控件调用绘制函数***/
	dRect rect = this->getClientRect();
	glPushMatrix();
	glTranslatef( rect.left, rect.top, 0.0 );
	dWidget* tag;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		tag = this->m_child[i];
		tag->onDraw( frame );
	}
	glPopMatrix();
	
	glPopMatrix();
	SwapBuffers( this->m_hDC );
}

/***消息函数和绘制线程***/
LRESULT CALLBACK frameWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( dFrame::findFrame( hwnd ) == 0 )
		return DefWindowProc( hwnd, msg, wParam, lParam );
	dFrame* frame = dFrame::findFrame( hwnd );
	dTask task;
	dQuad dqd = frame->getShape();
	switch( msg )
	{
		case WM_NCHITTEST:
		{
				POINTS ps = MAKEPOINTS( lParam );
				return frame->nchitTest( ps.x, ps.y );
		}
		case WM_MOUSEFIRST ... WM_MOUSELAST:
		{
			POINTS p = MAKEPOINTS( lParam );
			task.mouse_x = p.x;
			task.mouse_y = p.y;
			break;
		}
		case WM_MOVE:case WM_SIZE:
		case WM_MOVING:case WM_SIZING:
		case WM_KEYFIRST ... WM_KEYLAST:
		{
			POINT p;
			GetCursorPos( &p );
			p.x -= dqd.x;
			p.y -= dqd.y;
			task.mouse_x = p.x;
			task.mouse_y = p.y;
			break;
		}
		default:
			return frame->getWndProc()( hwnd, msg, wParam, lParam );
	}
	task.task_mask = msg;
	task.task_paramL = lParam;
	task.task_paramW = wParam;
	frame->onEvent( task );
	return frame->getWndProc()( hwnd, msg, wParam, lParam );
}
DWORD WINAPI frameDrawThreadProc( LPVOID p )
{
	dFrame* frame = (dFrame*)p;
	frame->m_b_run = true;
	wglMakeCurrent( frame->m_hDC, frame->m_hRC );
	while( frame->m_b_run )
	{
		if( frame->m_b_pause )
		{
			while( frame->m_b_pause );
			wglMakeCurrent( frame->m_hDC, frame->m_hRC );
		}
		frame->onDraw( *frame );
		Sleep(40);
	}
	return 0;
}

/********************dButton类的成员函数********************/
/***构造函数***/ 
dButton::dButton( dField* parent ) : dWidget( parent )
{
	this->m_b_preshed = false;
	this->m_b_focused = false;
	this->m_tx_text.setAlign( dAcenter );
}
dButton::~dButton( void )
{
	
}

bool dButton::setShape( const dQuad& q )
{
	if( q.width < 0 or q.height < 0 ) 
		return false;
	this->m_shape = q;
	this->m_tx_text.setShape( dQuad( 0, 0, q.width, q.height ) );
	return true;
}
void dButton::setBorderColor( dColor cl )
{
	this->m_cl_Border = cl;
}
void dButton::setBackgColor( dColor cl )
{
	this->m_cl_Backg = cl;
}
void dButton::setTextColor( dColor cl )
{
	this->m_tx_text.setColor( cl );
}
void dButton::setTextFont( DWORD height, DWORD weight, bool Italic, bool Underline, bool StrikeOut )
{
	this->m_tx_text.setFont( height, weight, Italic, Underline, StrikeOut );
}
void dButton::setText( const string& tx )
{
	this->m_tx_text = tx;
}
dColor dButton::getBorderColor( void )
{
	return this->m_cl_Border;
}
dColor dButton::getBackgColor( void )
{
	return this->m_cl_Backg;
}
dColor dButton::getTextColor( void )
{
	return this->m_tx_text.getColor();
}
string dButton::getText( void )
{
	return this->m_tx_text;
}
/***需要任何继承本类的类重载的重要函数***/
duiClass dButton::toClass( void )	//用以迅速分辨子类的具体类型
{
	return dCbutton;
}
void dButton::onEvent( dTask task )		//用于处理事件的函数 
{
	switch( task.task_mask )
	{
	case WM_MOUSEMOVE:
		if( this->m_shape.contain( task.mouse_x + this->m_shape.x, task.mouse_y + this->m_shape.y ) )//如果鼠标在范围内 
				this->m_b_focused = true;
		else
		{
			this->m_b_focused = false;
			this->m_b_preshed = false;
		}
		break;
	case WM_LBUTTONDOWN:
		if( this->m_b_focused )//如果鼠标在范围内 
			this->m_b_preshed = true;
		break;
	case WM_LBUTTONUP:
		if( this->m_b_focused and this->m_b_preshed )//如果鼠标在范围内 
		{
			this->m_b_preshed = false;
			dFrame* frame = dynamic_cast<dFrame*>(this->getTopField());
			dEvent dev;
			dev.event_mask = dVclickL;
			dev.event_param = 0;
			dev.widget = this;
			if( frame != 0 )
				frame->pushEvent( dev );
		}
		break;
	}
	this->m_tx_text.onEvent( task );
}
void dButton::onDraw( dFrame& frame )		//用于处理绘制过程的函数
{
	glColor4ubv( this->m_cl_Border.value );
	dRect rect;
	rect = this->m_shape;
	glLineWidth( 0.5 );
	glBegin( GL_LINES );
	glVertex2i( rect.left, rect.top );
	glVertex2i( rect.right, rect.top );
	
	glVertex2i( rect.right, rect.top );
	glVertex2i( rect.right, rect.bottom );
	
	glVertex2i( rect.right, rect.bottom );
	glVertex2i( rect.left, rect.bottom );
	
	glVertex2i( rect.left, rect.bottom );
	glVertex2i( rect.left, rect.top );
	glEnd();
	/***根据状态选择配色***/
	dColor clbk = this->m_cl_Backg;
	unsigned char ccen;		//配色灰度级 
	ccen = (clbk.red + clbk.green + clbk.blue)/3;
	if( this->m_b_focused )
		if( this->m_b_preshed )
			clbk.alpha = 0x40;
		else
			clbk.alpha = 0xa0;
	else
	{
		//clbk.red = ccen;
		//clbk.green = ccen;
		//clbk.blue = ccen;
		//clbk.alpha = 0xa0;
		clbk.alpha = 0xff;
	}
	glBegin( GL_QUADS );
	glColor4ubv( clbk.value );
	glVertex2i( rect.left, rect.top );
	glVertex2i( rect.right, rect.top );
	glVertex2i( rect.right, rect.bottom );
	glVertex2i( rect.left, rect.bottom );
	glEnd();
	/*
	dColor cltx = this->m_tx_text.getColor();
	cltx.alpha = clbk.alpha - 0x20;
	this->m_tx_text.setColor( cltx );
	glColor4ubv( cltx.value );
	*/
	glPushMatrix( );
	glTranslatef( this->m_shape.x, this->m_shape.y, 0 );
	this->m_tx_text.onDraw( frame );
	glPopMatrix();
}
/********************dText类的成员函数********************/ 
string dText::w2t( const wstring& sToMatch)  
{
    string sResult;  
    int iLen = WideCharToMultiByte( CP_ACP, 0, sToMatch.c_str(), -1, NULL, 0, NULL, FALSE ); // 计算转换后字符串的长度。（包含字符串结束符）  
    char *lpsz = new char[iLen];  
    WideCharToMultiByte( CP_OEMCP, 0, sToMatch.c_str(), -1, lpsz, iLen, NULL, FALSE); // 正式转换。  
    sResult.assign( lpsz, iLen - 1 ); // 对string对象进行赋值。  
    delete []lpsz;  
    return sResult;  
}
wstring dText::t2w( const string& tstr )
{
	if( tstr.length() == 0 )
		return L"\0";
	int clen = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, 0, 0 );
	wchar_t* nstr = new wchar_t[clen+1];	//为结束符保存位置
	int retn = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, nstr, clen+1 );
	nstr[clen] = L'\0';
	//wprintf( L"%s -> %d -> %S return:%d\n", tstr.data(), clen, nstr, retn );
	wstring ret = nstr;
	delete[] nstr;
	return ret;
}
/***构造函数会设置初始字体***/ 
dText::dText( dField* field ):dWidget( field )
{
	this->m_caller = 0;
	this->m_width = 0;
	this->m_list = 0;
	this->m_Height = 0;
	this->m_Wieght = 0;
	this->m_Italic = false;		//字体是否斜体 
	this->m_Underline = false;	//字体是否下划线
	this->m_StrikeOut = false;	//字体是否删除线 
	this->m_align = dAleft;
	
	this->m_busing = false;
}
dText::dText( const wstring& str,  dField* field ):dWidget( field )
{
	this->m_text = str;
	this->m_caller = 0;
	this->m_width = 0;
	this->m_Height = 0;
	this->m_Wieght = 0;
	this->m_Italic = false;		//字体是否斜体 
	this->m_Underline = false;	//字体是否下划线
	this->m_StrikeOut = false;	//字体是否删除线 
	this->m_align = dAleft;
	
	if( this->m_text.length() == 0 )
		this->m_list = 0;
	else
		this->m_list = new GLuint[this->m_text.length()];
	
	this->m_busing = false;
}
dText::dText( const string& str,  dField* field ):dWidget( field )
{
	this->m_text = this->t2w( str );
	this->m_caller = 0;
	this->m_width = 0;
	this->m_Height = 0;
	this->m_Wieght = 0;
	this->m_Italic = false;		//字体是否斜体 
	this->m_Underline = false;	//字体是否下划线
	this->m_StrikeOut = false;	//字体是否删除线 
	this->m_align = dAleft;
	
	if( this->m_text.length() == 0 )
		this->m_list = 0;
	else
		this->m_list = new GLuint[this->m_text.length()];
	
	this->m_busing = false;
}
dText::~dText( void )
{
	this->clear();	//清除显示列表
	if( this->m_list != 0 )
		delete[] this->m_list;
}
/***设置字体时如果显示列表不为空会重置显示列表***/ 
void dText::setFont( DWORD height, DWORD weight, bool Italic, bool Underline, bool StrikeOut )
{
	while( this->m_busing );
	this->m_busing = true;
	this->m_Height = height;
	this->m_Wieght = weight;
	this->m_Italic = Italic;
	this->m_Underline = Underline;
	this->m_StrikeOut = StrikeOut;
	this->m_busing = false;
	this->clear();
}
/***设置内容时如果显示列表不为空会重置显示列表***/ 
void dText::setText( const wstring& text )
{
	while( this->m_busing );
	this->m_busing = true;
	this->m_text = text;
	this->m_busing = false;
	this->clear();
	if( this->m_list != 0 )
		delete[] this->m_list;
	if( this->m_text.length() == 0 )
		this->m_list = 0;
	else
		this->m_list = new GLuint[this->m_text.length()];
}		
void dText::setText( const string& text )
{
	while( this->m_busing );
	this->m_busing = true;
	this->m_text = this->t2w( text );
	this->m_busing = false;
	this->clear();
	if( this->m_list != 0 )
		delete[] this->m_list;
	if( this->m_text.length() == 0 )
		this->m_list = 0;
	else
		this->m_list = new GLuint[this->m_text.length()];
}	
dText& dText::operator= ( const wstring& text )
{
	this->setText( text );
	return *this;
}
dText& dText::operator= ( const string& text )
{
	this->setText( text );
	return *this;
}
/***设置颜色***/
void dText::setColor( const dColor& cl )
{
	this->m_color = cl;
	this->clear();
}
dText::operator wstring( void )
{
	return this->m_text;
}
dText::operator string( void )
{
	return this->w2t( this->m_text );
}
string dText::getText()
{
	return this->w2t( this->m_text );
}
dColor dText::getColor( void )
{
	return this->m_color;
}
/***创建显示列表***/ 
bool dText::create( dFrame* frame )
{
	/***首先确认是否有必要重建***/ 
	while( this->m_busing );
	this->m_busing = true;
	if( this->m_caller != 0 or this->m_text.length() == 0 or this->m_Height == 0 )
	{
		this->m_busing = false;
		return true;	//此种情况属于没必要重建的情况 
	}
	/***首先确认是否处在OpenGL环境中***/
	if( frame == 0 )
		frame = dynamic_cast<dFrame*>(this->getTopField());
	if( frame == 0 )
	{
		this->m_busing = false;
		return false;
	}
	HDC hDC = frame->getHDC();
	if( hDC == 0 )
	{
		this->m_busing = false;
		return false;
	}
	int leng = this->m_text.length();
	//wprintf( L"%S\n", this->m_text.data() );
	//printf( "%d\n", leng );
	//Sleep( 200 );
	/***创建字体***/
	glColor4ubv( this->m_color.value );
	HFONT hFont = CreateFont( 
		this->m_Height, this->m_Height/2, 0, 0, this->m_Wieght,
		this->m_Italic, this->m_Underline, this->m_StrikeOut,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, NULL  ); 
	if( hFont == 0 )
	{
		this->m_busing = false;
		return false;
	}
	/***选择新建的字体***/ 
	hFont = (HFONT)SelectObject( hDC, (HGDIOBJ)hFont );
	/***申请并创建显示列表***/
	this->m_caller = glGenLists( 1 );
	//this->m_list = new GLuint[leng];
	for( int i = 0; i < leng; i++ )
	{
		this->m_list[i] = glGenLists( 1 );
		while(!wglUseFontBitmapsW( hDC, this->m_text[i], 1, this->m_list[i] ));
	}
	
	glNewList( this->m_caller, GL_COMPILE );
	for( int i = 0; i < leng; i++ )
		glCallList( this->m_list[i] );
	glEndList();
	hFont = (HFONT)SelectObject( hDC, hFont );
	DeleteObject( (HGDIOBJ)hFont );
	
	int pA[4];
	int pB[4];
	glColor4f( 1, 0, 0, 1.0f );
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i( 0, 0 );
	glGetIntegerv( GL_CURRENT_RASTER_POSITION, pA );
	glCallList( this->m_caller );
	glGetIntegerv( GL_CURRENT_RASTER_POSITION, pB );
	glPopMatrix();
	this->m_width = pB[0] - pA[0];
	//cout << pA[0] << ' ' << pB[0] << ' ' << this->m_width << endl;
	//Sleep( 200 );
	glColor4ubv( this->m_color.value );
	this->m_busing = false;
	return true;
}
void dText::setAlign( dgnMask mask )
{
	this->m_align = mask;
}
/***清空显示列表***/ 
void dText::clear( void )
{
	while( this->m_busing );
	this->m_busing = true;
	if( this->m_caller == 0 )
	{
		this->m_busing = false;
		return;
	}
	for( int i = 0; i < this->m_text.length(); i++ )
		glDeleteLists(this->m_list[this->m_text.length()-1], 1 );
	glDeleteLists(this->m_caller, 1 );
	//delete[] this->m_list;
	this->m_caller = 0;
	this->m_width = 0;
	this->m_busing = false;
	return;
}
/***获取字体高度***/
DWORD dText::getHeight( void )
{
	return this->m_Height;
}
/***获取宽字符长度***/ 
int dText::getLeng( void )
{
	return this->m_text.length();
}
LONG dText::getWidth( void )
{
	//this->create();
	return this->m_width;
}
/***绘制文字***/
void dText::onDraw( dFrame& frame )
{
	glColor4ubv( this->m_color.value );
	//printf( "R:%x G:%x B:%x A:%x\n", this->m_color.red, this->m_color.green, this->m_color.blue, this->m_color.alpha  );
	if( !this->create( &frame ) )
		return;
	dRect rect;
	rect = this->m_shape;
	switch( this->m_align )
	{
	case dAcenter:
		glRasterPos2i( (rect.left + rect.right - this->m_width ) >> 1,(rect.bottom + rect.top + this->m_Height) >> 1 );
		//printf( "%d %d\n",this->m_width, this->m_Height );
		break;
	case dAleft:
		glRasterPos2i( rect.left,(rect.bottom + rect.top + this->m_Height) >> 1 );
		break;
	case dAright:
		glRasterPos2i( rect.right - this->m_width,(rect.bottom + rect.top + this->m_Height) >> 1 );
		break;
	}
	glCallList( this->m_caller );
	return;
}
duiClass dText::toClass( void )
{
	return dCtext;
}
void dText::onEvent( dTask task )
{
	switch( task.task_mask )
	{
		case WM_SIZE:case WM_MOVE:
		case WM_SIZING:case WM_MOVING:
			this->clear();
	}
}

dchain<dFrame*> dFrame::global_frame_list = dchain<dFrame*>();
//end of file;	
}
#endif
