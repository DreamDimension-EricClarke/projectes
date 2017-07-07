#ifndef __Device_H__
#define __Device_H__

#include <windows.h>
#include <string>

using namespace std;

/*设备模式*/
enum EDeviceState {
    DeviceState_Config,                             	//配置模式
    DeviceState_Communicate,                        	//通讯模式
};

/*设备工作模式*/
#define WorkMode_Cmd		"CMD"                       //AT指令模式
#define WorkMode_Net		"NET"    					//网络透传模式
#define WorkMode_Httpd		"HTTPD"     				//Httpd模式
#define WorkMode_Sms		"SMS"   					//短信透传模式

/*网络连接模式*/
#define	SocketMode_Short  	"short"						//短连接
#define SocketMode_Long   	"long"						//长连接

/*网络透传模式服务器连接协议*/
#define SocketProtocal_TCP  "TCP"                       //TCP
#define SocketProtocal_UDP  "UDP"                       //UDP

/*网络透传模式下心跳包模式*/
#define HeartType_Com		"COM"      					//向串口发送心跳包
#define HeartType_Net		"NET"   					//向服务器发送心跳包

/*网络透传模式下注册包的发送模式*/
#define RegisterSend_Link	"link"						//在建立连接时发送一次注册包
#define RegisterSend_Data	"data"						//在每次发送数据包前
#define RegisterSend_Both   "link&data" 				//同时支持以上两种格式

/*注册包类型*/
#define RegisterType_ICCID	"ICCID"                     //ICCID码
#define RegisterType_IMEI	"IMEI"                      //IMEI码
#define RegisterType_REGID	"REGID"                     //注册ID
#define RegisterType_REGDT	"REGDT"                     //用户自定义

/*网络透传模式的参数*/
struct SSocketParam {
	bool    				b_serverA_enable = false;   //服务器A是否启用
	string  				s_serverA_addr;           	//服务器A地址
	string  				s_serverA_port;           	//服务器A端口
	string					s_serverA_protocal;         //服务器A连接协议
	string     				s_serverA_mode;           	//服务器A的连接模式

	bool    				b_serverB_enable = false;   //服务器A是否启用
	string  				s_serverB_addr;           	//服务器B地址
	string  				s_serverB_port;           	//服务器B地址
	string					s_serverB_protocal;         //服务器B连接协议
	string     				s_serverB_mode;           	//服务器B的连接模式

	bool            		b_heartbeat_enable = false; //是否启用心跳包
	string          		s_heartbeat_interval;     	//心跳包间隔时间
	string          		s_heartbeat_value;        	//心跳包内容(只保存16进制字符串)
	string  				s_heartbeat_type;         	//心跳包的发送模式

	bool            		b_register_enable = false;  //是否启用注册包
	string					s_register_send;          	//注册包的发送模式
	string					s_register_type;            //注册包类型
	string          		s_register_value;           //注册包值(只保存16进制字符串)

	bool            		b_cloud_untr = false;       //是否启用透传云
	string          		s_cloud_devnum;            	//透传云设备编号
	string          		s_cloud_password;          	//透传云通讯密码
};

/*HTTPD模式下HTTP请求的方式*/
#define HttpMethod_Get		"GET"		                //Get模式
#define HttpMethod_Post     "POST"                      //Post模式

/*设备在HTTPD模式下的参数*/
struct SHttpdParam {
	string					s_http_method;              //Http请求方式
	string					s_http_url;                 //Http请求的URL
	string					s_server_addr;              //服务器地址
	string					s_server_port;              //服务器端口
	string					s_http_header;              //Http请求头
};

/*设备在短信透传模式下的参数*/
struct SMessageParam {
	string         			s_phone_number;             //短信透传模式的电话号码
};

/*运营商*/
#define Operator_CMNET		"CMNET"                     //移动
#define Operator_UNINET     "UNINET"                    //联通

/*串口校验方式*/
#define ComParity_None		"NONE"                      //无校验
#define ComParity_Odd		"ODD"                       //奇校验
#define ComParity_Even 		"EVEN"                      //偶校验

/*串口流控制*/
#define ComFlowCtl_None		"NONE"                      //不使用流控制
#define ComFlowCtl_CRTS		"CRTS"                      //使用CTS,RTS控制
#define ComFlowCtl_RS485	"RS485"                     //使用RS485控制

/*设备参数*/
struct SDeviceParam {
	string       		s_work_mode;    				//设备的工作模式

	SSocketParam    	socket_param;             		//设备在网络透传模式下的参数
	SHttpdParam     	httpd_param;              		//设备在Httpd模式下的参数
	SMessageParam   	message_param;            		//设备在短信透传模式下的参数

	/*全局参数*/
	string          	operator_apn;             		//其他运营商 APN
	string          	operator_user;            		//其他运营商 账户
	string          	operator_password;        		//其他运营商 密码
	string        		com_baudrate;                  	//串口波特率
	string     			com_parity;                    	//串口校验方式
	string        		com_data_bit;                  	//串口数据位数
	string        		com_stop_bit;                  	//串口停止位数
	string				com_flow_control;              	//串口流控制
	bool            	b_show_back = false;            //指令回显
	bool            	b_RFC2217_like = false;         //类RFC2217
	bool            	b_telephone = false;            //通话功能
	bool            	b_usart_at = false;             //串口AT指令
	bool            	b_net_at = false;               //网络AT指令
	bool            	b_cache_data = false;           //短信AT指令
	string          	command_password;           	//命令密码
	string          	startup_message;            	//启动信息
};

/*设备抽象类*/
class CDevice {
	private:
		/*连接设备所使用的串口的参数*/
		HANDLE  		m_comm_handle;  			//串口句柄
		bool            m_brun;                     //串口监听线程是否正在运行

	public:
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
			comm_arg中,只有波特率,数据位数,停止位数
			有效,其他参数会被忽略 BuildCommDCB
		*/
		CDevice( int = -1, DCB* = 0 );

		/*
		析构函数用于关闭串口连接,清除已使用的内存空间
		#####################################################
		*/
		~CDevice( void );

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
			有效,其他参数会被忽略
		*/
		bool OpenComm( int, DCB* = 0 );

		/*
		CloseComm函数用于关闭串口连接
		#####################################################
		返回值:
		    bool            若串口成功关闭,则返回true
		                    若串口未打开或关闭失败,返回false
		*/
		bool CloseComm( void );
		
		/*
		IsCommOpen函数获取串口状态
		#####################################################
        返回值:
			bool            若串口已打开则返回true
                            反之返回false
		*/
		bool IsCommOpen( void );

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
		bool QueryParam( SDeviceParam& );

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
		bool WriteParam( const SDeviceParam& );

		/*SetDeviceState函数用于选择设备当前的工作模式`
		#####################################################
		参数:
			EDeviceState state 设置过程的目标状态
		=====================================================
		返回值:
			bool            若成功,返回true
			                若失败,返回false
		*/
		bool SetDeviceState( EDeviceState );

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
		int Command( string& );

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
		int Command( char*, int = 0, int = -1  );

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
		bool GetWorkMode( SDeviceParam& param );

		/*SetWorkMode用于设置工作模式
		#####################################################
		参数:
			const SDeviceParam param  将要设置的目标工作模式
		=====================================================
		返回值:
			bool                若设置成功则返回true
			                    若设置失败则返回false
		*/
		bool SetWorkMode( const SDeviceParam& param );

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
		bool GetSockParam( SDeviceParam& param );

		/*SetSockParam用于设置网络透传模式的参数
		#####################################################
		参数:
			const SDeviceParam param  将要设置的目标工作模式
		=====================================================
		返回值:
			bool                若设置成功则返回true
			                    若设置失败则返回false
		*/
		bool SetSockParam( const SDeviceParam& param );

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
		bool GetHTTPDParam( SDeviceParam& param );

		/*SetSockParam用于设置网络透传模式的参数
		#####################################################
		参数:
			const SDeviceParam param  将要设置的目标工作模式
		=====================================================
		返回值:
			bool                若设置成功则返回true
			                    若设置失败则返回false
		*/
		bool SetHTTPDParam( const SDeviceParam& param );

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
		bool GetMessageParam( SDeviceParam& param );

		/*SetSockParam用于设置网络透传模式的参数
		#####################################################
		参数:
			const SDeviceParam param  将要设置的目标工作模式
		=====================================================
		返回值:
			bool                若设置成功则返回true
			                    若设置失败则返回false
		*/
		bool SetMessageParam( const SDeviceParam& param );

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
		bool GetGlobalParam( SDeviceParam& param );

		/*SetSockParam用于设置网络透传模式的参数
		#####################################################
		参数:
			const SDeviceParam param  将要设置的目标工作模式
		=====================================================
		返回值:
			bool                若设置成功则返回true
			                    若设置失败则返回false
		*/
		bool SetGlobalParam( const SDeviceParam& param );
		
		/*ListenProc用于创建监听线程线程函数
		*/
		friend DWORD WINAPI ListenProc( LPVOID p );
};

DWORD WINAPI ListenProc( LPVOID p );

#endif

















































