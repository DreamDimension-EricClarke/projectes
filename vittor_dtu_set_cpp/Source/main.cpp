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
	HFONT hFont = CreateFont( 14, 6, 0, 0, 200, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "宋体" );
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
			OnCommSwitch( layout.GetHandle( "按钮_串口开关" ), BN_CLICKED );
		}
		PostQuitMessage( 0 );
		break;
	case WM_DEVICECHANGE:
		EnumCommPort();
		if( device.IsCommOpen() ) {
			bRunning = false;
			OnCommSwitch( layout.GetHandle( "按钮_串口开关" ), BN_CLICKED );
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
	
	if( hwnd == layout.GetHandle( "按钮_帮助信息" ) ) {
		sprintf( cmdline, "AT+H\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_模块重启" ) ) {
		sprintf( cmdline, "AT+Z\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_查询版本" ) ) {
		sprintf( cmdline, "AT+VER?\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_查询本机号码" ) ) {
		sprintf( cmdline, "AT+CNUM?\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_存为用户默认" ) ) {
		sprintf( cmdline, "AT+CFGTF\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_恢复用户默认" ) ) {
		sprintf( cmdline, "AT+RELD\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_恢复出厂设置" ) ) {
		sprintf( cmdline, "AT+CLEAR\r\n" );
		device.Command( cmdline, 2048 );
	} else if( hwnd == layout.GetHandle( "按钮_保存参数" ) ) {
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
		GetWindowText( layout.GetHandle( "组合框_串口号" ), buffer, 256 );
		if( sscanf( buffer, "COM%d", &comm_num ) <= 0 ) {
			layout.Log( "请选择串口号" );
			bRunning = false;
			return 0;
		} else {
			//构成串口参数

			comm_param.DCBlength = sizeof(comm_param);
			comm_param.fBinary = TRUE;

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("组合框_串口波特率"), buffer, 256 );
			sscanf( buffer, "%d", &comm_param.BaudRate );


			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("组合框_数据位数"), buffer, 256 );
			sscanf( buffer, "%d", &comm_param.ByteSize );

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("组合框_停止位数"), buffer, 256 );
			if( !strcmp( buffer, "1" ) )
				comm_param.StopBits = ONESTOPBIT;
			else if( !strcmp( buffer, "1.5" ) )
				comm_param.StopBits = ONE5STOPBITS;
			else if( !strcmp( buffer, "2" ) )
				comm_param.StopBits = TWOSTOPBITS;

			memset( buffer, 0, 256 );
			GetWindowText( layout.GetHandle("组合框_校验方式"), buffer, 256 );
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
			GetWindowText( layout.GetHandle("组合框_流控制"), buffer, 256 );
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
				SetWindowText( hwnd, "关闭串口" );
				layout.Log( "串口已打开" );
				WriteComConfig();
			} else {
				layout.Log( "串口打开失败" );
			}
			bRunning = false;
			return 0;
		}
	} else {
		if( device.CloseComm() ) {
			SendMessage( hwnd, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon( GetModuleHandle(0),
				             (LPCSTR)RES_BMP_RED ) );
			SetWindowText( hwnd, "打开串口" );
			layout.Log( "串口已关闭" );
		} else {
			layout.Log( "串口关闭失败" );
		}
		bRunning = false;
		return 0;
	}
}


LRESULT OnWorkMode( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED )
		return 0;
	bRunning = true;
	hwnd = layout.GetHandle( "组_工作模式" );
	HDC hdc = GetWindowDC( hwnd );
	HDC hbmc = CreateCompatibleDC( hdc );
	HBITMAP hbmp = 0;
	HWND h_net = layout.GetHandle( "组_设备参数_网络透传" );
	HWND h_http = layout.GetHandle( "组_设备参数_HTTPD" );
	HWND h_message = layout.GetHandle( "组_设备参数_短信透传" );

	ShowWindow( h_net, SW_HIDE );
	ShowWindow( h_http, SW_HIDE );
	ShowWindow( h_message, SW_HIDE );
	
	if( SendMessage( layout.GetHandle( "单选按钮__工作模式_网络透传" ), BM_GETCHECK, 0, 0 ) ) {

		hbmp = LoadBitmap( GetModuleHandle(0), (LPCSTR)RES_BMP_SOCKET );
		ShowWindow( h_net, SW_SHOW );
	} else if( SendMessage( layout.GetHandle( "单选按钮__工作模式_HTTPD" ), BM_GETCHECK, 0, 0 ) ) {

		hbmp = LoadBitmap( GetModuleHandle(0), (LPCSTR)RES_BMP_HTTPD );
		ShowWindow( h_http, SW_SHOW );
	} else if( SendMessage( layout.GetHandle( "单选按钮__工作模式_短信透传" ), BM_GETCHECK, 0,
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
	if( SendMessage( layout.GetHandle( "复选框_服务器A" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "组_服务器A参数_网络透传" ), SW_SHOW );
	else {
		ShowWindow( layout.GetHandle( "组_服务器A参数_网络透传" ), SW_HIDE );
	}
	if( SendMessage( layout.GetHandle( "复选框_服务器B" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "组_服务器B参数_网络透传" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "组_服务器B参数_网络透传" ), SW_HIDE );
	if( SendMessage( layout.GetHandle( "复选框_心跳包" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "组_心跳包参数_网络透传" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "组_心跳包参数_网络透传" ), SW_HIDE );
	if( SendMessage( layout.GetHandle( "复选框_注册包" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "组_注册包参数_网络透传" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "组_注册包参数_网络透传" ), SW_HIDE );
	if( SendMessage( layout.GetHandle( "复选框_透传云" ), BM_GETCHECK, 0, 0 ) )
		ShowWindow( layout.GetHandle( "组_透传云参数_网络透传" ), SW_SHOW );
	else
		ShowWindow( layout.GetHandle( "组_透传云参数_网络透传" ), SW_HIDE );

	hwnd = layout.GetHandle( "主窗口" );
	rect.left = 16;
	rect.top = 178;
	rect.right = 521;
	rect.bottom = 418;
	//GetWindowRect( layout.GetHandle("组_设备参数_网络透传"), &rect );
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
	layout.Log( "完成" );
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
		layout.Log( "配置失败" );
		bRunning = false;
		return 0;
	}
	layout.Log( "完成" );
	bRunning = false;
	return 0;
}

LRESULT OnEnterConfigState( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED or bRunning )
		return 0;
	bRunning = true;
	if( !device.SetDeviceState( DeviceState_Config ) ) {
		layout.Log( "进入配置模式失败" );
		bRunning = false;
		return 1;
	}
	bRunning = false;
	return 0;
}

void EnumCommPort( void ) {
	HWND hWnd = layout.GetHandle( "组合框_串口号" );
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
			SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)szPortName );       // 串口字符串保存
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
	data += GetText( "组合框_串口号" ) + " ";
	data += GetText( "组合框_串口波特率" ) + " ";
	data += GetText( "组合框_数据位数" ) + " ";
	data += GetText( "组合框_停止位数" ) + " ";
	data += GetText( "组合框_校验方式" ) + " ";
	data += GetText( "组合框_流控制" ) + " ";
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
	
	SetText( "组合框_串口号", arg_a );
	SetText( "组合框_串口波特率", arg_b );
	SetText( "组合框_数据位数", arg_c );
	SetText( "组合框_停止位数", arg_d );
	SetText( "组合框_校验方式", arg_e );
	SetText( "组合框_流控制", arg_f );
	delete[] buf;
	CloseHandle( hFile );
}


LRESULT OnTemplate( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED )
		return 0;
	if( !device.IsCommOpen() )
		OnCommSwitch( layout.GetHandle( "按钮_串口开关" ), BN_CLICKED );
	if( OnEnterConfigState( 0, BN_CLICKED ) )
		return 1;
	SDeviceParam param;
	char buffer[128];
	char *temp = 0;
	if( hwnd == layout.GetHandle( "按钮_电子柜配置模板" ) or hwnd == layout.GetHandle( "按钮_DTU配置模板" ) ) {
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
			layout.Log( "未能获取IMEI" );
		} else {
			temp = CutArg( buffer );
			param.socket_param.s_register_value += string(temp);                //少读取一位?/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		param.operator_apn = "CMMTM";

		param.com_baudrate = GetText( "组合框_串口波特率" );
		param.com_parity = GetText( "组合框_校验方式" );
		param.com_data_bit = GetText( "组合框_数据位数" );
		param.com_stop_bit = GetText( "组合框_停止位数" );
		param.com_flow_control = "RS485";
		
		param.b_cache_data = true;
		param.b_net_at = true;
		param.b_show_back = true;
		
		param.command_password = "usr.cn";
		param.startup_message = "USR-GM3";
	} else if( hwnd == layout.GetHandle( "按钮_校车通配置模板" ) ) {
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
			layout.Log( "未能获取IMEI" );
			return 1;
		} else {
			temp = CutArg( buffer );
			param.socket_param.s_register_value += string(temp) + "00018CDD0D0A";   //8CDD本应该是得到计算的错误校验
		}
		param.operator_apn = "CMMTM";

		param.com_baudrate = GetText( "组合框_串口波特率" );
		param.com_parity = GetText( "组合框_校验方式" );
		param.com_data_bit = GetText( "组合框_数据位数" );
		param.com_stop_bit = GetText( "组合框_停止位数" );
		param.com_flow_control = "RS485";

		param.b_cache_data = true;
		param.b_net_at = true;
		param.b_show_back = true;

		param.command_password = "usr.cn";
		param.startup_message = "USR-GM3";
	}
	ShowParam( param );
	device.WriteParam( param );                                     //配置失败?///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
}

LRESULT OnApn( HWND hwnd, WORD motify_code ) {
	if( motify_code != CBN_SELCHANGE )
		return 0;
	char buf[256];
	LONG sel = SendMessage( hwnd, CB_GETCURSEL, 0, 0 );
	SendMessage( hwnd, CB_GETLBTEXT, sel, (LPARAM)buf );
//	string apn = GetText( "组合框_运营商及APN" );
	string apn = buf;
	hwnd = layout.GetHandle( "文本框_运营商及APN" );
	if( apn == "移动" ) {
		apn = "\"" + string(Operator_CMNET) + "\",\"\",\"\"";
		SetText( "文本框_运营商及APN", apn );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else if( apn == "联通" ) {
		apn = "\"" + string(Operator_UNINET) + "\",\"\",\"\"";
		SetText( "文本框_运营商及APN", apn );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else {
		apn = "\"\",\"\",\"\"";
		SetText( "文本框_运营商及APN", apn );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) & (~ES_READONLY) & (~WS_DISABLED) );
	}
	return 0;
}

void ShowParam( const SDeviceParam& param ) {
	SetCheck( "单选按钮__工作模式_网络透传", false );
	SetCheck( "单选按钮__工作模式_HTTPD", false );
	SetCheck( "单选按钮__工作模式_短信透传", false );

	//解析param
	if( param.s_work_mode == WorkMode_Cmd ) {
		return ;
	} else if ( param.s_work_mode == WorkMode_Net ) {
		SetCheck( "单选按钮__工作模式_网络透传", true );
	} else if( param.s_work_mode == WorkMode_Httpd ) {
		SetCheck( "单选按钮__工作模式_HTTPD", true );
	} else if( param.s_work_mode == WorkMode_Sms ) {
		SetCheck( "单选按钮__工作模式_短信透传", true );
	}

	//网络透传参数
	if( param.socket_param.b_serverA_enable ) {
		SetCheck( "复选框_服务器A", true );
		SetText( "文本框_服务器地址_A", param.socket_param.s_serverA_addr );
		SetText( "文本框_服务器端口_A", param.socket_param.s_serverA_port );
		SetText( "组合框_连接协议_A", param.socket_param.s_serverA_protocal );
		if( param.socket_param.s_serverA_mode == SocketMode_Long ) {
			SetWindowText( layout.GetHandle( "组合框_连接方式_A" ), "长连接" );
		} else {
			SetWindowText( layout.GetHandle( "组合框_连接方式_A" ), "短连接" );
		}
	} else {
		SetCheck( "复选框_服务器A", false );
	}

	if( param.socket_param.b_serverB_enable ) {
		SetCheck( "复选框_服务器B", true );
		SetText( "文本框_服务器地址_B", param.socket_param.s_serverB_addr );
		SetText( "文本框_服务器端口_B", param.socket_param.s_serverB_port );
		SetText( "组合框_连接协议_B", param.socket_param.s_serverB_protocal );
		if( param.socket_param.s_serverB_mode == SocketMode_Long ) {
			SetWindowText( layout.GetHandle( "组合框_连接方式_B" ), "长连接" );
		} else {
			SetWindowText( layout.GetHandle( "组合框_连接方式_B" ), "短连接" );
		}
	} else {
		SetCheck( "复选框_服务器B", false );
	}

	if( param.socket_param.b_heartbeat_enable ) {
		string buf;
		char c;
		SetCheck( "复选框_心跳包", true );
		SetText( "文本框_心跳包间隔", param.socket_param.s_heartbeat_interval );
		SetCheck( "复选框_十六进制心跳数据", true );
		SetText( "文本框_心跳包数据", param.socket_param.s_heartbeat_value );

		if( param.socket_param.s_heartbeat_type == HeartType_Com ) {
			SetWindowText( layout.GetHandle( "组合框_心跳包类型" ), "向串口发送" );
		} else {
			SetWindowText( layout.GetHandle( "组合框_心跳包类型" ), "向服务器发送" );
		}
	} else {
		SetCheck( "复选框_心跳包", false );
	}

	if( param.socket_param.b_register_enable ) {
		string buf;
		char c;
		SetCheck( "复选框_注册包", true );
		SetCheck( "复选框_十六进制注册数据", true );
		SetText( "文本框_注册包数据", param.socket_param.s_register_value );


		if( param.socket_param.s_register_type == RegisterType_ICCID ) {
			SetText( "组合框_注册包类型", "ICCID" );
		} else if( param.socket_param.s_register_type == RegisterType_IMEI ) {
			SetText( "组合框_注册包类型", "IMEI" );
		} else if( param.socket_param.s_register_type == RegisterType_REGID ) {
			SetText( "组合框_注册包类型", "注册ID" );
		} else if( param.socket_param.s_register_type == RegisterType_REGDT ) {
			SetText( "组合框_注册包类型", "自定义数据" );
		}

		if( param.socket_param.s_register_send == RegisterSend_Link ) {
			SetText( "组合框_注册包发送方式", "连接建立时发送一次" );
		} else if( param.socket_param.s_register_send == RegisterSend_Data ) {
			SetText( "组合框_注册包发送方式", "每次发送指令之前发送" );
		} else if( param.socket_param.s_register_send == RegisterSend_Both ) {
			SetText( "组合框_注册包发送方式", "同时启用以上两种" );
		}

	} else {
		SetCheck( "复选框_注册包类型", false );
	}

	if( param.socket_param.b_cloud_untr ) {
		SetCheck( "复选框_透传云", true );
		SetText( "文本框_设备编号", param.socket_param.s_cloud_devnum );
		SetText( "文本框_通讯密码", param.socket_param.s_cloud_password );
	} else {
		SetCheck( "复选框_透传云", false );
	}

	//设置HTTPD参数
	SetText( "组合框_HTTPD方法", param.httpd_param.s_http_method );
	SetText( "文本框_HTTPDURL", param.httpd_param.s_http_url );
	SetText( "文本框_HTTPD地址", param.httpd_param.s_server_addr );
	SetText( "文本框_HTTPD端口", param.httpd_param.s_server_port );
	SetText( "文本框_HTTPD头信息", param.httpd_param.s_http_header );

	//设置短信透传参数
	SetText( "文本框_目标电话号", param.message_param.s_phone_number );

	//设置全局参数
	if( param.operator_apn == Operator_CMNET ) {
		SetText( "组合框_运营商及APN", "移动" );
		SetWindowLong( layout.GetHandle( "文本框_运营商及APN" ), GWL_STYLE, GetWindowLong( layout.GetHandle( "文本框_运营商及APN" ), GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else if( param.operator_apn == Operator_UNINET ) {
		SetText( "组合框_运营商及APN", "联通" );
		SetWindowLong( layout.GetHandle( "文本框_运营商及APN" ), GWL_STYLE, GetWindowLong( layout.GetHandle( "文本框_运营商及APN" ), GWL_STYLE ) | ES_READONLY | WS_DISABLED );
	} else {
		SetText( "组合框_运营商及APN", "其他" );
		SetWindowLong( layout.GetHandle( "文本框_运营商及APN" ), GWL_STYLE, GetWindowLong( layout.GetHandle( "文本框_运营商及APN" ), GWL_STYLE )  & (~ES_READONLY) & (~WS_DISABLED) );
	}
	string tempstr("\"" + param.operator_apn + "\",\"" + param.operator_user + "\",\"" + param.operator_password + "\"");
	SetText( "文本框_运营商及APN", tempstr );
	SetText( "组合框_设备_波特率", param.com_baudrate );
	SetText( "组合框_设备_流控", param.com_flow_control );
	SetText( "组合框_设备_校验", param.com_parity );
	SetText( "组合框_设备_数据位数", param.com_data_bit );
	SetText( "组合框_设备_停止位数", param.com_stop_bit );


	SetCheck( "复选框_指令回显", param.b_show_back );
	SetCheck( "复选框_类RFC2217", param.b_RFC2217_like );
	SetCheck( "复选框_通话功能", param.b_telephone );
	SetCheck( "复选框_串口AT指令", param.b_usart_at );
	SetCheck( "复选框_网络AT指令", param.b_net_at );
	SetCheck( "复选框_允许缓存", param.b_cache_data );

	SetText( "文本框_命令密码", param.command_password );
	SetText( "文本框_启动信息", param.startup_message );

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
	//读取界面信息,组成参数
	if( GetCheck( "单选按钮__工作模式_网络透传" ) )
		param.s_work_mode = WorkMode_Net;
	else if( GetCheck( "单选按钮__工作模式_HTTPD" ) )
		param.s_work_mode = WorkMode_Httpd;
	else if( GetCheck( "单选按钮__工作模式_短信透传" ) )
		param.s_work_mode = WorkMode_Sms;

	if( param.s_work_mode == WorkMode_Net ) {
		param.socket_param.b_serverA_enable = GetCheck( "复选框_服务器A" );
		if( param.socket_param.b_serverA_enable ) {
			param.socket_param.s_serverA_addr = GetText( "文本框_服务器地址_A" );
			param.socket_param.s_serverA_port = GetText( "文本框_服务器端口_A" );
			param.socket_param.s_serverA_protocal = GetText( "组合框_连接协议_A" );
			if( GetText( "组合框_连接方式_A" ) == "短连接" )
			    param.socket_param.s_serverA_mode = SocketMode_Short;
			else
				param.socket_param.s_serverA_mode = SocketMode_Long;
		}

		param.socket_param.b_serverB_enable = GetCheck( "复选框_服务器B" );
		if( param.socket_param.b_serverB_enable ) {
			param.socket_param.s_serverB_addr = GetText( "文本框_服务器地址_B" );
			param.socket_param.s_serverB_port = GetText( "文本框_服务器端口_B" );
			param.socket_param.s_serverB_protocal = GetText( "组合框_连接协议_B" );
			if( GetText( "组合框_连接方式_B" ) == "短连接" )
			    param.socket_param.s_serverB_mode = SocketMode_Short;
			else
				param.socket_param.s_serverB_mode = SocketMode_Long;
		}

		param.socket_param.b_heartbeat_enable = GetCheck( "复选框_心跳包" );
		if( param.socket_param.b_heartbeat_enable ) {
			param.socket_param.s_heartbeat_interval = GetText( "文本框_心跳包间隔" );
			if( GetText( "组合框_心跳包类型" ) == "向串口发送" )
				param.socket_param.s_heartbeat_type = HeartType_Com;
			else
				param.socket_param.s_heartbeat_type = HeartType_Net;
			param.socket_param.s_heartbeat_value = GetText( "文本框_心跳包数据" );
			if( !GetCheck( "复选框_十六进制心跳数据" ) )
				Text2Hex( param.socket_param.s_heartbeat_value );
		}

		param.socket_param.b_register_enable = GetCheck( "复选框_注册包" );
		if( param.socket_param.b_register_enable ) {
			arg_s = GetText( "组合框_注册包发送方式" );
			if( arg_s == "连接建立时发送一次" )
				param.socket_param.s_register_send = RegisterSend_Link;
			else if( arg_s == "每次发送指令之前发送" )
			    param.socket_param.s_register_send = RegisterSend_Data;
			else
				param.socket_param.s_register_send = RegisterSend_Both;
			arg_s = GetText( "组合框_注册包类型" );
			if( arg_s == "ICCID" )
				param.socket_param.s_register_type = RegisterType_ICCID;
			else if( arg_s == "IMEI" )
				param.socket_param.s_register_type = RegisterType_IMEI;
			else if( arg_s == "注册ID" )
				param.socket_param.s_register_type = RegisterType_REGID;
			else
				param.socket_param.s_register_type = RegisterType_REGDT;
			param.socket_param.s_register_value = GetText( "文本框_注册包内容" );
			if( !GetCheck( "复选框_十六进制注册数据" ) )
			    Text2Hex( param.socket_param.s_register_value );
		}

		param.socket_param.b_cloud_untr = GetCheck( "复选框_透传云" );
		if( param.socket_param.b_cloud_untr ) {
			param.socket_param.s_cloud_devnum = GetText( "文本框_设备编号" );
			param.socket_param.s_cloud_password = GetText( "文本框_通讯密码" );
		}
	} else if( param.s_work_mode == WorkMode_Httpd ) {
	    param.httpd_param.s_http_header = GetText( "文本框_HTTPD请求头信息" );
	    param.httpd_param.s_http_method = GetText( "组合框_HTTPD方法" );
	    param.httpd_param.s_http_url = GetText( "文本框_HTTPD_URL" );
	    param.httpd_param.s_server_addr = GetText( "文本框_HTTPD服务器地址" );
	    param.httpd_param.s_server_port = GetText( "文本框_HTTPD服务器端口" );
	} else if( param.s_work_mode == WorkMode_Sms ) {
    	param.message_param.s_phone_number = GetText( "文本框_目标电话号" );
	}


	arg_s = GetText( "文本框_运营商及APN" );
	memset( buffer, 0, 512 );
	strcpy( buffer, arg_s.data() );
	temp = CutArg( buffer );
	sscanf( temp, "%s%s%s", arg_b, arg_c, arg_d );
	param.operator_apn = arg_b;
	param.operator_user = arg_c;
	param.operator_password = arg_d;

	param.com_baudrate = GetText( "组合框_设备_波特率" );
	param.com_parity = GetText( "组合框_设备_校验" );
	param.com_data_bit = GetText( "组合框_设备_数据位数" );
	param.com_stop_bit = GetText( "组合框_设备_停止位数" );
	param.com_flow_control = GetText( "组合框_设备_流控" );

	param.b_show_back = GetCheck( "复选框_指令回显" );
	param.b_RFC2217_like = GetCheck( "复选框_类RFC2217" );
	param.b_telephone = GetCheck( "复选框_通话功能" );
	param.b_usart_at = GetCheck( "复选框_串口AT指令" );
	param.b_net_at = GetCheck( "复选框_网络AT指令" );
	param.b_cache_data = GetCheck( "复选框_允许缓存" );

	param.command_password = GetText( "文本框_命令密码" );
	param.startup_message = GetText( "文本框_启动信息" );
}

LRESULT OnSendCmd( HWND hwnd, WORD motify_code ) {
	if( motify_code != BN_CLICKED )
		return 0;
	string cmdline = GetText( "文本框_手动指令" );
	if( GetCheck( "复选框_自动补全" ) ) {
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

