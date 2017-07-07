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
���캯�����ڴ���һ���豸����ʵ��
#####################################################
����:
	int comm_num	�����豸ͬʱ����򿪵Ĵ��ں���,
					������-1(Ĭ��ֵ)�򲻴��κδ���
					���򿪴���ʧ��,�豸��ֱ�Ӵ�����
					�����豸����֮��ʹ��OpenComm��
					����
	-------------------------------------------------
	DCB* comm_arg   ���ڵ����ýṹ��ָ��,������,��
	                ��ֻ��,������,������,����������
	                ����,�������¶�ȡ���Ĵ��ڲ���д��
	                ԭ��ַ
=====================================================
ע��:
	comm_arg��,ֻ�в�����,У����ʽ,����λ��,ֹͣλ��
	��Ч,���������ᱻ���� BuildCommDCB
*/
CDevice::CDevice( int comm_num, DCB* comm_arg ):
	m_comm_handle(INVALID_HANDLE_VALUE), m_brun(false) {
	if( comm_num < 0 )
		return;
	/*�����ں�����Ч���Դ򿪴���*/
	OpenComm( comm_num, comm_arg );
}

/*
�����������ڹرմ�������,�����ʹ�õ��ڴ�ռ�
#####################################################
*/
CDevice::~CDevice( void ) {
	/*������γ��ԶϿ���������*/
	CloseComm();
}

/*
OpenComm�������ڴ�ָ���˿ں����Ӧ�Ķ˿�
#####################################################
����:
	int comm_num    ����Ҫ�򿪵Ĵ��ڵĺ���
	-------------------------------------------------
	DCB* comm_arg   ���ڵ����ýṹ��ָ��,������,��
	                ��ֻ��,������,������,����������
	                ����,�������¶�ȡ���Ĵ��ڲ���д��
	                ԭ��ַ
=====================================================
����ֵ:
	bool            ���򿪳ɹ�,����true
	                ����ʧ��,����false
=====================================================
ע��:
	comm_arg��,ֻ�в�����,����λ��,ֹͣλ��
	��Ч,���������ᱻ���� BuildCommDCB
*/
bool CDevice::OpenComm( int comm_num, DCB* comm_arg ) {
	if( comm_num < 0 or comm_arg == 0 )
		return false;
	//��������Ѿ�������,��ֱ�ӷ����ɹ�
	if( m_comm_handle != INVALID_HANDLE_VALUE )
		return true;
	/*���촮������*/
	char comm_name[64];
	COMMTIMEOUTS comm_timeout;

	/*���촮������*/
	sprintf( comm_name, "COM%d", comm_num );

	/*���Դ򿪴���*/
	m_comm_handle = CreateFile(
	                    comm_name,                          //��������
	                    GENERIC_READ | GENERIC_WRITE,       //ͨ�õĶ�д���
	                    0,
	                    0,
	                    OPEN_EXISTING,                      //ֻ���Ѿ����ڵ��ļ�
	                    0,
	                    0
	                );
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;                             //�����ʧ��,�������Ĳ��轫������

	/*���ô��ڳ�ʱ����*/
	comm_timeout.ReadIntervalTimeout = 200;
	comm_timeout.ReadTotalTimeoutConstant = 2000;
	comm_timeout.ReadTotalTimeoutMultiplier = 0;
	comm_timeout.WriteTotalTimeoutConstant = 2000;
	comm_timeout.WriteTotalTimeoutMultiplier = 0;

	/*���ô��ڳ�ʱ����*/
	if( !SetCommTimeouts( m_comm_handle, &comm_timeout ) ) {
		CloseHandle( m_comm_handle );
		m_comm_handle = INVALID_HANDLE_VALUE;
		return false;
	}

	/*���ô�������*/
	if( !SetCommState( m_comm_handle, comm_arg ) ) {
		CloseHandle( m_comm_handle );
		m_comm_handle = INVALID_HANDLE_VALUE;
		return false;
	}
	
	/*��ʼ����*/
	m_brun = true;
	//CreateThread( 0, 0, ListenProc, this, 0, 0 );
	return true;
}

/*
CloseComm�������ڹرմ�������
#####################################################
����ֵ:
    bool            �����ڳɹ��ر�,�򷵻�true
                    ������δ�򿪻�ر�ʧ��,����false
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
IsCommOpen������ȡ����״̬
#####################################################
����ֵ:
	bool            �������Ѵ��򷵻�true
                    ��֮����false
*/
bool CDevice::IsCommOpen( void ) {
	return m_comm_handle != INVALID_HANDLE_VALUE;
}

/*
QueryParam�����Ӵ����������в���,���浽���Ա������
#####################################################
����:
	SDeviceParam&   �豸����������,���ڽ�������������
	                ��
=====================================================
����ֵ:
    bool            ������ɹ�,�򷵻�true
                    ������ʧ��,�򷵻�false
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
WriteParam��������ͨ�����ڽ��������õ��豸��
#####################################################
����:
    const SDeviceParam&     �������ý����豸��������
							Ϣ
=====================================================
����ֵ:
	bool            		���ɹ�,����true
                    		��ʧ�ܷ���false
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

/*SetDeviceState��������ѡ���豸��ǰ�Ĺ���ģʽ`
#####################################################
����:
	EDeviceState state ���ù��̵�Ŀ��״̬
=====================================================
����ֵ:
	bool            ���ɹ�,����true
	                ��ʧ��,����false
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

/*Command�������ڽ���һ�δ��ڻ���
#####################################################
����:
	strng& cmd      cmd��Ҫ���͵��ַ���,ͬʱҲ������
	                �մ��ڵķ�����Ϣ
	                ��cmdΪ���ַ���,�򴮿ڲ�����ָ��,
	                ֻ�ȴ�һ�ν���
=====================================================
����ֵ:
	int             ���ؽ��յ��ķ�����������
=====================================================
ע��:
    �˷������������д���0�ֽڶ����·������ݱ��ض�,
    ���б�Ҫ��������,��ʹ����������غ���
*/
int CDevice::Command( string& cmd ) {
	DWORD dw_number;
	WINBOOL bRead;
	char *s_buffer = new char[128];
	memset( s_buffer, 0, 128 );
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return 0;
	if( cmd.length() != 0 ) {
		layout.Log( "��λ��:" + cmd );
		WriteFile( m_comm_handle, cmd.data(), cmd.length(), &dw_number, 0 );
	}
	dw_number = 0;
	bRead = ReadFile( m_comm_handle, s_buffer, 128, &dw_number, 0 );
	if( !bRead or dw_number == 0 ) {
		layout.Log( "��λ��δ��Ӧ!" );
		cmd = "";
		return 0;
	}
	cmd = s_buffer;
	layout.Log( "��λ��:" + cmd );
	delete[] s_buffer;
	return dw_number;
}

/*Command�������ڽ���һ�δ��ڻ���
#####################################################
����:
	char* cmd		cmd��Ҫ���͵�ָ��Ļ���,ͬʱҲ��
	                �����մ��ڵķ�����Ϣ
	-------------------------------------------------
	int buflen      �������ܴ�С,��ʾ�˽���ָ��ʱ����
	                �Ļ�������С,��Ϊ0 ��ʾ��cmdlen��
	                ��С
	-------------------------------------------------
	int cmdlen      ָ����ֽڳ���,��ʾ�˼������͵�ָ
	                ��ĳ���
					��Ϊ0��ʾ������ָ��,ֻ��
	                ��һ������
                    ��Ϊ-1��ʾcmd��һ���ַ���,Ҳ����
                    ˵ָ�����ݰ����ڵ�һ��'\0'����֮
                    ǰ����������,��ʱ����һ�ֽھ���0
                    ��Ҳ���ᷢ���κ�ָ��,���ȴ�һ��
                    �����ĵ���
=====================================================
����ֵ:
	int             ���شӴ��ڽ��յ��ķ������ֽ�����
*/
int CDevice::Command( char* cmd, int buflen, int cmdlen  ) {
	DWORD dw_number;
	WINBOOL bRead;
	if( m_comm_handle == INVALID_HANDLE_VALUE or cmd == 0 )
		return 0;
	if( cmdlen != 0 )
		if( cmdlen == -1 ) {
			layout.Log( "��λ��:" + string(cmd) );
			WriteFile( m_comm_handle, cmd, strlen(cmd), &dw_number, 0 );
		}
		else {
			string line = "��λ��(Hex):";
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
		layout.Log( "��λ��δ��Ӧ!" );
		memset( cmd, 0, buflen );
		return 0;
	}
	layout.Log( "��λ��" + string(cmd) );
	return dw_number;
}

/*GetWorkMode���ڻ�ȡ����ģʽ
#####################################################
����:
	SDeviceParam& param ���ڽ��չ���ģʽ����
	                    ��
=====================================================
����ֵ:
	bool                ����ȡ�ɹ�,�򷵻�true
	            		���򷵻�false��
	                    workmode������Ч
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

/*SetWorkMode�������ù���ģʽ
#####################################################
����:
	const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
=====================================================
����ֵ:
	bool                �����óɹ��򷵻�true
	                    ������ʧ���򷵻�false
*/
bool CDevice::SetWorkMode( const SDeviceParam& param ) {
	string cmdline = "AT+WKMOD=\"" + param.s_work_mode + "\"\r\n";
	Command( cmdline );
	if( !strstr( cmdline.data(), "OK" ) )
		return false;
	return true;
}

/*GetSockParam���ڻ�ȡ����͸������
#####################################################
����:
	SDeviceParam&       ���ڽ��ղ���������
=====================================================
����ֵ:
	bool                ��û���κδ����򷵻�true
	                    �������˴����򷵻�false
=====================================================
ע��:
	param�а��������������������κ�����������ܵ��޸�
*/
bool CDevice::GetSockParam( SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	const char *arg_a = cmdline+128;
	const char *arg_b = cmdline+256;
	const char *arg_c = cmdline+384;
	char *temp = 0;

	//��ѯA�������Ƿ�������
	sprintf( cmdline, "AT+SOCKAEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )      //����ڷ���������û���ҵ�OK˵����ѯ�����������˴���
	    return false;
	if( strstr( cmdline, "on" ) ) {     //����ڷ����������ҵ���on����,���ʾ������A��������

		param.socket_param.b_serverA_enable = true;

		sprintf( cmdline, "AT+SOCKA?\r\n" ); //��ѯ������A���������
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
	
	//��ѯB�������Ƿ�������
	sprintf( cmdline, "AT+SOCKBEN?\r\n" );
	Command( cmdline, 512 );
	if( !strstr( cmdline, "OK" ) )      //����ڷ���������û���ҵ�OK˵����ѯ�����������˴���
	    return false;
	if( strstr( cmdline, "on" ) ) {     //����ڷ����������ҵ���on����,���ʾ������B��������

		param.socket_param.b_serverB_enable = true;

		sprintf( cmdline, "AT+SOCKB?\r\n" ); //��ѯ������B���������
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
	
	//��ѯ�������Ƿ���
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
	
	//��ѯע����Ƿ���
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
	
	//����Ƿ���������͸��
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


/*SetSockParam������������͸��ģʽ�Ĳ���
#####################################################
����:
	const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
=====================================================
����ֵ:
	bool                �����óɹ��򷵻�true
	                    ������ʧ���򷵻�false
*/
bool CDevice::SetSockParam( const SDeviceParam& param ) {
	if( m_comm_handle == INVALID_HANDLE_VALUE )
		return false;
	char cmdline[512];
	string arg_a;
	
	//������A
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
	
	//������B
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
	
	//������
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

	//ע���
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
		
	//��͸��
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


/*GetHTTPDParam���ڻ�ȡHTTPD����
#####################################################
����:
	SDeviceParam&       ���ڽ��ղ���������
=====================================================
����ֵ:
	bool                ��û���κδ����򷵻�true
	                    �������˴����򷵻�false
=====================================================
ע��:
	param�а��������������������κ�����������ܵ��޸�
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


/*SetSockParam������������͸��ģʽ�Ĳ���
#####################################################
����:
	const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
=====================================================
����ֵ:
	bool                �����óɹ��򷵻�true
	                    ������ʧ���򷵻�false
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


/*GetMessageParam���ڻ�ȡ����͸������
#####################################################
����:
	SDeviceParam&       ���ڽ��ղ���������
=====================================================
����ֵ:
	bool                ��û���κδ����򷵻�true
	                    �������˴����򷵻�false
=====================================================
ע��:
	param�а��������������������κ�����������ܵ��޸�
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


/*SetSockParam������������͸��ģʽ�Ĳ���
#####################################################
����:
	const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
=====================================================
����ֵ:
	bool                �����óɹ��򷵻�true
	                    ������ʧ���򷵻�false
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


/*GetGlobalParam���ڻ�ȡȫ�ֲ���
#####################################################
����:
	SDeviceParam&       ���ڽ��ղ���������
=====================================================
����ֵ:
	bool                ��û���κδ����򷵻�true
	                    �������˴����򷵻�false
=====================================================
ע��:
	param�а��������������������κ�����������ܵ��޸�
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
	
	//��ѯAPN���˻�����
	sprintf( cmdline, "AT+APN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	temp = CutArg( cmdline );
	sscanf( temp, "%s%s%s", arg_a, arg_b, arg_c );
	param.operator_apn = string( arg_a );
	param.operator_user = string( arg_b );
	param.operator_password = string( arg_c );
	
	//��ѯ���ڲ���
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
	
	//��ѯ�Ƿ����
	sprintf( cmdline, "AT+E?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_show_back = true;
	else
		param.b_show_back = false;
		
	//��ѯ�Ƿ���RFC2217
	sprintf( cmdline, "AT+RFCEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_RFC2217_like = true;
	else
		param.b_RFC2217_like = false;
		
	//��ѯ�Ƿ�ͨ������
	sprintf( cmdline, "AT+CALEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_telephone = true;
	else
		param.b_telephone = false;
		
	//��ѯ�Ƿ񴮿�ATָ��
	sprintf( cmdline, "AT+UATEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_usart_at = true;
	else
		param.b_usart_at = false;

	//��ѯ�Ƿ�����ATָ��
	sprintf( cmdline, "AT+NATEN?\r\n" );
	Command( cmdline, 1024 );
	if( !strstr( cmdline, "OK" ) )
		return false;
	if( strstr( cmdline, "on" ) )
		param.b_net_at = true;
	else
		param.b_net_at = false;
		
	//��ѯ�Ƿ�����������
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


/*SetSockParam������������͸��ģʽ�Ĳ���
#####################################################
����:
	const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
=====================================================
����ֵ:
	bool                �����óɹ��򷵻�true
	                    ������ʧ���򷵻�false
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
