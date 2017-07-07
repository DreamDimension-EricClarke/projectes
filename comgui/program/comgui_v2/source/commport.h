#ifndef __commport__
#define __commport__

#include <windows.h>
#include "layoutlexer.h"

class commport {
	private:
		unsigned short	m_ComNum;	//串口号 
		HANDLE			m_hPort;	//串口句柄
		DCB				m_Param;	//串口参数
		long			m_Rfblock;	//刷新间隔
		layoutlexer&	m_layout;	//布局窗口 
		dDword			m_Baudrate;	//期望的波特率
		dWord			m_Databits;	//期望的数据位数
		dWord			m_Stopbits;	//期望的停止位 
		DWORD			m_TimerID;	//定时器ID 
		
		/***监视器数据***/ 
		long			m_dt_addr;			//当前终端 
		unsigned char	m_dt_temprature;	//温度数据 
		unsigned char	m_dt_wedness;		//湿度数据
		unsigned char	m_dt_mosk;			//烟雾数据
		unsigned char	m_dt_lamp;			//光照情况
	private:
		static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
		static LRESULT CALLBACK InitProc( HWND, UINT, WPARAM, LPARAM );
	public:
		commport( layoutlexer& );	//构造函数会绑定布局窗口 
		~commport( void );
		
		bool	init( string );		//使用文档进行初始化 
		int		run( void );		//开始运行
		
		void	process( MSG );		//处理窗口消息
		
		bool	start( void );		//扫描可用串口并打开
		bool	start( int );		//打开指定串口
		bool	stop( void );		//关闭串口
		bool	CheckDevOption( void );//刷新设备选择
		
		bool	refresh( void );	//刷新当前数据
		bool	setlamp( bool );	//修改光照情况
		bool	Display( void );	//将当前数据显示 
		
		void	addlogline( string );	//添加运行日志
		void	clearlog( void );		//清除运行日志
};

#endif
