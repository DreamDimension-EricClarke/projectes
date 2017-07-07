#ifndef __dcl__
#define __dcl__

#include <cl/opencl.h>

using namespace std;

namespace dream{	

/***
***dCLGP���װ����򵥵�OpenCLʹ��,������û��̫��
��������ļ�ʱʹ����� 
***ʹ�÷���:����д˳������з���������ü��� 

***/
class dCLGP {
	private:
		cl_platform_id		m_platform;		//ƽ̨��� 
		cl_device_id		m_device;		//�豸��� 
		cl_context			m_context;		//�����ľ�� 
		cl_program			m_program;		//���������
		cl_kernel			m_kernel;		//�ں˶�����
		cl_command_queue	m_queue;		//������о��
		int 				m_arg_count;	//�ں˺����������� 
	public:
		//���캯����ƽ̨����,��ȡ�ں˶���
		//����:
		//	cl_nint num_code Դ�������� 
		//	char** sources ָ��һ����0��β��Դ�����ַ���,��Ҳ��0��β
		//	char* kname �������ں˺����ĺ��������ַ��� 
		//	bool bOrdered	ָʾ��������Ƿ�����ִ��,��false��������в��������� 
		dCLGP( cl_uint num_code, const char** sources, char* kname, bool bOrdered = true );
		
		//�����������ͷ�����ƽ̨��Դ
		~dCLGP( );
		
		//Ϊƽ̨����һ��GPU�ϵ��ڴ�ռ�
		//����:
		//	size_t size	�ڴ��С,���ֽ�Ϊ��λ
		//	void* host_ptr	�����ڴ�ռ�,������,���ڴ洴�����Զ����������ڴ��е�����
		//����ֵ:
		//	cl_mem	 OpenCL�ڴ����ָ��,����ֱ���������ں˺����Ĳ���ʹ�� 
		cl_mem	CreateBuffer( size_t size, void* host_ptr = 0 );
		
		//��GPU�ڴ��ж�ȡ����
		//����: 
		//	cl_mem	memory Ҫ��ȡ���ڴ���� 
		//	size_t	offset Ҫ��ȡ���ڴ��ƫ�� 
		//	size_t	length Ҫ��ȡ���ڴ�Ĵ�С 
		//	void*	buffer ��ȡ���ݵı���λ�� 
		// ����ֵ:
		//	bool	��ȡ�Ƿ�ɹ� 
		bool	ReadBuffer( cl_mem mem, size_t offset, size_t length,  void* buffer );
	
		//��GPU�ڴ���д������
		//����: 
		//	cl_mem	memory Ҫд����ڴ���� 
		//	size_t	offset Ҫд����ڴ��ƫ�� 
		//	size_t	length Ҫд����ڴ�Ĵ�С 
		//	void*	buffer д�����ݵı���λ�� 
		// ����ֵ:
		//	bool	д���Ƿ�ɹ�	
		size_t	WriteBuffer( cl_mem mem, size_t offset, size_t length, void* buffer );
		
		//���ں˺�������һ������
		//����:
		//	tpn arg��������,�������κ����� 
		//����ֵ:
		//	�Ƿ�ɹ��ش����˲��� 
		template<typename tpn>
		bool	AddKernelArg( tpn& arg );
		
		//��ʼ�����ں˺���
		//����:
		//	size_t x global_work_item ��x��������
		//	size_t y global_work_item ��y��������
		//	size_t z global_work_item ��z�������� 
		//	global_work_item����������x*y*z�Ľ��
		bool RunKernel( cl_uint dim, size_t* global_size, size_t* group_size );
};

//end of file
}
#endif
