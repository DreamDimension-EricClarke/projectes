#ifndef __dframe__
#define __dframe__
/**
***	dFrame ��ϵ��Ϊ�����ṩ�� 
***	�Կؼ�Ϊ������GUI���� 
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
	
/***�����ඨ��***/
class dFrame;		//�봰��ֱ����ص�GUI�� frame��ӵ�д��ڵ�field 
class dField;		//�ṩ�������ܵĿؼ��� field����ӵ���ӿؼ�ʹ�ÿؼ���ϵ����״ 
class dText;		//���ֿռ� 
class dButton;		//��ť�ؼ�
class dEditor;		//�༭��ؼ�
class dSlider;		//����ؼ�
class dSelect;		//�˵��ؼ�
class dWidget; 		//�ṩ������Ŀؼ����� 
class dText;		//���ڻ��Ƶ��ַ���

/***���㴫�εĽṹ��***/ 
struct dColor;
struct dQuad;
struct dRect;

/***��frame����Ϣ���б�����ʱ���ݵ���Ϣ***/
#define DM_UPDATA	(WM_USER + 0x1280)
/***����ʵ�ִ��ڹ��̵ĺ���***/
LRESULT CALLBACK frameWndProc( HWND, UINT, WPARAM, LPARAM );
/***����ʵ�ֻ����̵߳ĺ���***/
DWORD WINAPI frameDrawThreadProc( LPVOID );
/***������Ӹ��ӻ��ƹ��ܵĻ�����***/
typedef void(*dRender)( dFrame&, dField& );

/***���Ա���ʵ�����ı��***/ 
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

/***�������ֵĶ�������***/
enum dgnMask {
	dAleft,
	dAright,
	dAcenter
};

/***���Ա���¼����͵ı��***/ 
enum devMask {
	/***�������û�ͨ���¼�***/ 
	dVclickL,
	dVclickR
};

/***���Ա����ɫ�Ľṹ��***/ 
struct dColor {
	GLubyte value[4];			//��������ʹ��ָ�븳ֵ�򴫲� 
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

/***���ڱ���¼��Ľṹ��***/
struct dEvent {
	dWidget*	widget;			//��ʾ�����¼��Ŀؼ�
	devMask		event_mask;		//�¼����ͱ��
	LONG_PTR	event_param;	//�¼����������ָ��
};
struct dTask {
	UINT		task_mask;		//�¼���� 
	WPARAM		task_paramW;	//�¼����������ָ�� 
	LPARAM		task_paramL;	//�¼����������ָ�� 
	long		mouse_x;
	long		mouse_y;		//�¼�����ʱ����ڵ�ǰ�ؼ��ͻ�������ϵ�ڵ�λ�� 
};

/***�������Ա����״��λ�õĽṹ��***/ 
struct dQuad {
	long x;
	long y;
	long width;
	long height;
	dQuad( void );
	dQuad( long, long, long, long );
	dQuad& operator= ( const dQuad& );
	dQuad& operator= ( const dRect& );
	bool	contain( long, long );		//�Ƿ������һ���� 
	bool	contain( dQuad& );	//�Ƿ������һ��quad 
	bool	belongto( dQuad& );	//�Ƿ�һ��quad���� 
	bool	overlap( dQuad& );	//�Ƿ���һ��quad�ص� 
};

/***�������Ա����״��λ�õĽṹ��***/
struct dRect {
	long left;
	long right;
	long top;
	long bottom;
	dRect( void );
	dRect( long, long, long, long );
	dRect& operator= ( const dRect& );
	dRect& operator= ( const dQuad& );
	bool	contain( long, long );		//�Ƿ������һ���� 
	bool	contain( dRect& );	//�Ƿ������һ��rect 
	bool	belongto( dRect& );	//�Ƿ�һ��rect���� 
	bool	overlap( dRect& );	//�Ƿ���һ��rect�ص� 
};

class dWidget {
	protected:
		/***��������***/ 	
		dQuad	m_shape;	//����λ�ü���״
		string	m_id;		//������Ӧ��Ϣ��id,�趨�˸��ؼ���������»�ȷ��idΨһ
		dField*	m_field;	//������ָ�� 
		dRender	m_render;
		
		/***���Ҫ�����ܵ��¼�ϵͳ����Ҫ�ĳ�Ա����***/ 
		//bool	m_b_lbt;	//�������ڱ������µļ�¼
		//bool	m_b_rbt;	//����Ҽ��ڱ������µļ�¼ 
	
	/***dFieldͨ����Ϊ�κ�dWidget����ĸ��������,Ӧ����dWidgetӵ�з���Ȩ��***/
	friend class dField; 
	public:
		/***��Ҫ�κμ̳б���������ص���Ҫ����***/
		virtual duiClass toClass( void );	//����Ѹ�ٷֱ�����ľ�������
		virtual void onEvent( dTask );		//���ڴ����¼��ĺ��� 
		virtual void onDraw( dFrame& );		//���ڴ�����ƹ��̵ĺ���
		
		/***���캯����涨���ؼ��Ĵ���***/
		dWidget( dField* );					//ͬʱ���趨һ��ȫ�������ڲ��ظ���id
		~dWidget( void );					//�����������Զ����Լ��Ӹ��ؼ��г��� 
		
		/***������������***/ 
		virtual bool setId( const string& );	//��������Id,Id����Ϊ��,�����ظ�
		string	getId( void ) const;			//��ȡId
		virtual bool setShape( const dQuad& );	//�����Ҫ���������ຯ��
		dQuad	getShape( void ) const;			//��ȡλ�ü���״
		dField*	getField( void ) const;			//��ȡ��ǰ�ؼ�������
		dField*	getTopField( void );			//��ȡ��ǰ�ؼ����ڵĶ����� 
		bool	catchFocus( void );				//Ϊ�Լ�ץ����̽���,�����ø��ؼ���catchFocus���� 
		dWidget*	searchFrame( const string& );
		dWidget*	searchFrame( const dWidget* );	//�����ſؼ����������ؼ�
		void	setRender( dRender );				//���û����� 
		dRender	getRender( void );					//��ȡ������ 
};

class dField : public dWidget {
	protected:
		/***fieldӵ���ӿؼ��б�***/
		dchain<dWidget*>	m_child;
		/***field�б�Ҫȷ�������ڼ��̽���***/
		dWidget* m_key_focus; 
		/***��������***/
		dColor	m_cl_board;
		dColor	m_cl_backg;
	public:
		/***���캯��***/ 
		dField( dField* );
		~dField( void );	//�������������Լ�������֮ǰ��ɢ�ӿؼ���
		
		/***������������***/
		bool	catchFocus( dWidget* = 0 );			//Ϊ�ӿؼ�ץȡ���̽���,������0���ʾΪ�Լ�ץ����̽���
		void	clearFocus( void );					//����������ڵļ��̽���
		void	setBorderColor( const dColor& );	//���ñ߿���ɫ
		dColor	getBorderColor( void );				//��ȡ�߿���ɫ 
		void	setBackgColor( const dColor& );		//���ñ�����ɫ
		dColor	getBackgColor( void );				//��ȡ������ɫ
		/***�й��ӿؼ��Ĳ�������***/
		dWidget*	searchChild( const string& );	//����id���� 
		dWidget*	searchChild( const dWidget* );	//���ӿؼ����������ؼ�
		dWidget*	enumChild( int );				//���հ�װ�����о��ӿؼ�
		bool		removeChild( const string& );	//���ӽڵ�ֱ��ɾ�� 
		dWidget*	pickChild( long, long );		//���ӿؼ�����Ѱ�ұ����ص� 
		bool		kickChild( const dWidget* );	//���ӿؼ����� 
		void		clearChild( void );				//����ӿؼ���,ɾ�������ӽڵ� 
		bool		installChild( dWidget* );		//���ؼ���װ��������,�������� 
		
		/***��Ҫ�κμ̳б���������ص���Ҫ����***/
		virtual duiClass toClass( void );	//����Ѹ�ٷֱ�����ľ�������
		virtual void onEvent( dTask );		//���ڴ����¼��ĺ��� 
		virtual void onDraw( dFrame& );		//���ڴ�����ƹ��̵ĺ���
};

class dText: public dWidget {
	private:
		wstring	m_text;			//�ַ�����
		LONG	m_width;		//�ַ������ؿ�� 
		GLuint*	m_list;			//��ʾ�б��б� 
		GLuint	m_caller;		//��ʾ�б��ܱ� 
		dColor	m_color;		//����ʱ����ɫ 
		/***��������,�޸�ʱ��������ʾ�б�***/ 
		DWORD	m_Height;		//����߶� 
		DWORD	m_Wieght;		//�����ϸ�̶� 
		bool	m_Italic;		//�����Ƿ�б�� 
		bool	m_Underline;	//�����Ƿ��»���
		bool	m_StrikeOut;	//�����Ƿ�ɾ���� 
		dgnMask	m_align;		//�������� 
		volatile bool m_busing;	//ͬ���� 
	private:
		string w2t( const wstring& );
		wstring t2w( const string& );
	public:
		/***���캯�������ó�ʼ����***/ 
		dText( dField* = 0 );
		dText( const wstring&, dField* = 0 );
		dText( const string&, dField* = 0 );
		~dText( void );
		
		/***��������ʱ�����ʾ�б�Ϊ�ջ�������ʾ�б�***/ 
		void setFont( DWORD, DWORD, bool, bool, bool );
		/***������������***/ 
		void setText( const wstring& );
		void setText( const string& );
		dText& operator= ( const wstring& );
		dText& operator= ( const string& );
		/***������ɫ***/
		void setColor( const dColor& );
		/***���ö��뷽ʽ***/
		void setAlign( dgnMask );
		
		/***��ȡ�ַ�������***/ 
		operator wstring( void );
		operator string( void );
		string getText();
		/***��ȡ����߶�***/
		DWORD getHeight( void );
		/***��ȡ���ַ�����***/ 
		int getLeng( void );
		/***��ȡ�ַ������ؿ��***/ 
		LONG getWidth( void );
		/***��ȡ�ַ���ɫ***/ 
		dColor getColor( void );
		
		/***������ʾ�б�***/ 
		bool create( dFrame* = 0 );
		/***�����ʾ�б�***/ 
		void clear( void );

		void onEvent( dTask );		//���ڴ����¼��ĺ��� 
		void onDraw( dFrame& );		//���ڴ�����ƹ��̵ĺ���
		duiClass toClass( void );	//���ڱ������ 
};

class dFrame : public dField {
	private:
		/***frame�Ļ����ص�***/ 
		long	m_SysbarHeight;		//�������߶�,0��ʾ��ʹ�ñ����� 
		long	m_HintbarHeight;	//��ʾ���߶�,0��ʾ��ʹ����ʾ��
		long	m_BorderWidth;		//�߿��Ƚ�����4~6 
		dText	m_tx_Sysbar;		//�������������� 
		dText m_tx_Hintbar; 		//��ʾ����ʾ���� 
		dColor	m_cl_Sysbar;		//������������ɫ
		dColor	m_cl_SysbarBorder;	//�������߿���ɫ 
		dColor	m_cl_Hintbar;		//��ʾ��������ɫ
		dColor	m_cl_HintbarBorder;	//��ʾ���߿���ɫ
		/***��Ҫ���ʹ�õĲ���***/
		PIXELFORMATDESCRIPTOR m_pfd;//���ظ�ʽ������ 
		/***frame�Ĺ����Բ���***/
		HINSTANCE	m_hInstance;	//����ʵ����� 
		HWND	 	m_hWnd;			//����ʵ����� 
		volatile HDC	m_hDC;		//�豸������� 
		volatile HGLRC	m_hRC;		//��Ⱦ�����ľ��
		HANDLE		m_hDrawThread;	//�����߳̾�� 
		WNDPROC		m_proc;			//�û��Զ��崰�ڹ���
		HCURSOR		m_cursor;		//�û��Զ������ 
		HICON		m_icon;			//�û��Զ���ͼ�� 
		volatile bool	m_b_run;	//�����߳̿��� 
		volatile bool	m_b_pause;	//�����߳���ͣ���� 
		bool		m_b_draft;		//�Ƿ����ÿհ״���ק
		bool		m_b_size;		//�Ƿ����ñ�Ե����
		//bool		m_b_minibx;		//�Ƿ�������С��ť
		//bool		m_b_maxbx;		//�Ƿ����÷Ŵ�ť
		//bool		m_b_clsbx;		//�Ƿ����ùرհ�ť 
		dchain<dEvent>	m_ev_list;	//���û����ݵĿؼ��¼�,���¼��б�����ʱ���򴰿ڴ��� 
		/***frame�ྲ̬����***/
		static dchain<dFrame*> global_frame_list;	//ȫ��frame�б�,���ڴ��κεط��ҵ�����frame
		friend DWORD WINAPI frameDrawThreadProc( LPVOID );
	public:
		/***���캯��***/
		dFrame( void );		//frame��������Ϊ�ӿؼ�����
		~dFrame( void );	//frame����ʱ���ɢ�ӿؼ���,���ٴ���
		
		/***�����������***/
		//�߿������
		void	setBorderWidth( long );
		long	getBorderWidth( void );
		//�����߶���� 
		void	setSysbarHeight( long );
		long	getSysbarHeight( void );
		//��ʾ��߶���� 
		void	setHintbarHeight( long );
		long	getHintbarHeight( void );
		//������������ 
		void	setSysbarText( const string& );
		string	getSysbarText( void );
		//��ʾ��������� 
		void	setHintbarText( const string& );
		string	getHintbarText( void );
		//�������ɫ��� 
		void	setSysbarColor( const dColor& );
		dColor	getSysbarColor( void );
		//�����߿���ɫ��� 
		void	setSysbarBorderColor( const dColor& );
		dColor	getSysbarBorderColor( void );
		//�����������ɫ��� 
		void	setSysbarTextColor( const dColor& );
		dColor	getSysbarTextColor( void );
		//��ʾ����ɫ��� 
		void	setHintbarColor( const dColor& );
		dColor	getHintbarColor( void );
		//��ʾ��߿���ɫ��� 
		void	setHintbarBorderColor( const dColor& );
		dColor	getHintbarBorderColor( void );
		//��ʾ��߿���ɫ��� 
		void	setHintbarTextColor( const dColor& );
		dColor	getHintbarTextColor( void );
		//Ӧ�ó���ͼ�����
		void	setIcon( HICON );
		HICON	getIcon( void );
		//Ӧ�ó���������
		void	setCursor( HCURSOR );
		HCURSOR	getCursor( void );
		//��ק�������
		void	setDraftFunc( bool );
		bool	getDraftFunc( void );
		//���Ź������
		void	setSizeFunc( bool );
		bool	getSizeFunc( void );
		//
		//λ�ü���״����
		bool 	setShape( const dQuad& );	//���صĺ����ᱻ�������
		//Id���
		bool	setId( const string& );		//���صĻ���frame���н��в���
		//��ȡ�ͻ���λ�ü���״
		dRect	getClientRect( void );		//�ӿؼ����������ԭ���ǿͻ������Ͻ�
		//��ȡ�ͻ����豸��������
		HDC		getHDC( void );
		/***�����Ժ���***/
		static dFrame* findFrame( const string& );//ͨ��idѰ��frame 
		static dFrame*	findFrame( HWND );	//ͨ������Ѱ��frame
		static dFrame* enumFrame( int );	//���հ�װ�����о�frame
		LRESULT	nchitTest( long, long );	//�������������������� 
		bool	createWindow( void );		//��������
		bool	destroyWindow( void );		//���ٴ���,���رհ�ť�����ʱ�ᴥ��������� 
		HWND	getWindow( void );			//��ȡ��ǰ���ھ��
		void	setWndProc( WNDPROC );		//���ô��ڻص����� 
		WNDPROC	getWndProc( void );			//��ȡ���ڻص����� 
		void	pushEvent( dEvent );		//�ϴ�һ��event������������Ϣ
		bool	pickEvent( dEvent* );		//��ȡ���ȱ��ϴ���event 
		/***��Ҫ�κμ̳б���������ص���Ҫ����***/
		virtual duiClass toClass( void );	//����Ѹ�ٷֱ�����ľ�������
		virtual void onEvent( dTask );		//���ڴ����¼��ĺ��� 
		virtual void onDraw( dFrame& );		//���ڴ�����ƹ��̵ĺ���
};

class dButton: public dWidget {
	protected:
		/***��ť��ɫ�е�͸���ȶ�������״̬�ı���ǰ����������***/ 
		dColor 	m_cl_Border;	//��ť�߿�ɫ 
		dColor 	m_cl_Backg;		//��ť����ɫ
		dText	m_tx_text;		//��ť��������
		bool	m_b_preshed;	//��ť�Ƿ񱻰���
		bool	m_b_focused;	//��ť�Ƿ�ע�� 
	public:
		/***���캯��***/ 
		dButton( dField* );
		~dButton( void );
		/***��ۺ���***/
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
		/***��Ҫ�κμ̳б���������ص���Ҫ����***/
		duiClass toClass( void );	//����Ѹ�ٷֱ�����ľ�������
		void onEvent( dTask );		//���ڴ����¼��ĺ��� 
		void onDraw( dFrame& );		//���ڴ�����ƹ��̵ĺ���
};

}
#endif
