#ifndef __dxml__
#define __dxml__
#include <stdexcept>
#include <string>

#include "dream.h"
#include "dchain.cpp"
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
	dxmlʹ�ø����ת��ʵ��:
		&lt;		'<'
		&gt;		'>'
		&apos;		'\''
		&quot;		'\"'
		&amp; 		'&'
		&endl;		'\n'
		&tab;		'\t'
		&spac;		' '
	dxmlҪ��رձ�ǩʹ��</��ͷ������< /������ 
***/

namespace dream {

#define __dxml_def_attr_name "attribute"
#define __dxml_def_attr_value "value"

class dXmlnode;	//dxml�ڵ���,����ʵ���ĵ��ڵ�
class dXmlattr;	//dxml������
class dXmlanal;	//dxml������
class dXmlxcas;	//dxml��������б� 
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
		static dXmlnode*	newnode( string = "", dXmlnode* = 0 );	//����һ���ڵ� 
		static dXmlnode*	newtext( string, dXmlnode* );			//����һ���ı� 
		
		/***��λ����***/ 
		dXmlnode*	getFather( void ) const;					//��ȡ���ڵ�ָ�� 
		dXmlnode*	getPrevious( string = "" ) const;			//��������ͨ�ڵ���ɸѡӵ���ض����ƵĽڵ� 
		dXmlnode*	getNext( string = "" ) const;				//�������õ��ַ���Ϊ��ʱ�������нڵ�
		dXmlnode*	getBrother( int = 0, string = "" );			//�������������Ƶ��ֵܽڵ�
		dXmlnode*	getChild( int = 0, string = "" ) const;		//��ȡ�ӽڵ�����ָ��,�ı��ڵ���׳��쳣
		
		/***�ӽڵ��������,�ı��ڵ���׳��쳣***/ 
		bool		installChild( dXmlnode*, int = -1 );//��ĳλ�ð�װ�ӽڵ�,��Ŀ��ڵ��и��ڵ���ʧ��
		bool		pickoutChild( dXmlnode* );			//��Ŀ��ڵ����ڱ��ڵ�,���߳�
		void		clearallChild( void );				//��������ӽڵ�
		
		/***���ڵ��������***/
		bool		isText( void )const;				//�жϱ��ڵ��Ƿ�Ϊ�ı��ڵ� 
		
		bool		setName( string );					//���ñ��ڵ������,�ı��ڵ���׳��쳣 
		string		getName( void ) const;				//��ȡ���ڵ������,�ı��ڵ���׳��쳣 
		bool		setText( const string& );			//�����ı�����,������ͨ�ڵ���׳��쳣 
		string		getText( void ) const;				//��ȡ�ı�����,������ͨ�ڵ���׳��쳣 
		
		/***�κι������ԵĲ��������ı��ڵ㶼���׳��쳣***/ 
		bool		setAttr( string, const string& );	//�����ض����ԵĲ���,û�д����Ի��Զ����� 
		string		getAttr( string ) const;			//��ȡ�ض����ԵĲ���,û�д����Ի��׳��쳣 
		dXmlattr*	enumAttr( int ) const;				//����˳���оٿɼ������� 
		bool		removeAttr( string );				//ɾ���ض����� 
		void		clearallAttr( void );				//�����������
		
		/***��������***/
		dXmlnode* copy( dXmlnode* = 0 ) const;			//�����ڵ���������ݿ������µĽڵ�
};

/***dxml��������б���***/
/***xcas�������dxml�ĵ�����Ϊ��һ��xcas�ĵ�Ԥ����ڵ�����***/ 
class dXmlxcas {
	private:
		dXmlnode*	m_root;
		dXmlnode*	m_rule;
	private:
		dXmlxcas( void );	//���ƹ��캯���Ա�֤ÿ���ڵ��ĵ����Ƕ����� 
	public:
		~dXmlxcas( void );
		
		bool	addRule( string, bool = false );//���һ������
		bool	isRule( string );				//�жϱ�ǵı�ǩ�����Ƿ��ѱ���װ 
		bool	findRule( string );				//Ѱ��һ������ 
		bool	isRefuse( void );				//�Ƿ�ܾ�ǰ�� 
		bool	removeRule( void );				//ɾ����ǰ���� 
		void	clearRule( void );				//������й��� 
		
		bool	setAttr( string, string, bool );	//Ϊ��ǰ������ӻ�����һ������
		string	getAttr( string );					//��ȡĳ���Թ���Ĺ���������û�д˹����򷵻ؿ��ַ��� 
		bool	isInherit( string );				//�ж�ĳ�����Ƿ��Ŵ�,û�д���������Ϊ���Ŵ� 
		bool	removeAttr( string );				//ɾ��ĳ�����Թ��� 
		void	clearAttr( void );					//������Թ��� 
		
		bool	addNode( string );				//Ϊ��ǰ������ӱر��ڵ� 
		bool	isNode( string );				//�жϽڵ�����Ƿ���� 
		bool	removeNode( string );			//ɾ��ĳ���ڵ���� 
		void	clearNode( void );				//������нڵ���� 
		
		const dXmlnode* getTree( void );		//��ȡ���ù����� 
};
/***dxml��������***/ 
/***�����漰�����﷨�Ĺ淶�������ڴ�����***/ 
class dXmlanal {
	private:
		int				m_version;	//��ʹ�õ�dxml�汾��
		/***���������Ա���ĵĲ���***/
		char*			m_code;		//�ĵ�Դ���� 
		int				m_offs;		//��ǰƫ����
		dXmlxcas*		m_xcas;		//��������б�(ʼ��ָ��������ڵı���)
	private:
		static string cutwith( const char*, const string& );	//�ý�����־��ȡһ�� 
		static string seekfor( const char*, const string& );	//�����ɱ�־��ȡһ�� 
		dXmlnode* code2node( dXmlnode* );	//�Ӵ����ȡ�ڵ�,�������˸��ڵ���Ϊ���ڵ��ȡ�����ӽڵ�(������������ǩ),�����ȡһ���ڵ� 
	public:
		static bool		confirmName( string& );
		static string	transtextM2C( const string& );
		static string	transtextC2M( const string& );
		
		dXmlanal( void );		//��������ʼ�� 
		~dXmlanal( void );		//����ʱҪ����ռ�
		
		static string	generatCode( const dXmlnode* );			//�ӽڵ����ɴ���
		dXmlnode*		analysisCode( const string& );			//�Ӵ������ɽڵ�,ֻ�ܶ�ȡһ�����ı��ڵ�

		int			getOffs( void );
		void		clearReg( void );		//��������Ĵ���
};

}
#endif
