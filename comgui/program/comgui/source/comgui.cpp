#ifndef __comgui_cpp__
#define __comgui_cpp__
#include "comgui.h"
#include "chain.cpp"
#include <fstream>
#include <windows.h>
#include "item_lexer.h"

comgui gui;

comgui::comgui()
{
	this->m_comm = INVALID_HANDLE_VALUE;
	this->m_commport = 0;
	this->m_timerid = 0;
	this->m_current_div = 0;
	this->m_b_talking = false;
	this->m_b_err = false;
	m_baud_table[0] = 300;
	m_baud_table[1] = 600;
	m_baud_table[2] = 1200;
	m_baud_table[3] = 2400;
	m_baud_table[4] = 4800;
	m_baud_table[5] = 9600;
	m_baud_table[6] = 19200;
	m_baud_table[7] = 38400;
	m_baud_table[8] = 43000;
	m_baud_table[9] = 56000;
	m_baud_table[10] = 57600;
	m_baud_table[11] = 115200;
}

comgui::~comgui()
{
	this->clear();
}
void comgui::clear()
{
	delete this->m_layout.frame;
	memset( &this->m_layout, 0, sizeof( this->m_layout ) );
	if( this->m_comm != INVALID_HANDLE_VALUE )
	{
		CloseHandle( this->m_comm );
		this->m_comm = 0;
	}
	if( 0 != this->m_timerid )
	{
		KillTimer( 0, this->m_timerid );
		this->m_timerid = 0;
	}
	
	HANDLE hF = CreateFile( this->m_config_file.data(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	if( hF !=  INVALID_HANDLE_VALUE )
	{
		string code = this->m_config.generateCode();
		DWORD dwWritten;
		WriteFile( hF, code.data(), code.length(), &dwWritten, 0 );
		CloseHandle( hF );
	}
}

bool comgui::initial( string fname )
{
	this->initLayout();
	this->m_config_file = fname;
	/***用配置文档对程序进行微调***/ 
	HANDLE hFile = CreateFile( 
		fname.data(), GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		readInitFile( hFile );
		CloseHandle( hFile );
	}
}

bool comgui::initLayout( void )
{
	/***初始化主窗口***/
	m_layout.frame = new dFrame( );
	m_layout.frame->setShape( dQuad( 128, 128, 768, 384 ) );					//窗口外框尺寸
	m_layout.frame->setSysbarHeight( 24 );										//标题栏尺寸
	m_layout.frame->setHintbarHeight( 24 );										//提示栏尺寸
	m_layout.frame->setBackgColor( dColor( 0xcc, 0xce, 0xd0, 0x00 ) );			//背景色
	m_layout.frame->setSysbarColor( dColor( 0x3a, 0x3a, 0x3a, 0x00 ) );			//标题栏颜色 
	m_layout.frame->setHintbarColor( dColor( 0x3a, 0x3a, 0x3a, 0x00 ) );		//提示栏颜色
	m_layout.frame->setSysbarBorderColor( dColor( 0x00, 0x00, 0x00, 0xff ) );	//标题栏边框颜色
	m_layout.frame->setHintbarBorderColor( dColor( 0x00, 0x00, 0x00, 0xff ) );	//提示栏边框颜色 
	m_layout.frame->setSysbarText( "  ComGUI" );								//标题栏内容
	m_layout.frame->setSysbarTextColor( dColor( 0xaa, 0xaa, 0xaa, 0x00 ) );		//标题栏文字颜色
	m_layout.frame->createWindow();
	/***初始化退出按钮***/ 
	
	m_layout.button_exit = new dButton( m_layout.frame );
	m_layout.button_exit->setShape( dQuad( 704, -24, 64, 24 ) );				//按钮位置 
	m_layout.button_exit->setBackgColor( dColor( 0xec, 0x3a, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_exit->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_exit->setTextFont( 21, 700, false, false, false );			//按钮内容字体 
	m_layout.button_exit->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );		//按钮文字颜色 
	m_layout.button_exit->setText( ">=<" );										//按钮文字内容 
	/***初始化缩小按钮***/ 
	m_layout.button_mini = new dButton( m_layout.frame );
	m_layout.button_mini->setShape( dQuad( 640, -24, 64, 24 ) );				//按钮位置 
	m_layout.button_mini->setBackgColor( dColor( 0x3a, 0x3a, 0xec, 0x00 ) );	//按钮背景色 
	m_layout.button_mini->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_mini->setTextFont( 21, 700, false, false, false );			//按钮内容字体 
	m_layout.button_mini->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );		//按钮文字颜色
	m_layout.button_mini->setText( "-=-" );										//按钮文字内容 
	
	/***主配置界面选择按钮***/ 
	m_layout.button_basic = new dButton( m_layout.frame );
	m_layout.button_basic->setShape( dQuad( 2, 28, 256, 56 ) );					//按钮位置 
	m_layout.button_basic->setBackgColor( dColor( 0x3a, 0xac, 0xec, 0x00 ) );	//按钮背景色 
	m_layout.button_basic->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_basic->setTextFont( 32, 700, false, false, false );			//按钮内容字体 
	m_layout.button_basic->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_basic->setText( "基本配置界面" );							//按钮文字内容 
	
		
	/***设备配置界面选择按钮***/ 
	for( int i = 0; i < 4; i++ )
	{
		char name[256];
		sprintf( name, "终端 %d 配置界面", i );
		m_layout.button_dev[i] = new dButton( m_layout.frame );
		m_layout.button_dev[i]->setShape( dQuad( 2, 84 + 56*i, 256, 56 ) );			//按钮位置 
		m_layout.button_dev[i]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
		m_layout.button_dev[i]->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
		m_layout.button_dev[i]->setTextFont( 32, 700, false, false, false );		//按钮内容字体 
		m_layout.button_dev[i]->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
		m_layout.button_dev[i]->setText( name );									//按钮文字内容 
	}
	
	/***基础配置界面***/
	m_layout.field_basic = new dField( m_layout.frame );
	m_layout.field_basic->setShape( dQuad( 256, 0, 512, 336 ) );				//外形 
	m_layout.field_basic->setBackgColor( dColor( 0x00, 0x00, 0x00, 0xff ) );	//背景色 
	m_layout.field_basic->setBorderColor( dColor( 0x00, 0x00, 0x00, 0xff ) );	//边框色 
	
	/***启动按钮***/ 
	m_layout.button_start = new dButton( m_layout.field_basic );
	m_layout.button_start->setShape( dQuad( 58, 196, 384, 48 ) );				//按钮位置 
	m_layout.button_start->setBackgColor( dColor( 0x3a, 0xec, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_start->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_start->setTextFont( 32, 700, false, false, false );			//按钮内容字体 
	m_layout.button_start->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_start->setText( "启动串口链接" );							//按钮文字内容 
	
	/***串口号调节按钮***/ 
	m_layout.button_portDn = new dButton( m_layout.field_basic );
	m_layout.button_portDn->setShape( dQuad( 16, 252, 128, 48 ) );				//按钮位置 
	m_layout.button_portDn->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_portDn->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_portDn->setTextFont( 40, 700, false, false, false );		//按钮内容字体 
	m_layout.button_portDn->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_portDn->setText( "<<" );									//按钮文字内容 
	/***串口号调节按钮***/ 
	m_layout.button_portUp  = new dButton( m_layout.field_basic );
	m_layout.button_portUp->setShape( dQuad( 368, 252, 128, 48 ) );				//按钮位置 
	m_layout.button_portUp->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_portUp->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_portUp->setTextFont( 40, 700, false, false, false );		//按钮内容字体 
	m_layout.button_portUp->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_portUp->setText( ">>" );									//按钮文字内容 
	
	/***串口号显示框***/
	m_layout.text_comport = new dText( "COM00", m_layout.field_basic );
	m_layout.text_comport->setAlign( dAcenter );								//文字对齐方式 
	m_layout.text_comport->setShape( dQuad( 144, 252, 224, 48 ) );				//文字框架 
	m_layout.text_comport->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_comport->setFont( 36, 1000, false, false, false );			//文字字体 
	
	m_layout.text_baudHint = new dText( "串口波特率", m_layout.field_basic );	//波特率提示文本 
	m_layout.text_baudHint->setShape( dQuad( 16, 18, 256, 32 ) );				//文字框架 
	m_layout.text_baudHint->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_baudHint->setFont( 24, 700, false, false, false );			//文字字体 
	
	m_layout.button_baudDn = new dButton( m_layout.field_basic );				//波特率微调按钮 
	m_layout.button_baudDn->setShape( dQuad( 272, 18, 48, 32 ) );				//按钮位置 
	m_layout.button_baudDn->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_baudDn->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_baudDn->setTextFont( 24, 700, false, false, false );		//按钮内容字体 
	m_layout.button_baudDn->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_baudDn->setText( "<<<" );									//按钮文字内容 
	
	m_layout.button_baudUp = new dButton( m_layout.field_basic );				//波特率微调按钮
	m_layout.button_baudUp->setShape( dQuad( 466, 18, 48, 32 ) );				//按钮位置 
	m_layout.button_baudUp->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_baudUp->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_baudUp->setTextFont( 24, 700, false, false, false );		//按钮内容字体 
	m_layout.button_baudUp->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_baudUp->setText( ">>>" );									//按钮文字内容 
	
	m_layout.text_baudrate = new dText( "串口尚未连接", m_layout.field_basic );	//波特率显示文本
	m_layout.text_baudrate->setAlign( dAcenter );								//文字对齐方式 
	m_layout.text_baudrate->setShape( dQuad( 320, 18, 144, 32 ) );				//文字框架 
	m_layout.text_baudrate->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_baudrate->setFont( 20, 400, false, false, false );			//文字字体 
	
	m_layout.text_bytesizeHint = new dText( "数据位数", m_layout.field_basic );		//数据位数提示文本 
	m_layout.text_bytesizeHint->setShape( dQuad( 16, 50, 256, 32 ) );				//文字框架 
	m_layout.text_bytesizeHint->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_bytesizeHint->setFont( 24, 700, false, false, false );			//文字字体 
	
	m_layout.button_bytesizeDn = new dButton( m_layout.field_basic );				//数据位数微调按钮 
	m_layout.button_bytesizeDn->setShape( dQuad( 272, 50, 48, 32 ) );				//按钮位置 
	m_layout.button_bytesizeDn->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_bytesizeDn->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_bytesizeDn->setTextFont( 24, 700, false, false, false );		//按钮内容字体 
	m_layout.button_bytesizeDn->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_bytesizeDn->setText( "<<<" );									//按钮文字内容 
	
	m_layout.button_bytesizeUp = new dButton( m_layout.field_basic );				//数据位数微调按钮
	m_layout.button_bytesizeUp->setShape( dQuad( 466, 50, 48, 32 ) );				//按钮位置 
	m_layout.button_bytesizeUp->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_bytesizeUp->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_bytesizeUp->setTextFont( 24, 700, false, false, false );		//按钮内容字体 
	m_layout.button_bytesizeUp->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_bytesizeUp->setText( ">>>" );									//按钮文字内容 
	
	m_layout.text_bytesize = new dText( "串口尚未连接", m_layout.field_basic );	//数据位数显示文本
	m_layout.text_bytesize->setAlign( dAcenter );								//文字对齐方式 
	m_layout.text_bytesize->setShape( dQuad( 320, 50, 144, 32 ) );				//文字框架 
	m_layout.text_bytesize->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_bytesize->setFont( 20, 400, false, false, false );			//文字字体 
	
	m_layout.text_fparityHint = new dText( "奇偶校验", m_layout.field_basic );		//奇偶校验提示文本 
	m_layout.text_fparityHint->setShape( dQuad( 16, 82, 256, 32 ) );				//文字框架 
	m_layout.text_fparityHint->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_fparityHint->setFont( 24, 700, false, false, false );				//文字字体 
	
	m_layout.button_fparityDn = new dButton( m_layout.field_basic );				//奇偶校验微调按钮 
	m_layout.button_fparityDn->setShape( dQuad( 272, 82, 48, 32 ) );				//按钮位置 
	m_layout.button_fparityDn->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_fparityDn->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_fparityDn->setTextFont( 24, 700, false, false, false );			//按钮内容字体 
	m_layout.button_fparityDn->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_fparityDn->setText( "<<<" );									//按钮文字内容 
	
	m_layout.button_fparityUp = new dButton( m_layout.field_basic );				//奇偶校验微调按钮
	m_layout.button_fparityUp->setShape( dQuad( 466, 82, 48, 32 ) );				//按钮位置 
	m_layout.button_fparityUp->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_fparityUp->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_fparityUp->setTextFont( 24, 700, false, false, false );			//按钮内容字体 
	m_layout.button_fparityUp->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_fparityUp->setText( ">>>" );									//按钮文字内容 
	
	m_layout.text_fparity = new dText( "串口尚未连接", m_layout.field_basic );	//奇偶校验显示文本
	m_layout.text_fparity->setAlign( dAcenter );								//文字对齐方式 
	m_layout.text_fparity->setShape( dQuad( 320, 82, 144, 32 ) );				//文字框架 
	m_layout.text_fparity->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_fparity->setFont( 20, 400, false, false, false );				//文字字体 
	
	m_layout.text_parityHint = new dText( "奇偶校验位", m_layout.field_basic );		//奇偶校验位提示文本 
	m_layout.text_parityHint->setShape( dQuad( 16, 114, 256, 32 ) );				//文字框架 
	m_layout.text_parityHint->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );			//文字颜色 
	m_layout.text_parityHint->setFont( 24, 700, false, false, false );				//文字字体 
	
	m_layout.button_parityDn = new dButton( m_layout.field_basic );					//奇偶校验位微调按钮 
	m_layout.button_parityDn->setShape( dQuad( 272, 114, 48, 32 ) );				//按钮位置 
	m_layout.button_parityDn->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_parityDn->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_parityDn->setTextFont( 24, 700, false, false, false );			//按钮内容字体 
	m_layout.button_parityDn->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );		//按钮文字颜色
	m_layout.button_parityDn->setText( "<<<" );										//按钮文字内容 
	
	m_layout.button_parityUp = new dButton( m_layout.field_basic );					//奇偶校验位微调按钮 
	m_layout.button_parityUp->setShape( dQuad( 466, 114, 48, 32 ) );				//按钮位置 
	m_layout.button_parityUp->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_parityUp->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_parityUp->setTextFont( 24, 700, false, false, false );			//按钮内容字体 
	m_layout.button_parityUp->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );		//按钮文字颜色
	m_layout.button_parityUp->setText( ">>>" );										//按钮文字内容 
	
	m_layout.text_parity = new dText( "串口尚未连接", m_layout.field_basic );	//奇偶校验位显示文本
	m_layout.text_parity->setAlign( dAcenter );									//文字对齐方式 
	m_layout.text_parity->setShape( dQuad( 320, 114, 144, 32 ) );				//文字框架 
	m_layout.text_parity->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );			//文字颜色 
	m_layout.text_parity->setFont( 20, 400, false, false, false );				//文字字体 
	
	m_layout.text_stopbitHint = new dText( "停止位", m_layout.field_basic );		//停止位提示文本 
	m_layout.text_stopbitHint->setShape( dQuad( 16, 146, 256, 32 ) );				//文字框架 
	m_layout.text_stopbitHint->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_stopbitHint->setFont( 24, 700, false, false, false );				//文字字体 
	
	m_layout.button_stopbitDn = new dButton( m_layout.field_basic );				//停止位微调按钮 
	m_layout.button_stopbitDn->setShape( dQuad( 272, 146, 48, 32 ) );				//按钮位置 
	m_layout.button_stopbitDn->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_stopbitDn->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_stopbitDn->setTextFont( 24, 700, false, false, false );			//按钮内容字体 
	m_layout.button_stopbitDn->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_stopbitDn->setText( "<<<" );									//按钮文字内容 
	
	m_layout.button_stopbitUp = new dButton( m_layout.field_basic );				//停止位微调按钮 
	m_layout.button_stopbitUp->setShape( dQuad( 466, 146, 48, 32 ) );				//按钮位置 
	m_layout.button_stopbitUp->setBackgColor( dColor( 0x3a, 0x3c, 0xea, 0x00 ) );	//按钮背景色 
	m_layout.button_stopbitUp->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_stopbitUp->setTextFont( 24, 700, false, false, false );			//按钮内容字体 
	m_layout.button_stopbitUp->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );	//按钮文字颜色
	m_layout.button_stopbitUp->setText( ">>>" );									//按钮文字内容 
	
	m_layout.text_stopbit = new dText( "串口尚未连接", m_layout.field_basic );		//停止位显示文本
	m_layout.text_stopbit->setAlign( dAcenter );									//文字对齐方式 
	m_layout.text_stopbit->setShape( dQuad( 320, 146, 144, 32 ) );					//文字框架 
	m_layout.text_stopbit->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );			//文字颜色 
	m_layout.text_stopbit->setFont( 20, 400, false, false, false );					//文字字体 
	
	m_layout.field_device = new dField( this->m_layout.frame );						//设备配置界面 
	m_layout.field_device->setShape( dQuad( 768, 0, 512, 336 ) );					//外形 
	m_layout.field_device->setBackgColor( dColor( 0x00, 0x00, 0x00, 0xff ) );		//背景色 
	m_layout.field_device->setBorderColor( dColor( 0x00, 0x00, 0x00, 0xff ) );		//边框色 
	
	
	m_layout.button_refresh = new dButton( this->m_layout.field_device );			//刷新按钮
	m_layout.button_refresh->setShape( dQuad( 58, 196, 384, 48 ) );					//按钮位置 
	m_layout.button_refresh->setBackgColor( dColor( 0x3a, 0xec, 0x3a, 0x00 ) );		//按钮背景色 
	m_layout.button_refresh->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );	//按钮边框颜色 
	m_layout.button_refresh->setTextFont( 28, 700, false, false, false );			//按钮内容字体 
	m_layout.button_refresh->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );		//按钮文字颜色
	m_layout.button_refresh->setText( "刷新数据" );									//按钮文字内容 
	
	m_layout.button_switch = new dButton( this->m_layout.field_device );			//灯的开关按钮 
	m_layout.button_switch->setShape( dQuad( 336, 138, 128, 40 ) );					//按钮位置 
	m_layout.button_switch->setBackgColor( dColor( 0x3a, 0x3a, 0xec, 0x00 ) );		//按钮背景色 
	m_layout.button_switch->setBorderColor( dColor( 0xff, 0xff, 0xff, 0x30 ) );		//按钮边框颜色 
	m_layout.button_switch->setTextFont( 20, 700, false, false, false );			//按钮内容字体
	m_layout.button_switch->setTextColor( dColor( 0xff, 0xff, 0xff, 0x00 ) );		//按钮文字颜色
	m_layout.button_switch->setText( "灯光 开/关" );								//按钮文字内容 
	
	m_layout.text_temperature = new dText( "温度传感器:暂无数据", this->m_layout.field_device );//温度数据 
	m_layout.text_temperature->setShape( dQuad( 24, 18, 144, 32 ) );				//文字框架 
	m_layout.text_temperature->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_temperature->setFont( 20, 400, false, false, false );				//文字字体 
	
	m_layout.text_humidity = new dText( "湿度传感器:暂无数据", this->m_layout.field_device );//湿度数据 
	m_layout.text_humidity->setShape( dQuad( 24, 58, 144, 32 ) );				//文字框架 
	m_layout.text_humidity->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );		//文字颜色 
	m_layout.text_humidity->setFont( 20, 400, false, false, false );			//文字字体 
	
	m_layout.text_smoke = new dText( "烟雾传感器:暂无数据", this->m_layout.field_device );//烟雾数据 
	m_layout.text_smoke->setShape( dQuad( 24, 98, 144, 32 ) );					//文字框架 
	m_layout.text_smoke->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );			//文字颜色 
	m_layout.text_smoke->setFont( 20, 400, false, false, false );				//文字字体 
	
	m_layout.text_lamp = new dText( "灯光:暂无数据", this->m_layout.field_device );	//灯光数据 
	m_layout.text_lamp->setShape( dQuad( 24, 138, 144, 32 ) );					//文字框架 
	m_layout.text_lamp->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );			//文字颜色 
	m_layout.text_lamp->setFont( 20, 400, false, false, false );				//文字字体 
	
	m_layout.text_devnum = new dText( "当前终端地址:0", this->m_layout.field_device );	//当前终端号码 
	m_layout.text_devnum->setShape( dQuad( 256, 32, 256, 32 ) );						//文字框架 
	m_layout.text_devnum->setColor( dColor( 0x33, 0x33, 0x33, 0x33 ) );					//文字颜色 
	m_layout.text_devnum->setFont( 20, 400, false, false, false );						//文字字体 
	
}

void comgui::readInitFile( HANDLE hFile )
{
	/***读取文件***/ 
	DWORD bufct = GetFileSize( hFile, 0 );
	char *bufcd = new char[bufct+1];
	bufcd[bufct] = '\0';
	ReadFile( hFile, bufcd, bufct, &bufct, 0 );
	/***解析配置脚本***/ 
	LexerCompRep rep = m_config.compilerCode( bufcd );
	if( rep.error == lexerEsuccess )
	{
		if( m_config.findItem( "commport" ) )
		{	//如果设置了默认串口号,则尝试串口号 
			char tmpbuf[32];
			this->m_commport = (int)m_config.getHex();
			sprintf( tmpbuf, "COM%02d", this->m_commport );
			this->m_layout.text_comport->setText( tmpbuf );
			this->onBtnStart();
		}
		if( m_config.findItem( "baudrate" ) )
		{	//如果设置了默认波特率,则尝试设置 
			this->m_commDCB.BaudRate = (DWORD)m_config.getHex();
			this->setCommState();
		}
		if( m_config.findItem( "bytesize" ) )
		{	//如果设置了默认数据位,则尝试设置 
			this->m_commDCB.ByteSize = (DWORD)m_config.getHex();
			this->setCommState();
		}
	}
	delete[] bufcd;
}

void comgui::readCommState( void )
{
	if( INVALID_HANDLE_VALUE == this->m_comm )
	{
		*this->m_layout.text_baudrate = "串口尚未连接";
		*this->m_layout.text_bytesize = "串口尚未连接";
		*this->m_layout.text_fparity = "串口尚未连接";
		*this->m_layout.text_parity = "串口尚未连接";
		*this->m_layout.text_stopbit = "串口尚未连接";
		return;
	}
	if( !GetCommState( this->m_comm, &this->m_commDCB ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "参数读取失败 错误代码%d", GetLastError() ); 
		this->m_layout.frame->setHintbarText( tmpbuf );
		return;
	}
	char tmpbuf[256];
	
	sprintf( tmpbuf, "%d帧/秒", this->m_commDCB.BaudRate );
	*this->m_layout.text_baudrate = tmpbuf;
	
	sprintf( tmpbuf, "%d位", this->m_commDCB.ByteSize );
	*this->m_layout.text_bytesize = tmpbuf;
	
	sprintf( tmpbuf, "%s", ((0 == this->m_commDCB.fParity)?"否":"是") );
	*this->m_layout.text_fparity = tmpbuf;
	
	switch( this->m_commDCB.Parity )
	{
	case EVENPARITY: //偶校验 
		sprintf( tmpbuf, "偶校验", this->m_commDCB.Parity );
		break;
	case MARKPARITY: //标记校验，所发信息帧第9位恒为1 
		sprintf( tmpbuf, "1标记校验", this->m_commDCB.Parity );
		break;
	case NOPARITY: //无校验 
		sprintf( tmpbuf, "无校验", this->m_commDCB.Parity );
		break;
	case ODDPARITY: //奇校验
		sprintf( tmpbuf, "奇校验", this->m_commDCB.Parity );
		break;
	case SPACEPARITY:
		sprintf( tmpbuf, "0标记校验", this->m_commDCB.Parity );
		break;
	}
	*this->m_layout.text_parity = tmpbuf;
	
	switch( this->m_commDCB.StopBits )
	{
	case ONESTOPBIT: //1停止位 
		sprintf( tmpbuf, "1位", this->m_commDCB.StopBits );
		break;
	case ONE5STOPBITS: //1.5停止位 
		sprintf( tmpbuf, "1.5位", this->m_commDCB.StopBits );
		break;
	case TWOSTOPBITS: //2停止位
		sprintf( tmpbuf, "2位", this->m_commDCB.StopBits );
		break;
	}
	*this->m_layout.text_stopbit = tmpbuf;
	
	/***如果串口配置读取成功则填充默认配置***/
	this->refreshConfig();
}

void comgui::setCommState( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->readCommState();
		this->m_layout.frame->setHintbarText( "参数设置失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	
	/***上同步锁***/
	while( this->m_b_talking );
	this->m_b_talking = true; 
	
	if( !SetCommState( this->m_comm, &this->m_commDCB ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "参数设置失败 错误代码:%d", GetLastError() );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->readCommState();
		
		this->m_b_talking = false;
		return;
	}
	this->m_layout.frame->setHintbarText( "" );
	this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
	this->readCommState();
	
	this->readCommState();
	this->m_layout.frame->setHintbarText( "串口配置成功" );
	this->m_layout.frame->setHintbarTextColor( dColor( 0xcc, 0xff, 0xcc, 0x80 ) );
	
	this->m_b_talking = false;
	return;
}

void comgui::onBtnExit( void )
{
	/***如果退出按钮被点击则直接退出程序***/ 
	PostMessage( this->m_layout.frame->getWindow(), WM_QUIT, 0, 0 );
	return;
}
void comgui::onBtnMini( void )
{
	/***如果最小化按钮被点击则缩小窗口***/
	ShowWindow( this->m_layout.frame->getWindow(), SW_MINIMIZE );
}
void comgui::onBtnStart( void )
{
	/***串口句柄的值决定按钮意义***/
	if( INVALID_HANDLE_VALUE == this->m_comm )	//按钮尝试打开串口
	{
		char name[32];
		sprintf( name, "COM%d", this->m_commport );	//构建串口名 
		this->m_comm = CreateFile( 
			name, GENERIC_READ | GENERIC_WRITE, 0, 0,
		 	OPEN_EXISTING,
		0, NULL );
		/***无论失败与否都要更新显示数据***/ 
		this->readCommState();
		if( INVALID_HANDLE_VALUE == this->m_comm )	//如果操作失败
		{
			this->m_layout.frame->setHintbarText( "操作失败:串口未连接或已被占用" );
			this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
			return;
		}
		/***设置超时***/
		COMMTIMEOUTS comm_timeout;
		comm_timeout.ReadIntervalTimeout = 20;
		comm_timeout.ReadTotalTimeoutConstant = 100;
		comm_timeout.ReadTotalTimeoutMultiplier = 20;
		comm_timeout.WriteTotalTimeoutConstant = 0;
		comm_timeout.WriteTotalTimeoutMultiplier = 0;
		SetCommTimeouts( this->m_comm, &comm_timeout );
		/***建立刷新定时器***/
		this->m_timerid = SetTimer( 0, 1000, 1000, GuiTimerProc );
		/***改变按钮风格及意义***/
		this->m_layout.button_start->setText( "关闭串口链接" );
		this->m_layout.button_start->setBackgColor( dColor( 0xec, 0x3a, 0x3a, 0x00 ) );
		return;
	}
	/***串口已打开则意味着要关闭串口***/
	
	/***上同步锁***/
	while( this->m_b_talking );
	this->m_b_talking = true; 
	
	KillTimer( 0, this->m_timerid );
	this->m_timerid = 0;
	CloseHandle( this->m_comm );
	this->m_comm = INVALID_HANDLE_VALUE;
	this->readCommState();
	/***改变按钮风格***/
	this->m_layout.button_start->setText( "启动串口链接" );
	this->m_layout.button_start->setBackgColor( dColor( 0x3a, 0xec, 0x3a, 0x00 ) );
	this->m_b_talking = false;
	return;
}
void comgui::onBtnPortDn( void )
{
	if( INVALID_HANDLE_VALUE != this->m_comm )	//如果串口已经打开则串口号应当被锁定
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口号已被锁定" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	if( this->m_commport <= 0 )
	{
		this->m_layout.frame->setHintbarText( "操作失败:无法设定更小的串口号" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commport -= 1;
	char tmpbuf[32];
	sprintf( tmpbuf, "COM%02d", this->m_commport );
	this->m_layout.text_comport->setText( tmpbuf );
	return;
}
void comgui::onBtnPortUp( void )
{
	if( INVALID_HANDLE_VALUE != this->m_comm )	//如果串口已经打开则串口号应当被锁定
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口号已被锁定" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	if( this->m_commport >= 10 )
	{
		this->m_layout.frame->setHintbarText( "操作失败:无法设定更大的串口号" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commport += 1;
	char tmpbuf[32];
	sprintf( tmpbuf, "COM%02d", this->m_commport );
	this->m_layout.text_comport->setText( tmpbuf );
	return;
}

void comgui::onBtnBaudDn( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	int i;
	for( i = 11; i >= 0; i-- )
		if( this->m_baud_table[i] < this->m_commDCB.BaudRate ) break;
	if( i >= 0 )
	{
		this->m_commDCB.BaudRate = this->m_baud_table[i];
		this->setCommState();
		return;
	}
	this->m_layout.frame->setHintbarText( "操作失败:不能设置更小的波特率" );
	this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
	return;
}
void comgui::onBtnBaudUp( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	int i;
	for( i = 0; i <= 11; i++ )
		if( this->m_baud_table[i] > this->m_commDCB.BaudRate ) break;
	if( i <= 11 )
	{
		this->m_commDCB.BaudRate = this->m_baud_table[i];
		this->setCommState();
		return;
	}
	this->m_layout.frame->setHintbarText( "操作失败:不能设置更小的波特率" );
	this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
	return;
}
void comgui::onBtnBytesizeDn( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.ByteSize -= 1;
	this->setCommState();
	return;
}
void comgui::onBtnBytesizeUp( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.ByteSize += 1;
	this->setCommState();
	return;
}
void comgui::onBtnFparityDn( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.fParity = 0;
	this->m_commDCB.Parity = 0;
	this->setCommState();
	return;
}
void comgui::onBtnFparityUp( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.fParity = 1;
	this->m_commDCB.Parity = 1;
	this->setCommState();
	return;
}
void comgui::onBtnParityDn( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.Parity -= 1;
	this->setCommState();
	return;
}
void comgui::onBtnParityUp( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.Parity += 1;
	this->setCommState();
	return;
}
void comgui::onBtnStopbitsDn( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.StopBits -= 1;
	this->setCommState();
	return;
}
void comgui::onBtnStopbitsUp( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	this->m_commDCB.StopBits += 1;
	this->setCommState();
	return;
}

void comgui::onBtnBase( void )
{
	m_layout.field_basic->setShape( dQuad( 256, 0, 512, 336 ) );
	m_layout.field_device->setShape( dQuad( 768, 0, 512, 336 ) );
	
	
	m_layout.button_basic->setBackgColor( dColor( 0x3a, 0xac, 0xec, 0x00 ) );	//按钮背景色 
	for( int i = 0; i < 4; i++ )
		m_layout.button_dev[i]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	this->readCommState();
	return;
}
void comgui::onBtnDev0( void )
{
	this->m_current_div = 0;
	m_layout.field_device->setShape( dQuad( 256, 0, 512, 336 ) );
	m_layout.field_basic->setShape( dQuad( 768, 0, 512, 336 ) );
	
	m_layout.button_basic->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[0]->setBackgColor( dColor( 0x3a, 0xac, 0xec, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[1]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[2]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[3]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	
	this->Display();
	return;
}
void comgui::onBtnDev1( void )
{
	this->m_current_div = 1;
	m_layout.field_device->setShape( dQuad( 256, 0, 512, 336 ) );
	m_layout.field_basic->setShape( dQuad( 768, 0, 512, 336 ) );
	
	m_layout.button_basic->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[0]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[1]->setBackgColor( dColor( 0x3a, 0xac, 0xec, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[2]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[3]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	
	this->Display();
	return;
}
void comgui::onBtnDev2( void )
{
	this->m_current_div = 2;
	m_layout.field_device->setShape( dQuad( 256, 0, 512, 336 ) );
	m_layout.field_basic->setShape( dQuad( 768, 0, 512, 336 ) );
	
	m_layout.button_basic->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[0]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[1]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[2]->setBackgColor( dColor( 0x3a, 0xac, 0xec, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[3]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	
	this->Display();
	return;
}
void comgui::onBtnDev3( void )
{
	this->m_current_div = 3;
	m_layout.field_device->setShape( dQuad( 256, 0, 512, 336 ) );
	m_layout.field_basic->setShape( dQuad( 768, 0, 512, 336 ) );
	
	m_layout.button_basic->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[0]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[1]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[2]->setBackgColor( dColor( 0xec, 0xac, 0x3a, 0x00 ) );	//按钮背景色 
	m_layout.button_dev[3]->setBackgColor( dColor( 0x3a, 0xac, 0xec, 0x00 ) );	//按钮背景色 
	
	this->Display();
	return;
}

void comgui::onBtnSwitch( void )
{
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
		this->m_layout.frame->setHintbarText( "操作失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		return;
	}
	
	/***上同步锁***/
	while( this->m_b_talking ); 
	this->m_b_talking;
	
	/***构建控制指令***/
	unsigned char cmd[7];
	DWORD dwWritten;
	cmd[0] = 0x3a;		//指令起始符 
	*((unsigned short*)&cmd[1]) = this->m_current_div;	//当前终端号 
	cmd[3] = 0x0a;		//灯光指令功能码 
	if( this->m_lamp == 0 ) 
		cmd[4] = 0x01;		//关闭灯光 
	else
		cmd[4] = 0x00;		//打开灯光 
	cmd[5] = cmd[0] xor cmd[1] xor cmd[2] xor cmd[3] xor cmd[4];	//计算校验码
	cmd[6] = 0x23;	//结束符
	
	if( !WriteFile( this->m_comm, cmd, 7, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "控制指令发送失败 错误代码:%d", GetLastError() );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		return;
	}
	if( dwWritten != 7 )
	{
		this->m_layout.frame->setHintbarText( "操作失败:控制指令发送异常" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		return;
	}
	this->m_b_talking = false;
	return;
}
void comgui::onBtnRefresh( void )
{
	this->Refresh();
}

void comgui::refreshConfig( void )
{
	if( this->m_config.findItem( "commport" ) )
		this->m_config.setValue( (unsigned long long)this->m_commport );
	else
		this->m_config.pushItem( "commport", (unsigned long long)this->m_commport );
		
	if( m_config.findItem( "baudrate" ) )
		this->m_config.setValue( (unsigned long long)this->m_commDCB.BaudRate );
	else
		this->m_config.pushItem( "baudrate", (unsigned long long)this->m_commDCB.BaudRate );
	
	if( m_config.findItem( "bytesize" ) )
		this->m_config.setValue( (unsigned long long)this->m_commDCB.ByteSize );
	else
		this->m_config.pushItem( "bytesize", (unsigned long long)this->m_commDCB.ByteSize );
}

void comgui::process( void )
{
	dEvent event;
	/***获取事件***/ 
	if( !this->m_layout.frame->pickEvent( &event ) )
		return;
	int id = event.widget->getId();
	/***分化事件,触发调度***/ 
	if( id == this->m_layout.button_exit->getId() )
		this->onBtnExit();
	else if( id == this->m_layout.button_mini->getId() )
		this->onBtnMini();
	else if( id == this->m_layout.button_start->getId() )
		this->onBtnStart();
	else if( id == this->m_layout.button_portDn->getId() )
		this->onBtnPortDn();
	else if( id == this->m_layout.button_portUp->getId() )
		this->onBtnPortUp();
	else if( id == this->m_layout.button_baudDn->getId() )
		this->onBtnBaudDn();
	else if( id == this->m_layout.button_baudUp->getId() )
		this->onBtnBaudUp();
	else if( id == this->m_layout.button_bytesizeDn->getId() )
		this->onBtnBytesizeDn();
	else if( id == this->m_layout.button_bytesizeUp->getId() )
		this->onBtnBytesizeUp();
	else if( id == this->m_layout.button_fparityDn->getId() )
		this->onBtnFparityDn();
	else if( id == this->m_layout.button_fparityUp->getId() )
		this->onBtnFparityUp();
	else if( id == this->m_layout.button_parityDn->getId() )
		this->onBtnParityDn();
	else if( id == this->m_layout.button_parityUp->getId() )
		this->onBtnParityUp();
	else if( id == this->m_layout.button_stopbitDn->getId() )
		this->onBtnStopbitsDn();
	else if( id == this->m_layout.button_stopbitUp->getId() )
		this->onBtnStopbitsUp();
	else if( id == this->m_layout.button_basic->getId() )
		this->onBtnBase();
	else if( id == this->m_layout.button_dev[0]->getId() )
		this->onBtnDev0();
	else if( id == this->m_layout.button_dev[1]->getId() )
		this->onBtnDev1();
	else if( id == this->m_layout.button_dev[2]->getId() )
		this->onBtnDev2();
	else if( id == this->m_layout.button_dev[3]->getId() )
		this->onBtnDev3();
	else if( id == this->m_layout.button_refresh->getId() )
		this->onBtnRefresh();
	else if( id == this->m_layout.button_switch->getId() )
		this->onBtnSwitch();
	
}
void comgui::Refresh( void )
{
	if( INVALID_HANDLE_VALUE == this->m_comm )
	{
		this->m_layout.frame->setHintbarText( "刷新失败:串口尚未连接" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_err = true;
		return;
	}
	
	/***上同步锁***/
	while( this->m_b_talking );
	this->m_b_talking = true;
	
	/***构建查询指令***/
	unsigned char cmd_ask[6];
	cmd_ask[0] = 0x3a;	//指令起始符
	*((unsigned short*)(&cmd_ask[1])) = this->m_current_div; //地址数据
	cmd_ask[3] = 0x02;	//查询单个终端所有数据的指令
	cmd_ask[4] = cmd_ask[0] xor cmd_ask[1] xor cmd_ask[2] xor cmd_ask[3]; 	//计算校验码
	cmd_ask[5] = 0x23;	//结束符
	/***发送指令***/
	DWORD dwWritten;
	if( !WriteFile( this->m_comm, cmd_ask, 6, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "查询指令发送失败 错误代码:%d", GetLastError() );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		
		this->m_b_talking = false;
		this->m_b_err = true;
		return;
	}
	if( dwWritten != 6 )
	{
		this->m_layout.frame->setHintbarText( "刷新失败:查询指令发送异常" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		
		this->m_b_err = true;
		this->m_b_talking = false;
		return;
	}
	/***等待接收数据***/
	unsigned char cmd_rcv[10];		//构建返回空间
	DWORD dwRead;
	if( !ReadFile( this->m_comm, cmd_rcv, 10, &dwRead, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "读取反馈数据失败 错误代码:%d", GetLastError() );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		
		this->m_b_err = true;
		this->m_b_talking = false;
		return;
	}
	/***解析数据***/
	unsigned char Xor = cmd_rcv[0]^cmd_rcv[1]^cmd_rcv[2]^cmd_rcv[3]^cmd_rcv[4]^cmd_rcv[5]^cmd_rcv[6]^cmd_rcv[7]; //计算校验码
	if( dwRead != 10 ) //检查读取数据  
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据字节数错误:%d", dwRead );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		
		this->m_b_err = true;
		return;
	}
	if( cmd_rcv[0] != 0x3a )	//检查起始符 
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据起始符错误:0x%x", cmd_rcv[0] );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		
		this->m_b_err = true;
		return;
	}
	if( *((unsigned short*)(cmd_rcv + 1)) != this->m_current_div )//检查设备地址
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据地址错误:0x%x", *((unsigned short*)cmd_rcv + 1) );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		
		this->m_b_err = true;
		return;
	}
	if( cmd_rcv[3] != 0x02 )	//检查功能码
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "刷新失败:反馈数据功能码错误:0x%x", cmd_rcv[3] );
		this->m_layout.frame->setHintbarText( tmpbuf );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		
		this->m_b_err = true;
		return;
	}
	if( cmd_rcv[8] != Xor )		//检查校验码		
	{
		this->m_layout.frame->setHintbarText( "刷新失败:反馈数据错误" );
		this->m_layout.frame->setHintbarTextColor( dColor( 0xff, 0xcc, 0xcc, 0x80 ) );
		this->m_b_talking = false;
		
		this->m_b_err = true;
		return;
	}
		
	this->m_temperature	= 	cmd_rcv[4];
	this->m_humidity	=	cmd_rcv[5];
	this->m_smoke		= 	cmd_rcv[6];
	this->m_lamp 		= 	cmd_rcv[7];
	
	char tmpbuf[128];
	sprintf( tmpbuf, "刷新成功 反馈数据:%x %x %x %x %x %x %x %x %x %x", cmd_rcv[0], cmd_rcv[1],
	cmd_rcv[2], cmd_rcv[3], cmd_rcv[4], cmd_rcv[5], cmd_rcv[6], cmd_rcv[7], cmd_rcv[8], cmd_rcv[9] );
	this->m_layout.frame->setHintbarTextColor( dColor( 0xcc, 0xff, 0xcc, 0x80 ) );
	this->m_layout.frame->setHintbarText( tmpbuf );
	
	this->m_b_talking = false;
	this->m_b_err = false;
	this->Display();	//将读取的数据展示 
	return;
}

void CALLBACK GuiTimerProc( HWND hWnd, UINT msg, UINT_PTR id, DWORD dwTime )
{
	gui.Refresh();
}


void comgui::Display( void )
{
	/***上同步锁***/
	while( this->m_b_talking );
	this->m_b_talking = true; 
	
	char tmpbuf[256];
	sprintf( tmpbuf, "当前终端地址:%d", this->m_current_div );
	*this->m_layout.text_devnum = tmpbuf;
	
	if( this->m_comm == INVALID_HANDLE_VALUE )
	{
			*m_layout.text_temperature = "温度传感器:暂无数据";				//温度数据 
			*m_layout.text_humidity = "湿度传感器:暂无数据";				//湿度数据 
			*m_layout.text_smoke = "烟雾传感器:暂无数据";					//烟雾数据 
			*m_layout.text_lamp = "灯光:暂无数据";							//灯光数据 
			this->m_b_talking = false;
			return;
	}
	if( this->m_b_err )
	{
			*m_layout.text_temperature = "温度传感器:数据可能错误";				//温度数据 
			*m_layout.text_humidity = "湿度传感器:数据可能错误";				//湿度数据 
			*m_layout.text_smoke = "烟雾传感器:数据可能错误";					//烟雾数据 
			*m_layout.text_lamp = "灯光:数据可能错误";							//灯光数据 
			this->m_b_talking = false;
			return;
	}
	
	sprintf( tmpbuf, "温度传感器:%d", this->m_temperature );
	*m_layout.text_temperature = tmpbuf;			//温度数据 
	
	sprintf( tmpbuf, "湿度传感器:%d", this->m_humidity );
	*m_layout.text_humidity = tmpbuf;				//湿度数据 
	
	if( this->m_smoke == 1 )
		sprintf( tmpbuf, "烟雾传感器:有烟雾" );
	else
		sprintf( tmpbuf, "烟雾传感器:无烟雾" );
	*m_layout.text_smoke = tmpbuf;					//烟雾数据 
	
	if( this->m_lamp == 0 )
		sprintf( tmpbuf, "灯光:已启动" );
	else
		sprintf( tmpbuf, "灯光:已关闭" );
	*m_layout.text_lamp = tmpbuf;					//灯光数据 
	
	this->m_b_talking = false;
	return;
}
#endif
