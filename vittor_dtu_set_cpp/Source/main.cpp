#ifndef __MAIN__
#define __MAIN__
#include <windows.h>
#include <wingdi.h>
#include <string.h>
#include "main.h"

CLayout layout( MainProc );
CDevice device;
bool bRunning = false;
volatile bool brecv = false;
CChain<char*> combuf;
HANDLE TaskThread;
LRESULT (*TaskFunc)(HWND, WORD) = 0;
HWND TaskWnd = 0;
WORD TaskNCode = 0;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	TaskThread = CreateThread( 0, 0, TaskProc, 0, CREATE_SUSPENDED, 0 );
	HFONT hFont = CreateFont( 14, 6, 0, 0, 200, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "����" );
	layout.Init( hFont );
	EnumCommPort();
	ReadComConfig();
	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}


LRESULT CALLBACK MainProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_SHOWWINDOW:
	case WM_MOVE:
	case WM_SIZE:
		OnWorkMode( 0, 0 );
		break;
	case WM_CLOSE:
		if( device.IsCommOpen() ) {
			bRunning = false;
			OnCommSwitch( layout.GetHandle( "��ť_���ڿ���" ), BN_CLICKED );
		}
		PostQuitMessage( 0 );
		break;
	case WM_DEVICECHANGE:
		EnumCommPort();
		if( device.IsCommOpen() ) {
			bRunning = false;
			OnCommSwitch( layout.GetHandle( "��ť_���ڿ���" ), BN_CLICKED );
		}
		break;
	case WM_CTLCOLORSTATIC:
		SetTextColor( (HDC)wParam, RGB( 0, 0, 0 ) );
		SetBkColor( (HDC)wParam, RGB( 240, 240, 240 ) );
		//return (LRESULT)COLOR_WINDOW;
		return (LRESULT)GetStockObject(NULL_BRUSH);
	case WM_COMMAND: {
			LRESULT ret;
			if( layout.CallProc( (HWND)lParam, HIWORD(wParam), ret ) )
				return ret;
			break;
		}
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
}

LRESULT OnSubFunc( HWND hwnd, WORD notify_code ) {
	if( notify_code != BN_CLICKED or bRunning )
		return 0;
	bRunning = true;
	static char cmdline[2048];
	
	if( hwnd == layout.GetHandle( "��ť_������Ϣ" ) ) {
		sprintf( cmdline, "AT+H\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_ģ������" ) ) {
		sprintf( cmdline, "AT+Z\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_��ѯ�汾" ) ) {
		sprintf( cmdline, "AT+VER?\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_��ѯ��������" ) ) {
		sprintf( cmdline, "AT+CNUM?\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_��Ϊ�û�Ĭ��" ) ) {
		sprintf( cmdline, "AT+CFGTF\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_�ָ��û�Ĭ��" ) ) {
		sprintf( cmdline, "AT+RELD\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_�ָ���������" ) ) {
		sprintf( cmdline, "AT+CLEAR\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "��ť_�������" ) ) {
		sprintf( cmdline, "AT+S\r\n" );
		device.Command( cmdline, 2048 );
	}
	
	bRunning = false;
	return 0;
}


LRESULT OnCommSwitch( HWND hwnd, WORD notify_code ) {
	if( notify_code != BN_CLICKED or bRunning )
		return 0;
	bRunning = true;
	char buffer[256];
	memset( buffer, 0, 256 );
	if( !device.IsCommOpen() ) {

		int comm_num;
		DCB comm_param;

		memset( buffer, 0, 256 );
		memset( &comm_param, 0, sizeof( comm_param ) );
		GetWindowText( layout.GetHandle( "��Ͽ�_���ں�" ), buffer, 256 );
		if( sscanf( buffer, "COM%d", &comm_num ) <= 0 ) {
			layout.Log( "��ѡ�񴮿ں�" );
			bRunning = false;
			return 0;
		} else {
			//���ɴ��ڲ���

			comm_param.DCBlength = sizeof(comm_param);
			comm_param.fBinary = TRUE;

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("��Ͽ�_���ڲ�����"), buffer, 256 );
			sscanf( buffer, "%d", &comm_param.BaudRate );


			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("��Ͽ�_����λ��"), buffer, 256 );
			sscanf( buffer, "%d", &comm_param.ByteSize );

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("��Ͽ�_ֹͣλ��"), buffer, 256 );
			if( !strcmp( buffer, "1" ) )
				comm_param.StopBits = ONESTOPBIT;
			else if( !strcmp( buffer, "1.5" ) )
				comm_param.StopBits = ONE5STOPBITS;
			else if( !strcmp( buffer, "2" ) )
				comm_param.StopBits = TWOSTOPBITS;

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("��Ͽ�_У�鷽ʽ"), buffer, 256 );
			if( !strcmp( buffer, "NONE" ) ) {
				comm_param.fParity = FALSE;
				comm_param.Parity = NOPARITY;
			} else if( !strcmp( buffer, "ODD" ) ) {
				comm_param.fParity = TRUE;
				comm_param.Parity = ODDPARITY;
			} else if( !strcmp( buffer, "EVEN" ) ) {
				comm_param.fParity = TRUE;
				comm_param.Parity = EVENPARITY;
			}

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("��Ͽ�_������"), buffer, 256 );
			if( !strcmp( buffer, "NONE" ) ) {
				comm_param.fOutxCtsFlow = FALSE;
				comm_param.fRtsControl = RTS_CONTROL_DISABLE;
			} else if( !strcmp( buffer, "CRTS" ) ) {
				comm_param.fOutxCtsFlow = TRUE;
				comm_param.fRtsControl = RTS_CONTROL_ENABLE;
			}

			if( device.OpenComm( comm_num, &comm_param ) ) {
				SendMessage( hwnd, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon( GetModuleHandle(0),
				             (LPCSTR)RES_BMP_GREEN ) );
				SetWindowText( hwnd, "�رմ���" );
				layout.Log( "�����Ѵ�" );
				WriteComConfig();
			} else {
				layout.Log( "���ڴ�ʧ��" );
			}
			bRunning = false;
			return 0;
		}
	} else {
		if( device.CloseComm() ) {
			SendMessage( hwnd, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon( GetModuleHandle(0),
				             (LPCSTR)RES_BMP_RED ) );
			SetWindowText( hwnd, "�򿪴���" );
			layout.Log( "�����ѹر�" );
		} else {
			layout.Log( "���ڹر�ʧ��" );
		}
		bRunning = false;
		return 0;
	}
}


LRESULT OnWorkMode( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED )
		return 0;
	bRunning = true;
	hwnd = layout.GetHandle( "��_����ģʽ" );
	HDC hdc = GetWindowDC( hwnd );
	HDC hbmc = CreateCompatibleDC( hdc );
	HBITMAP hbmp = 0;
	HWND h_net = layout.GetHandle( "��_�豸����_����͸��" );
	HWND h_http = layout.GetHandle( "��_�豸����_HTTPD" );
	HWND h_message = layout.GetHandle( "��_�豸����_����͸��" );

	ShowWindow( h_net, SW_HIDE );
	ShowWindow( h_http, SW_HIDE );
	ShowWindow( h_message, SW_HIDE );
	
	if( SendMessage( layout.GetHandle( "��ѡ��ť__����ģʽ_����͸��" ), BM_GETCHECK, 0, 0 ) ) {

		hbmp = LoadBitmap( GetModuleHandle(0), (LPCSTR)RES_BMP_SOCKET );
		ShowWindow( h_net, SW_SHOW );
	} else if( SendMessage( layout.GetHandle( "��ѡ��ť__����ģʽ_HTTPD" ), BM_GETCHECK, 0, 0 ) ) {

		hbmp = LoadBitmap( GetModuleHandle(0), (LPCSTR)RES_BMP_HTTPD );
		ShowWindow( h_http, SW_SHOW );
	} else if( SendMessage( layout.GetHandle( "��ѡ��ť__����ģʽ_����͸��" ), BM_GETCHECK, 0,
	                        0 ) ) {

		hbmp = LoadBitmap( GetModuleHandle(0), (LPCSTR)RES_BMP_MESSAGE );
		ShowWindow( h_message, SW_SHOW );
	}
	SelectObject( hbmc, hbmp );
	BitBlt( hdc, 8, 50, 480, 76, hbmc, 0, 38, SRCCOPY );
	DeleteDC( hbmc );
	ReleaseDC( hwnd, hdc );
	DeleteObject( (HGDIOBJ)hbmp );
	bRunning = false;
	return 0;
}

LRESULT OnFuncCheck( HWND hwnd, WORD motify_code ) {
	RECT rect;
	if( motify_code != BN_CLICKED )
		return 0;
	bRunning = true;
	if( SendMessage( layout.GetHandle( "��ѡ��_������A" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "��_������A����_����͸��" ), SW_SHOW );
	else {
		ShowWindow( layout.GetHandle( "��_������A����_����͸��" ), SW_HIDE );
	}
	if( SendMessage( layout.GetHandle( "��ѡ��_������B" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "��_������B����_����͸��" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "��_������B����_����͸��" ), SW_HIDE );
	if( SendMessage( layout.GetHandle( "��ѡ��_������" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "��_����������_����͸��" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "��_����������_����͸��" ), SW_HIDE );
	if( SendMessage( layout.GetHandle( "��ѡ��_ע���" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "��_ע�������_����͸��" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "��_ע�������_����͸��" ), SW_HIDE );
	if( SendMessage( layout.GetHandle( "��ѡ��_͸����" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "��_͸���Ʋ���_����͸��" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "��_͸���Ʋ���_����͸��" ), SW_HIDE );

	hwnd = layout.GetHandle( "������" );
	rect.left = 16;
	rect.top = 178;
	rect.right = 521;
	rect.bottom = 418;
	//GetWindowRect( layout.GetHandle("��_�豸����_����͸��"), &rect );
	InvalidateRect( hwnd, &rect, TRUE );
	UpdateWindow( hwnd );
	bRunning = false;
	return 0;
}

LRESULT OnQueryParam( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED or bRunning )
		return 0;
	bRunning = true;
	SDeviceParam param;
	device.QueryParam( param );
	ShowParam( param );
	layout.Log( "���" );
	bRunning = false;
	return 0;
}

LRESULT OnSetParam( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED or bRunning )
		return 0;
	bRunning = true;
	SDeviceParam param;
	PullParam( param );
	if( !device.WriteParam( param ) ) {
		layout.Log( "����ʧ��" );
		bRunning = false;
		return 0;
	}
	layout.Log( "���" );
	bRunning = false;
	return 0;
}

LRESULT OnEnterConfigState( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED or bRunning )
		return 0;
	bRunning = true;
	if( !device.SetDeviceState( DeviceState_Config ) ) {
		layout.Log( "��������ģʽʧ��" );
		bRunning = false;
		return 1;
	}
	bRunning = false;
	return 0;
}

void EnumCommPort( void ) {
	HWND hWnd = layout.GetHandle( "��Ͽ�_���ں�" );
	SendMessage( hWnd, CB_RESETCONTENT, 0, 0 );
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM\\", 0, KEY_READ,
	                 &hKey) ) {
		return;
	}
	CHAR Name[25];
	UCHAR szPortName[25];
	LONG Status;
	DWORD dwIndex = 0;
	DWORD dwName;
	DWORD dwSizeofPortName;
	DWORD Type;
	dwName = sizeof(Name);
	dwSizeofPortName = sizeof(szPortName);
	do {
		Status = RegEnumValue( hKey, dwIndex++, Name, &dwName, NULL, &Type, szPortName,
		                       &dwSizeofPortName );
		if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)) {
			SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)szPortName );       // �����ַ�������
		}
	} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA));
	RegCloseKey(hKey);
	SendMessage( hWnd, CB_SETCURSEL, 0, 0 );
}

bool GetCheck( const string& name ) {
	return SendMessage( layout.GetHandle( name ), BM_GETCHECK, 0, 0 ) == BST_CHECKED;
}

LRESULT SetCheck( const string& name, bool checked ) {
	return SendMessage( layout.GetHandle( name ), BM_SETCHECK, (checked?BST_CHECKED:BST_UNCHECKED), 0 );
}

LRESULT OnEnterCommunicateState( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED or bRunning )
		return 1;
	bRunning = true;
	device.SetDeviceState( DeviceState_Communicate );
	bRunning = false;
	return 0;
}

bool GetText( const string& name, string& text ) {
	static char text_s[1024];
	text_s[0] = 0;
	HWND hwnd = layout.GetHandle( name );
	if( hwnd == 0 )
		return false;
	GetWindowText( hwnd, text_s, 1024 );
	text = string(text_s);
	return true;
}

string GetText( const string& name ) {
	static char text_s[1024];
	text_s[0] = 0;
	HWND hwnd = layout.GetHandle( name );
	GetWindowText( hwnd, text_s, 1024 );
	return string(text_s);
}

bool SetText( const string& name, const string& text ) {
	HWND hwnd = layout.GetHandle( name );
	if( hwnd == 0 )
		return false;
	SetWindowText( hwnd, text.data() );
	return true;
}

char* CutArg( char* arg ) {
	char* tag = 0;
	if( arg == 0 )
		return tag;
	tag = strstr( arg, ":" );
	if( tag == 0 )
		tag = arg;
	else
		tag += 1;
	arg = tag;
	for( ; *arg != 0; arg++ )
		if( *arg == '\"' or *arg == ',' or arg < 0 )
			*arg = ' ';
		else if( *arg == '\r' )
			*arg = 0;
	return tag;
}


bool Text2Hex( string& text ) {
	char hex[1024];
	char xh;
	char xl;
	memset( hex, 0, 1024 );
	for( int it = 0, ih = 0; text[it] != 0; it++ ) {
		xl = text[it]&0x0F;
		xh = text[it]>>4;
		hex[ih++] = xh + (xh>=10?'A':'0');
		hex[ih++] = xl + (xl>=10?'A':'0');
	}
	text = string(hex);
}

void WriteComConfig( void ) {
	string data;
	HANDLE hFile;
	DWORD dwR;
	hFile = CreateFile( "vittor_com_config", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	data += GetText( "��Ͽ�_���ں�" ) + " ";
	data += GetText( "��Ͽ�_���ڲ�����" ) + " ";
	data += GetText( "��Ͽ�_����λ��" ) + " ";
	data += GetText( "��Ͽ�_ֹͣλ��" ) + " ";
	data += GetText( "��Ͽ�_У�鷽ʽ" ) + " ";
	data += GetText( "��Ͽ�_������" ) + " ";
	WriteFile( hFile, data.data(), data.length(), &dwR, 0 );
	CloseHandle( hFile );
}

void ReadComConfig( void ) {
	char data[256];
	const char *arg_a = data;
	const char *arg_b = data+32;
	const char *arg_c = data+64;
	const char *arg_d = data+96;
	const char *arg_e = data+128;
	const char *arg_f = data+160;
	HANDLE hFile;
	DWORD dwR;
	hFile = CreateFile( "vittor_com_config", GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
	if(hFile == INVALID_HANDLE_VALUE )
		return;
	dwR = GetFileSize( hFile, 0 );
	char *buf = new char[dwR+1];
	buf[dwR] = 0;
	ReadFile( hFile, buf, dwR, &dwR, 0 );
	
	memset( data, 0, 256 );
	sscanf( buf, "%s%s%s%s%s%s", arg_a, arg_b, arg_c, arg_d, arg_e, arg_f );
	
	SetText( "��Ͽ�_���ں�", arg_a );
	SetText( "��Ͽ�_���ڲ�����", arg_b );
	SetText( "��Ͽ�_����λ��", arg_c );
	SetText( "��Ͽ�_ֹͣλ��", arg_d );
	SetText( "��Ͽ�_У�鷽ʽ", arg_e );
	SetText( "��Ͽ�_������", arg_f );
	delete[] buf;
	CloseHandle( hFile );
}


LRESULT OnTemplate( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED )
		return 0;
	if( !device.IsCommOpen() )
		OnCommSwitch( layout.GetHandle( "��ť_���ڿ���" ), BN_CLICKED );
	if( OnEnterConfigState( 0, BN_CLICKED ) )
		return 1;
	SDeviceParam param;
	char buffer[128];
	char *temp = 0;
	if( hwnd == layout.GetHandle( "��ť_���ӹ�����ģ��" ) or hwnd == layout.GetHandle( "��ť_DTU����ģ��" ) ) {
		param.s_work_mode = WorkMode_Net;
		param.socket_param.b_serverA_enable = true;
		param.socket_param.s_serverA_addr = "iot.pouaka.com";
		param.socket_param.s_serverA_port = "20024";
		param.socket_param.s_serverA_protocal = SocketProtocal_TCP;
		param.socket_param.s_serverA_mode = SocketMode_Long;
		param.socket_param.b_heartbeat_enable = true;
		param.socket_param.s_heartbeat_interval = "120";
		param.socket_param.s_heartbeat_type = HeartType_Net;
		param.socket_param.s_heartbeat_value = "00";
		param.socket_param.b_register_enable = true;
		param.socket_param.s_register_send = RegisterSend_Link;
		param.socket_param.s_register_type = RegisterType_REGDT;
		param.socket_param.s_register_value = "01010";
		sprintf( buffer,  "AT+IMEI?\r\n" );
		device.Command( buffer, 128 );
		if( !strstr( buffer, "OK" ) ) {
			layout.Log( "δ�ܻ�ȡIMEI" );
		} else {
			temp = CutArg( buffer );
			param.socket_param.s_register_value += string(temp);                //�ٶ�ȡһλ?/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		param.operator_apn = "CMMTM";

		param.com_baudrate = GetText( "��Ͽ�_���ڲ�����" );
		param.com_parity = GetText( "��Ͽ�_У�鷽ʽ" );
		param.com_data_bit = GetText( "��Ͽ�_����λ��" );
		param.com_stop_bit = GetText( "��Ͽ�_ֹͣλ��" );
		param.com_flow_control = "RS485";
		
		param.b_cache_data = true;
		param.b_net_at = true;
		param.b_show_back = true;
		
		param.command_password = "usr.cn";
		param.startup_message = "USR-GM3";
	} else if( hwnd == layout.GetHandle( "��ť_У��ͨ����ģ��" ) ) {
		param.s_work_mode = WorkMode_Net;
		param.socket_param.b_serverA_enable = true;
		param.socket_param.s_serverA_addr = "iot.pouaka.com";
		param.socket_param.s_serverA_port = "20019";
		param.socket_param.s_serverA_protocal = SocketProtocal_TCP;
		param.socket_param.s_serverA_mode = SocketMode_Long;
		param.socket_param.b_heartbeat_enable = true;
		param.socket_param.s_heartbeat_interval = "60";
		param.socket_param.s_heartbeat_type = HeartType_Net;
		param.socket_param.s_heartbeat_value = "7878051300018CDD0D0A";
		param.socket_param.b_register_enable = true;
		param.socket_param.s_register_send = RegisterSend_Link;
		param.socket_param.s_register_type = RegisterType_REGDT;
		param.socket_param.s_register_value = "78780D010";
		sprintf( buffer,  "AT+IMEI?\r\n" );
		device.Command( buffer, 128 );
		if( !strstr( buffer, "OK" ) ) {
			layout.Log( "δ�ܻ�ȡIMEI" );
			return 1;
		} else {
			temp = CutArg( buffer );
			param.socket_param.s_register_value += string(temp) + "00018CDD0D0A";   //8CDD��Ӧ���ǵõ�����Ĵ���У��
		}
		param.operator_apn = "CMMTM";

		param.com_baudrate = GetText( "��Ͽ�_���ڲ�����" );
		param.com_parity = GetText( "��Ͽ�_У�鷽ʽ" );
		param.com_data_bit = GetText( "��Ͽ�_����λ��" );
		param.com_stop_bit = GetText( "��Ͽ�_ֹͣλ��" );
		param.com_flow_control = "RS485";

		param.b_cache_data = true;
		param.b_net_at = true;
		param.b_show_back = true;

		param.command_password = "usr.cn";
		param.startup_message = "USR-GM3";
	}
	ShowParam( param );
	device.WriteParam( param );                                     //����ʧ��?///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
}

LRESULT OnApn( HWND hwnd, WORD motify_code ) {
	if( motify_code != CBN_SELCHANGE )
		return 0;
	char buf[256];
	LONG sel = SendMessage( hwnd, CB_GETCURSEL, 0, 0 );
	SendMessage( hwnd, CB_GETLBTEXT, sel, (LPARAM)buf );
//	string apn = GetText( "��Ͽ�_��Ӫ�̼�APN" );
	string apn = buf;
	hwnd = layout.GetHandle( "�ı���_��Ӫ�̼�APN" );
	if( apn == "�ƶ�" ) {
		apn = "\"" + string(Operator_CMNET) + "\",\"\",\"\"";
		SetText( "�ı���_��Ӫ�̼�APN", apn );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else if( apn == "��ͨ" ) {
		apn = "\"" + string(Operator_UNINET) + "\",\"\",\"\"";
		SetText( "�ı���_��Ӫ�̼�APN", apn );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else {
		apn = "\"\",\"\",\"\"";
		SetText( "�ı���_��Ӫ�̼�APN", apn );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) & (~ES_READONLY) & (~WS_DISABLED) );
	}
	return 0;
}

void ShowParam( const SDeviceParam& param ) {
	SetCheck( "��ѡ��ť__����ģʽ_����͸��", false );
	SetCheck( "��ѡ��ť__����ģʽ_HTTPD", false );
	SetCheck( "��ѡ��ť__����ģʽ_����͸��", false );

	//����param
	if( param.s_work_mode == WorkMode_Cmd ) {
		return ;
	} else if ( param.s_work_mode == WorkMode_Net ) {
		SetCheck( "��ѡ��ť__����ģʽ_����͸��", true );
	} else if( param.s_work_mode == WorkMode_Httpd ) {
		SetCheck( "��ѡ��ť__����ģʽ_HTTPD", true );
	} else if( param.s_work_mode == WorkMode_Sms ) {
		SetCheck( "��ѡ��ť__����ģʽ_����͸��", true );
	}

	//����͸������
	if( param.socket_param.b_serverA_enable ) {
		SetCheck( "��ѡ��_������A", true );
		SetText( "�ı���_��������ַ_A", param.socket_param.s_serverA_addr );
		SetText( "�ı���_�������˿�_A", param.socket_param.s_serverA_port );
		SetText( "��Ͽ�_����Э��_A", param.socket_param.s_serverA_protocal );
		if( param.socket_param.s_serverA_mode == SocketMode_Long ) {
			SetWindowText( layout.GetHandle( "��Ͽ�_���ӷ�ʽ_A" ), "������" );
		} else {
			SetWindowText( layout.GetHandle( "��Ͽ�_���ӷ�ʽ_A" ), "������" );
		}
	} else {
		SetCheck( "��ѡ��_������A", false );
	}

	if( param.socket_param.b_serverB_enable ) {
		SetCheck( "��ѡ��_������B", true );
		SetText( "�ı���_��������ַ_B", param.socket_param.s_serverB_addr );
		SetText( "�ı���_�������˿�_B", param.socket_param.s_serverB_port );
		SetText( "��Ͽ�_����Э��_B", param.socket_param.s_serverB_protocal );
		if( param.socket_param.s_serverB_mode == SocketMode_Long ) {
			SetWindowText( layout.GetHandle( "��Ͽ�_���ӷ�ʽ_B" ), "������" );
		} else {
			SetWindowText( layout.GetHandle( "��Ͽ�_���ӷ�ʽ_B" ), "������" );
		}
	} else {
		SetCheck( "��ѡ��_������B", false );
	}

	if( param.socket_param.b_heartbeat_enable ) {
		string buf;
		char c;
		SetCheck( "��ѡ��_������", true );
		SetText( "�ı���_���������", param.socket_param.s_heartbeat_interval );
		SetCheck( "��ѡ��_ʮ��������������", true );
		SetText( "�ı���_����������", param.socket_param.s_heartbeat_value );

		if( param.socket_param.s_heartbeat_type == HeartType_Com ) {
			SetWindowText( layout.GetHandle( "��Ͽ�_����������" ), "�򴮿ڷ���" );
		} else {
			SetWindowText( layout.GetHandle( "��Ͽ�_����������" ), "�����������" );
		}
	} else {
		SetCheck( "��ѡ��_������", false );
	}

	if( param.socket_param.b_register_enable ) {
		string buf;
		char c;
		SetCheck( "��ѡ��_ע���", true );
		SetCheck( "��ѡ��_ʮ������ע������", true );
		SetText( "�ı���_ע�������", param.socket_param.s_register_value );


		if( param.socket_param.s_register_type == RegisterType_ICCID ) {
			SetText( "��Ͽ�_ע�������", "ICCID" );
		} else if( param.socket_param.s_register_type == RegisterType_IMEI ) {
			SetText( "��Ͽ�_ע�������", "IMEI" );
		} else if( param.socket_param.s_register_type == RegisterType_REGID ) {
			SetText( "��Ͽ�_ע�������", "ע��ID" );
		} else if( param.socket_param.s_register_type == RegisterType_REGDT ) {
			SetText( "��Ͽ�_ע�������", "�Զ�������" );
		}

		if( param.socket_param.s_register_send == RegisterSend_Link ) {
			SetText( "��Ͽ�_ע������ͷ�ʽ", "���ӽ���ʱ����һ��" );
		} else if( param.socket_param.s_register_send == RegisterSend_Data ) {
			SetText( "��Ͽ�_ע������ͷ�ʽ", "ÿ�η���ָ��֮ǰ����" );
		} else if( param.socket_param.s_register_send == RegisterSend_Both ) {
			SetText( "��Ͽ�_ע������ͷ�ʽ", "ͬʱ������������" );
		}

	} else {
		SetCheck( "��ѡ��_ע�������", false );
	}

	if( param.socket_param.b_cloud_untr ) {
		SetCheck( "��ѡ��_͸����", true );
		SetText( "�ı���_�豸���", param.socket_param.s_cloud_devnum );
		SetText( "�ı���_ͨѶ����", param.socket_param.s_cloud_password );
	} else {
		SetCheck( "��ѡ��_͸����", false );
	}

	//����HTTPD����
	SetText( "��Ͽ�_HTTPD����", param.httpd_param.s_http_method );
	SetText( "�ı���_HTTPDURL", param.httpd_param.s_http_url );
	SetText( "�ı���_HTTPD��ַ", param.httpd_param.s_server_addr );
	SetText( "�ı���_HTTPD�˿�", param.httpd_param.s_server_port );
	SetText( "�ı���_HTTPDͷ��Ϣ", param.httpd_param.s_http_header );

	//���ö���͸������
	SetText( "�ı���_Ŀ��绰��", param.message_param.s_phone_number );

	//����ȫ�ֲ���
	if( param.operator_apn == Operator_CMNET ) {
		SetText( "��Ͽ�_��Ӫ�̼�APN", "�ƶ�" );
		SetWindowLong( layout.GetHandle( "�ı���_��Ӫ�̼�APN" ), GWL_STYLE, GetWindowLong( layout.GetHandle( "�ı���_��Ӫ�̼�APN" ), GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else if( param.operator_apn == Operator_UNINET ) {
		SetText( "��Ͽ�_��Ӫ�̼�APN", "��ͨ" );
		SetWindowLong( layout.GetHandle( "�ı���_��Ӫ�̼�APN" ), GWL_STYLE, GetWindowLong( layout.GetHandle( "�ı���_��Ӫ�̼�APN" ), GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else {
		SetText( "��Ͽ�_��Ӫ�̼�APN", "����" );
		SetWindowLong( layout.GetHandle( "�ı���_��Ӫ�̼�APN" ), GWL_STYLE, GetWindowLong( layout.GetHandle( "�ı���_��Ӫ�̼�APN" ), GWL_STYLE )  & (~ES_READONLY) & (~WS_DISABLED) );
	}
	string tempstr("\"" + param.operator_apn + "\",\"" + param.operator_user + "\",\"" + param.operator_password + "\"");
	SetText( "�ı���_��Ӫ�̼�APN", tempstr );
	SetText( "��Ͽ�_�豸_������", param.com_baudrate );
	SetText( "��Ͽ�_�豸_����", param.com_flow_control );
	SetText( "��Ͽ�_�豸_У��", param.com_parity );
	SetText( "��Ͽ�_�豸_����λ��", param.com_data_bit );
	SetText( "��Ͽ�_�豸_ֹͣλ��", param.com_stop_bit );


	SetCheck( "��ѡ��_ָ�����", param.b_show_back );
	SetCheck( "��ѡ��_��RFC2217", param.b_RFC2217_like );
	SetCheck( "��ѡ��_ͨ������", param.b_telephone );
	SetCheck( "��ѡ��_����ATָ��", param.b_usart_at );
	SetCheck( "��ѡ��_����ATָ��", param.b_net_at );
	SetCheck( "��ѡ��_������", param.b_cache_data );

	SetText( "�ı���_��������", param.command_password );
	SetText( "�ı���_������Ϣ", param.startup_message );

	OnFuncCheck( 0, BN_CLICKED );
}

void PullParam( SDeviceParam& param ) {
	char buffer[512];
	char *arg_a = buffer;
	char *arg_b = buffer + 128;
	char *arg_c = buffer + 256;
	char *arg_d = buffer + 384;
	char *temp = 0;
    string arg_s;
	//��ȡ������Ϣ,��ɲ���
	if( GetCheck( "��ѡ��ť__����ģʽ_����͸��" ) )
		param.s_work_mode = WorkMode_Net;
	else if( GetCheck( "��ѡ��ť__����ģʽ_HTTPD" ) )
		param.s_work_mode = WorkMode_Httpd;
	else if( GetCheck( "��ѡ��ť__����ģʽ_����͸��" ) )
		param.s_work_mode = WorkMode_Sms;

	if( param.s_work_mode == WorkMode_Net ) {
		param.socket_param.b_serverA_enable = GetCheck( "��ѡ��_������A" );
		if( param.socket_param.b_serverA_enable ) {
			param.socket_param.s_serverA_addr = GetText( "�ı���_��������ַ_A" );
			param.socket_param.s_serverA_port = GetText( "�ı���_�������˿�_A" );
			param.socket_param.s_serverA_protocal = GetText( "��Ͽ�_����Э��_A" );
			if( GetText( "��Ͽ�_���ӷ�ʽ_A" ) == "������" )
			    param.socket_param.s_serverA_mode = SocketMode_Short;
			else
				param.socket_param.s_serverA_mode = SocketMode_Long;
		}

		param.socket_param.b_serverB_enable = GetCheck( "��ѡ��_������B" );
		if( param.socket_param.b_serverB_enable ) {
			param.socket_param.s_serverB_addr = GetText( "�ı���_��������ַ_B" );
			param.socket_param.s_serverB_port = GetText( "�ı���_�������˿�_B" );
			param.socket_param.s_serverB_protocal = GetText( "��Ͽ�_����Э��_B" );
			if( GetText( "��Ͽ�_���ӷ�ʽ_B" ) == "������" )
			    param.socket_param.s_serverB_mode = SocketMode_Short;
			else
				param.socket_param.s_serverB_mode = SocketMode_Long;
		}

		param.socket_param.b_heartbeat_enable = GetCheck( "��ѡ��_������" );
		if( param.socket_param.b_heartbeat_enable ) {
			param.socket_param.s_heartbeat_interval = GetText( "�ı���_���������" );
			if( GetText( "��Ͽ�_����������" ) == "�򴮿ڷ���" )
				param.socket_param.s_heartbeat_type = HeartType_Com;
			else
				param.socket_param.s_heartbeat_type = HeartType_Net;
			param.socket_param.s_heartbeat_value = GetText( "�ı���_����������" );
			if( !GetCheck( "��ѡ��_ʮ��������������" ) )
				Text2Hex( param.socket_param.s_heartbeat_value );
		}

		param.socket_param.b_register_enable = GetCheck( "��ѡ��_ע���" );
		if( param.socket_param.b_register_enable ) {
			arg_s = GetText( "��Ͽ�_ע������ͷ�ʽ" );
			if( arg_s == "���ӽ���ʱ����һ��" )
				param.socket_param.s_register_send = RegisterSend_Link;
			else if( arg_s == "ÿ�η���ָ��֮ǰ����" )
			    param.socket_param.s_register_send = RegisterSend_Data;
			else
				param.socket_param.s_register_send = RegisterSend_Both;
			arg_s = GetText( "��Ͽ�_ע�������" );
			if( arg_s == "ICCID" )
				param.socket_param.s_register_type = RegisterType_ICCID;
			else if( arg_s == "IMEI" )
				param.socket_param.s_register_type = RegisterType_IMEI;
			else if( arg_s == "ע��ID" )
				param.socket_param.s_register_type = RegisterType_REGID;
			else
				param.socket_param.s_register_type = RegisterType_REGDT;
			param.socket_param.s_register_value = GetText( "�ı���_ע�������" );
			if( !GetCheck( "��ѡ��_ʮ������ע������" ) )
			    Text2Hex( param.socket_param.s_register_value );
		}

		param.socket_param.b_cloud_untr = GetCheck( "��ѡ��_͸����" );
		if( param.socket_param.b_cloud_untr ) {
			param.socket_param.s_cloud_devnum = GetText( "�ı���_�豸���" );
			param.socket_param.s_cloud_password = GetText( "�ı���_ͨѶ����" );
		}
	} else if( param.s_work_mode == WorkMode_Httpd ) {
	    param.httpd_param.s_http_header = GetText( "�ı���_HTTPD����ͷ��Ϣ" );
	    param.httpd_param.s_http_method = GetText( "��Ͽ�_HTTPD����" );
	    param.httpd_param.s_http_url = GetText( "�ı���_HTTPD_URL" );
	    param.httpd_param.s_server_addr = GetText( "�ı���_HTTPD��������ַ" );
	    param.httpd_param.s_server_port = GetText( "�ı���_HTTPD�������˿�" );
	} else if( param.s_work_mode == WorkMode_Sms ) {
    	param.message_param.s_phone_number = GetText( "�ı���_Ŀ��绰��" );
	}


	arg_s = GetText( "�ı���_��Ӫ�̼�APN" );
	memset( buffer, 0, 512 );
	strcpy( buffer, arg_s.data() );
	temp = CutArg( buffer );
	sscanf( temp, "%s%s%s", arg_b, arg_c, arg_d );
	param.operator_apn = arg_b;
	param.operator_user = arg_c;
	param.operator_password = arg_d;

	param.com_baudrate = GetText( "��Ͽ�_�豸_������" );
	param.com_parity = GetText( "��Ͽ�_�豸_У��" );
	param.com_data_bit = GetText( "��Ͽ�_�豸_����λ��" );
	param.com_stop_bit = GetText( "��Ͽ�_�豸_ֹͣλ��" );
	param.com_flow_control = GetText( "��Ͽ�_�豸_����" );

	param.b_show_back = GetCheck( "��ѡ��_ָ�����" );
	param.b_RFC2217_like = GetCheck( "��ѡ��_��RFC2217" );
	param.b_telephone = GetCheck( "��ѡ��_ͨ������" );
	param.b_usart_at = GetCheck( "��ѡ��_����ATָ��" );
	param.b_net_at = GetCheck( "��ѡ��_����ATָ��" );
	param.b_cache_data = GetCheck( "��ѡ��_������" );

	param.command_password = GetText( "�ı���_��������" );
	param.startup_message = GetText( "�ı���_������Ϣ" );
}

LRESULT OnSendCmd( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED )
		return 0;
	string cmdline = GetText( "�ı���_�ֶ�ָ��" );
	if( GetCheck( "��ѡ��_�Զ���ȫ" ) ) {
		size_t pos = cmdline.find( "AT+" );
		if( pos == string::npos or pos != 0 )
			pos = cmdline.find( "at+" );
		if( pos == string::npos or pos != 0 ) {
			cmdline = "AT+" + cmdline;
		}
		cmdline += "\r\n";
	}
	device.Command( cmdline );
}

DWORD WINAPI TaskProc( LPVOID p ) {
	while( true ) {
		if( TaskFunc != 0 )
			TaskFunc( TaskWnd, TaskNCode );
		TaskFunc = 0;
		TaskWnd = 0;
		TaskNCode = 0;
		SuspendThread( TaskThread );
	}
}

#endif

