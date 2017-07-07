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
		dXmlnode*	m_frame;		//�����ָ��
		HWND		m_hwnd;			//����ܴ���
		widgets		m_widgets;		//���뻥���Ŀؼ��б� 
		HMENU		m_idcount;		//�ؼ�ID 
		
		static	WNDPROC	group_proc;
	private:
		bool	lexerAny( dXmlnode*, HWND );			//��xml�ĵ�����һϵ���ӿؼ� 
		bool	lexerWindow( dXmlnode*, WNDPROC );		//��xml�ĵ���������
		bool	lexerText(  dXmlnode*, HWND );			//��xml�ĵ���������
		bool	lexerButton( dXmlnode*, HWND );			//��xml�ĵ�������ť 
		bool	lexerGroup( dXmlnode*, HWND );			//��xml�ĵ���������
		bool	lexerList( dXmlnode*, HWND );			//��xml�ĵ������б� 
		static LRESULT CALLBACK GroupProc( HWND, UINT, WPARAM, LPARAM );
	public:
		layoutlexer( void );		//Ĭ�Ϲ��캯��
		~layoutlexer( void );		//Ĭ����������
		
		bool	lexer( string, WNDPROC );	//���������ļ������ɲ���
		
		bool	isWidget( WORD );	//���ĳ�����Ƿ��ǻ����ؼ�
		string	getName( WORD );	//��ȡ�����Ӧ�ؼ�������
		WORD	getID( string );	//��ȡ���ֶ�Ӧ�Ŀؼ�����
		HWND	getHWND( WORD );	//��ȡID��Ӧ�Ŀؼ����� 
		HWND	getHWND( string );	//��ȡ���ֶ�Ӧ�Ĵ��� 
		HWND	getMain( void );	//��ȡ������ 
};
#endif
