#ifndef __cplatform__
#define __cplatform__

#include "clayout.h"
#include "dxml.h"
#include <string>
#include <windows.h>

using namespace std;
using namespace dream;

#define MSG_SETPLATFORM	(WM_USER + 3)
#define MSG_SETCONFIRM	(WM_USER + 4)
#define MSG_SETINITFILE	(WM_USER + 5)
#define MSG_LOADDEF		(WM_USER + 6)
#define MSG_SETDEF		(WM_USER + 7)

class cplatform {
	private:
		static LRESULT CALLBACK InitProc( HWND, UINT, WPARAM, LPARAM );	//��ʼ�����ڵĴ��ڹ���
		static LRESULT CALLBACK MainProc( HWND, UINT, WPARAM, LPARAM );	//�����ڴ��ڹ��� 
		
	private:
		/***Ԥ�ڵĴ��ڲ���***/ 
		DWORD	m_block;	//�����ÿ��ˢ�¼��
		DWORD	m_baudrate;	//���ڲ���������
		WORD	m_bytesize;	//����λ��
		WORD	m_stopbits;	//ֹͣλ��( 0 = 1; 1 = 1.5; 2 = 2; )
		
		/***����ʱ����***/
		DWORD		m_TimerDate;	//ʱ�䶨ʱ�� 
		DWORD		m_TimerID;
		clayout		m_layout;	//������layout
		BYTE		m_lineCount;//��־����,ÿ32����־���Զ�ˢ��һ��

		/***ƽ̨����***/ 
		BYTE	m_CommNum;	//��ǰ���ں� 
		HANDLE	m_hPort;	//��ǰ���ھ�� 
		BYTE	m_DevAddr;	//��ǰ����豸��ַ
		//WORD	m_DevAddr;	//��ǰ����豸��ַ
		BYTE	m_lamp;		//��ǰ�ƹ�״̬ 
		
		volatile bool	m_sync;		//ͬ���� 
		
	public:
		cplatform( void );
		~cplatform( void );
		
		bool	init( void );			//��ʼ�������³�ʼ���������� !!!�⽫��ʾ��ʼ������!
		bool	refresh( bool );		//ˢ�¼������
		bool	ctlwater( char );		//�ƹ���ƺ��� 
		bool	ctlrefresh( void );		//�������ݵ��ϴ���� 
		void	addLogLine( string );	//���һ����־
		bool	OpenComm( int );		//��ָ������
		bool	ConfigComm( void );		//�����ô��ڲ��� 
		bool	CloseComm( void );		//�رմ򿪵Ķ˿� 
};

#endif
