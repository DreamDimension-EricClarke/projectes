#ifndef __Device_H__
#define __Device_H__

#include <windows.h>
#include <string>

using namespace std;

/*�豸ģʽ*/
enum EDeviceState {
    DeviceState_Config,                             	//����ģʽ
    DeviceState_Communicate,                        	//ͨѶģʽ
};

/*�豸����ģʽ*/
#define WorkMode_Cmd		"CMD"                       //ATָ��ģʽ
#define WorkMode_Net		"NET"    					//����͸��ģʽ
#define WorkMode_Httpd		"HTTPD"     				//Httpdģʽ
#define WorkMode_Sms		"SMS"   					//����͸��ģʽ

/*��������ģʽ*/
#define	SocketMode_Short  	"short"						//������
#define SocketMode_Long   	"long"						//������

/*����͸��ģʽ����������Э��*/
#define SocketProtocal_TCP  "TCP"                       //TCP
#define SocketProtocal_UDP  "UDP"                       //UDP

/*����͸��ģʽ��������ģʽ*/
#define HeartType_Com		"COM"      					//�򴮿ڷ���������
#define HeartType_Net		"NET"   					//�����������������

/*����͸��ģʽ��ע����ķ���ģʽ*/
#define RegisterSend_Link	"link"						//�ڽ�������ʱ����һ��ע���
#define RegisterSend_Data	"data"						//��ÿ�η������ݰ�ǰ
#define RegisterSend_Both   "link&data" 				//ͬʱ֧���������ָ�ʽ

/*ע�������*/
#define RegisterType_ICCID	"ICCID"                     //ICCID��
#define RegisterType_IMEI	"IMEI"                      //IMEI��
#define RegisterType_REGID	"REGID"                     //ע��ID
#define RegisterType_REGDT	"REGDT"                     //�û��Զ���

/*����͸��ģʽ�Ĳ���*/
struct SSocketParam {
	bool    				b_serverA_enable = false;   //������A�Ƿ�����
	string  				s_serverA_addr;           	//������A��ַ
	string  				s_serverA_port;           	//������A�˿�
	string					s_serverA_protocal;         //������A����Э��
	string     				s_serverA_mode;           	//������A������ģʽ

	bool    				b_serverB_enable = false;   //������A�Ƿ�����
	string  				s_serverB_addr;           	//������B��ַ
	string  				s_serverB_port;           	//������B��ַ
	string					s_serverB_protocal;         //������B����Э��
	string     				s_serverB_mode;           	//������B������ģʽ

	bool            		b_heartbeat_enable = false; //�Ƿ�����������
	string          		s_heartbeat_interval;     	//���������ʱ��
	string          		s_heartbeat_value;        	//����������(ֻ����16�����ַ���)
	string  				s_heartbeat_type;         	//�������ķ���ģʽ

	bool            		b_register_enable = false;  //�Ƿ�����ע���
	string					s_register_send;          	//ע����ķ���ģʽ
	string					s_register_type;            //ע�������
	string          		s_register_value;           //ע���ֵ(ֻ����16�����ַ���)

	bool            		b_cloud_untr = false;       //�Ƿ�����͸����
	string          		s_cloud_devnum;            	//͸�����豸���
	string          		s_cloud_password;          	//͸����ͨѶ����
};

/*HTTPDģʽ��HTTP����ķ�ʽ*/
#define HttpMethod_Get		"GET"		                //Getģʽ
#define HttpMethod_Post     "POST"                      //Postģʽ

/*�豸��HTTPDģʽ�µĲ���*/
struct SHttpdParam {
	string					s_http_method;              //Http����ʽ
	string					s_http_url;                 //Http�����URL
	string					s_server_addr;              //��������ַ
	string					s_server_port;              //�������˿�
	string					s_http_header;              //Http����ͷ
};

/*�豸�ڶ���͸��ģʽ�µĲ���*/
struct SMessageParam {
	string         			s_phone_number;             //����͸��ģʽ�ĵ绰����
};

/*��Ӫ��*/
#define Operator_CMNET		"CMNET"                     //�ƶ�
#define Operator_UNINET     "UNINET"                    //��ͨ

/*����У�鷽ʽ*/
#define ComParity_None		"NONE"                      //��У��
#define ComParity_Odd		"ODD"                       //��У��
#define ComParity_Even 		"EVEN"                      //żУ��

/*����������*/
#define ComFlowCtl_None		"NONE"                      //��ʹ��������
#define ComFlowCtl_CRTS		"CRTS"                      //ʹ��CTS,RTS����
#define ComFlowCtl_RS485	"RS485"                     //ʹ��RS485����

/*�豸����*/
struct SDeviceParam {
	string       		s_work_mode;    				//�豸�Ĺ���ģʽ

	SSocketParam    	socket_param;             		//�豸������͸��ģʽ�µĲ���
	SHttpdParam     	httpd_param;              		//�豸��Httpdģʽ�µĲ���
	SMessageParam   	message_param;            		//�豸�ڶ���͸��ģʽ�µĲ���

	/*ȫ�ֲ���*/
	string          	operator_apn;             		//������Ӫ�� APN
	string          	operator_user;            		//������Ӫ�� �˻�
	string          	operator_password;        		//������Ӫ�� ����
	string        		com_baudrate;                  	//���ڲ�����
	string     			com_parity;                    	//����У�鷽ʽ
	string        		com_data_bit;                  	//��������λ��
	string        		com_stop_bit;                  	//����ֹͣλ��
	string				com_flow_control;              	//����������
	bool            	b_show_back = false;            //ָ�����
	bool            	b_RFC2217_like = false;         //��RFC2217
	bool            	b_telephone = false;            //ͨ������
	bool            	b_usart_at = false;             //����ATָ��
	bool            	b_net_at = false;               //����ATָ��
	bool            	b_cache_data = false;           //����ATָ��
	string          	command_password;           	//��������
	string          	startup_message;            	//������Ϣ
};

/*�豸������*/
class CDevice {
	private:
		/*�����豸��ʹ�õĴ��ڵĲ���*/
		HANDLE  		m_comm_handle;  			//���ھ��
		bool            m_brun;                     //���ڼ����߳��Ƿ���������

	public:
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
			comm_arg��,ֻ�в�����,����λ��,ֹͣλ��
			��Ч,���������ᱻ���� BuildCommDCB
		*/
		CDevice( int = -1, DCB* = 0 );

		/*
		�����������ڹرմ�������,�����ʹ�õ��ڴ�ռ�
		#####################################################
		*/
		~CDevice( void );

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
			��Ч,���������ᱻ����
		*/
		bool OpenComm( int, DCB* = 0 );

		/*
		CloseComm�������ڹرմ�������
		#####################################################
		����ֵ:
		    bool            �����ڳɹ��ر�,�򷵻�true
		                    ������δ�򿪻�ر�ʧ��,����false
		*/
		bool CloseComm( void );
		
		/*
		IsCommOpen������ȡ����״̬
		#####################################################
        ����ֵ:
			bool            �������Ѵ��򷵻�true
                            ��֮����false
		*/
		bool IsCommOpen( void );

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
		bool QueryParam( SDeviceParam& );

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
		bool WriteParam( const SDeviceParam& );

		/*SetDeviceState��������ѡ���豸��ǰ�Ĺ���ģʽ`
		#####################################################
		����:
			EDeviceState state ���ù��̵�Ŀ��״̬
		=====================================================
		����ֵ:
			bool            ���ɹ�,����true
			                ��ʧ��,����false
		*/
		bool SetDeviceState( EDeviceState );

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
		int Command( string& );

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
		int Command( char*, int = 0, int = -1  );

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
		bool GetWorkMode( SDeviceParam& param );

		/*SetWorkMode�������ù���ģʽ
		#####################################################
		����:
			const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
		=====================================================
		����ֵ:
			bool                �����óɹ��򷵻�true
			                    ������ʧ���򷵻�false
		*/
		bool SetWorkMode( const SDeviceParam& param );

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
		bool GetSockParam( SDeviceParam& param );

		/*SetSockParam������������͸��ģʽ�Ĳ���
		#####################################################
		����:
			const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
		=====================================================
		����ֵ:
			bool                �����óɹ��򷵻�true
			                    ������ʧ���򷵻�false
		*/
		bool SetSockParam( const SDeviceParam& param );

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
		bool GetHTTPDParam( SDeviceParam& param );

		/*SetSockParam������������͸��ģʽ�Ĳ���
		#####################################################
		����:
			const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
		=====================================================
		����ֵ:
			bool                �����óɹ��򷵻�true
			                    ������ʧ���򷵻�false
		*/
		bool SetHTTPDParam( const SDeviceParam& param );

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
		bool GetMessageParam( SDeviceParam& param );

		/*SetSockParam������������͸��ģʽ�Ĳ���
		#####################################################
		����:
			const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
		=====================================================
		����ֵ:
			bool                �����óɹ��򷵻�true
			                    ������ʧ���򷵻�false
		*/
		bool SetMessageParam( const SDeviceParam& param );

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
		bool GetGlobalParam( SDeviceParam& param );

		/*SetSockParam������������͸��ģʽ�Ĳ���
		#####################################################
		����:
			const SDeviceParam param  ��Ҫ���õ�Ŀ�깤��ģʽ
		=====================================================
		����ֵ:
			bool                �����óɹ��򷵻�true
			                    ������ʧ���򷵻�false
		*/
		bool SetGlobalParam( const SDeviceParam& param );
		
		/*ListenProc���ڴ��������߳��̺߳���
		*/
		friend DWORD WINAPI ListenProc( LPVOID p );
};

DWORD WINAPI ListenProc( LPVOID p );

#endif

















































