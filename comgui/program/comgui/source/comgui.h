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
	/***基础框架内容***/ 
	dFrame* frame = 0;			//基础框架 
	dButton* button_exit = 0;	//退出按钮 
	dButton* button_mini = 0;	//最小化按钮 
	
	dButton* button_basic = 0;	//基本配置界面选择按钮 
	dButton* button_dev[4] = {0, 0, 0, 0};	//四个设备选择按钮 
	
	dField* field_basic = 0;	//基础配置界面域 
	dField* field_device = 0;	//设备配置界面 
	
	/***基础配置界面内容***/ 
	dButton* button_start = 0;	//启动按钮 
	
	dButton* button_portDn = 0;	//串口号微调按钮 
	dButton* button_portUp = 0;	//串口号微调按钮
	dText* text_comport = 0;	//串口号显示文本
	
	dText* text_baudHint = 0;	//波特率提示文本 
	dButton* button_baudDn = 0;	//波特率微调按钮 
	dButton* button_baudUp = 0;	//波特率微调按钮
	dText* text_baudrate = 0;	//波特率显示文本
	
	dText* text_bytesizeHint = 0;	//数据位数提示文本 
	dButton* button_bytesizeDn = 0;	//数据位数微调按钮 
	dButton* button_bytesizeUp = 0;	//数据位数微调按钮
	dText* text_bytesize = 0;		//数据位数显示文本
	
	dText* text_fparityHint = 0;	//奇偶校验提示文本 
	dButton* button_fparityDn = 0;	//奇偶校验微调按钮 
	dButton* button_fparityUp = 0;	//奇偶校验微调按钮
	dText* text_fparity = 0;		//奇偶校验显示文本
	
	dText* text_parityHint = 0;		//奇偶校验位提示文本 
	dButton* button_parityDn = 0;	//奇偶校验位微调按钮 
	dButton* button_parityUp = 0;	//奇偶校验位微调按钮
	dText* text_parity = 0;			//奇偶校验位显示文本
	
	dText* text_stopbitHint = 0;	//停止位提示文本 
	dButton* button_stopbitDn = 0;	//停止位微调按钮 
	dButton* button_stopbitUp = 0;	//停止位微调按钮
	dText* text_stopbit = 0;		//停止位显示文本
	
	dButton* button_refresh = 0;	//刷新按钮
	dButton* button_switch = 0;		//灯的开关按钮 
	dText* text_temperature = 0;	//温度数据 
	dText* text_humidity = 0;		//湿度数据 
	dText* text_smoke = 0;			//气体数据 
	dText* text_lamp = 0;			//灯光数据 
	dText* text_devnum = 0;			//当前终端号码 
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
		
		volatile bool	m_b_talking;	//通信同步锁 
		volatile bool	m_b_err;		//上一次刷新结果 
		
		unsigned char m_temperature;	//温度数据 
		unsigned char m_humidity;		//湿度数据 
		unsigned char m_smoke;			//烟雾数据
		unsigned char m_lamp;			//灯光数据 
		
		string m_config_file;			//默认配置文档名 
		item_lexer m_config;			//默认配置 
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
