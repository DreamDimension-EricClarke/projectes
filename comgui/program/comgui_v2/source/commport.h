#ifndef __commport__
#define __commport__

#include <windows.h>
#include "layoutlexer.h"

class commport {
	private:
		unsigned short	m_ComNum;	//���ں� 
		HANDLE			m_hPort;	//���ھ��
		DCB				m_Param;	//���ڲ���
		long			m_Rfblock;	//ˢ�¼��
		layoutlexer&	m_layout;	//���ִ��� 
		dDword			m_Baudrate;	//�����Ĳ�����
		dWord			m_Databits;	//����������λ��
		dWord			m_Stopbits;	//������ֹͣλ 
		DWORD			m_TimerID;	//��ʱ��ID 
		
		/***����������***/ 
		long			m_dt_addr;			//��ǰ�ն� 
		unsigned char	m_dt_temprature;	//�¶����� 
		unsigned char	m_dt_wedness;		//ʪ������
		unsigned char	m_dt_mosk;			//��������
		unsigned char	m_dt_lamp;			//�������
	private:
		static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
		static LRESULT CALLBACK InitProc( HWND, UINT, WPARAM, LPARAM );
	public:
		commport( layoutlexer& );	//���캯����󶨲��ִ��� 
		~commport( void );
		
		bool	init( string );		//ʹ���ĵ����г�ʼ�� 
		int		run( void );		//��ʼ����
		
		void	process( MSG );		//��������Ϣ
		
		bool	start( void );		//ɨ����ô��ڲ���
		bool	start( int );		//��ָ������
		bool	stop( void );		//�رմ���
		bool	CheckDevOption( void );//ˢ���豸ѡ��
		
		bool	refresh( void );	//ˢ�µ�ǰ����
		bool	setlamp( bool );	//�޸Ĺ������
		bool	Display( void );	//����ǰ������ʾ 
		
		void	addlogline( string );	//���������־
		void	clearlog( void );		//���������־
};

#endif
