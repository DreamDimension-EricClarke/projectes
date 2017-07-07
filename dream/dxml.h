#ifndef __dxml__
#define __dxml__
#include <stdexcept>
#include <string>

#include "dream.h"
#include "dchain.h"
/***
	dXml��dream dimension��xml��ʵ��
	dxmlʹ�ø�Ϊ�ϸ���﷨Ҫ��:
		dxml��ע�Ͳ������κβ��� 
		dxmlҪ����������ֵ�����õ����Ű���,����ֵ����Ϊ��
		dxmlҪ���κ������ڲ��������'-'��'_'������κα�׼���,���ܰ����հ� 
		dxmlҪ��ע����ʼ��<!--��ע����ֹ��-->û���ж�
		dxmlҪ��ʹ��<? dxml ... ... ?>���ĵ����ж���
			version dxml�汾��
		dxml ʹ��dxcas ����Ϊһ��������Ĭ�����Ժͱ�Ҫ�ڵ� 
		dxml ��Ϊ ' ' '\n' '\r' '\t'���ǿհ׷�,����ǩ֮��ֻ�пհ׷�ʱ,�������ı��ڵ� 
	dxmlʹ�ø����ת��ʵ��:
		&lt;		'<'
		&gt;		'>'
		&apos;		'\''
		&quot;		'\"'
		&amp; 		'&'
		&cr;		'\r'
		&lf;		'\n'
		&tab;		'\t'
		&spac;		' '
	dxmlҪ��رձ�ǩʹ��</��ͷ������< /������ 
***/

namespace dream {

#define __dxml_def_attr_name "attribute"
#define __dxml_def_attr_value "value"

class dXmlnode;	//dxml�ڵ���,����ʵ���ĵ��ڵ�
class dXmlattr;	//dxml������
class dXmllexer;	//dxml������
typedef dchain<dXmlattr> dXmlatls;	//dxml�ڵ������б�

/***dxml�ڵ�������***/
class dXmlattr {
	private:
		string	m_name;
		string	m_value;
	public:
		
		dXmlattr( string = __dxml_def_attr_name, const string& = __dxml_def_attr_value );
		
		string	getName( void );
		bool	setName( string );
		
		string	getValue( void );
		bool	setValue( const string& );
};

/***dxmlnode������ʵ��һ��dxml�ĵ��ڵ�***/ 
/***�ڵ����������ģʽ,��ͨ�ڵ���ı��ڵ�***/
/***�ı��ڵ����ӵ�и��ڵ�***/ 
class dXmlnode {
	private: 
		dXmlnode*	m_father;	//ָ�򸸽ڵ��ָ�� 
		const bool	m_btext;	//��ȷ�ڵ����� 
		
		dXmlnode*	m_previous;	//��һ���ڵ� 
		dXmlnode*	m_next;		//��һ���ڵ� 
		dXmlnode*	m_child;	//�ӽڵ����е��׽ڵ� 
		
		string		m_name;		//�ڵ����� 
		dXmlatls	m_attr;		//�ڵ������б�
		
		string&		m_text;		//�ڵ�����,�����ı�ֵ���ڴ�״̬ 
	private:
		/***���캯���ܵ�����,���нڵ㶼�ڶѿռ�������***/
		dXmlnode( string, dXmlnode*, bool );	//���캯���趨�˻������� 
		
		dXmlattr*	findAttr( string ) const;		//��������Ѱ�����Խڵ� 
	public:
		
		/***��������������������,��ɢ����***/
		~dXmlnode( void ); 
		
		/***��̬�Ľڵ㴴������***/ 
		static dXmlnode*	newnode( string = "", dXmlnode* = 0 );		//����һ���ڵ� 
		static dXmlnode*	newtext( string, dXmlnode* );				//����һ���ı�
		static dXmlnode*	newcontent( string, string, dXmlnode* );	//����һ�������ı��ӽڵ�Ľڵ�
		
		/***��λ����***/ 
		dXmlnode*	getFather( void ) const;					//��ȡ���ڵ�ָ�� 
		dXmlnode*	getPrevious( string = "" ) const;			//��������ͨ�ڵ���ɸѡӵ���ض����ƵĽڵ� 
		dXmlnode*	getNext( string = "" ) const;				//�������õ��ַ���Ϊ��ʱ�������нڵ�
		dXmlnode*	getBrother( int = 0, string = "" );			//�������������Ƶ��ֵܽڵ�
		dXmlnode*	getChild( int = 0, string = "" ) const;		//��ȡ�ӽڵ�����ָ��,�ı��ڵ���׳��쳣
		
		/***�ӽڵ��������,�ı��ڵ���׳��쳣***/ 
		bool		installChild( dXmlnode*, int = -1 );		//��ĳλ�ð�װ�ӽڵ�,��Ŀ��ڵ��и��ڵ���ʧ��
		bool		pickoutChild( dXmlnode* );					//��Ŀ��ڵ����ڱ��ڵ�,���߳�
		void		clearallChild( void );						//��������ӽڵ�
		
		/***���ڵ��������***/
		bool		isText( void )const;			//�жϱ��ڵ��Ƿ�Ϊ�ı��ڵ� 
		
		bool		setName( string );				//���ñ��ڵ������,�ı��ڵ���׳��쳣 
		string		getName( void ) const;			//��ȡ���ڵ������,�ı��ڵ���׳��쳣 
		bool		setText( const string& );		//�����ı�����,������ͨ�ڵ���׳��쳣 
		
		string		getText( void ) const;			//��ȡ�ı�����,������ͨ�ڵ���׳��쳣 
		bool		getText( string& ) const;		//��ȡ�ı�����,������ͨ�ڵ��ʧ�� 
		
		string		getContent( string , int = 0 ) const;			//��ȡĳ�ӽڵ���ı��ӽڵ�����,�����������׳��쳣 
		bool		getContent( string&, string, int = 0 ) const;	//��ȡĳ�ӽڵ���ı��ӽڵ�����,����������ʧ��
		
		bool		setContent( const string&, string, int = 0 );		//����һ���ڵ���ı��ӽڵ� 
		
		/***�κι������ԵĲ��������ı��ڵ㶼���׳��쳣***/ 
		bool		setAttr( string, const string& );	//�����ض����ԵĲ���,û�д����Ի��Զ����� 
		string		getAttr( string ) const;			//��ȡ�ض����ԵĲ���,û�д����Ի��׳��쳣 
		bool		getAttr( string, string& ) const;	//��ȡ�ض����ԵĲ���,û���򷵻�false 
		dXmlattr*	enumAttr( int ) const;				//����˳���оٿɼ������� 
		bool		removeAttr( string );				//ɾ���ض����� 
		void		clearallAttr( void );				//�����������
		
		/***��������***/
		dXmlnode* copy( dXmlnode* = 0 ) const;			//�����ڵ���������ݿ������µĽڵ�
};
/***dxml��������***/ 
/***�����漰�����﷨�Ĺ淶�������ڴ�����***/ 
class dXmllexer {
	private:
		int				m_version;	//��ʹ�õ�dxml�汾��
		/***���������Ա���ĵĲ���***/
		char*			m_code;		//�ĵ�Դ���� 
		int				m_offs;		//��ǰƫ����
	private:
		static string cutwith( const char*, const string& );	//�ý�����־��ȡһ�� 
		static string seekfor( const char*, const string& );	//�����ɱ�־��ȡһ�� 
		dXmlnode* code2node( dXmlnode* );						//�Ӵ����ȡ�ڵ�,�������˸��ڵ���Ϊ���ڵ��ȡ�����ӽڵ�(������������ǩ),�����ȡһ���ڵ� 
		void		clearReg( void );							//��������Ĵ���
		dXmllexer( void );										//��������ʼ�� 
		~dXmllexer( void );		//����ʱҪ����ռ�
	public:
		
		static bool		confirmName( string& );
		static string	transtextM2C( const string& );
		static string	transtextC2M( const string& );
		
		static string		generatCode( const dXmlnode* );		//�ӽڵ����ɴ���
		static dXmlnode*	lexerCode( const string& );			//�Ӵ������ɽڵ�,ֻ�ܶ�ȡһ�����ı��ڵ�
};

}
#endif
