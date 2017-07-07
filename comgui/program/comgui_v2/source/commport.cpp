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
		case WM_USER + 1:	//�趨ָ�����Ϣ 
		case WM_COMMAND:
			break;
		default:
			return DefWindowProc( hwnd, Message, wParam, lParam );
	}
	return 0;
}

commport::commport( layoutlexer& layout ):	//���캯����󶨲��ִ��� 
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

bool	commport::init( string filename )		//ʹ���ĵ����г�ʼ�� 
{
	//���ļ� 
	HANDLE hf = CreateFile( filename.data(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
	if( hf == INVALID_HANDLE_VALUE )
		return false;	//��δ���ļ�,��ʧ��
	
	//��ȡ�ļ����� 
	DWORD dwFileSize = GetFileSize( hf, 0 );
	DWORD dwRead;
	string attr;
	char* buffer = new char[dwFileSize + 1];
	ReadFile( hf, buffer, dwFileSize, &dwRead, 0 );
	buffer[dwFileSize] = '\0';
	
	/***����dxml�ĵ�***/ 
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
	/***����GUI***/ 
	try {
		attr = doc->getAttr( "layout" );
		this->m_layout.lexer( attr, WndProc );
	}catch( ... ) {
		return false;
	}
	
	try {
		attr = doc->getAttr( "baudrate" );
		sscanf( attr.data(), "%d", &this->m_Baudrate );
		this->addlogline( "����������: " + attr );
	}catch( ... ) {
		this->addlogline( "δ�ҵ�����������,Ĭ����Ϊ9600");
		this->m_Baudrate = 9600;
	}
	
	try {
		attr = doc->getAttr( "databits" );
		sscanf( attr.data(), "%d", &this->m_Databits );
		this->addlogline( "����λ����: " + attr );
	}catch( ... ) {
		this->addlogline( "δ�ҵ�����λ������,Ĭ����Ϊ8λ" );
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
		this->addlogline( "ֹͣλ����: " + attr );
	}catch( ... ) {
		this->addlogline( "δ�ҵ�ֹͣλ����,Ĭ����λ1λ" );
		this->m_Stopbits = 0;
	}
	
	
	return true;
}

int		commport::run( void )		//��ʼ����
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

void	commport::process( MSG msg )		//��������Ϣ
{
	string name;
	try {
		name = this->m_layout.getName( LOWORD(msg.wParam) );
		if( name == "button_st" and BN_CLICKED == HIWORD(msg.wParam ) )
			if( this->m_hPort == INVALID_HANDLE_VALUE )
			{
				if( this->start( ) )
					SetWindowText( this->m_layout.getHWND( name ), "�ر�" );
			}
			else
			{
				if( this->stop() )
					SetWindowText( this->m_layout.getHWND( name ), "����" );
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
		//this->addlogline( "������δ֪��ָ��" );
		return;
	}
}

bool	commport::start( void )	//ɨ����ô��ڲ���
{
	this->addlogline( "����ɨ����Ч�Ĵ���" );
	for( int i = 0; i < 10; i++ )
		if( this->start( i ) )
			return true;
	this->addlogline( "δ�ҵ���Ч����" );
	return false;
}
bool	commport::start( int portnum )	//��ָ������
{
	if( portnum < 0 or portnum >= 10 )
		return false;
	char name[32];
	sprintf( name, "COM%d", portnum );	//���������� 
	this->m_hPort = CreateFile( 
		name, GENERIC_READ | GENERIC_WRITE, 0, 0,
	 	OPEN_EXISTING,
	0, NULL );
	if( INVALID_HANDLE_VALUE == this->m_hPort )	//�������ʧ��
		return false;
	
	
	this->addlogline( "����������: " + string(name) );
	
	this->m_ComNum = portnum;
	/***���ó�ʱ***/
	COMMTIMEOUTS comm_timeout;
	comm_timeout.ReadIntervalTimeout = 20;
	comm_timeout.ReadTotalTimeoutConstant = 100;
	comm_timeout.ReadTotalTimeoutMultiplier = 20;
	comm_timeout.WriteTotalTimeoutConstant = 0;
	comm_timeout.WriteTotalTimeoutMultiplier = 0;
	if( !SetCommTimeouts( this->m_hPort, &comm_timeout ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��ʱ��������ʧ�� �������:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	/***����ˢ�¶�ʱ��***/
	this->m_TimerID = SetTimer( this->m_layout.getMain(), 1000, 1000, NULL );
	
	/***���ô�������***/
	GetCommState( this->m_hPort, &this->m_Param );
	this->m_Param.BaudRate = this->m_Baudrate;
	this->m_Param.ByteSize = this->m_Databits;
	this->m_Param.StopBits = this->m_Stopbits;
	if( !SetCommState( this->m_hPort, &this->m_Param ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��������ʧ�� �������:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	
	{
		char tmpbuf[1024];
		GetCommState( this->m_hPort, &this->m_Param );
		sprintf( tmpbuf, "���ں���:%d\r\n������:%d\r\n����λ��%d\r\n", this->m_ComNum, this->m_Baudrate, this->m_Databits );
		SetWindowText( this->m_layout.getHWND( "text_comm" ), tmpbuf );
	}
	
	return true;
}
bool	commport::stop( void )	//�رմ���
{
	if( this->m_hPort != INVALID_HANDLE_VALUE )
	{
		this->addlogline( "���ڹرմ���" );
		CloseHandle( this->m_hPort );
		this->m_hPort = INVALID_HANDLE_VALUE;
		KillTimer( this->m_layout.getMain(), this->m_TimerID );
		{
			SetWindowText( this->m_layout.getHWND( "text_comm" ), "������δ����" );
		}
		return true;
	} 
	else
	{
		this->addlogline( "�޷��ر��豸:������δ����" );
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
	this->addlogline( "������ǰ����豸:" + string(tmpbuf) );
	return true;
}

bool	commport::refresh( void )	//ˢ�µ�ǰ����
{
	if( this->m_hPort == INVALID_HANDLE_VALUE )
	{
	this->addlogline( "ˢ��ʧ��:������δ��" );
	return false;
	}
	//this->addlogline( "ˢ��" );
	/***������ѯָ��***/
	unsigned char cmd_ask[6];
	cmd_ask[0] = 0x3a;	//ָ����ʼ��
	*((unsigned short*)(&cmd_ask[1])) = this->m_dt_addr; //��ַ����
	cmd_ask[3] = 0x02;	//��ѯ�����ն��������ݵ�ָ��
	cmd_ask[4] = cmd_ask[0] xor cmd_ask[1] xor cmd_ask[2] xor cmd_ask[3]; 	//����У����
	cmd_ask[5] = 0x23;	//������
	/***����ָ��***/
	DWORD dwWritten;
	if( !WriteFile( this->m_hPort, cmd_ask, 6, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��ѯָ���ʧ�� �������:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	if( dwWritten != 6 )
	{
		this->addlogline( "ˢ��ʧ��:��ѯָ����쳣" );
		return false;
	}
	/***�ȴ���������***/
	unsigned char cmd_rcv[10];		//�������ؿռ�
	DWORD dwRead;
	if( !ReadFile( this->m_hPort, cmd_rcv, 10, &dwRead, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��ȡ��������ʧ�� �������:%d", GetLastError() );
		this->addlogline( tmpbuf );
		return false;
	}
	/***��������***/
	unsigned char Xor = cmd_rcv[0]^cmd_rcv[1]^cmd_rcv[2]^cmd_rcv[3]^cmd_rcv[4]^cmd_rcv[5]^cmd_rcv[6]^cmd_rcv[7]; //����У����
	if( dwRead != 10 ) //����ȡ����  
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:���������ֽ�������:%d", dwRead );
		this->addlogline( tmpbuf );
		return false;
	}
	if( cmd_rcv[0] != 0x3a )	//�����ʼ�� 
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:����������ʼ������:0x%x", cmd_rcv[0] );
		this->addlogline( tmpbuf );
		return false;
	}
	if( *((unsigned short*)(cmd_rcv + 1)) != this->m_dt_addr )//����豸��ַ
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:�������ݵ�ַ����:0x%x", *((unsigned short*)cmd_rcv + 1) );
		this->addlogline( tmpbuf );
		return false;
	}
	if( cmd_rcv[3] != 0x02 )	//��鹦����
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:�������ݹ��������:0x%x", cmd_rcv[3] );
		this->addlogline( tmpbuf );
		return false;
	}
	if( cmd_rcv[8] != Xor )		//���У����		
	{
		this->addlogline( "ˢ��ʧ��:�������ݴ���" );
		return false;
	}
		
	this->m_dt_temprature	= 	cmd_rcv[4];
	this->m_dt_wedness		=	cmd_rcv[5];
	this->m_dt_mosk			= 	cmd_rcv[6];
	this->m_dt_lamp 		= 	cmd_rcv[7];
	
	char tmpbuf[128];
	//sprintf( tmpbuf, "ˢ�³ɹ� ��������:%x %x %x %x %x %x %x %x %x %x", cmd_rcv[0], cmd_rcv[1],
	//cmd_rcv[2], cmd_rcv[3], cmd_rcv[4], cmd_rcv[5], cmd_rcv[6], cmd_rcv[7], cmd_rcv[8], cmd_rcv[9] );
	//this->m_layout.frame->setHintbarTextColor( dColor( 0xcc, 0xff, 0xcc, 0x80 ) );
	//this->m_layout.frame->setHintbarText( tmpbuf );
	this->Display();	//����ȡ������չʾ
	return true;
}
bool	commport::setlamp( bool bOn )	//�޸Ĺ������
{
	
}
bool	commport::Display( void )	//����ǰ������ʾ
{
	if( this->m_hPort == INVALID_HANDLE_VALUE )
		return false;
	char tmpbuf[1024];
	
	sprintf( tmpbuf, "��ǰ�¶�����:%d", this->m_dt_temprature );
	SetWindowText( this->m_layout.getHWND( "text_temprature" ), tmpbuf );
	
	sprintf( tmpbuf, "��ǰʪ������:%d", this->m_dt_wedness );
	SetWindowText( this->m_layout.getHWND( "text_wetness" ), tmpbuf );
	
	sprintf( tmpbuf, "��ǰ�ƹ�����:%s", (this->m_dt_lamp == 0?"����":"Ϩ��") );
	SetWindowText( this->m_layout.getHWND( "text_lamp" ), tmpbuf );
	if( this->m_dt_lamp == 0 )
		SetWindowText( this->m_layout.getHWND( "lamp_btn" ), "�ص�" );
	else
		SetWindowText( this->m_layout.getHWND( "lamp_btn" ), "����" );
	
	sprintf( tmpbuf, "��ǰ��������:%s", (this->m_dt_mosk == 0?"����":"����") );
	SetWindowText( this->m_layout.getHWND( "text_smoke" ), tmpbuf );
}

void	commport::addlogline( string line )	//���������־
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
void	commport::clearlog( void )		//���������־
{
	HWND hwnd = this->m_layout.getHWND( "text_log" );
	SetWindowText( hwnd, "" );
	return;
}
#endif
