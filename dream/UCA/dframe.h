#ifndef __dframe__
#define __dframe__
/**
***	dFrame 类系列为窗口提供了 
***	以控件为基础的GUI服务 
***
**/
#include "dchain.h"
#include <string>
#include <Windows.h>
#include <WinGDI.h>
#include <gl/GL.h>
#include <gl/GLU.h>

using namespace std;

namespace dream {
	
/***基础类定义***/
class dFrame;		//与窗口直接相关的GUI类 frame是拥有窗口的field 
class dField;		//提供分区功能的控件类 field可以拥有子控件使得控件关系呈树状 
class dText;		//文字空间 
class dButton;		//按钮控件
class dEditor;		//编辑框控件
class dSlider;		//滑块控件
class dSelect;		//菜单控件
class dWidget; 		//提供最基础的控件功能 
class dText;		//用于绘制的字符串

/***方便传参的结构体***/ 
struct dColor;
struct dQuad;
struct dRect;

/***当frame的消息队列被更新时传递的消息***/
#define DM_UPDATA	(WM_USER + 0x1280)
/***用以实现窗口过程的函数***/
LRESULT CALLBACK frameWndProc( HWND, UINT, WPARAM, LPARAM );
/***用以实现绘制线程的函数***/
DWORD WINAPI frameDrawThreadProc( LPVOID );
/***用以添加附加绘制功能的绘制器***/
typedef void(*dRender)( dFrame&, dField& );

/***用以表明实例类别的标记***/ 
enum duiClass {
	dCwidget,	//dWidget
	dCframe,	//dFrame
	dCfield,	//dField 
	dCtext,		//dText
	dCbutton,	//dButton
	dCeditor,	//dEditor
	dCslider,	//dSlider
	dCselect,	//dSelect
};

/***用于文字的对齐设置***/
enum dgnMask {
	dAleft,
	dAright,
	dAcenter
};

/***用以表达事件类型的标记***/ 
enum devMask {
	/***用于向用户通报事件***/ 
	dVclickL,
	dVclickR
};

/***用以表达颜色的结构体***/ 
struct dColor {
	GLubyte value[4];			//数组用于使用指针赋值或传参 
	GLubyte& red = value[0];
	GLubyte& green = value[1];
	GLubyte& blue = value[2];
	GLubyte& alpha = value[3];
	dColor( void );
	dColor( GLubyte* );
	dColor( const dColor& );
	dColor( GLubyte, GLubyte, GLubyte, GLubyte );
	dColor& operator= ( const dColor& );
};

/***用于表达事件的结构体***/
struct dEvent {
	dWidget*	widget;			//表示触发事件的控件
	devMask		event_mask;		//事件类型标记
	LONG_PTR	event_param;	//事件参数或参数指针
};
struct dTask {
	UINT		task_mask;		//事件标记 
	WPARAM		task_paramW;	//事件参数或参数指针 
	LPARAM		task_paramL;	//事件参数或参数指针 
	long		mouse_x;
	long		mouse_y;		//事件发生时鼠标在当前控件客户区坐标系内的位置 
};

/***方便用以表达形状及位置的结构体***/ 
struct dQuad {
	long x;
	long y;
	long width;
	long height;
	dQuad( void );
	dQuad( long, long, long, long );
	dQuad& operator= ( const dQuad& );
	dQuad& operator= ( const dRect& );
	bool	contain( long, long );		//是否包含了一个点 
	bool	contain( dQuad& );	//是否包含了一个quad 
	bool	belongto( dQuad& );	//是否被一个quad包含 
	bool	overlap( dQuad& );	//是否与一个quad重叠 
};

/***方便用以表达形状及位置的结构体***/
struct dRect {
	long left;
	long right;
	long top;
	long bottom;
	dRect( void );
	dRect( long, long, long, long );
	dRect& operator= ( const dRect& );
	dRect& operator= ( const dQuad& );
	bool	contain( long, long );		//是否包含了一个点 
	bool	contain( dRect& );	//是否包含了一个rect 
	bool	belongto( dRect& );	//是否被一个rect包含 
	bool	overlap( dRect& );	//是否与一个rect重叠 
};

class dWidget {
	protected:
		/***基本参数***/ 	
		dQuad	m_shape;	//描述位置及形状
		string	m_id;		//用于响应消息的id,设定了父控件的情况下下会确保id唯一
		dField*	m_field;	//所在域指针 
		dRender	m_render;
		
		/***如果要更严密的事件系统则需要的成员变量***/ 
		//bool	m_b_lbt;	//鼠标左键在本区域按下的记录
		//bool	m_b_rbt;	//鼠标右键在本区域按下的记录 
	
	/***dField通常作为任何dWidget对象的父对象存在,应当对dWidget拥有访问权利***/
	friend class dField; 
	public:
		/***需要任何继承本类的类重载的重要函数***/
		virtual duiClass toClass( void );	//用以迅速分辨子类的具体类型
		virtual void onEvent( dTask );		//用于处理事件的函数 
		virtual void onDraw( dFrame& );		//用于处理绘制过程的函数
		
		/***构造函数会规定父控件的存在***/
		dWidget( dField* );					//同时会设定一个全树领域内不重复的id
		~dWidget( void );					//析构函数会自动将自己从父控件中除名 
		
		/***基本操作函数***/ 
		virtual bool setId( const string& );	//重新设置Id,Id不能为空,不能重复
		string	getId( void ) const;			//获取Id
		virtual bool setShape( const dQuad& );	//如果必要将调用子类函数
		dQuad	getShape( void ) const;			//获取位置及形状
		dField*	getField( void ) const;			//获取当前控件所在域
		dField*	getTopField( void );			//获取当前控件所在的顶层域 
		bool	catchFocus( void );				//为自己抓获键盘焦点,将调用父控件的catchFocus函数 
		dWidget*	searchFrame( const string& );
		dWidget*	searchFrame( const dWidget* );	//在整颗控件树内搜索控件
		void	setRender( dRender );				//设置绘制器 
		dRender	getRender( void );					//获取绘制器 
};

class dField : public dWidget {
	protected:
		/***field拥有子控件列表***/
		dchain<dWidget*>	m_child;
		/***field有必要确定区域内键盘焦点***/
		dWidget* m_key_focus; 
		/***绘制特性***/
		dColor	m_cl_board;
		dColor	m_cl_backg;
	public:
		/***构造函数***/ 
		dField( dField* );
		~dField( void );	//析构函数会在自己被销毁之前解散子控件树
		
		/***基本操作函数***/
		bool	catchFocus( dWidget* = 0 );			//为子控件抓取键盘焦点,若传递0则表示为自己抓获键盘焦点
		void	clearFocus( void );					//清除本区域内的键盘焦点
		void	setBorderColor( const dColor& );	//设置边框颜色
		dColor	getBorderColor( void );				//获取边框颜色 
		void	setBackgColor( const dColor& );		//设置背景颜色
		dColor	getBackgColor( void );				//获取背景颜色
		/***有关子控件的操作函数***/
		dWidget*	searchChild( const string& );	//按照id查找 
		dWidget*	searchChild( const dWidget* );	//在子控件树中搜索控件
		dWidget*	enumChild( int );				//按照安装次序列举子控件
		bool		removeChild( const string& );	//将子节点直接删除 
		dWidget*	pickChild( long, long );		//在子控件链中寻找被像素点 
		bool		kickChild( const dWidget* );	//将子控件除名 
		void		clearChild( void );				//清空子控件树,删除所有子节点 
		bool		installChild( dWidget* );		//将控件安装到子链中,不能重名 
		
		/***需要任何继承本类的类重载的重要函数***/
		virtual duiClass toClass( void );	//用以迅速分辨子类的具体类型
		virtual void onEvent( dTask );		//用于处理事件的函数 
		virtual void onDraw( dFrame& );		//用于处理绘制过程的函数
};

class dText: public dWidget {
	private:
		wstring	m_text;			//字符内容
		LONG	m_width;		//字符的像素宽度 
		GLuint*	m_list;			//显示列表列表 
		GLuint	m_caller;		//显示列表总表 
		dColor	m_color;		//绘制时的颜色 
		/***字体属性,修改时将重置显示列表***/ 
		DWORD	m_Height;		//字体高度 
		DWORD	m_Wieght;		//字体粗细程度 
		bool	m_Italic;		//字体是否斜体 
		bool	m_Underline;	//字体是否下划线
		bool	m_StrikeOut;	//字体是否删除线 
		dgnMask	m_align;		//对齐设置 
		volatile bool m_busing;	//同步锁 
	private:
		string w2t( const wstring& );
		wstring t2w( const string& );
	public:
		/***构造函数会设置初始字体***/ 
		dText( dField* = 0 );
		dText( const wstring&, dField* = 0 );
		dText( const string&, dField* = 0 );
		~dText( void );
		
		/***设置字体时如果显示列表不为空会重置显示列表***/ 
		void setFont( DWORD, DWORD, bool, bool, bool );
		/***设置字体内容***/ 
		void setText( const wstring& );
		void setText( const string& );
		dText& operator= ( const wstring& );
		dText& operator= ( const string& );
		/***设置颜色***/
		void setColor( const dColor& );
		/***设置对齐方式***/
		void setAlign( dgnMask );
		
		/***获取字符串内容***/ 
		operator wstring( void );
		operator string( void );
		string getText();
		/***获取字体高度***/
		DWORD getHeight( void );
		/***获取宽字符长度***/ 
		int getLeng( void );
		/***获取字符串像素宽度***/ 
		LONG getWidth( void );
		/***获取字符颜色***/ 
		dColor getColor( void );
		
		/***创建显示列表***/ 
		bool create( dFrame* = 0 );
		/***清空显示列表***/ 
		void clear( void );

		void onEvent( dTask );		//用于处理事件的函数 
		void onDraw( dFrame& );		//用于处理绘制过程的函数
		duiClass toClass( void );	//用于表明身份 
};

class dFrame : public dField {
	private:
		/***frame的画面特点***/ 
		long	m_SysbarHeight;		//标题栏高度,0表示不使用标题栏 
		long	m_HintbarHeight;	//提示栏高度,0表示不使用提示栏
		long	m_BorderWidth;		//边框宽度建议宽度4~6 
		dText	m_tx_Sysbar;		//标题栏标题内容 
		dText m_tx_Hintbar; 		//提示栏提示内容 
		dColor	m_cl_Sysbar;		//标题栏背景颜色
		dColor	m_cl_SysbarBorder;	//标题栏边框颜色 
		dColor	m_cl_Hintbar;		//提示栏背景颜色
		dColor	m_cl_HintbarBorder;	//提示栏边框颜色
		/***需要多次使用的参数***/
		PIXELFORMATDESCRIPTOR m_pfd;//像素格式描述符 
		/***frame的功能性参数***/
		HINSTANCE	m_hInstance;	//程序实例句柄 
		HWND	 	m_hWnd;			//窗口实例句柄 
		volatile HDC	m_hDC;		//设备描述句柄 
		volatile HGLRC	m_hRC;		//渲染上下文句柄
		HANDLE		m_hDrawThread;	//绘制线程句柄 
		WNDPROC		m_proc;			//用户自定义窗口过程
		HCURSOR		m_cursor;		//用户自定义鼠标 
		HICON		m_icon;			//用户自定义图标 
		volatile bool	m_b_run;	//绘制线程口令 
		volatile bool	m_b_pause;	//绘制线程暂停口令 
		bool		m_b_draft;		//是否启用空白处拖拽
		bool		m_b_size;		//是否启用边缘缩放
		//bool		m_b_minibx;		//是否启用缩小按钮
		//bool		m_b_maxbx;		//是否启用放大按钮
		//bool		m_b_clsbx;		//是否启用关闭按钮 
		dchain<dEvent>	m_ev_list;	//向用户传递的控件事件,当事件列表被更新时会向窗口传递 
		/***frame类静态属性***/
		static dchain<dFrame*> global_frame_list;	//全局frame列表,用于从任何地方找到所有frame
		friend DWORD WINAPI frameDrawThreadProc( LPVOID );
	public:
		/***构造函数***/
		dFrame( void );		//frame不可以作为子控件出现
		~dFrame( void );	//frame销毁时会解散子控件树,销毁窗口
		
		/***常规操作函数***/
		//边框宽的相关
		void	setBorderWidth( long );
		long	getBorderWidth( void );
		//标题框高度相关 
		void	setSysbarHeight( long );
		long	getSysbarHeight( void );
		//提示框高度相关 
		void	setHintbarHeight( long );
		long	getHintbarHeight( void );
		//标题框文字相关 
		void	setSysbarText( const string& );
		string	getSysbarText( void );
		//提示框文字相关 
		void	setHintbarText( const string& );
		string	getHintbarText( void );
		//标题框颜色相关 
		void	setSysbarColor( const dColor& );
		dColor	getSysbarColor( void );
		//标题框边框颜色相关 
		void	setSysbarBorderColor( const dColor& );
		dColor	getSysbarBorderColor( void );
		//标题框文字颜色相关 
		void	setSysbarTextColor( const dColor& );
		dColor	getSysbarTextColor( void );
		//提示框颜色相关 
		void	setHintbarColor( const dColor& );
		dColor	getHintbarColor( void );
		//提示框边框颜色相关 
		void	setHintbarBorderColor( const dColor& );
		dColor	getHintbarBorderColor( void );
		//提示框边框颜色相关 
		void	setHintbarTextColor( const dColor& );
		dColor	getHintbarTextColor( void );
		//应用程序图标相关
		void	setIcon( HICON );
		HICON	getIcon( void );
		//应用程序鼠标相关
		void	setCursor( HCURSOR );
		HCURSOR	getCursor( void );
		//拖拽功能相关
		void	setDraftFunc( bool );
		bool	getDraftFunc( void );
		//缩放功能相关
		void	setSizeFunc( bool );
		bool	getSizeFunc( void );
		//
		//位置及形状函数
		bool 	setShape( const dQuad& );	//重载的函数会被基类调用
		//Id相关
		bool	setId( const string& );		//重载的会在frame链中进行查重
		//获取客户区位置及形状
		dRect	getClientRect( void );		//子控件坐标的坐标原点是客户区左上角
		//获取客户区设备描述表句柄
		HDC		getHDC( void );
		/***功能性函数***/
		static dFrame* findFrame( const string& );//通过id寻找frame 
		static dFrame*	findFrame( HWND );	//通过窗口寻找frame
		static dFrame* enumFrame( int );	//按照安装次序列举frame
		LRESULT	nchitTest( long, long );	//测试像素坐标所在区域 
		bool	createWindow( void );		//创建窗口
		bool	destroyWindow( void );		//销毁窗口,当关闭按钮被点击时会触发这个函数 
		HWND	getWindow( void );			//获取当前窗口句柄
		void	setWndProc( WNDPROC );		//设置窗口回调函数 
		WNDPROC	getWndProc( void );			//获取窗口回调函数 
		void	pushEvent( dEvent );		//上传一个event将触发窗口消息
		bool	pickEvent( dEvent* );		//获取最先被上传的event 
		/***需要任何继承本类的类重载的重要函数***/
		virtual duiClass toClass( void );	//用以迅速分辨子类的具体类型
		virtual void onEvent( dTask );		//用于处理事件的函数 
		virtual void onDraw( dFrame& );		//用于处理绘制过程的函数
};

class dButton: public dWidget {
	protected:
		/***按钮颜色中的透明度都会随着状态改变提前设置无意义***/ 
		dColor 	m_cl_Border;	//按钮边框色 
		dColor 	m_cl_Backg;		//按钮背景色
		dText	m_tx_text;		//按钮文字内容
		bool	m_b_preshed;	//按钮是否被按下
		bool	m_b_focused;	//按钮是否被注意 
	public:
		/***构造函数***/ 
		dButton( dField* );
		~dButton( void );
		/***外观函数***/
		bool setShape( const dQuad& );
		void setBorderColor( dColor );
		void setBackgColor( dColor );
		void setTextColor( dColor );
		void setTextFont( DWORD, DWORD, bool, bool, bool );
		void setText( const string& );
		dColor getBorderColor( void );
		dColor getBackgColor( void );
		dColor getTextColor( void );
		string getText( void );
		/***需要任何继承本类的类重载的重要函数***/
		duiClass toClass( void );	//用以迅速分辨子类的具体类型
		void onEvent( dTask );		//用于处理事件的函数 
		void onDraw( dFrame& );		//用于处理绘制过程的函数
};

}
#endif
