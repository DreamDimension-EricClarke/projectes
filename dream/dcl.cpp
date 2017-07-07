#ifndef __dcl_cpp__
#define __dcl_cpp__

#include <string.h>
#include <stdexcept>
#include <stdio.h>
#include "dcl.h"

using namespace std;

namespace dream{	

/***
***dCLGP���װ����򵥵�OpenCLʹ��,������û��̫��
��������ļ�ʱʹ����� 
***ʹ�÷���:����д˳������з���������ü��� 
***/



//���캯����ƽ̨����,��ȡ�ں˶���
//����:
//	cl_nint num_code Դ�������� 
//	char** sources ָ��һ����0��β��Դ�����ַ���,��Ҳ��0��β
//	char* kname �������ں˺����ĺ��������ַ��� 
//	bool bOrdered	ָʾ��������Ƿ�����ִ��,��false��������в��������� 
dCLGP::dCLGP( cl_uint num_code, const char** sources, char* kname, bool bOrdered ):
m_arg_count(0)
{
	cl_int err_code;
	
	if( kname == 0 or sources == 0 or sources[0] == 0 )
		throw invalid_argument( "dCLGP has received one or more invalid arguments\n" );
	
	err_code = clGetPlatformIDs( 1, &m_platform, 0 );		//��ȡһ��ƽ̨
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot get a platform where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	err_code = clGetDeviceIDs( m_platform, CL_DEVICE_TYPE_GPU, 1, &m_device, 0 );	//��ȡһ��Ҫ���õ�GPU�豸
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot get a device where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	//���������� �˲�����������ʼ��GPU�����Ĳ��� 
	cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(m_platform), 0 };
	m_context = clCreateContext( prop, 1, &m_device, 0, 0, &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create context where error code is :%d\n", err_code );
		throw runtime_error( msg );
	}
	
	//�����������,�Ա�����豸 
	m_queue = clCreateCommandQueue( m_context, m_device, (bOrdered?0:CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE), &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create command queue where error code is :%d\n", err_code );
		
		clReleaseContext( m_context );
		
		throw runtime_error( msg );
	}
	
	//����������� 
	m_program = clCreateProgramWithSource( m_context, num_code, sources, 0, &err_code );
	if( err_code != CL_SUCCESS )
	{
		char msg[256];
		sprintf( msg, "dCLGP cannot create program where error code is :%d\n", err_code );
		
		clReleaseContext( m_context );
		clReleaseCommandQueue( m_queue );
		
		throw runtime_error( msg );
	}
	
	//�������ӳ��� 
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
	
	//�����ں� 
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

//�����������ͷ�����ƽ̨��Դ
dCLGP::~dCLGP( )
{
	clReleaseCommandQueue( m_queue );
	clReleaseKernel( m_kernel );
	clReleaseProgram( m_program );
	clReleaseContext( m_context );
}

//Ϊƽ̨����һ��GPU�ϵ��ڴ�ռ�
//����:
//	size_t size	�ڴ��С,���ֽ�Ϊ��λ
//	void* host_ptr	�����ڴ�ռ�,������,���ڴ洴�����Զ����������ڴ��е�����
//����ֵ:
//	cl_mem	 OpenCL�ڴ����ָ��,����ֱ���������ں˺����Ĳ���ʹ�� 
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

//��GPU�ڴ��ж�ȡ����
//����: 
//	cl_mem	memory Ҫ��ȡ���ڴ���� 
//	size_t	offset Ҫ��ȡ���ڴ��ƫ�� 
//	size_t	length Ҫ��ȡ���ڴ�Ĵ�С 
//	void*	buffer ��ȡ���ݵı���λ�� 
// ����ֵ:
//	bool	��ȡ�Ƿ�ɹ� 
bool	dCLGP::ReadBuffer( cl_mem mem, size_t offset, size_t length,  void* buffer )
{
	if( clEnqueueReadBuffer(m_queue,  mem, CL_TRUE, offset, length, buffer, 0, 0, 0 ) != CL_SUCCESS )
		return false;
	return true;
}

//��GPU�ڴ���д������
//����: 
//	cl_mem	memory Ҫд����ڴ���� 
//	size_t	offset Ҫд����ڴ��ƫ�� 
//	size_t	length Ҫд����ڴ�Ĵ�С 
//	void*	buffer д�����ݵı���λ�� 
// ����ֵ:
//	bool	д���Ƿ�ɹ�	
size_t	dCLGP::WriteBuffer( cl_mem mem, size_t offset, size_t length, void* buffer )
{
	if( clEnqueueWriteBuffer( m_queue, mem, CL_TRUE, offset, length, buffer, 0, 0, 0 ) != CL_SUCCESS )
		return false;
	return true;
}

//���ں˺�������һ������
//����:
//	tpn arg��������,�������κ����� 
//����ֵ:
//	�Ƿ�ɹ��ش����˲��� 
template<typename tpn>
bool	dCLGP::AddKernelArg( tpn& arg )
{
	if( clSetKernelArg( m_kernel, m_arg_count++, sizeof(arg), &arg ) != CL_SUCCESS )
		return false;
	return true;
}

//��ʼ�����ں˺���
//����:
//	size_t x global_work_item ��x��������
//	size_t y global_work_item ��y��������
//	size_t z global_work_item ��z�������� 
//	global_work_item����������x*y*z�Ľ��
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
