#ifndef __dcl_cpp__
#define __dcl_cpp__

#include <string.h>
#include <stdexcept>
#include <stdio.h>
#include "dcl.h"

using namespace std;

namespace dream{	

/***
***dCLGP类封装了最简单的OpenCL使用,方便了没有太多
定制需求的即时使用情况 
***使用方法:以书写顺序把所有方法按需调用即可 
***/



//构造函数会搭建平台环境,抽取内核对象
//参数:
//	cl_nint num_code 源代码数量 
//	char** sources 指向一列以0结尾的源代码字符串,列也以0结尾
//	char* kname 包含了内核函数的函数名的字符串 
//	bool bOrdered	指示命令队列是否有序执行,若false则命令队列不互相阻塞 
dCLGP::dCLGP( cl_uint num_code, const char** sources, char* kname, bool bOrdered ):
m_arg_count(0)
{
	cl_int err_code;
	
	if( kname == 0 or sources == 0 or sources[0] == 0 )
		throw invalid_argument( "dCLGP has received one or more invalid arguments\n" );
	
	err_code = clGetPlatformIDs( 1, &m_platform, 0 );		//获取一个平台
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot get a platform where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	err_code = clGetDeviceIDs( m_platform, CL_DEVICE_TYPE_GPU, 1, &m_device, 0 );	//获取一个要是用的GPU设备
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot get a device where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	//创建上下文 此步骤是真正开始与GPU互动的步骤 
	cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(m_platform), 0 };
	m_context = clCreateContext( prop, 1, &m_device, 0, 0, &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create context where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	//创建命令队列,以便操作设备 
	m_queue = clCreateCommandQueue( m_context, m_device, (bOrdered?0:CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE), &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create command queue where error code is :%d\n", err_code );
		
		clReleaseContext( m_context );
		
		throw runtime_error( msg );
	}
	
	//创建程序对象 
	m_program = clCreateProgramWithSource( m_context, num_code, sources, 0, &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create program where error code is :%d\n", err_code );
		
		clReleaseContext( m_context );
		clReleaseCommandQueue( m_queue );
		
		throw runtime_error( msg );
	}
	
	//编译连接程序 
	err_code = clBuildProgram( m_program, 1, &m_device,  0, 0, 0 );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot build program where error code is :%d\n", err_code );
		
		clReleaseContext( m_context );
		clReleaseCommandQueue( m_queue );
		clReleaseProgram( m_program );
		
		throw runtime_error( msg );
	}
	
	//创建内核 
	m_kernel = clCreateKernel( m_program, kname, &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create kernel where error code is :%d\n", err_code );
		
		clReleaseContext( m_context );
		clReleaseCommandQueue( m_queue );
		clReleaseProgram( m_program );
		
		throw runtime_error( msg );
	}	
}

//析构函数会释放所有平台资源
dCLGP::~dCLGP( )
{
	clReleaseCommandQueue( m_queue );
	clReleaseKernel( m_kernel );
	clReleaseProgram( m_program );
	clReleaseContext( m_context );
}

//为平台创建一个GPU上的内存空间
//参数:
//	size_t size	内存大小,以字节为单位
//	void* host_ptr	主机内存空间,若设置,则内存创建后自动拷贝主机内存中的数据
//返回值:
//	cl_mem	 OpenCL内存对象指针,可以直接用来做内核函数的参数使用 
cl_mem	dCLGP::CreateBuffer( size_t size, void* host_ptr )
{
	if( size <= 0 )
		throw invalid_argument( "CreateBuffer recieved bad memory size \n" );

	cl_int err_code;
	cl_mem mem = clCreateBuffer( m_context, CL_MEM_READ_WRITE | (host_ptr!=0?CL_MEM_COPY_HOST_PTR:0), size, host_ptr, &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "CreateBuffer cannot create allocat where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	return mem;
}

//从GPU内存中读取数据
//参数: 
//	cl_mem	memory 要读取的内存对象 
//	size_t	offset 要读取的内存的偏移 
//	size_t	length 要读取的内存的大小 
//	void*	buffer 读取数据的保存位置 
// 返回值:
//	bool	读取是否成功 
bool	dCLGP::ReadBuffer( cl_mem mem, size_t offset, size_t length,  void* buffer )
{
	if( clEnqueueReadBuffer(m_queue,  mem, CL_TRUE, offset, length, buffer, 0, 0, 0 ) != CL_SUCCESS )
		return false;
	return true;
}

//向GPU内存中写入数据
//参数: 
//	cl_mem	memory 要写入的内存对象 
//	size_t	offset 要写入的内存的偏移 
//	size_t	length 要写入的内存的大小 
//	void*	buffer 写入数据的保存位置 
// 返回值:
//	bool	写入是否成功	
size_t	dCLGP::WriteBuffer( cl_mem mem, size_t offset, size_t length, void* buffer )
{
	if( clEnqueueWriteBuffer( m_queue, mem, CL_TRUE, offset, length, buffer, 0, 0, 0 ) != CL_SUCCESS )
		return false;
	return true;
}

//给内核函数增加一个参数
//参数:
//	tpn arg参数数据,可以是任何类型 
//返回值:
//	是否成功地创建了参数 
template<typename tpn>
bool	dCLGP::AddKernelArg( tpn& arg )
{
	if( clSetKernelArg( m_kernel, m_arg_count++, sizeof(arg), &arg ) != CL_SUCCESS )
		return false;
	return true;
}

//开始运行内核函数
//参数:
//	size_t x global_work_item 的x轴向数量
//	size_t y global_work_item 的y轴向数量
//	size_t z global_work_item 的z轴向数量 
//	global_work_item的总数量是x*y*z的结果
bool dCLGP::RunKernel( cl_uint dim, size_t* global_size, size_t* group_size )
{
	if( dim < 1 or global_size == 0 or group_size == 0 )
		return false;
	if( clEnqueueNDRangeKernel( m_queue, m_kernel, dim, 0, global_size, group_size, 0, 0, 0 ) != CL_SUCCESS )
		return false;
	return true;
}

//end of file
}
#endif
