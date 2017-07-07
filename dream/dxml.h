#ifndef __dxml__
#define __dxml__
#include <stdexcept>
#include <string>

#include "dream.h"
#include "dchain.h"
/***
	dXml是dream dimension对xml的实现
	dxml使用更为严格的语法要求:
		dxml对注释不进行任何操作 
		dxml要求所有属性值必须用单引号包括,属性值不能为空
		dxml要求任何名字内不允许出现'-'和'_'以外的任何标准标点,不能包含空白 
		dxml要求注释起始符<!--和注释终止符-->没有中断
		dxml要求使用<? dxml ... ... ?>对文档进行定义
			version dxml版本号
		dxml 使用dxcas 属性为一棵树定制默认属性和必要节点 
		dxml 认为 ' ' '\n' '\r' '\t'都是空白符,当标签之间只有空白符时,不产生文本节点 
	dxml使用更多的转意实体:
		&lt;		'<'
		&gt;		'>'
		&apos;		'\''
		&quot;		'\"'
		&amp; 		'&'
		&cr;		'\r'
		&lf;		'\n'
		&tab;		'\t'
		&spac;		' '
	dxml要求关闭标签使用</开头而不是< /或其他 
***/

namespace dream {

#define __dxml_def_attr_name "attribute"
#define __dxml_def_attr_value "value"

class dXmlnode;	//dxml节点类,用于实现文档节点
class dXmlattr;	//dxml属性类
class dXmllexer;	//dxml解析器
typedef dchain<dXmlattr> dXmlatls;	//dxml节点属性列表

/***dxml节点属性类***/
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

/***dxmlnode类用来实现一个dxml文档节点***/ 
/***节点可以有两种模式,普通节点和文本节点***/
/***文本节点必须拥有父节点***/ 
class dXmlnode {
	private: 
		dXmlnode*	m_father;	//指向父节点的指针 
		const bool	m_btext;	//明确节点类型 
		
		dXmlnode*	m_previous;	//上一个节点 
		dXmlnode*	m_next;		//下一个节点 
		dXmlnode*	m_child;	//子节点链中的首节点 
		
		string		m_name;		//节点名字 
		dXmlatls	m_attr;		//节点属性列表
		
		string&		m_text;		//节点内容,保存文本值的内存状态 
	private:
		/***构造函数受到控制,所有节点都在堆空间中生成***/
		dXmlnode( string, dXmlnode*, bool );	//构造函数设定了基本参数 
		
		dXmlattr*	findAttr( string ) const;		//按照名字寻找属性节点 
	public:
		
		/***析构函数用于销毁数据,解散子树***/
		~dXmlnode( void ); 
		
		/***静态的节点创建函数***/ 
		static dXmlnode*	newnode( string = "", dXmlnode* = 0 );		//创建一个节点 
		static dXmlnode*	newtext( string, dXmlnode* );				//创建一段文本
		static dXmlnode*	newcontent( string, string, dXmlnode* );	//创建一个带有文本子节点的节点
		
		/***定位函数***/ 
		dXmlnode*	getFather( void ) const;					//获取父节点指针 
		dXmlnode*	getPrevious( string = "" ) const;			//从所有普通节点中筛选拥有特定名称的节点 
		dXmlnode*	getNext( string = "" ) const;				//当检索用的字符串为空时检索所有节点
		dXmlnode*	getBrother( int = 0, string = "" );			//检索所有有名称的兄弟节点
		dXmlnode*	getChild( int = 0, string = "" ) const;		//获取子节点链首指针,文本节点会抛出异常
		
		/***子节点操作函数,文本节点会抛出异常***/ 
		bool		installChild( dXmlnode*, int = -1 );		//在某位置安装子节点,若目标节点有父节点则失败
		bool		pickoutChild( dXmlnode* );					//若目标节点属于本节点,则踢出
		void		clearallChild( void );						//清除所有子节点
		
		/***本节点参数操作***/
		bool		isText( void )const;			//判断本节点是否为文本节点 
		
		bool		setName( string );				//设置本节点的名字,文本节点会抛出异常 
		string		getName( void ) const;			//获取本节点的名字,文本节点会抛出异常 
		bool		setText( const string& );		//设置文本内容,对于普通节点会抛出异常 
		
		string		getText( void ) const;			//获取文本内容,对于普通节点会抛出异常 
		bool		getText( string& ) const;		//获取文本内容,对于普通节点会失败 
		
		string		getContent( string , int = 0 ) const;			//获取某子节点的文本子节点内容,若不存在则抛出异常 
		bool		getContent( string&, string, int = 0 ) const;	//获取某子节点的文本子节点内容,若不存在则失败
		
		bool		setContent( const string&, string, int = 0 );		//设置一个节点的文本子节点 
		
		/***任何关于属性的操作对于文本节点都会抛出异常***/ 
		bool		setAttr( string, const string& );	//设置特定属性的参数,没有此属性会自动创建 
		string		getAttr( string ) const;			//获取特定属性的参数,没有此属性会抛出异常 
		bool		getAttr( string, string& ) const;	//获取特定属性的参数,没有则返回false 
		dXmlattr*	enumAttr( int ) const;				//按照顺序列举可见的属性 
		bool		removeAttr( string );				//删除特定属性 
		void		clearallAttr( void );				//清除所有属性
		
		/***拷贝函数***/
		dXmlnode* copy( dXmlnode* = 0 ) const;			//将本节点的所有内容拷贝成新的节点
};
/***dxml解析器类***/ 
/***所有涉及代码语法的规范都集成在此类中***/ 
class dXmllexer {
	private:
		int				m_version;	//所使用的dxml版本号
		/***解析后保留以便查阅的参数***/
		char*			m_code;		//文档源代码 
		int				m_offs;		//当前偏移量
	private:
		static string cutwith( const char*, const string& );	//用结束标志截取一段 
		static string seekfor( const char*, const string& );	//用容纳标志截取一段 
		dXmlnode* code2node( dXmlnode* );						//从代码读取节点,若设置了父节点则为父节点读取所有子节点(但保留结束标签),否则读取一个节点 
		void		clearReg( void );							//清除解析寄存器
		dXmllexer( void );										//解析器初始化 
		~dXmllexer( void );		//销毁时要清理空间
	public:
		
		static bool		confirmName( string& );
		static string	transtextM2C( const string& );
		static string	transtextC2M( const string& );
		
		static string		generatCode( const dXmlnode* );		//从节点生成代码
		static dXmlnode*	lexerCode( const string& );			//从代码生成节点,只能读取一个非文本节点
};

}
#endif
