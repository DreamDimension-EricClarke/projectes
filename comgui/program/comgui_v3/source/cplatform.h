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
		static LRESULT CALLBACK InitProc( HWND, UINT, WPARAM, LPARAM );	//初始化窗口的窗口过程
		static LRESULT CALLBACK MainProc( HWND, UINT, WPARAM, LPARAM );	//主窗口窗口过程 
		
	private:
		/***预期的串口参数***/ 
		DWORD	m_block;	//程序的每秒刷新间隔
		DWORD	m_baudrate;	//串口波特率设置
		WORD	m_bytesize;	//数据位数
		WORD	m_stopbits;	//停止位数( 0 = 1; 1 = 1.5; 2 = 2; )
		
		/***运行时参数***/
		DWORD		m_TimerDate;	//时间定时器 
		DWORD		m_TimerID;
		clayout		m_layout;	//主界面layout
		BYTE		m_lineCount;//日志行数,每32行日志会自动刷新一次

		/***平台数据***/ 
		BYTE	m_CommNum;	//当前串口号 
		HANDLE	m_hPort;	//当前串口句柄 
		BYTE	m_DevAddr;	//当前监测设备地址
		//WORD	m_DevAddr;	//当前监测设备地址
		BYTE	m_lamp;		//当前灯光状态 
		
		volatile bool	m_sync;		//同步锁 
		
	public:
		cplatform( void );
		~cplatform( void );
		
		bool	init( void );			//初始化或重新初始化串口配置 !!!这将显示初始化界面!
		bool	refresh( bool );		//刷新检测数据
		bool	ctlwater( char );		//灯光控制函数 
		bool	ctlrefresh( void );		//控制数据的上传与否 
		void	addLogLine( string );	//添加一行日志
		bool	OpenComm( int );		//打开指定串口
		bool	ConfigComm( void );		//重配置串口参数 
		bool	CloseComm( void );		//关闭打开的端口 
};

#endif
