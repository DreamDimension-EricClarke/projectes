#ifndef __dcl__
#define __dcl__

#include <cl/opencl.h>

using namespace std;

namespace dream{	

/***
***dCLGP类封装了最简单的OpenCL使用,方便了没有太多
定制需求的即时使用情况 
***使用方法:以书写顺序把所有方法按需调用即可 

***/
class dCLGP {
	private:
		cl_platform_id		m_platform;		//平台句柄 
		cl_device_id		m_device;		//设备句柄 
		cl_context			m_context;		//上下文句柄 
		cl_program			m_program;		//程序对象句柄
		cl_kernel			m_kernel;		//内核对象句柄
		cl_command_queue	m_queue;		//命令队列句柄
		int 				m_arg_count;	//内核函数参数个数 
	public:
		//构造函数会搭建平台环境,抽取内核对象
		//参数:
		//	cl_nint num_code 源代码数量 
		//	char** sources 指向一列以0结尾的源代码字符串,列也以0结尾
		//	char* kname 包含了内核函数的函数名的字符串 
		//	bool bOrdered	指示命令队列是否有序执行,若false则命令队列不互相阻塞 
		dCLGP( cl_uint num_code, const char** sources, char* kname, bool bOrdered = true );
		
		//析构函数会释放所有平台资源
		~dCLGP( );
		
		//为平台创建一个GPU上的内存空间
		//参数:
		//	size_t size	内存大小,以字节为单位
		//	void* host_ptr	主机内存空间,若设置,则内存创建后自动拷贝主机内存中的数据
		//返回值:
		//	cl_mem	 OpenCL内存对象指针,可以直接用来做内核函数的参数使用 
		cl_mem	CreateBuffer( size_t size, void* host_ptr = 0 );
		
		//从GPU内存中读取数据
		//参数: 
		//	cl_mem	memory 要读取的内存对象 
		//	size_t	offset 要读取的内存的偏移 
		//	size_t	length 要读取的内存的大小 
		//	void*	buffer 读取数据的保存位置 
		// 返回值:
		//	bool	读取是否成功 
		bool	ReadBuffer( cl_mem mem, size_t offset, size_t length,  void* buffer );
	
		//向GPU内存中写入数据
		//参数: 
		//	cl_mem	memory 要写入的内存对象 
		//	size_t	offset 要写入的内存的偏移 
		//	size_t	length 要写入的内存的大小 
		//	void*	buffer 写入数据的保存位置 
		// 返回值:
		//	bool	写入是否成功	
		size_t	WriteBuffer( cl_mem mem, size_t offset, size_t length, void* buffer );
		
		//给内核函数增加一个参数
		//参数:
		//	tpn arg参数数据,可以是任何类型 
		//返回值:
		//	是否成功地创建了参数 
		template<typename tpn>
		bool	AddKernelArg( tpn& arg );
		
		//开始运行内核函数
		//参数:
		//	size_t x global_work_item 的x轴向数量
		//	size_t y global_work_item 的y轴向数量
		//	size_t z global_work_item 的z轴向数量 
		//	global_work_item的总数量是x*y*z的结果
		bool RunKernel( cl_uint dim, size_t* global_size, size_t* group_size );
};

//end of file
}
#endif
