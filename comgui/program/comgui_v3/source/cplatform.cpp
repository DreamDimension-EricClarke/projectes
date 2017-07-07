#ifndef __cplatform_cpp__
#define __cplatform_cpp__

#include "cplatform.h"
#include "dxml.h"
#include <wingdi.h>
#include <time.h>

using namespace std;
using namespace dream;

LRESULT CALLBACK cplatform::InitProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )	//��ʼ�����ڵĴ��ڹ���
{
	static bool*		pbConfirm	= 0;
	static cplatform*	pPlatform	= 0;
	static clayout*		pLayout		= 0;
	static HDC			memDC		= 0;
	static HBITMAP		bmp			= 0;
	static char*		pFileName	= 0;
	switch( msg )
	{
		case WM_CREATE:
			memDC = CreateCompatibleDC( GetDC( hwnd ) );
			bmp = LoadBitmap( GetModuleHandle(0), "icon" );
			SelectObject( memDC, (HGDIOBJ)bmp );
			break;
		case WM_CLOSE:
			return 0;
		case WM_PAINT:
			//LoadBitmap( GetModuleHandle(0), "caption" );
			RECT rect;
			GetWindowRect( hwnd, &rect );
			SetStretchBltMode( GetDC( hwnd ), COLORONCOLOR );
			StretchBlt( GetDC(hwnd), 16, 218, 32,32,
			 memDC, 0, 0, 32, 32, SRCCOPY );
			break;
		case MSG_SETLAYOUT:
			pLayout = (clayout*)lParam;
			break;
		case MSG_SETPLATFORM:
			pPlatform = (cplatform*)lParam;
			break;
		case MSG_SETINITFILE:
			pFileName = (char*)lParam;
			PostMessage( hwnd, MSG_LOADDEF, 0, 0 );
			break;
		case MSG_LOADDEF: {
			HANDLE hInitFile = CreateFile( pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
			if( hInitFile == INVALID_HANDLE_VALUE )
			{
				MessageBox( hwnd, "��ʼ�������ĵ���ʧ!", "��ʼ������", MB_OK );
				PostQuitMessage( 0 );
			}
			DWORD dwLength = GetFileSize( hInitFile, 0 );
			char* pBuf = new char[dwLength + 1];
			DWORD dwRead;
			ReadFile( hInitFile, pBuf, dwLength, &dwRead, 0 );
			pBuf[dwLength] = '\0';
			
			dXmlanal anal;
			dXmlnode* node = anal.analysisCode( pBuf );
			
			string attr;
			if( node->getAttr( "baudrate", attr ) )
			{
				int BaudrateNum = 0;
				sscanf( attr.data(), "%d", &BaudrateNum );
				PostMessage( pLayout->getHandle( "combo_baudrate" ), CB_SETCURSEL, BaudrateNum, 0 );
			}
			
			if( node->getAttr( "stopbits", attr ) )
			{
				int StopbitsNum = 0;
				sscanf( attr.data(), "%d", &StopbitsNum );
				PostMessage( pLayout->getHandle( "combo_stopbits" ), CB_SETCURSEL, StopbitsNum, 0 );
			}
			
			if( node->getAttr( "bytesize", attr ) )
			{
				int BytesizeNum = 0;
				sscanf( attr.data(), "%d", &BytesizeNum );
				PostMessage( pLayout->getHandle( "combo_bytesize" ), CB_SETCURSEL, BytesizeNum, 0 );
			}
			
			if( node->getAttr( "block", attr ) )
			{
				SendMessage( pLayout->getHandle( "edit_block" ), WM_SETTEXT, 0, (LPARAM)attr.data() );
			}
			delete[] pBuf;
			CloseHandle( hInitFile );
			break;
		}
		case MSG_SETDEF: {
			dXmlnode* node = dXmlnode::newnode( "comgui", 0 );
			char buf[256]; 
			
			DWORD dwArg = SendMessage( pLayout->getHandle( "combo_baudrate" ), CB_GETCURSEL, 0, 0 );
			sprintf( buf, "%d", dwArg );
			node->setAttr( "baudrate", buf );
			
			dwArg = SendMessage( pLayout->getHandle( "combo_stopbits" ), CB_GETCURSEL, 0, 0 );
			sprintf( buf, "%d", dwArg );
			node->setAttr( "stopbits", buf );
			
			dwArg = SendMessage( pLayout->getHandle( "combo_bytesize" ), CB_GETCURSEL, 0, 0 );
			sprintf( buf, "%d", dwArg );
			node->setAttr( "bytesize", buf );
			
			memset( buf, 0, 256 );
			GetWindowText( pLayout->getHandle( "edit_block" ), buf, 256 );
			node->setAttr( "block", buf );
			
			string code = dXmlanal::generatCode( node );
			HANDLE hInitFile = CreateFile( pFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
			WriteFile( hInitFile, code.data(), code.length(), &dwArg, 0 );
			CloseHandle( hInitFile );
			break;
		}
		case MSG_SETCONFIRM:
			pbConfirm = (bool*)lParam;
			break;
		case WM_COMMAND:
			if( HIWORD(wParam) != BN_CLICKED )
				break;
			if( (HWND)lParam == pLayout->getHandle( "button_yes" ) )
			{
				char Buf[128];
				
				memset( Buf, 0, 128 );
				GetWindowText( pLayout->getHandle( "combo_baudrate" ), Buf, 128 );
				sscanf( Buf, "%d", &pPlatform->m_baudrate );
				
				pPlatform->m_stopbits = SendMessage( pLayout->getHandle( "combo_stopbits" ), CB_GETCURSEL, 0, 0 );
				
				memset( Buf, 0, 128 );
				GetWindowText( pLayout->getHandle( "combo_bytesize" ), Buf, 128 );
				sscanf( Buf, "%d", &pPlatform->m_bytesize );
				
				memset( Buf, 0, 128 );
				GetWindowText( pLayout->getHandle( "edit_block" ), Buf, 128 );
				sscanf( Buf, "%d", &pPlatform->m_block );
				
				*pbConfirm = true;
				PostQuitMessage( 0 );
				break;
			}
			else if( (HWND)lParam == pLayout->getHandle( "button_no" ) )
			{
				PostQuitMessage( 0 );
				break;
			}
			else if( (HWND)lParam == pLayout->getHandle( "button_load" ) )
			{
				PostMessage( hwnd, MSG_LOADDEF, 0, 0 );
				break;
			}
			else if( (HWND)lParam == pLayout->getHandle( "button_set" ) )
			{
				PostMessage( hwnd, MSG_SETDEF, 0, 0 );
				break;
			}
		default:
			break;
	}
	
	return DefWindowProc( hwnd, msg, wParam, lParam );
}
LRESULT CALLBACK cplatform::MainProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )	//�����ڴ��ڹ��� 
{
	static cplatform*	pPlatform = 0;
	switch( msg )
	{
		case MSG_SETPLATFORM:
			pPlatform = (cplatform*)lParam;
			pPlatform->refresh( false );
			break;
		case WM_CLOSE:
			PostQuitMessage( 0 );
			break;
		case WM_TIMER:
			if( wParam != 0 and pPlatform != 0 )
				if( wParam == pPlatform->m_TimerID )
				{
					pPlatform->refresh( true );
				}
				else if( wParam == pPlatform->m_TimerDate )
				{
					char buf[1024];
					time_t t;
					t = time( &t );
					struct tm *now;
					now = localtime( &t );
					sprintf( buf, "%d/%d/%d-%d:%d", now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min );
					SetWindowText( pPlatform->m_layout.getHandle( "text_date" ), buf );
				}
			break;
		case WM_COMMAND: {
			if( pPlatform == 0 )
				break;
			HWND hWg = (HWND)lParam;
			if( hWg == pPlatform->m_layout.getHandle( "button_start_stop" ) and HIWORD(wParam) == BN_CLICKED )
			{
				if( pPlatform->m_hPort == INVALID_HANDLE_VALUE )
				{
					pPlatform->addLogLine( "��ʼɨ����Ч����" );
					int i;
					for( i = 0; i < 10; i++ )
						if( pPlatform->OpenComm( i ) )
							break;
					if( i == 10 )
					{
						pPlatform->addLogLine( "����ɨ��ʧ��" );
						pPlatform->refresh( false );
						break;
					}
					SetWindowText( hWg, "�Ͽ�����" );
					char buf[1024];
					sprintf( buf, "����������\r\n���ں�:%d\r\n������:%d\r\nֹͣλ����:%s\r\n����λ��:%d",
					pPlatform->m_CommNum, 
					pPlatform->m_baudrate, 
					(pPlatform->m_stopbits==0?"1λ":(pPlatform->m_stopbits==1?"1.5λ":"2λ")),
					pPlatform->m_bytesize );
					SetWindowText( pPlatform->m_layout.getHandle( "text_parg"), buf );
					break;
				}
				else
				{
					pPlatform->CloseComm();
					SetWindowText( hWg, "�������" );
					SetWindowText( pPlatform->m_layout.getHandle( "text_parg"), "������δ����" );
					break;
				}
			}
			else if( hWg == pPlatform->m_layout.getHandle( "button_refresh" ) and HIWORD(wParam) == BN_CLICKED )
			{
				if( pPlatform->m_hPort == INVALID_HANDLE_VALUE )
				{
					pPlatform->addLogLine( "ˢ��ȡ��:������δ����" );
					break;
				}
				if( !pPlatform->refresh( true ) )
					pPlatform->addLogLine( "ˢ��ʧ��" );
				break;
			}
			else if( hWg == pPlatform->m_layout.getHandle( "list_device" ) and HIWORD(wParam) == LBN_SELCHANGE )
			{
				char buf[1024];
				pPlatform->m_DevAddr = SendMessage( hWg, LB_GETCURSEL, 0, 0 ) + 1;
				sprintf( buf, "��ǰ�豸��ַ:%d", pPlatform->m_DevAddr );
				pPlatform->addLogLine( buf );
				break;
			}
			else if( hWg == pPlatform->m_layout.getHandle( "button_reset" ) and HIWORD(wParam) == BN_CLICKED )
			{
				clayout* initwnd = new clayout;
				bool	bConfirm = false;	//�Ƿ�ȷ��
				MSG msg;
				
				if( pPlatform->m_hPort != INVALID_HANDLE_VALUE )
				{
					pPlatform->CloseComm();
					SetWindowText( pPlatform->m_layout.getHandle( "button_start_stop" ), "�������" );
				}
				
				if( !initwnd->create( "layout_init.dxml", InitProc ) )
					break;
				PostMessage( initwnd->getHandle(0), MSG_SETLAYOUT, 0, (LPARAM)initwnd );
				PostMessage( initwnd->getHandle(0), MSG_SETPLATFORM, 0, (LPARAM)pPlatform );
				PostMessage( initwnd->getHandle(0), MSG_SETCONFIRM, 0, (LPARAM)&bConfirm );
				PostMessage( initwnd->getHandle(0), MSG_SETINITFILE, 0, (LPARAM)"init.dxml" );
				
				SetWindowText( initwnd->getHandle( "button_yes" ), "ȷ��" );
				SetWindowText( initwnd->getHandle( "button_no" ), "ȡ��" );
				
				while( GetMessage( &msg, NULL, 0, 0 ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				
				delete initwnd;
				break;
			}
			else if( (HWND)lParam == pPlatform->m_layout.getHandle( "button_switch" ) )
			{
				if( pPlatform->m_hPort == INVALID_HANDLE_VALUE )
				{
					pPlatform->addLogLine( "�ٿ�ȡ��:������δ����" );
					break;
				}
				pPlatform->addLogLine( "���ڵ�������ģʽ" );
				if( !pPlatform->ctlrefresh( ) )
					pPlatform->addLogLine( "�ٿ�ʧ��" );
				break;
			}
			else if( (HWND)lParam == pPlatform->m_layout.getHandle( "button_water" ) )
			{
				if( pPlatform->m_hPort == INVALID_HANDLE_VALUE )
				{
					pPlatform->addLogLine( "�ٿ�ȡ��:������δ����" );
					break;
				}
				pPlatform->addLogLine( "���ڵ������װ��" );
				if( !pPlatform->ctlwater( !pPlatform->m_lamp ) )
					pPlatform->addLogLine( "�ٿ�ʧ��" );
				break;
			}
			else if( (HWND)lParam == pPlatform->m_layout.getHandle( "button_water_on" ) )
			{
				if( pPlatform->m_hPort == INVALID_HANDLE_VALUE )
				{
					pPlatform->addLogLine( "�ٿ�ȡ��:������δ����" );
					break;
				}
				pPlatform->addLogLine( "���ڿ������װ��" );
				if( !pPlatform->ctlwater( 1 ) )
					pPlatform->addLogLine( "�ٿ�ʧ��" );
				break;
			}
			else if( (HWND)lParam == pPlatform->m_layout.getHandle( "button_water_off" ) )
			{
				if( pPlatform->m_hPort == INVALID_HANDLE_VALUE )
				{
					pPlatform->addLogLine( "�ٿ�ȡ��:������δ����" );
					break;
				}
				pPlatform->addLogLine( "���ڹرչ��װ��" );
				if( !pPlatform->ctlwater( 0 ) )
					pPlatform->addLogLine( "�ٿ�ʧ��" );
				break;
			}
			else
				break;
			break;
		}
		default:
			break;
	}
	return DefWindowProc( hwnd, msg, wParam, lParam );
}


cplatform::cplatform( void ):
m_TimerID(0),
m_lineCount(0),
m_hPort(INVALID_HANDLE_VALUE),
m_sync(false),
m_TimerDate(0)
{
	
}
cplatform::~cplatform( void )
{
	this->CloseComm();
}

bool	cplatform::init( void )			//��ʼ�������³�ʼ���������� !!!�⽫��ʾ��ʼ������!
{
	clayout* initwnd = new clayout;
	bool	bConfirm = false;	//�Ƿ�ȷ��
	MSG msg;
	
	if( !initwnd->create( "layout_init.dxml", InitProc ) )
		return false;
	PostMessage( initwnd->getHandle(0), MSG_SETLAYOUT, 0, (LPARAM)initwnd );
	PostMessage( initwnd->getHandle(0), MSG_SETPLATFORM, 0, (LPARAM)this );
	PostMessage( initwnd->getHandle(0), MSG_SETCONFIRM, 0, (LPARAM)&bConfirm );
	PostMessage( initwnd->getHandle(0), MSG_SETINITFILE, 0, (LPARAM)"init.dxml" );
	
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	
	if( !bConfirm )
		return false;
	
	delete initwnd;
	
	if( !this->m_layout.create( "layout_main.dxml", MainProc ) )
	{
		MessageBox( NULL, "�������ʼ��ʧ��", "��ʼ��ʧ��", MB_OK );
		return false;
	}
	
	PostMessage( this->m_layout.getHandle(0), MSG_SETPLATFORM, 0, (LPARAM)this );
	this->m_TimerDate = SetTimer( this->m_layout.getHandle(0), 1001, 1000, 0 );	//����ϵͳʱ�䶨ʱ�� 
	return true;
}
bool	cplatform::refresh( bool bDis )		//ˢ�¼������
{
	if( this->m_hPort == INVALID_HANDLE_VALUE or !bDis )
	{
		SetWindowText(this->m_layout.getHandle( "text_temperature" ), "��������" );
		SetWindowText(this->m_layout.getHandle( "text_humidity" ), "��������" );
		SetWindowText(this->m_layout.getHandle( "text_smoke" ), "��������" );
		SetWindowText(this->m_layout.getHandle( "text_water" ), "��������" );
		SetWindowText(this->m_layout.getHandle( "text_humidity_mud" ), "��������" );
		SetWindowText( this->m_layout.getHandle( "button_water" ), "��ȿ���" );
		return false;
	}
	
	while( this->m_sync );
	this->m_sync = true;
	
	/***������ѯָ��***/
	unsigned char cmd_ask[6];
	cmd_ask[0] = 0x3a;	//ָ����ʼ��
	cmd_ask[1] = 0x00;
	cmd_ask[2] = this->m_DevAddr; //��ַ����
	cmd_ask[3] = 0x02;	//��ѯ�����ն��������ݵ�ָ��
	cmd_ask[4] = cmd_ask[0] xor cmd_ask[1] xor cmd_ask[2] xor cmd_ask[3]; 	//����У����
	cmd_ask[5] = 0x23;	//������
	/***����ָ��***/
	DWORD dwWritten;
	if( !WriteFile( this->m_hPort, cmd_ask, 6, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��ѯָ���ʧ�� �������:%d", GetLastError() );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	if( dwWritten != 6 )
	{
		this->addLogLine( "ˢ��ʧ��:��ѯָ����쳣" );
		this->m_sync = false;
		return false;
	}
	/***�ȴ���������***/
	unsigned char cmd_rcv[11];		//�������ؿռ�
	DWORD dwRead;
	if( !ReadFile( this->m_hPort, cmd_rcv, 11, &dwRead, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��ȡ��������ʧ�� �������:%d", GetLastError() );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	/***��������***/
	unsigned char Xor = cmd_rcv[0]^cmd_rcv[1]^cmd_rcv[2]^cmd_rcv[3]^cmd_rcv[4]^cmd_rcv[5]^cmd_rcv[6]^cmd_rcv[7]^cmd_rcv[8]; //����У����
	if( dwRead != 11 ) //����ȡ����
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:���������ֽ�������:%d", dwRead );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	if( cmd_rcv[0] != 0x3a )	//�����ʼ�� 
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:����������ʼ������:0x%x", cmd_rcv[0] );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	//if( *((unsigned short*)(cmd_rcv + 1)) != this->m_DevAddr )//����豸��ַ
	if( cmd_rcv[2] != this->m_DevAddr )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:�������ݵ�ַ����:0x%x", cmd_rcv[2] );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	
	
	if( cmd_rcv[3] != 0x02 )	//��鹦����
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "ˢ��ʧ��:�������ݹ��������:0x%x", cmd_rcv[3] );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	if( cmd_rcv[9] != Xor )		//���У����		
	{
		this->addLogLine( "ˢ��ʧ��:�������ݴ���" );
		this->m_sync = false;
		return false;
	}
	
	{
		char Buf[256];
		
		sprintf( Buf, "��ǰ�¶�%d", cmd_rcv[4] );
		SetWindowText(this->m_layout.getHandle( "text_temperature" ), Buf );
		
		sprintf( Buf, "��ǰʪ��%d", cmd_rcv[5] );
		SetWindowText(this->m_layout.getHandle( "text_humidity" ), Buf );
		
		sprintf( Buf, "%s", (cmd_rcv[6]==1?"����������ɢ":"δ��⵽��������") );
		SetWindowText(this->m_layout.getHandle( "text_smoke" ), Buf );
		
		sprintf( Buf, "���װ����%s(%d)", (cmd_rcv[7]==0?"�ر�":"����" ), cmd_rcv[6] );
		SetWindowText(this->m_layout.getHandle( "text_water" ), Buf );
		this->m_lamp = cmd_rcv[7];
		SetWindowText( this->m_layout.getHandle( "button_water" ), (cmd_rcv[7]==0?"�������װ��":"�رչ��װ��") );
		
		sprintf( Buf, "��ǰ����ʪ��%03.2lf", 100.0 - 100.0/128.0*cmd_rcv[8] );
		SetWindowText(this->m_layout.getHandle( "text_humidity_mud" ), Buf );
	}
	this->m_sync = false;
	return true;
}
bool	cplatform::ctlwater( char stat )		//��ȿ��ƺ��� 
{
	/***��������ָ��***/
	unsigned char cmd[7];
	DWORD dwWritten;
	cmd[0] = 0x3a;				//ָ����ʼ�� 
	cmd[1] = 0x00;				//�̶�
	cmd[2] = this->m_DevAddr;	//��ǰ�ն˺�
	cmd[3] = 0x0a;				//���ָ�����
	cmd[4] = stat;
	cmd[5] = cmd[0] xor cmd[1] xor cmd[2] xor cmd[3] xor cmd[4];	//����У����
	cmd[6] = 0x23;				//������
	
	while( this->m_sync );
	this->m_sync = true;
	
	if( !WriteFile( this->m_hPort, cmd, 7, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "����ָ���ʧ�� �������:%d", GetLastError() );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	if( dwWritten != 7 )
	{
		this->addLogLine( "����ʧ��:����ָ����쳣" );
		this->m_sync = false;
		return false;
	}
	this->addLogLine( "ָ��ͳɹ�" );
	
	DWORD dwRead;
	ReadFile( this->m_hPort, cmd, 7, &dwRead, 0 );
	this->m_sync = false;
	return true;
}

bool	cplatform::ctlrefresh( void )		//�������ݵ��ϴ���� 
{
	unsigned char cmd[6];
	DWORD dwWritten;
	cmd[0] = 0x3a;				//ָ����ʼ�� 
	cmd[1] = 0x00;				//�̶�
	//cmd[2] = this->m_DevAddr;	//��ǰ�ն˺�
	cmd[2] = 0x03;				//��ǰ�ն˺�
	cmd[3] = 0x0f;				//�л��ϴ����� 
	cmd[4] = cmd[0] xor cmd[1] xor cmd[2] xor cmd[3];	//����У����
	cmd[5] = 0x23;				//������
	
	while( this->m_sync );
	this->m_sync = true;
	
	if( !WriteFile( this->m_hPort, cmd, 6, &dwWritten, 0 ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "����ָ���ʧ�� �������:%d", GetLastError() );
		this->addLogLine( tmpbuf );
		this->m_sync = false;
		return false;
	}
	if( dwWritten != 6 )
	{
		this->addLogLine( "����ʧ��:����ָ����쳣" );
		this->m_sync = false;
		return false;
	}
	this->addLogLine( "ָ��ͳɹ�" );
	
	DWORD dwRead;
	ReadFile( this->m_hPort, cmd, 7, &dwRead, 0 );
	this->m_sync = false;
	return true;
}
void	cplatform::addLogLine( string log )	//���һ����־
{
	char buffer[1024];
	HWND hwnd = this->m_layout.getHandle( "text_log" );
	GetWindowText( hwnd, buffer, 1024 );
	string rtn(buffer);
	rtn = rtn + "\r\n";
	rtn = rtn + log;
	SetWindowText( hwnd, rtn.data() );
	SendMessage( hwnd, EM_SCROLL, SB_BOTTOM, 0 );
	return;
}
bool	cplatform::OpenComm( int nComm )//��ָ������
{
	if( nComm < 0 or nComm >= 10 )
		return false;
	char name[32];
	sprintf( name, "COM%d", nComm );	//���������� 
	this->m_hPort = CreateFile( name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0 );
	if( INVALID_HANDLE_VALUE == this->m_hPort )	//�������ʧ��
		return false;
	
	if( !this->ConfigComm() )
		return false;
	
	/***����ˢ�¶�ʱ��***/
	this->m_TimerID = SetTimer( this->m_layout.getHandle(0), 1000, this->m_block, NULL );
	this->m_CommNum = nComm;
	this->addLogLine( "����������: " + string(name) );
	
	return true;
}
bool	cplatform::ConfigComm( void )
{
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
		this->addLogLine( tmpbuf );
		return false;
	}
	
	/***���ô�������***/
	DCB		Param;
	GetCommState( this->m_hPort, &Param );
	Param.BaudRate = this->m_baudrate;
	Param.ByteSize = this->m_bytesize;
	Param.StopBits = this->m_stopbits;
	if( !SetCommState( this->m_hPort, &Param ) )
	{
		char tmpbuf[256];
		sprintf( tmpbuf, "��������ʧ�� �������:%d", GetLastError() );
		this->addLogLine( tmpbuf );
		return false;
	}
	return true;
}
bool	cplatform::CloseComm( void )	//�رմ򿪵Ķ˿� 
{
	if( this->m_hPort == INVALID_HANDLE_VALUE )
		return false;
	CloseHandle( this->m_hPort );
	this->m_hPort = INVALID_HANDLE_VALUE;
	if( this->m_TimerID != 0 )
	{
		KillTimer( this->m_layout.getHandle(0), this->m_TimerID );
		this->m_TimerID = 0;
	}
	return true;
}

#endif
