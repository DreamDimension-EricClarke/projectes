#ifndef __Device_CPP__
#define __Device_CPP__


#include "Device.h"
#include "Layout.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#if __DEBUG__ || _DEBUG
#include <iostream>
using namespace std;
#endif

/*
构造函数用于创建一个设备抽象实例
#####################################################
参数:
	int comm_num	创建设备同时请求打开的串口号码,
					若传入-1(默认值)则不打开任何串口
					若打开串口失败,设备被直接创建可
					以在设备创建之后使用OpenComm打开
					串口
	-------------------------------------------------
	DCB* comm_arg   串口的配置结构体指针,若空置,则串
	                口只打开,不配置,若设置,将尝试配置
	                串口,并将最新读取到的串口参数写入
	                原地址
=====================================================
注意:
	comm_arg中,只有波特率,校验形式,数据位数,停止位数
	有效,其他参数会被忽略 BuildCommDCB
*/
CDevice::CDevice( int comm_num, DCB* comm_arg ):
	m_comm_handle(INVALID_HANDLE_VALUE), m_brun(false) {
	if( comm_num < 0 )
		return;
	/*若串口号码有效则尝试打开串口*/
	OpenComm( comm_num, comm_arg );
}

/*
析构函数用于关闭串口连接,清除已使用的内存空间
#####################################################
*/
CDevice::~CDevice( void ) {
	/*无论如何尝试断开串口连接*/
	CloseComm();
}

/*
OpenComm函数用于打开指定端口号码对应的端口
#####################################################
参数:
	int comm_num    即将要打开的串口的号码
	-------------------------------------------------
	DCB* comm_arg   串口的配置结构体指针,若空置,则串
	                口只打开,不配置,若设置,将尝试配置
	                串口,并将最新读取到的串口参数写入
	                原地址
=====================================================
返回值:
	bool            若打开成功,返回true
	                若打开失败,返回false
=====================================================
注意:
	comm_arg中,只有波特率,数据位数,停止位数
	有效,其他参数会被忽略 BuildCommDCB
*/
bool CDevice::OpenComm( int comm_num, DCB* comm_arg ) {
	if( comm_num < 0 or comm_arg == 0 )
		return false;
	//如果串口已经被打开了,则直接反馈成功
	if( m_comm_handle != INVALID_HANDLE_VALUE )
		return true;
	/*构造串口名称*/
	char comm_name[64];
	COMMTIMEOUTS comm_timeout;

	/*构造串口名称*/
	sprintf( comm_name, "COM%d", comm_num );

	/*尝试打开串口*/
	m_comm_handle = CreateFile(
	                    comm_name,                          //串口名字
	                    GENERIC_READ | GENERIC_WRITE,       //通用的读写许可
	                    0,
	                    0,
	                    OPEN_EXISTING,                      //只打开已经存在的文件
	                    0,
	                    0
	                );
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;                             //如果打开失败,接下来的步骤将被跳过

	/*配置串口超时参数*/
	comm_timeout.ReadIntervalTimeout = 200;
	comm_timeout.ReadTotalTimeoutConstant = 2000;
	comm_timeout.ReadTotalTimeoutMultiplier = 0;
	comm_timeout.WriteTotalTimeoutConstant = 2000;
	comm_timeout.WriteTotalTimeoutMultiplier = 0;

	/*设置串口超时配置*/
	if( !SetCommTimeouts( m_comm_handle, &comm_timeout ) ) {
		CloseHandle( m_comm_handle );
		m_comm_handle = INVALID_HANDLE_VALUE;
		return false;
	}

	/*设置串口配置*/
	if( !SetCommState( m_comm_handle, comm_arg ) ) {
		CloseHandle( m_comm_handle );
		m_comm_handle = INVALID_HANDLE_VALUE;
		return false;
	}
	
	/*开始监听*/
	m_brun = true;
	//CreateThread( 0, 0, ListenProc, this, 0, 0 );
	return true;
}

/*
CloseComm函数用于关闭串口连接
#####################################################
返回值:
    bool            若串口成功关闭,则返回true
                    若串口未打开或关闭失败,返回false
*/
bool CDevice::CloseComm( void ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	m_brun = false;
	CloseHandle( m_comm_handle );
	m_comm_handle = INVALID_HANDLE_VALUE;
	return true;
}

/*
IsCommOpen函数获取串口状态
#####################################################
返回值:
	bool            若串口已打开则返回true
                    反之返回false
*/
bool CDevice::IsCommOpen( void ) {
	return m_comm_handle != INVALID_HANDLE_VALUE;
}

/*
QueryParam函数从串口请求所有参数,保存到类成员变量中
#####################################################
参数:
	SDeviceParam&   设备参数的引用,用于接收请求来的数
	                据
=====================================================
返回值:
    bool            若请求成功,则返回true
                    若请求失败,则返回false
*/
bool CDevice::QueryParam( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	GetWorkMode( param );

	if( param.s_work_mode == WorkMode_Cmd ) {
		return true;
	} else if( param.s_work_mode == WorkMode_Net ) {
		if( !GetSockParam( param ) ) {
			return false;
		}
	} else if( param.s_work_mode == WorkMode_Httpd ) {
		if( !GetHTTPDParam( param ) ) {
			return false;
		}
	} else if( param.s_work_mode == WorkMode_Sms ) {
		if( !GetMessageParam( param ) ) {
			return false;
		}
	}
	
	if( !GetGlobalParam( param ) ) {
		return false;
	}
	return true;
}

/*
WriteParam函数用于通过串口将参数配置到设备上
#####################################################
参数:
    const SDeviceParam&     即将设置进入设备的配置信
							息
=====================================================
返回值:
	bool            		若成功,返回true
                    		若失败返回false
*/
bool CDevice::WriteParam( const SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	
	if( !SetWorkMode( param ) )
		return false;
	if( param.s_work_mode == WorkMode_Net ) {
		if( !SetSockParam( param ) )
			return false;
	} else if( param.s_work_mode == WorkMode_Httpd ) {
		if( !SetHTTPDParam( param ) )
			return false;
	} else if( param.s_work_mode == WorkMode_Sms ) {
		if( !SetMessageParam( param ) )
			return false;
	}
	
	if( !SetGlobalParam( param ) )
		return false;

	string cmdline = "AT+S\r\n";
    Command( cmdline );
    cmdline.clear();
    Sleep( 10 );
    Command( cmdline );
    if( !strstr( cmdline.data(), "OK" ) )
		return false;
	return true;
}

/*SetDeviceState函数用于选择设备当前的工作模式`
#####################################################
参数:
	EDeviceState state 设置过程的目标状态
=====================================================
返回值:
	bool            若成功,返回true
	                若失败,返回false
*/
bool CDevice::SetDeviceState( EDeviceState state ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	string cmdline;
	switch( state ) {
	case DeviceState_Communicate:
		cmdline = "AT+ENTM\r";
		Command( cmdline );
		if( cmdline.find( "+ok" ) == string::npos )
			return false;
		return true;
	case DeviceState_Config:
		Sleep( 1 );
		cmdline = "+++";
		Command( cmdline );
		if( cmdline.find( "a" ) == string::npos ) {
			if( cmdline.find( "+++" ) == string::npos )
				return false;
			return true;
		}
		cmdline = "a";
		Command( cmdline );
		if( cmdline.find( "+ok" ) == string::npos )
			return false;
		return true;
	}
}

/*Command函数用于进行一次串口互动
#####################################################
参数:
	strng& cmd      cmd是要发送的字符串,同时也用来接
	                收串口的反馈信息
	                若cmd为空字符串,则串口不发送指令,
	                只等待一次接收
=====================================================
返回值:
	int             返回接收到的反馈的数据量
=====================================================
注意:
    此方法可能因反馈中存在0字节而导致反馈数据被截断,
    若有必要避免此情况,请使用另外的重载函数
*/
int CDevice::Command( string& cmd ) {
	DWORD dw_number;
	WINBOOL bRead;
	char *s_buffer = new char[128];
	memset( s_buffer, 0, 128 );
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return 0;
	if( cmd.length() != 0 ) {
		layout.Log( "上位机:" + cmd );
		WriteFile( m_comm_handle, cmd.data(), cmd.length(), &dw_number, 0 );
	}
	dw_number = 0;
	bRead = ReadFile( m_comm_handle, s_buffer, 128, &dw_number, 0 );
	if( !bRead or dw_number == 0 ) {
		layout.Log( "下位机未响应!" );
		cmd = "";
		return 0;
	}
	cmd = s_buffer;
	layout.Log( "下位机:" + cmd );
	delete[] s_buffer;
	return dw_number;
}

/*Command函数用于进行一次串口互动
#####################################################
参数:
	char* cmd		cmd是要发送的指令的缓冲,同时也用
	                来接收串口的反馈信息
	-------------------------------------------------
	int buflen      缓冲区总大小,标示了接收指令时可用
	                的缓冲区大小,若为0 表示与cmdlen等
	                大小
	-------------------------------------------------
	int cmdlen      指令的字节长度,标示了即将发送的指
	                令的长度
					若为0表示不发送指令,只等
	                待一个反馈
                    若为-1表示cmd是一个字符串,也就是
                    说指令内容包括在第一个'\0'出现之
                    前的所有内容,此时若第一字节就是0
                    则也不会发送任何指令,而等待一个
                    反馈的到来
=====================================================
返回值:
	int             返回从串口接收到的反馈的字节数量
*/
int CDevice::Command( char* cmd, int buflen, int cmdlen  ) {
	DWORD dw_number;
	WINBOOL bRead;
	if( m_comm_handle == INVALID_HANDLE_VALUE or cmd == 0 )
		return 0;
	if( cmdlen != 0 )
		if( cmdlen == -1 ) {
			layout.Log( "上位机:" + string(cmd) );
			WriteFile( m_comm_handle, cmd, strlen(cmd), &dw_number, 0 );
		}
		else {
			string line = "上位机(Hex):";
			for( int i = 0; i < cmdlen; i++ ) {
				line += cmd[i]>>4 + ((cmd[i]>>4)>=10?'A':'0');
				line += cmd[i]&0x0f + ((cmd[i]&0x0f)>=10?'A':'0');
			}
			layout.Log( line );
			WriteFile( m_comm_handle, cmd, cmdlen, &dw_number, 0 );
		}
	dw_number = 0;
	memset( cmd, 0, buflen );
	if( buflen == 0 )
		buflen = strlen( cmd );
	bRead = ReadFile( m_comm_handle, cmd, buflen, &dw_number, 0 );
	if( !bRead or dw_number == 0 ) {
		layout.Log( "下位机未响应!" );
		memset( cmd, 0, buflen );
		return 0;
	}
	layout.Log( "下位机" + string(cmd) );
	return dw_number;
}

/*GetWorkMode用于获取工作模式
#####################################################
参数:
	SDeviceParam& param 用于接收工作模式的引
	                    用
=====================================================
返回值:
	bool                若获取成功,则返回true
	            		否则返回false且
	                    workmode内容无效
*/
bool CDevice::GetWorkMode( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[128] = "AT+WKMOD?\r\n";
	char *temp = 0;
	Command( cmdline, 128 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	if( sscanf( temp, "%s", cmdline+64 ) < 1 )
		return false;
	param.s_work_mode = string( cmdline+64 );
	return true;
}

/*SetWorkMode用于设置工作模式
#####################################################
参数:
	const SDeviceParam param  将要设置的目标工作模式
=====================================================
返回值:
	bool                若设置成功则返回true
	                    若设置失败则返回false
*/
bool CDevice::SetWorkMode( const SDeviceParam& param ) {
	string cmdline = "AT+WKMOD=\"" + param.s_work_mode + "\"\r\n";
	Command( cmdline );
	if( !strstr( cmdline.data(), "OK" ) )
		return false;
	return true;
}

/*GetSockParam用于获取网络透传参数
#####################################################
参数:
	SDeviceParam&       用于接收参数的引用
=====================================================
返回值:
	bool                若没有任何错误则返回true
	                    若出现了错误则返回false
=====================================================
注意:
	param中包含的所有其他数据在任何情况都不会受到修改
*/
bool CDevice::GetSockParam( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	const char *arg_a = cmdline+128;
	const char *arg_b = cmdline+256;
	const char *arg_c = cmdline+384;
	char *temp = 0;

	//查询A服务器是否被启用了
	sprintf( cmdline, "AT+SOCKAEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )      //如果在反馈数据中没有找到OK说明查询过程中遇到了错误
	    return false;
	if( strstr( cmdline, "on" ) ) {     //如果在反馈数据中找到了on字样,则表示服务器A被启用了

		param.socket_param.b_serverA_enable = true;

		sprintf( cmdline, "AT+SOCKA?\r\n" ); //查询服务器A的相关配置
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		if( sscanf( temp, "%s%s%s", arg_a, arg_b, arg_c ) < 3 )
			return false;
		param.socket_param.s_serverA_protocal = string(arg_a);
		param.socket_param.s_serverA_addr = string(arg_b);
		param.socket_param.s_serverA_port = string(arg_c);
		
		if( param.socket_param.s_serverA_protocal == SocketProtocal_TCP ) {
			sprintf( cmdline, "AT+SOCKASL?\r\n" );
			Command( cmdline, 512 );
			if( !strstr( cmdline, "OK" ) )
				return false;
			temp = CutArg( cmdline );
			if( sscanf( temp, "%s", arg_a ) < 1 )
				return false;
			param.socket_param.s_serverA_mode = string(arg_a);
		}
	} else {
		param.socket_param.b_serverA_enable = false;
	}
	
	//查询B服务器是否被启用了
	sprintf( cmdline, "AT+SOCKBEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )      //如果在反馈数据中没有找到OK说明查询过程中遇到了错误
	    return false;
	if( strstr( cmdline, "on" ) ) {     //如果在反馈数据中找到了on字样,则表示服务器B被启用了

		param.socket_param.b_serverB_enable = true;

		sprintf( cmdline, "AT+SOCKB?\r\n" ); //查询服务器B的相关配置
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		if( sscanf( temp, "%s%s%s", arg_a, arg_b, arg_c ) < 3 )
			return false;
		param.socket_param.s_serverB_protocal = string(arg_a);
		param.socket_param.s_serverB_addr = string(arg_b);
		param.socket_param.s_serverB_port = string(arg_c);

		if( param.socket_param.s_serverB_protocal == SocketProtocal_TCP ) {
			sprintf( cmdline, "AT+SOCKBSL?\r\n" );
			Command( cmdline, 512 );
			if( !strstr( cmdline, "OK" ) )
				return false;
			temp = CutArg( cmdline );
			if( sscanf( temp, "%s", arg_a ) < 1 )
				return false;
			param.socket_param.s_serverB_mode = string(arg_a);
		}
	} else {
		param.socket_param.b_serverB_enable = false;
	}
	
	//查询心跳包是否开启
	sprintf( cmdline, "AT+HEARTEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) ) {
		
		param.socket_param.b_heartbeat_enable = true;
		
		sprintf( cmdline, "AT+HEARTDT?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( temp, "%s", arg_a );
		param.socket_param.s_heartbeat_value = string(arg_a);
		
		sprintf( cmdline, "AT+HEARTTP?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( temp, "%s", arg_a );
		param.socket_param.s_heartbeat_type = string(arg_a);
		
		sprintf( cmdline, "AT+HEARTTM?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( temp, "%s", arg_a );
		param.socket_param.s_heartbeat_interval = string(arg_a);
	} else {
		param.socket_param.b_heartbeat_enable = false;
	}
	
	//查询注册包是否开启
	sprintf( cmdline, "AT+REGEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) ) {
		param.socket_param.b_register_enable = true;
		
		sprintf( cmdline, "AT+REGSND?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( temp, "%s", arg_a );
		param.socket_param.s_register_send = string( arg_a );
		
		sprintf( cmdline, "AT+REGTP?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( temp, "%s", arg_a );
		param.socket_param.s_register_type = string( arg_a );

		if( param.socket_param.s_register_type == RegisterType_ICCID ) {
			sprintf( cmdline, "AT+ICCID?\r\n" );
		} else if( param.socket_param.s_register_type == RegisterType_IMEI ) {
			sprintf( cmdline, "AT+IMEI?\r\n" );
		} else if( param.socket_param.s_register_type == RegisterType_REGID ) {
			sprintf( cmdline, "AT+REGID?\r\n" );
		} else if( param.socket_param.s_register_type == RegisterType_REGDT ) {
			sprintf( cmdline, "AT+REGDT?\r\n" );
		}
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( temp, "%s", arg_a );
		param.socket_param.s_register_value = string( arg_a );
		
	} else {
		param.socket_param.b_register_enable = false;
	}
	
	//检查是否启用了云透传
	sprintf( cmdline, "AT+CLOUDEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) ) {
		param.socket_param.b_cloud_untr = true;
		
		sprintf( cmdline, "AT+CLOUDID?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( cmdline, "%s", arg_a );
		param.socket_param.s_cloud_devnum = string( arg_a );

		sprintf( cmdline, "AT+CLOUDPA?\r\n" );
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
		temp = CutArg( cmdline );
		sscanf( cmdline, "%s", arg_a );
		param.socket_param.s_cloud_password = string( arg_a );
	} else {
		param.socket_param.b_cloud_untr = false;
	}
	return true;
}


/*SetSockParam用于设置网络透传模式的参数
#####################################################
参数:
	const SDeviceParam param  将要设置的目标工作模式
=====================================================
返回值:
	bool                若设置成功则返回true
	                    若设置失败则返回false
*/
bool CDevice::SetSockParam( const SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	string arg_a;
	
	//服务器A
	if( param.socket_param.b_serverA_enable ) {
		arg_a = "on";
	}
	else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+SOCKAEN=\"%s\"\r\n", arg_a.data() );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	if( param.socket_param.b_serverA_enable ) {
		sprintf( cmdline, "AT+SOCKA=\"%s\",\"%s\",%s\r\n",
			param.socket_param.s_serverA_protocal.data(),
			param.socket_param.s_serverA_addr.data(),
			param.socket_param.s_serverA_port.data()
		);
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;

		sprintf( cmdline, "AT+SOCKASL=\"%s\"\r\n",
			param.socket_param.s_serverA_mode.data()
		);
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
	}
	
	//服务器B
	if( param.socket_param.b_serverB_enable ) {
		arg_a = "on";
	}
	else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+SOCKBEN=\"%s\"\r\n",
	arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	if( param.socket_param.b_serverB_enable ) {
		sprintf( cmdline, "AT+SOCKB=\"%s\",\"%s\",%s\r\n",
			param.socket_param.s_serverB_protocal.data(),
			param.socket_param.s_serverB_addr.data(),
			param.socket_param.s_serverB_port.data()
		);
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;

		sprintf( cmdline, "AT+SOCKBSL=\"%s\"\r\n",
			param.socket_param.s_serverB_mode.data()
		);
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
	}
	
	//心跳包
	if( param.socket_param.b_heartbeat_enable ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+HEARTEN=\"%s\"\r\n",
        arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	sprintf( cmdline, "AT+HEARTTM=%s\r\n",
		param.socket_param.s_heartbeat_interval.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	sprintf( cmdline, "AT+HEARTTP=\"%s\"\r\n",
		param.socket_param.s_heartbeat_type.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	sprintf( cmdline, "AT+HEARTDT=\"%s\"\r\n",
		param.socket_param.s_heartbeat_value.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	//注册包
	if( param.socket_param.b_register_enable ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+REGEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	sprintf( cmdline, "AT+REGTP=\"%s\"\r\n",
		param.socket_param.s_register_type.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	sprintf( cmdline, "AT+REGSND=\"%s\"\r\n",
		param.socket_param.s_register_send.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	sprintf( cmdline, "AT+REGDT=\"%s\"\r\n",
		param.socket_param.s_register_value.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	//云透传
	if( param.socket_param.b_cloud_untr ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+CLOUDEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( param.socket_param.b_cloud_untr ) {
		sprintf( cmdline, "AT+CLOUDID=\"%s\"\r\n",
			param.socket_param.s_cloud_devnum.data()
		);
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;

		sprintf( cmdline, "AT+CLOUDPA=\"%s\"\r\n",
			param.socket_param.s_cloud_password.data()
		);
		Command( cmdline, 512 );
		if( !strstr( cmdline, "OK" ) )
			return false;
	}

	return true;
}


/*GetHTTPDParam用于获取HTTPD参数
#####################################################
参数:
	SDeviceParam&       用于接收参数的引用
=====================================================
返回值:
	bool                若没有任何错误则返回true
	                    若出现了错误则返回false
=====================================================
注意:
	param中包含的所有其他数据在任何情况都不会受到修改
*/
bool CDevice::GetHTTPDParam( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	const char *arg_a = cmdline+128;
	const char *arg_b = cmdline+256;
	const char *arg_c = cmdline+384;
	char *temp = 0;

	sprintf( cmdline, "AT+HTPTP?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s", arg_a );
	param.httpd_param.s_http_method = string( arg_a );

	sprintf( cmdline, "AT+HTPURL?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s", arg_a );
	param.httpd_param.s_http_url = string( arg_a );

	sprintf( cmdline, "AT+HTPSV?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s%s", arg_a, arg_b );
	param.httpd_param.s_server_addr = string( arg_a );
	param.httpd_param.s_server_port = string( arg_b );

	sprintf( cmdline, "AT+HTPHD?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s", arg_a );
	param.httpd_param.s_http_header = string( arg_a );

	return true;
}


/*SetSockParam用于设置网络透传模式的参数
#####################################################
参数:
	const SDeviceParam param  将要设置的目标工作模式
=====================================================
返回值:
	bool                若设置成功则返回true
	                    若设置失败则返回false
*/
bool CDevice::SetHTTPDParam( const SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	string arg_a;
	
	sprintf( cmdline, "AT+HTPTP=\"%s\"\r\n",
		param.httpd_param.s_http_method.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	sprintf( cmdline, "AT+HTPURL=\"%s\"\r\n",
		param.httpd_param.s_http_url.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	sprintf( cmdline, "AT+HTPSV=\"%s\",%s\r\n",
		param.httpd_param.s_server_addr.data(),
		param.httpd_param.s_server_port.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	sprintf( cmdline, "AT+HTPHD=\"%s\"\r\n",
		param.httpd_param.s_http_header.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	return true;
}


/*GetMessageParam用于获取短信透传参数
#####################################################
参数:
	SDeviceParam&       用于接收参数的引用
=====================================================
返回值:
	bool                若没有任何错误则返回true
	                    若出现了错误则返回false
=====================================================
注意:
	param中包含的所有其他数据在任何情况都不会受到修改
*/
bool CDevice::GetMessageParam( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	const char *arg_a = cmdline+128;
	const char *arg_b = cmdline+256;
	const char *arg_c = cmdline+384;
	char *temp = 0;
	
	sprintf( cmdline, "AT+DSTNUM?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s", arg_a );
	param.message_param.s_phone_number = string( arg_a );
	return true;
}


/*SetSockParam用于设置网络透传模式的参数
#####################################################
参数:
	const SDeviceParam param  将要设置的目标工作模式
=====================================================
返回值:
	bool                若设置成功则返回true
	                    若设置失败则返回false
*/
bool CDevice::SetMessageParam( const SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	sprintf( cmdline, "AT+DSTNUM=\"%s\"\r\n",
		param.message_param.s_phone_number.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	return true;
}


/*GetGlobalParam用于获取全局参数
#####################################################
参数:
	SDeviceParam&       用于接收参数的引用
=====================================================
返回值:
	bool                若没有任何错误则返回true
	                    若出现了错误则返回false
=====================================================
注意:
	param中包含的所有其他数据在任何情况都不会受到修改
*/
bool CDevice::GetGlobalParam( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[1024];
	const char *arg_a = cmdline+128;
	const char *arg_b = cmdline+256;
	const char *arg_c = cmdline+384;
	const char *arg_d = cmdline+512;
	const char *arg_e = cmdline+640;
	char *temp = 0;
	
	//查询APN及账户密码
	sprintf( cmdline, "AT+APN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s%s%s", arg_a, arg_b, arg_c );
	param.operator_apn = string( arg_a );
	param.operator_user = string( arg_b );
	param.operator_password = string( arg_c );
	
	//查询串口参数
	sprintf( cmdline, "AT+UART?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s%s%s%s%s", arg_a, arg_b, arg_c, arg_d, arg_e );
	param.com_baudrate = string( arg_a );
	param.com_parity = string( arg_b );
	param.com_data_bit = string( arg_c );
	param.com_stop_bit = string( arg_d );
	param.com_flow_control = string( arg_e );
	
	//查询是否回显
	sprintf( cmdline, "AT+E?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_show_back = true;
	else
		param.b_show_back = false;
		
	//查询是否类RFC2217
	sprintf( cmdline, "AT+RFCEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_RFC2217_like = true;
	else
		param.b_RFC2217_like = false;
		
	//查询是否通话功能
	sprintf( cmdline, "AT+CALEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_telephone = true;
	else
		param.b_telephone = false;
		
	//查询是否串口AT指令
	sprintf( cmdline, "AT+UATEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_usart_at = true;
	else
		param.b_usart_at = false;

	//查询是否网络AT指令
	sprintf( cmdline, "AT+NATEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_net_at = true;
	else
		param.b_net_at = false;
		
	//查询是否允许缓存数据
	sprintf( cmdline, "AT+CACHEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_cache_data = true;
	else
		param.b_cache_data = false;
		
	sprintf( cmdline, "AT+CMDPW?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s", arg_a );
	param.command_password = string(arg_a);
	
	sprintf( cmdline, "AT+STMSG?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s", arg_a );
	param.startup_message = string(arg_a);
	
	return true;
}


/*SetSockParam用于设置网络透传模式的参数
#####################################################
参数:
	const SDeviceParam param  将要设置的目标工作模式
=====================================================
返回值:
	bool                若设置成功则返回true
	                    若设置失败则返回false
*/
bool CDevice::SetGlobalParam( const SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	string arg_a;

	sprintf( cmdline, "AT+APN=\"%s\",\"%s\",\"%s\"\r\n",
		param.operator_apn.data(),
		param.operator_user.data(),
		param.operator_password.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	sprintf( cmdline, "AT+UART=%s,\"%s\",%s,%s,\"%s\"\r\n",
		param.com_baudrate.data(),
		param.com_parity.data(),
		param.com_data_bit.data(),
		param.com_stop_bit.data(),
		param.com_flow_control.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	if( param.b_show_back ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+E=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	if( param.b_RFC2217_like ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+RFCEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	if( param.b_telephone ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+CALEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	if( param.b_usart_at ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+UATEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	if( param.b_net_at ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+NATEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
		
	if( param.b_cache_data ) {
		arg_a = "on";
	} else {
		arg_a = "off";
	}
	sprintf( cmdline, "AT+CACHEN=\"%s\"\r\n",
		arg_a.data()
	);
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	sprintf( cmdline, "AT+STMSG=\"%s\"\r\n", param.startup_message.data() );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;

	sprintf( cmdline, "AT+CMDPW=\"%s\"\r\n", param.command_password.data() );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	return true;
}

DWORD WINAPI ListenProc( LPVOID p ) {
	WINBOOL brecv;
	char buffer[4096];
	char *ptr = 0;
	DWORD dwRead;
	while( device.m_brun ) {
		brecv = ReadFile( device.m_comm_handle, buffer, 4096, &dwRead, 0 );
		if( brecv and dwRead > 0 ) {
			layout.Log( ptr );
			if( brecv ) {
				ptr = new char[dwRead + 1];
				ptr[dwRead] = 0;
				memcpy( ptr, buffer, dwRead );
				combuf.Push( ptr );
			}
		}
	}
	return 0;
}

#endif
