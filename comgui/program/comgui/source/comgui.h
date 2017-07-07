#ifndef __comgui__
#define __comgui__
#include <string>
#include "dframe.h"
#include "chain.h"
#include "item_lexer.h" 
using namespace std;
using namespace dream;

void CALLBACK GuiTimerProc( HWND hWnd, UINT msg, UINT_PTR id, DWORD dwTime );

struct dfmFrame {
	public:
	/***�����������***/ 
	dFrame* frame = 0;			//������� 
	dButton* button_exit = 0;	//�˳���ť 
	dButton* button_mini = 0;	//��С����ť 
	
	dButton* button_basic = 0;	//�������ý���ѡ��ť 
	dButton* button_dev[4] = {0, 0, 0, 0};	//�ĸ��豸ѡ��ť 
	
	dField* field_basic = 0;	//�������ý����� 
	dField* field_device = 0;	//�豸���ý��� 
	
	/***�������ý�������***/ 
	dButton* button_start = 0;	//������ť 
	
	dButton* button_portDn = 0;	//���ں�΢����ť 
	dButton* button_portUp = 0;	//���ں�΢����ť
	dText* text_comport = 0;	//���ں���ʾ�ı�
	
	dText* text_baudHint = 0;	//��������ʾ�ı� 
	dButton* button_baudDn = 0;	//������΢����ť 
	dButton* button_baudUp = 0;	//������΢����ť
	dText* text_baudrate = 0;	//��������ʾ�ı�
	
	dText* text_bytesizeHint = 0;	//����λ����ʾ�ı� 
	dButton* button_bytesizeDn = 0;	//����λ��΢����ť 
	dButton* button_bytesizeUp = 0;	//����λ��΢����ť
	dText* text_bytesize = 0;		//����λ����ʾ�ı�
	
	dText* text_fparityHint = 0;	//��żУ����ʾ�ı� 
	dButton* button_fparityDn = 0;	//��żУ��΢����ť 
	dButton* button_fparityUp = 0;	//��żУ��΢����ť
	dText* text_fparity = 0;		//��żУ����ʾ�ı�
	
	dText* text_parityHint = 0;		//��żУ��λ��ʾ�ı� 
	dButton* button_parityDn = 0;	//��żУ��λ΢����ť 
	dButton* button_parityUp = 0;	//��żУ��λ΢����ť
	dText* text_parity = 0;			//��żУ��λ��ʾ�ı�
	
	dText* text_stopbitHint = 0;	//ֹͣλ��ʾ�ı� 
	dButton* button_stopbitDn = 0;	//ֹͣλ΢����ť 
	dButton* button_stopbitUp = 0;	//ֹͣλ΢����ť
	dText* text_stopbit = 0;		//ֹͣλ��ʾ�ı�
	
	dButton* button_refresh = 0;	//ˢ�°�ť
	dButton* button_switch = 0;		//�ƵĿ��ذ�ť 
	dText* text_temperature = 0;	//�¶����� 
	dText* text_humidity = 0;		//ʪ������ 
	dText* text_smoke = 0;			//�������� 
	dText* text_lamp = 0;			//�ƹ����� 
	dText* text_devnum = 0;			//��ǰ�ն˺��� 
};

class comgui {
	private:
		dfmFrame m_layout;
		
		int		m_commport;
		DCB		m_commDCB;
		HANDLE	m_comm;
		DWORD	m_baud_table[12];
		UINT	m_timerid;
		short	m_current_div;
		
		volatile bool	m_b_talking;	//ͨ��ͬ���� 
		volatile bool	m_b_err;		//��һ��ˢ�½�� 
		
		unsigned char m_temperature;	//�¶����� 
		unsigned char m_humidity;		//ʪ������ 
		unsigned char m_smoke;			//��������
		unsigned char m_lamp;			//�ƹ����� 
		
		string m_config_file;			//Ĭ�������ĵ��� 
		item_lexer m_config;			//Ĭ������ 
	private:
		bool initLayout( void );
		void readInitFile( HANDLE );
		void readCommState( void );
		void setCommState( void );
		
		void onBtnExit( void );
		void onBtnMini( void );
		void onBtnStart( void );
		void onBtnPortDn( void );
		void onBtnPortUp( void );
		
		void onBtnBaudDn( void );
		void onBtnBaudUp( void );
		void onBtnBytesizeDn( void );
		void onBtnBytesizeUp( void );
		void onBtnFparityDn( void );
		void onBtnFparityUp( void );
		void onBtnParityDn( void );
		void onBtnParityUp( void );
		void onBtnStopbitsDn( void );
		void onBtnStopbitsUp( void );
		
		void onBtnBase( void );
		void onBtnDev0( void );
		void onBtnDev1( void );
		void onBtnDev2( void );
		void onBtnDev3( void );
		
		void onBtnSwitch( void );
		void onBtnRefresh( void );
		
		void refreshConfig( void );
	public:
		comgui();
		~comgui();
		void clear();
		bool initial( string );
		void process( void );
		void Refresh( void );
		void Display( void );
};

#endif
