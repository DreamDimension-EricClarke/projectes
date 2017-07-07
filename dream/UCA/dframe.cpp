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

/***********dColor�ṹ���Ա����***********/
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


/***********dQuad�ṹ���Ա����***********/
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

/***********dRect�ṹ��ĳ�Ա����***********/
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
bool	dRect::contain( long x, long y )		//�Ƿ������һ���� 
{
	return (x >= this->left and x <= this->right and y >= this->top and y <= this->bottom);
}
bool	dRect::contain( dRect& r )	//�Ƿ������һ��rect 
{
	return ( this->contain( r.left, r.top ) and this->contain( r.right, r.bottom ) );
}
bool	dRect::belongto( dRect& r )	//�Ƿ�һ��rect���� 
{
	return r.contain( *this );
}
bool	dRect::overlap( dRect& r )	//�Ƿ���һ��rect�ص�
{
	return !( 
		r.left > this->right or
		r.right < this->left or
		r.top > this->bottom or
		r.bottom < this->top );
}

/********************dWidget��ĳ�Ա����********************/
/***��Ҫ�κμ̳б���������ص���Ҫ����***/
duiClass dWidget::toClass( void )	//����Ѹ�ٷֱ�����ľ�������
{
	return dCwidget;
}
void dWidget::onEvent( dTask task )		//���ڴ����¼��ĺ��� 
{
	/***�˺�����ʱΪ��***/ 
}
void dWidget::onDraw( dFrame& frame )		//���ڴ�����ƹ��̵ĺ���
{
	/***�κ�����ʱΪ��***/ 
}

/***���캯����涨���ؼ��Ĵ���***/
dWidget::dWidget( dField* field )					//ͬʱ���趨һ��ȫ�������ڲ��ظ���id
{
	this->m_field = 0;
	/***����Ĭ��id***/ 
	
//	this->m_id = 0;	//��һ������Ĭ��id 
//	/***���ָ������***/ 
//	if( field != 0 )
//	{
//		for( int i = 0; !field->installChild( this ); this->m_id = ++i );	//�������µ�id���Խ����� 
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
dWidget::~dWidget( void )					//�����������Զ����Լ��Ӹ��ؼ��г��� 
{
	if( this->m_field != 0 )
		this->m_field->kickChild( this );	//����������ӿؼ��б����ҵ�����ؼ������ 
}

/***������������***/ 
bool	dWidget::setId( const string& id )
{
	if( 0 == searchFrame( id ) )
	{
		this->m_id = id;
		return true;
	}
	return false;
}
string	dWidget::getId( void )	const						//��ȡId
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
dQuad dWidget::getShape( void )	const			//��ȡλ�ü���״
{
	return this->m_shape;
}
dField* dWidget::getField( void ) const			//��ȡ��ǰ�ؼ�������
{
	return this->m_field;
}
dField*	dWidget::getTopField( void )		//��ȡ��ǰ�ؼ����ڵĶ����� 
{
	dField* t = dynamic_cast< dField*>(this);
	if( t == 0 )
		t = this->m_field;
	for( ; t != 0 and t->m_field != 0; t = t->m_field );
	return t;
}
bool	dWidget::catchFocus( void )			//Ϊ�Լ�ץ����̽���,�����ø��ؼ���catchFocus����  
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
dWidget*	dWidget::searchFrame( const dWidget* ptr )	//�����ſؼ����������ؼ�
{
	dField* top = this->getTopField();
	if( top == 0 )
		return 0;
	else if( top == ptr )
		return top;
	else
		return top->searchChild( ptr );
}
void	dWidget::setRender( dRender render )			//���û����� 
{
	this->m_render = render;
}
dRender	dWidget::getRender( void )					//��ȡ������ 
{
	return this->m_render;
}

/********************dField��ĳ�Ա����********************/
/***���캯��***/ 
dField::dField( dField* field ):dWidget( field )
{
	this->m_key_focus = 0;
	this->m_render = 0;
}
dField::~dField( void )	//�������������Լ�������֮ǰ��ɢ�ӿؼ���
{
	while( !this->m_child.bEmpty() )
		delete this->m_child[0];	//ֱ��ɾ���ӿؼ�,�ӿؼ����Զ����Լ����� 
}

/***������������***/
bool	dField::catchFocus( dWidget* tag )			//Ϊ�ӿؼ�ץȡ���̽���,������0���ʾΪ�Լ�ץ����̽���
{
	if( tag == 0 )
	{
		if( this->getField() != 0 )
			return this->getField()->catchFocus( this );
		return false;
	}
	if( this->searchChild( tag ) != 0 )	//ȷ��Ҫ��Ŀؼ����Լ����ӿؼ�
	{
		this->clearFocus();
		this->m_key_focus = tag;
		if( this->getField() != 0 )
			return this->getField()->catchFocus( this );
	}
	return false;
}
void	dField::clearFocus( void )					//����������ڵļ��̽���
{
	this->m_key_focus = 0;	
}
void	dField::setBorderColor( const dColor& cl )	//���ñ߿���ɫ
{
	this->m_cl_board = cl;
}
dColor	dField::getBorderColor( void )				//��ȡ�߿���ɫ 
{
	return this->m_cl_board;
}
void	dField::setBackgColor( const dColor& cl )	//���ñ�����ɫ
{
	this->m_cl_backg = cl;
}
dColor	dField::getBackgColor( void )				//��ȡ������ɫ
{
	return this->m_cl_backg;
}
/***�й��ӿؼ��Ĳ�������***/
dWidget*	dField::searchChild( const string& id )
{
	dWidget* tag;
	dField* tcd;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		/***���Ե�ǰĿ���Ƿ����Ҫ��***/ 
		tag = this->m_child[i];
		if( id == tag->getId() )
			return tag;
		/***�����ǰĿ��ӵ���ӿؼ�,��Ӧ�ò������ӿؼ�***/ 
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
dWidget*	dField::searchChild( const dWidget* tg )	//���ӿؼ����������ؼ�
{
	if( tg == 0 )
		return 0;
	this->m_child.focus( 0 );
	dWidget* tag;
	dField* tcd;
	for( int i = 0; this->m_child.at(i) != 0; i++ )
	{
		/***���Ե�ǰĿ���Ƿ����Ҫ��***/ 
		tag = this->m_child[i];
		if( tag == tg )
			return tag;
		/***�����ǰĿ��ӵ���ӿؼ�,��Ӧ�ò������ӿؼ�***/ 
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
dWidget*	dField::enumChild( int n )				//���հ�װ�����о��ӿؼ�
{
		try {
			return this->m_child[ n ];
		}catch( ... )
		{
			return 0;
		}
}
bool		dField::removeChild( const string& id )	//���ӽڵ�ֱ��ɾ�� 
{
	dWidget* tag = this->searchChild( id );
	if( tag != 0 )
	{
		//ɾ��ʱ�ؼ����Զ����� 
		delete tag;
		return true;
	}
	return false;	
}

dWidget*	dField::pickChild( long x, long y )		//���ӿؼ�����Ѱ�ұ����ص� 
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
bool		dField::kickChild( const dWidget* tag )		//���ӿؼ����� 
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
void		dField::clearChild( void )				//����ӿؼ���,ɾ�������ӽڵ� 
{
	while( !this->m_child.bEmpty() )
		delete this->m_child[0];
}
bool		dField::installChild( dWidget* tag )		//���ؼ���װ��������,�������� 
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

/***��Ҫ�κμ̳б���������ص���Ҫ����***/
duiClass dField::toClass( void )	//����Ѹ�ٷֱ�����ľ�������
{
	return dCfield;
}
void dField::onEvent( dTask task )		//���ڴ����¼��ĺ��� 
{
	/***field�Լ�Ҫ��������Ϣ***/ 
	switch( task.task_mask )
	{
		case WM_LBUTTONDOWN:		//�����field�ڵ��������������̽��� 
			if( this->m_shape.contain( task.mouse_x + this->m_shape.x, task.mouse_y + this->m_shape.y ) )
				this->catchFocus();
			break;
		case WM_RBUTTONDOWN:		//��field�ڿհ״������Ҽ��ᴥ���Ҽ��¼� 
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
	/***���ޱ����Ĵ������е�task***/
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
void dField::onDraw( dFrame& frame )		//���ڴ�����ƹ��̵ĺ���
{
	/***�����Լ�***/
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
	
	/***Ϊ�ӿؼ����û��ƹ���***/ 
	glPushMatrix();
	glTranslatef( this->m_shape.x, this->m_shape.y, 0.0f );
	/***���ƺ������������õĻ���������***/ 
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

/*********************dFrame��ĳ�Ա����********************/
/***���캯��***/
dFrame::dFrame( void ):dField( 0 )		//frame��������Ϊ�ӿؼ�����
{
	/***����û��Ĭ�Ϲ��캯���ĳ�Ա����***/ 
	this->m_BorderWidth = 0;
	/***�д����ŵĹ���***/ 
	//this->m_b_clsbx = true;
	//this->m_b_draft = true;
	//this->m_b_maxbx = true;
	//this->m_b_minibx = true;
	this->m_b_run = false;		//run�����ɻ����߳�����,��destroyWindows������� 
	this->m_b_pause = false;	//�����߳���ͣ����
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
	/***��ʼ����id***/
	//for( int i = 0; !this->setId( i ); i++ );		//�������µ�id���Խ����� 
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
dFrame::~dFrame( void )	//frame����ʱ���ɢ�ӿؼ���,���ٴ���
{
	this->destroyWindow();
	return;
}

/***�����������***/
//�߿������
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
//�����߶���� 
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
//��ʾ��߶���� 
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
//������������ 
void	dFrame::setSysbarText( const string& s )
{
	this->m_tx_Sysbar = s;
	this->m_tx_Sysbar.setFont( this->m_SysbarHeight*0.75, 700, false, false, false );
}
string	dFrame::getSysbarText( void )
{
	return this->m_tx_Sysbar;
}
//��ʾ��������� 
void	dFrame::setHintbarText( const string& s )
{
	this->m_tx_Hintbar = s;
	this->m_tx_Hintbar.setFont( this->m_HintbarHeight*0.75, 700, false, false, false );
}
string	dFrame::getHintbarText( void )
{
	return this->m_tx_Hintbar;
}
//�������ɫ��� 
void	dFrame::setSysbarColor( const dColor& cl )
{
	this->m_cl_Sysbar = cl;
} 
dColor	dFrame::getSysbarColor( void )
{
	return this->m_cl_Sysbar;
}
//�����߿���ɫ��� 
void	dFrame::setSysbarBorderColor( const dColor& cl )
{
	this->m_cl_SysbarBorder = cl;
}
dColor	dFrame::getSysbarBorderColor( void )
{
	return this->m_cl_SysbarBorder;
}
//�����������ɫ��� 
void	dFrame::setSysbarTextColor( const dColor& cl )
{
	this->m_tx_Sysbar.setColor( cl );
}
dColor	dFrame::getSysbarTextColor( void )
{
	return m_tx_Sysbar.getColor();
}
//��ʾ����ɫ��� 
void	dFrame::setHintbarColor( const dColor& cl )
{
	this->m_cl_Hintbar = cl;
}
dColor	dFrame::getHintbarColor( void )
{
	return this->m_cl_Hintbar;
}
//��ʾ��߿���ɫ��� 
void	dFrame::setHintbarBorderColor( const dColor& cl )
{
	this->m_cl_HintbarBorder = cl;
}
dColor	dFrame::getHintbarBorderColor( void )
{
	return this->m_cl_HintbarBorder;
}
//��ʾ��߿���ɫ��� 
void	dFrame::setHintbarTextColor( const dColor& cl )
{
	this->m_tx_Hintbar.setColor( cl );
}
dColor	dFrame::getHintbarTextColor( void )
{
	return this->m_tx_Hintbar.getColor();
}
//Ӧ�ó���ͼ�����

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
//Ӧ�ó���������
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
//��ק�������
void	dFrame::setDraftFunc( bool b )
{
	this->m_b_draft = b;
}
bool	dFrame::getDraftFunc( void )
{
	return this->m_b_draft;
}
//���Ź������
void	dFrame::setSizeFunc( bool b )
{
	this->m_b_size = b;
}
bool	dFrame::getSizeFunc( void )
{
	return this->m_b_size;
}
//λ�ü���״����
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
//Id���
bool	dFrame::setId( const string& id )		//���صĻ���frame���н��в���
{
	if( 0 == searchFrame( id ) and 0 == findFrame( id ) )
	{
		this->m_id = id;
		return true;
	}
	return false;
}
//��ȡ�ͻ���λ�ü���״
dRect	dFrame::getClientRect( void )		//�ӿؼ����������ԭ���ǿͻ������Ͻ�
{
	return dRect( 
	this->m_BorderWidth,
	this->m_shape.width - this->m_BorderWidth,
	this->m_SysbarHeight,
	this->m_shape.height - this->m_BorderWidth );
}
//��ȡ�ͻ����豸��������
HDC		dFrame::getHDC( void )
{
	return this->m_hDC;
}
/***�����Ժ���***/
dFrame* dFrame::findFrame( const string& id )	//ͨ��idѰ��frame 
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
dFrame*	dFrame::findFrame( HWND hwnd )			//ͨ������Ѱ��frame
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
dFrame* dFrame::enumFrame( int n )			//���հ�װ�����о�frame
{
	try {
		return dFrame::global_frame_list[n];
	}catch( ... ) {
		return 0;
	}
}
LRESULT	dFrame::nchitTest( long x, long y )	//�������������������� 
{
	dRect rect;
	dRect client = this->getClientRect();
	rect = this->m_shape;
	long bw = this->m_BorderWidth;
	long sw = this->m_SysbarHeight;
	if( x>= rect.left and x <= rect.right and y >= rect.top and y <= rect.bottom )
	{
		if( y <= rect.top + bw and this->m_b_size )	//�ϱ߿�
			if( x <= bw )
				return HTTOPLEFT;
			else if( x >= rect.right - bw )
				return HTTOPRIGHT;
			else
				return HTTOP; 
		else if( y <= rect.top + sw )	//������
			if( this->pickChild( x - rect.left - client.left, y - rect.top - client.top ) == 0 )
				return HTCAPTION;
			else return HTCLIENT;
		else if( y >= rect.bottom - bw and this->m_b_size )	//�±߿� 
			if( x <= bw )
				return HTBOTTOMLEFT;
			else if( x >= rect.right - bw )
				return HTBOTTOMRIGHT;
			else
				return HTBOTTOM; 
		else if( this->m_b_size )	//���ұ߿� 
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
bool	dFrame::createWindow( void )		//��������
{
	WNDCLASS cls;
	int iFormat; 
	/***���ܴ����������***/ 
	if( this->m_hWnd != 0 )
		return false;
	memset( &cls, 0, sizeof(cls) );

	cls.hCursor = this->m_cursor;
	cls.hIcon = this->m_icon;
	cls.hInstance = this->m_hInstance;
	cls.lpfnWndProc = frameWndProc;
	cls.lpszClassName = "dream_frame_wnd_class";
	cls.style = CS_OWNDC;
	/***ע�ᴰ����***/ 
	if( !RegisterClass( &cls ) )
		return false;
	/***��������***/  
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
	//��ʼ�����ظ�ʽ
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
	while( !this->m_b_run );	//�ȴ��߳��������б�� 
	return true;
}
bool	dFrame::destroyWindow( void )	//���ٴ���,���رհ�ť�����ʱ�ᴥ��������� 
{
	//���߻����̸߳�ֹͣ������
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
HWND	dFrame::getWindow( void )			//��ȡ��ǰ���ھ��
{
	return this->m_hWnd;
}
void	dFrame::setWndProc( WNDPROC proc )		//���ô��ڻص����� 
{
	if( proc == 0 )
		this->m_proc = DefWindowProc;
	else
		this->m_proc = proc;
	return;
}
WNDPROC	dFrame::getWndProc( void )			//��ȡ���ڻص�����
{
	return this->m_proc;
}
void	dFrame::pushEvent( dEvent ev )		//�ϴ�һ��event������������Ϣ
{
	this->m_ev_list.push( ev );
	PostMessage( this->m_hWnd, DM_UPDATA, 0, 0 );
}
bool	dFrame::pickEvent( dEvent* pev )		//��ȡ���ȱ��ϴ���event 
{
	try {
		*pev = this->m_ev_list[0];
		this->m_ev_list.remove( 0 );
		return true;
	}catch( ... ) {
		return false;
	}
}

/***��Ҫ�κμ̳б���������ص���Ҫ����***/
duiClass dFrame::toClass( void )	//����Ѹ�ٷֱ�����ľ�������
{
	return dCframe;
}
void dFrame::onEvent( dTask task )		//���ڴ����¼��ĺ��� 
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
	case WM_SIZE:	//�б�Ҫ�Ļ�Ӧ�����pause����û����߳���ͣ���� 
	{
		this->m_b_pause = true;
		RECT rect;
		GetWindowRect( this->m_hWnd, &rect );
		this->m_shape.x = rect.left;
		this->m_shape.y = rect.top;
		this->m_shape.width = rect.right - rect.left;
		this->m_shape.height = rect.bottom - rect.top;
		/***������Ⱦ���***/ 
		wglDeleteContext( this->m_hRC );
		ReleaseDC( this->m_hWnd, this->m_hDC );
		this->m_hDC = GetDC( this->m_hWnd );
		this->m_hRC = wglCreateContext( this->m_hDC );
		this->m_b_pause = false;
		break;
	}
	}
	/***���ޱ����Ĵ������е�task***/
	dQuad tqd;
	dWidget* tag;
	dTask ttk;
	for( int i = 0; this->m_child.at( i ) != 0; i++ )
	{
		tag = this->m_child[i];
		ttk = task;
		/***������������ͻ�������ϵ***/ 
		tqd = this->getClientRect();
		ttk.mouse_x -= tqd.x;
		ttk.mouse_y -= tqd.y;
		/***������������Ŀ��ؼ�����ϵ***/ 
		tqd = tag->getShape();
		ttk.mouse_x -= tqd.x;
		ttk.mouse_y -= tqd.y;
		/***����Ŀ���¼�***/ 
		tag->onEvent( ttk );
	}
	this->m_tx_Hintbar.onEvent( task );
	this->m_tx_Sysbar.onEvent( task );
}
void dFrame::onDraw( dFrame& frame )	//���ڴ�����ƹ��̵ĺ��� 
{
	/***��ʱ����ԭ���봰��ԭ���غ�***/ 
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
	/***���Ʊ���***/ 
	glBegin( GL_QUADS );
	glColor4ubv( this->m_cl_backg.value );
	glVertex2i( 0, 0 );
	glVertex2i( this->m_shape.width, 0 );
	glVertex2i( this->m_shape.width, this->m_shape.height );
	glVertex2i( 0, this->m_shape.height );
	glEnd();
	if( this->m_BorderWidth != 0 )
	{	//��������˱߿�����Ƴ��� 
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
	/***���Ʊ�����***/
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
	/***������ʾ��***/ 
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
	
	/***Ϊ�����ӿؼ����û��ƺ���***/
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

/***��Ϣ�����ͻ����߳�***/
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

/********************dButton��ĳ�Ա����********************/
/***���캯��***/ 
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
/***��Ҫ�κμ̳б���������ص���Ҫ����***/
duiClass dButton::toClass( void )	//����Ѹ�ٷֱ�����ľ�������
{
	return dCbutton;
}
void dButton::onEvent( dTask task )		//���ڴ����¼��ĺ��� 
{
	switch( task.task_mask )
	{
	case WM_MOUSEMOVE:
		if( this->m_shape.contain( task.mouse_x + this->m_shape.x, task.mouse_y + this->m_shape.y ) )//�������ڷ�Χ�� 
				this->m_b_focused = true;
		else
		{
			this->m_b_focused = false;
			this->m_b_preshed = false;
		}
		break;
	case WM_LBUTTONDOWN:
		if( this->m_b_focused )//�������ڷ�Χ�� 
			this->m_b_preshed = true;
		break;
	case WM_LBUTTONUP:
		if( this->m_b_focused and this->m_b_preshed )//�������ڷ�Χ�� 
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
void dButton::onDraw( dFrame& frame )		//���ڴ�����ƹ��̵ĺ���
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
	/***����״̬ѡ����ɫ***/
	dColor clbk = this->m_cl_Backg;
	unsigned char ccen;		//��ɫ�Ҷȼ� 
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
/********************dText��ĳ�Ա����********************/ 
string dText::w2t( const wstring& sToMatch)  
{
    string sResult;  
    int iLen = WideCharToMultiByte( CP_ACP, 0, sToMatch.c_str(), -1, NULL, 0, NULL, FALSE ); // ����ת�����ַ����ĳ��ȡ��������ַ�����������  
    char *lpsz = new char[iLen];  
    WideCharToMultiByte( CP_OEMCP, 0, sToMatch.c_str(), -1, lpsz, iLen, NULL, FALSE); // ��ʽת����  
    sResult.assign( lpsz, iLen - 1 ); // ��string������и�ֵ��  
    delete []lpsz;  
    return sResult;  
}
wstring dText::t2w( const string& tstr )
{
	if( tstr.length() == 0 )
		return L"\0";
	int clen = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, 0, 0 );
	wchar_t* nstr = new wchar_t[clen+1];	//Ϊ����������λ��
	int retn = MultiByteToWideChar( CP_ACP, 0, tstr.data(), -1, nstr, clen+1 );
	nstr[clen] = L'\0';
	//wprintf( L"%s -> %d -> %S return:%d\n", tstr.data(), clen, nstr, retn );
	wstring ret = nstr;
	delete[] nstr;
	return ret;
}
/***���캯�������ó�ʼ����***/ 
dText::dText( dField* field ):dWidget( field )
{
	this->m_caller = 0;
	this->m_width = 0;
	this->m_list = 0;
	this->m_Height = 0;
	this->m_Wieght = 0;
	this->m_Italic = false;		//�����Ƿ�б�� 
	this->m_Underline = false;	//�����Ƿ��»���
	this->m_StrikeOut = false;	//�����Ƿ�ɾ���� 
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
	this->m_Italic = false;		//�����Ƿ�б�� 
	this->m_Underline = false;	//�����Ƿ��»���
	this->m_StrikeOut = false;	//�����Ƿ�ɾ���� 
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
	this->m_Italic = false;		//�����Ƿ�б�� 
	this->m_Underline = false;	//�����Ƿ��»���
	this->m_StrikeOut = false;	//�����Ƿ�ɾ���� 
	this->m_align = dAleft;
	
	if( this->m_text.length() == 0 )
		this->m_list = 0;
	else
		this->m_list = new GLuint[this->m_text.length()];
	
	this->m_busing = false;
}
dText::~dText( void )
{
	this->clear();	//�����ʾ�б�
	if( this->m_list != 0 )
		delete[] this->m_list;
}
/***��������ʱ�����ʾ�б�Ϊ�ջ�������ʾ�б�***/ 
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
/***��������ʱ�����ʾ�б�Ϊ�ջ�������ʾ�б�***/ 
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
/***������ɫ***/
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
/***������ʾ�б�***/ 
bool dText::create( dFrame* frame )
{
	/***����ȷ���Ƿ��б�Ҫ�ؽ�***/ 
	while( this->m_busing );
	this->m_busing = true;
	if( this->m_caller != 0 or this->m_text.length() == 0 or this->m_Height == 0 )
	{
		this->m_busing = false;
		return true;	//�����������û��Ҫ�ؽ������ 
	}
	/***����ȷ���Ƿ���OpenGL������***/
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
	/***��������***/
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
	/***ѡ���½�������***/ 
	hFont = (HFONT)SelectObject( hDC, (HGDIOBJ)hFont );
	/***���벢������ʾ�б�***/
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
/***�����ʾ�б�***/ 
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
/***��ȡ����߶�***/
DWORD dText::getHeight( void )
{
	return this->m_Height;
}
/***��ȡ���ַ�����***/ 
int dText::getLeng( void )
{
	return this->m_text.length();
}
LONG dText::getWidth( void )
{
	//this->create();
	return this->m_width;
}
/***��������***/
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
