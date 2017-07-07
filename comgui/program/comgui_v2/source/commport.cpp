#ifndef __commport_cpp__
#define __commport_cpp__

#include "commport.h"

LRESULT CALLBACK commport::WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		
		/* Upon destruction, tell the main thread to stop */
		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK commport::InitProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam ) {
	commport* port;
	switch( Message) {
		case WM_USER + 1:	//设定指针的消息 
		case WM_COMMAND:
			break;
		default:
			return DefWindowProc( hwnd, Message, wParam, lParam );
	}
	return 0;
}

commport::commport( layoutlexer& layout ):	//构造函数会绑定布局窗口 
m_ComNum( 0 ), m_hPort( INVALID_HANDLE_VALUE ), m_Rfblock( 1000 ), 
m_layout( layout ), m_Baudrate( 9600 ), m_Databits( 8 ),m_TimerID(0)
{
	
}
commport::~commport( void )
{
	if( this->m_hPort != INVALID_HANDLE_VALUE )
		CloseHandle( this->m_hPort );
	return;
}

bool	commport::init( string filename )		//使用文档进行初始化 
{
	//打开文件 
	HANDLE hf = CreateFile( filename.data(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
	if( hf == INVALID_HANDLE_VALUE )
		return false;	//若未打开文件,则失败
	
	//读取文件内容 
	DWORD dwFileSize = GetFileSize( hf, 0 );
	DWORD dwRead;
	string attr;
	char* buffer = new char[dwFileSize + 1];
	ReadFile( hf, buffer, dwFileSize, &dwRead, 0 );
	buffer[dwFileSize] = '\0';
	
	/***解析dxml文档***/ 
	dXmlanal cXml;
	dXmlnode* doc;
	try{
		doc = cXml.analysisCode( buffer );
	}catch( invalid_argument& e ){
		char buf[256];
		sprintf( buf, " initial document error in %s offset %d :%s", filename.data(), cXml.getOffs(), e.what() );
		MessageBox( NULL, buf, "error", MB_OK );
		return false;
	}
	/***建立GUI***/ 
	try {
		attr = doc->getAttr( "layout" );
		this->m_layout.lexer( attr, WndProc );
	}catch( ... ) {
		return false;
	}
	
	try {
		attr = doc->getAttr( "baudrate" );
		sscanf( attr.data(), "%d", &this->m_Baudrate );
		this->addlogline( "波特率设置: " + attr );
	}catch( ... ) {
		this->addlogline( "未找到波特率设置,默认置为9600");
		this->m_Baudrate = 9600;
	}
	
	try {
		attr = doc->getAttr( "databits" );
		sscanf( attr.data(), "%d", &this->m_Databits );
		this->addlogline( "数据位设置: " + attr );
	}catch( ... ) {
		this->addlogline( "未找到数据位数设置,默认置为8位" );
		this->m_Databits = 8;
	}
	
	try {
		attr = doc->getAttr( "stopbits" );
		sscanf( attr.data(), "%d", &this->m_Stopbits );
		if( this->m_Stopbits == 0 )
			attr = "1";
		else if( this->m_Stopbits == 1 )
			attr = "1.5";
		else if( this->m_Stopbits == 2 )
			attr = "2";
		this->addlogline( "停止位设置: " + attr );
	}catch( ... ) {
		this->addlogline( "未找到停止位设置,默认置位1位" );
		this->m_Stopbits = 0;
	}
	
	
	return true;
}

int		commport::run( void )		//开始运行
{
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) > 0 )
	{
		switch( msg.message )
		{
			case WM_COMMAND:
				this->process( msg );
				break;
			case WM_TIMER:
				if( msg.wParam != 0 and msg.wParam == this->m_TimerID )
					this->refresh();
				break;
			default:
				break;
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		
	}
	return msg.wParam;
}

void	commport::process( MSG msg )		//处理窗口消息
{
	string name;
	try {
		name = this->m_layout.getName( LOWORD(msg.wParam) );
		if( name == "button_st" and BN_CLICKED == HIWORD(msg.wParam ) )
			if( this->m_hPort == INVALID_HANDLE_VALUE )
			{
				if( this->start( ) )
					SetWindowText( this->m_layout.getHWND( name ), "关闭" );
			}
			else
			{
				if( this->stop() )
					SetWindowText( this->m_layout.getHWND( name ), "启动" );
			}
		else if( name == "button_refresh" and BN_CLICKED == HIWORD(msg.wParam ) )
			this->refresh();
		else if( name == "dev_list" and LBN_SELCHANGE == HIWORD(msg.wParam ) )
			this->CheckDevOption();
		else if( name == "lamp_btn" )
			if( this->m_dt_lamp == 1 )
				this->setlamp( 	false );
			else
				this->setlamp( true );
		else if( name == "text_log" and EN_MAXTEXT == HIWORD( msg.wParam ) )
			this->clearlog();
	}catch( ... ) {
		//this->addlogline( "触发了未知的指令" );
		return;
	}
}

bool	commport::start( void )	//扫描可用串口并打开
{
	this->addlogline( "正在扫描有效的串口" );
	for( int i = 0; i < 10; i++ )
		if( this->start( i ) )
			return true;
	this->addlogline( "未找到有效串口" );
	return false;
}
bool	commport::start( int portnum )	//打开指定串口
{
	if( portnum < 0 or portnum >= 10 )
		return false;
	char name[32];
	sprintf( name, "COM%d", portnum );	//构建串口名 
	this->m_hPort = CreateFile( 
		name, GENERIC_READ | GENERIC_WRITE, 0, 0,
	 	OPEN_EXISTING,
	0, NULL );
	if( INVALID_HANDLE_VALUE == this->m_hPort )	//如果操作失败
		return false;
	
	
	this->addlogline( "已启动串口: " + string(name) );
	
	this->m_ComNum = portnum;
	/***设置超时***/
	COMMTIMEOUTS comm_timeout;
	comm_timeout.ReadIntervalTimeout = 20;
	comm_timeout.ReadTotalTimeoutConstant = 100;
	comm_timeout.ReadTotalTimeoutMultiplier = 20;
	comm_timeout.WriteTotalTimeoutConstant = 0;
	comm_timeout.WriteTotalTimeoutMultiplier = 0;
	if( !SetCommTimeouts( this->m_hPort, &comm_timeout ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "超时设置配置失败 错误代码:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	/***建立刷新定时器***/
	this->m_TimerID = SetTimer( this->m_layout.getMain(), 1000, 1000, NULL );
	
	/***设置串口配置***/
	GetCommState( this->m_hPort, &this->m_Param );
	this->m_Param.BaudRate = this->m_Baudrate;
	this->m_Param.ByteSize = this->m_Databits;
	this->m_Param.StopBits = this->m_Stopbits;
	if( !SetCommState( this->m_hPort, &this->m_Param ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "串口配置失败 错误代码:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	
	{
		char tmpbuf[1024];
		GetCommState( this->m_hPort, &this->m_Param );
		sprintf( tmpbuf, "串口号码:%d\r\n波特率:%d\r\n数据位数%d\r\n", this->m_ComNum, this->m_Baudrate, this->m_Databits );
		SetWindowText( this->m_layout.getHWND( "text_comm" ), tmpbuf );
	}
	
	return true;
}
bool	commport::stop( void )	//关闭串口
{
	if( this->m_hPort != INVALID_HANDLE_VALUE )
	{
		this->addlogline( "正在关闭串口" );
		CloseHandle( this->m_hPort );
		this->m_hPort = INVALID_HANDLE_VALUE;
		KillTimer( this->m_layout.getMain(), this->m_TimerID );
		{
			SetWindowText( this->m_layout.getHWND( "text_comm" ), "串口尚未连接" );
		}
		return true;
	} 
	else
	{
		this->addlogline( "无法关闭设备:串口尚未启动" );
		return false; 
	}
}

bool	commport::CheckDevOption( void )
{
	char tmpbuf[256]; 
	memset( tmpbuf, 0, 256 );
	
	this->m_dt_addr = SendMessage( this->m_layout.getHWND( "dev_list" ), LB_GETCURSEL, 0, 0 );
	if( this->m_dt_addr == LB_ERR )
		return false;
	
	SendMessage( this->m_layout.getHWND( "dev_list" ), LB_GETTEXT, this->m_dt_addr, (LPARAM)tmpbuf );
	this->addlogline( "修正当前监测设备:" + string(tmpbuf) );
	return true;
}

bool	commport::refresh( void )	//刷新当前数据
{
	if( this->m_hPort == INVALID_HANDLE_VALUE )
	{
	this->addlogline( "刷新失败:串口尚未打开" );
	return false;
	}
	//this->addlogline( "刷新" );
	/***构建查询指令***/
	unsigned char cmd_ask[6];
	cmd_ask[0] = 0x3a;	//指令起始符
	*((unsigned short*)(&cmd_ask[1])) = this->m_dt_addr; //地址数据
	cmd_ask[3] = 0x02;	//查询单个终端所有数据的指令
	cmd_ask[4] = cmd_ask[0] xor cmd_ask[1] xor cmd_ask[2] xor cmd_ask[3]; 	//计算校验码
	cmd_ask[5] = 0x23;	//结束符
	/***发送指令***/
	DWORD dwWritten;
	if( !WriteFile( this->m_hPort, cmd_ask, 6, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "查询指令发送失败 错误代码:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	if( dwWritten != 6 )
	{
		this->addlogline( "刷新失败:查询指令发送异常" );
		return false;
	}
	/***等待接收数据***/
	unsigned char cmd_rcv[10];		//构建返回空间
	DWORD dwRead;
	if( !ReadFile( this->m_hPort, cmd_rcv, 10, &dwRead, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "读取反馈数据失败 错误代码:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	/***解析数据***/
	unsigned char Xor = cmd_rcv[0]^cmd_rcv[1]^cmd_rcv[2]^cmd_rcv[3]^cmd_rcv[4]^cmd_rcv[5]^cmd_rcv[6]^cmd_rcv[7]; //计算校验码
	if( dwRead != 10 ) //检查读取数据  
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据字节数错误:%d", dwRead );
		this->addlogline( tmpbuf );
		return false;
	}
	if( cmd_rcv[0] != 0x3a )	//检查起始符 
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据起始符错误:0x%x", cmd_rcv[0] );
		this->addlogline( tmpbuf );
		return false;
	}
	if( *((unsigned short*)(cmd_rcv + 1)) != this->m_dt_addr )//检查设备地址
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据地址错误:0x%x", *((unsigned short*)cmd_rcv + 1) );
		this->addlogline( tmpbuf );
		return false;
	}
	if( cmd_rcv[3] != 0x02 )	//检查功能码
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据功能码错误:0x%x", cmd_rcv[3] );
		this->addlogline( tmpbuf );
		return false;
	}
	if( cmd_rcv[8] != Xor )		//检查校验码		
	{
		this->addlogline( "刷新失败:反馈数据错误" );
		return false;
	}
		
	this->m_dt_temprature	= 	cmd_rcv[4];
	this->m_dt_wedness		=	cmd_rcv[5];
	this->m_dt_mosk			= 	cmd_rcv[6];
	this->m_dt_lamp 		= 	cmd_rcv[7];
	
	char tmpbuf[128];
	//sprintf( tmpbuf, "刷新成功 反馈数据:%x %x %x %x %x %x %x %x %x %x", cmd_rcv[0], cmd_rcv[1],
	//cmd_rcv[2], cmd_rcv[3], cmd_rcv[4], cmd_rcv[5], cmd_rcv[6], cmd_rcv[7], cmd_rcv[8], cmd_rcv[9] );
	//this->m_layout.frame->setHintbarTextColor( dColor( 0xcc, 0xff, 0xcc, 0x80 ) );
	//this->m_layout.frame->setHintbarText( tmpbuf );
	this->Display();	//将读取的数据展示
	return true;
}
bool	commport::setlamp( bool bOn )	//修改光照情况
{
	
}
bool	commport::Display( void )	//将当前数据显示
{
	if( this->m_hPort == INVALID_HANDLE_VALUE )
		return false;
	char tmpbuf[1024];
	
	sprintf( tmpbuf, "当前温度数据:%d", this->m_dt_temprature );
	SetWindowText( this->m_layout.getHWND( "text_temprature" ), tmpbuf );
	
	sprintf( tmpbuf, "当前湿度数据:%d", this->m_dt_wedness );
	SetWindowText( this->m_layout.getHWND( "text_wetness" ), tmpbuf );
	
	sprintf( tmpbuf, "当前灯光数据:%s", (this->m_dt_lamp == 0?"点亮":"熄灭") );
	SetWindowText( this->m_layout.getHWND( "text_lamp" ), tmpbuf );
	if( this->m_dt_lamp == 0 )
		SetWindowText( this->m_layout.getHWND( "lamp_btn" ), "关灯" );
	else
		SetWindowText( this->m_layout.getHWND( "lamp_btn" ), "开灯" );
	
	sprintf( tmpbuf, "当前烟雾数据:%s", (this->m_dt_mosk == 0?"无烟":"有烟") );
	SetWindowText( this->m_layout.getHWND( "text_smoke" ), tmpbuf );
}

void	commport::addlogline( string line )	//添加运行日志
{
	char buffer[1024];
	HWND hwnd = this->m_layout.getHWND( "text_log" );
	GetWindowText( hwnd, buffer, 1024 );
	string rtn(buffer);
	rtn = rtn + "\r\n";
	rtn = rtn + line;
	SetWindowText( hwnd, rtn.data() );
	SendMessage( hwnd, EM_SCROLL, SB_BOTTOM, 0 );
	return;
}
void	commport::clearlog( void )		//清除运行日志
{
	HWND hwnd = this->m_layout.getHWND( "text_log" );
	SetWindowText( hwnd, "" );
	return;
}
#endif
