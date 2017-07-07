#ifndef __dxml_cpp__
#define __dxml_cpp__

#include <string>
#include <string.h>
#include "dxml.h"

namespace dream {

/********************dxml常量********************/
static const string dxml_blanklist( " \n\t\r" );

/********************dxmlattr类的成员函数********************/ 

dXmlattr::dXmlattr( string name, const string& value )
{
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlattr::dXmlattr( string, const string& ) : invalid attribute name: " + name );
	if( value.length() == 0 )
		throw invalid_argument( "dXmlattr::dXmlattr( string, const string& ) : empty attribute value" );
	
	this->m_name = name;
	this->m_value = value;
}

string	dXmlattr::getName( void )
{
	return this->m_name;
}
bool	dXmlattr::setName( string name )
{
	if( dXmlanal::confirmName( name ) )
		this->m_name = name;
	else
		return false;
	return true;
}

string	dXmlattr::getValue( void )
{
	return this->m_value;
}
bool	dXmlattr::setValue( const string& value )
{
	if( value.length() == 0 )
		return false;
	
	this->m_value = value;
	return true;
}

/********************dxmlnode类的成员函数********************/
/***构造函数受到控制,所有节点都在堆空间中生成***/
dXmlnode::dXmlnode( string name, dXmlnode* father, bool bText ):	//构造函数设定了基本参数 
	m_text(m_name), m_child(0), m_father(0), m_next(0), m_previous(0),m_btext( bText )
{
	if( !bText and !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::dXmlnode( string, dXmlnode*, bool ):invalid node name: " + name );
	if( father != 0 and father->isText() )
		throw invalid_argument( "dXmlnode::dXmlnode( string, dXmlnode*, bool ):cannet set text node to father " );
	if( bText and father == 0 )
		throw invalid_argument( "dXmlnode::dXmlnode( string, dXmlnode*, bool ):a father pointer needed by text node" );
	
	this->m_name = name;
	
	if( father != 0 )
		father->installChild( this, -1 );
	
	return;
}

dXmlattr*	dXmlnode::findAttr( string name ) const		//按照名字寻找属性节点 
{
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::findAttr( string ):invalid attribute name: " + name );
	dXmlattr* tag;
	int i = 0;
	for( tag = this->m_attr.at(i++); tag != 0; tag = this->m_attr.at(i++) )
		if( tag->getName() == name )
			return tag;
	return tag;
}

/***析构函数用于销毁数据,解散子树***/
dXmlnode::~dXmlnode( void )
{
	if( this->m_father != 0 )
		this->m_father->pickoutChild( this );
	this->clearallAttr();
	this->clearallChild();
	return;
}

/***静态的节点创建函数***/ 
dXmlnode*	dXmlnode::newnode( string name, dXmlnode* father )	//创建一个节点 
{
	return new dXmlnode( name, father, false );
}
dXmlnode*	dXmlnode::newtext( string text, dXmlnode* father )	//创建一段文本 
{
	return new dXmlnode( text, father, true );
}

/***定位函数***/ 
dXmlnode*	dXmlnode::getFather( void )	const					//获取父节点指针 
{
	return this->m_father;
}
dXmlnode*	dXmlnode::getPrevious( string name ) const				//从所有普通节点中筛选拥有特定名称的节点 
{
	if( name.length() == 0 )	//如果输入空字符串则返回上一个节点 
		return this->m_previous;
		
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getPrevious( string ):invalid node name: " + name );
	
	/***向链首方向检索指定节点***/ 
	for( dXmlnode* tag = this->m_previous; tag != 0; tag = tag->m_previous )
		if( !tag->isText() and tag->m_name == name )
			return tag;
	return 0;
}
dXmlnode*	dXmlnode::getNext( string name ) const				//当检索用的字符串为空时检索所有节点
{
	if( name.length() == 0 )	//如果输入空字符串则返回上一个节点 
		return this->m_next;
	
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getNext( string ):invalid node name: " + name );
	
	/***向链尾方向检索指定节点***/ 
	for( dXmlnode* tag = this->m_next; tag != 0; tag = tag->m_next )
		if( !tag->isText() and tag->m_name == name )
			return tag;
	return 0;
}
dXmlnode*	dXmlnode::getBrother( int pos, string name )	//检索所有有名称的兄弟节点
{
	dXmlnode* tag;
	if( pos >= 0 )
		for( tag = this; tag->m_previous != 0; tag = tag->m_previous );	//找到首节点
	else
		for( tag = this; tag->m_next != 0; tag = tag->m_next );	//找到尾节点
	
	if( name.length() != 0 and !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getBrother( int, string ):invalid node name: " + name );
	
	if( pos >= 0 )
		for( ; tag != 0; tag = tag->m_next )
			if( name.length() == 0 and pos-- == 0 )
				return tag;
			else if( tag->m_name == name and pos-- == 0 )
				return tag;
			else;
	else
		for( ; tag != 0; tag = tag->m_previous )
			if( name.length() == 0 and pos++ == -1 )
				return tag;
			else if( tag->m_name == name and pos++ == -1 )
				return tag;
			else;
	
	return 0;
}
dXmlnode*	dXmlnode::getChild( int pos , string name ) const	//获取子节点链首指针,文本节点会抛出异常
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getChild( int,string ):invalid node type" );
		
	if( name.length() != 0 and !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getChild( int, string ):invalid node name: " + name );
	
	if( this->m_child == 0 )
		return 0;
	return this->m_child->getBrother( pos, name );
}
/***子节点操作函数,文本节点会抛出异常***/ 
bool		dXmlnode::installChild( dXmlnode* node, int pos )	//在某位置安装子节点,若目标节点有父节点则失败
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::installChild( dXmlnode*, int ):invalid node type" );
	if( node == 0 or node->m_father != 0 )
		return false;
	dXmlnode* tag;
	if( this->m_child == 0 )
		if( pos == 0 or pos == -1 )		//当子节点链为空时只能在末端或首端安装节点 
		{
			this->m_child = node;
			node->m_father = this;
			return true;
		}
		else
			return false;
	else if( pos >= 0 )
	for( tag = this->m_child; tag != 0 and pos != 0; tag = tag->m_next )
		pos -= 1;
	else
	{
		for( tag = this->m_child; tag->m_next != 0; tag = tag->m_next );
		for( ; tag != 0 and pos != -1; tag = tag->m_previous )
			pos += 1;
	}
	
	if( tag != 0 )
	{
		node->m_father = this;
		if( pos < 0 )
		{
			node->m_previous = tag;
			node->m_next = tag->m_next;
			if( tag->m_next != 0 )
				tag->m_next->m_previous = node;
			tag->m_next = node;
		}
		else
		{
		node->m_previous = tag->m_previous;
		node->m_next = tag;
		if( tag->m_previous != 0 )
			tag->m_previous->m_next = node;
		else
			this->m_child = node;
		tag->m_previous = node;
		}
		return true;
	}
	else
		return false;
}
bool		dXmlnode::pickoutChild( dXmlnode* node )			//若目标节点属于本节点,则踢出
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::pickoutChild( dXmlnode* ):invalid node type" );
	
	if( node == 0 or node->m_father != this )
		return false;
	
	if( node->m_previous != 0 )
		node->m_previous->m_next = node->m_next;
	else
		this->m_child = node->m_next;
	
	if( node->m_next != 0 )
		node->m_next->m_previous = node->m_previous;
	
	node->m_father = 0;
	
	return true;
}
void		dXmlnode::clearallChild( void )				//清除所有子节点
{
	while( this->m_child != 0 )
		delete this->m_child;
}

/***本节点参数操作***/
bool		dXmlnode::isText( void ) const						//判断本节点是否为文本节点 
{
	return this->m_btext;
}

bool		dXmlnode::setName( string name )				//设置本节点的名字,文本节点会抛出异常 
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::setName( string ):invalid node type" );
	if( !dXmlanal::confirmName( name ) )	//对名字查错
		return false;
	
	this->m_name = name;
	return true; 
}
string		dXmlnode::getName( void ) const					//获取本节点的名字,文本节点会抛出异常 
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getName( string ):invalid node type" );
	return this->m_name;
}
bool		dXmlnode::setText( const string& text )				//设置文本内容,对于普通节点会抛出异常 
{
	if( !this->m_btext )
		throw invalid_argument( "dXmlnode::setText( const string& ):invalid node type" );
	if( text.length() == 0 )
		return false;
	this->m_text = text;
	return true;
}
string		dXmlnode::getText( void ) const					//获取文本内容,对于普通节点会抛出异常 
{
	if( !this->m_btext )
		throw invalid_argument( "dXmlnode::getText( void ):invalid node type" );
	return this->m_text;
}

/***任何关于属性的操作对于文本节点都会抛出异常***/
bool		dXmlnode::setAttr( string name, const string& value )	//设置特定属性的参数,没有此属性会自动创建
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::setAttr( string, string ):invalid node type" );
	
	if( !dXmlanal::confirmName( name ) )
		return false;
	if( value.length() == 0 )	//空字符串会导致属性被删除
		return this->removeAttr( name );
	dXmlattr* tag = this->findAttr( name );
	if( tag == 0 )
		this->m_attr.push( dXmlattr( name, value ) );
	else
		tag->setValue( value );
	return true;
}

string		dXmlnode::getAttr( string name ) const				//获取特定属性的参数,没有此属性会抛出异常 
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getAttr( string ):invalid node type" );
	
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getAttr( string ):invalid attribute name: " + name );
	
	dXmlattr* tag = this->findAttr( name );
	if( tag == 0 )
		throw invalid_argument( "dXmlnode::getAttr( string ):no such attribute named:" + name );
		//return "";
	
	return tag->getValue();
}
dXmlattr*	dXmlnode::enumAttr( int pos ) const				//按照顺序列举可见的属性
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getAttr( string ):invalid node type" );
	return this->m_attr.at( pos );
}
bool		dXmlnode::removeAttr( string name )				//删除特定属性
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::removeAttr( string ):invalid node type" );
	
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::removeAttr( string ):invalid attribute name: " + name );
		
	dXmlattr* tag = 0;
	int i = 0;
	string t;
	for( tag = this->m_attr.at(i++); tag != 0; tag = this->m_attr.at(i++) )
	{
		t = tag->getName();
		if( t == name )
		{
			this->m_attr.remove( i );
			return true;
		}
	}
	return false;
}
void		dXmlnode::clearallAttr( void )				//清除所有属性
{
	this->m_attr.clear();
}
/***拷贝函数***/
dXmlnode* dXmlnode::copy( dXmlnode* base ) const						//将本节点的所有内容拷贝成新的节点
{
	if( this->m_btext )	//如果本节点是文本节点 
		return dXmlnode::newtext( this->m_text, base );	//克隆同样的文本节点即可 
	//否则构造新的节点 
	dXmlnode* tag = dXmlnode::newnode( this->m_name, base );
	
	int i = 0;
	dXmlattr* atp;
	for( atp = this->m_attr.at(i++); atp != 0; atp = this->m_attr.at(i++) )
		tag->setAttr( atp->getName(), atp->getValue() );//为新建的节点设置属性 
	//复制所有的子节点 
	for( dXmlnode* cd = this->m_child; cd != 0; cd = cd->m_next )
		cd->copy( tag );
	return tag;
}
/********************dXmlxcas类的成员函数********************/
dXmlxcas::dXmlxcas( void ):	//控制构造函数以保证每个节点文档都是堆数据
	m_rule(0)
{
	this->m_root = dXmlnode::newnode( "xml-case-cading-attribute-sheet" );
}
dXmlxcas::~dXmlxcas( void )
{
	delete this->m_root;
}

bool	dXmlxcas::addRule( string label, bool bRefuse )//添加一条规则
{	//label对应了规则针对的标签名 bRefuse对应了规则是否拒绝上层遗传 
	if( !dXmlanal::confirmName( label ) )
		return false;	//针对的标签名称应是一个合法的标签名 
	if( this->isRule( label ) )
		return false;	//不能重复定义同一规则 
	dXmlnode* tag = dXmlnode::newnode( "rule", this->m_root );
	
	tag->setAttr( "label", label );
	
	if( bRefuse )
		tag->setAttr( "refuse", "true" );
	else
		tag->setAttr( "refuse", "false" );
			
	this->m_rule = tag;
	return true;
}
bool	dXmlxcas::isRule( string label )
{
	if( !dXmlanal::confirmName( label ) )
		return false;
	for( dXmlnode* tag = this->m_root->getChild(); tag != 0; tag = tag->getNext() )
		if( tag->getAttr( "label" ) == label )
			return true;
	return false;
}
bool	dXmlxcas::findRule( string label )			//寻找一条规则 
{
	if( !dXmlanal::confirmName( label ) )
		return false;
	for( dXmlnode* tag = this->m_root->getChild(); tag != 0; tag = tag->getNext() )
		if( tag->getAttr( "label" ) == label )
		{
			this->m_rule = tag;
			return true;
		}
	return false;
}
bool	dXmlxcas::isRefuse( void )				//是否拒绝前辈 
{
	if( this->m_rule == 0 or this->m_rule->getAttr( "refuse" ) == "false" );
		return false;
	return true;
}
bool	dXmlxcas::removeRule( void )			//删除当前规则 
{
	if( this->m_rule == 0 )
		return false;
	dXmlnode* tag = this->m_rule;
	if( this->m_rule->getNext() == 0 )
		this->m_rule = this->m_rule->getPrevious();
	else
		this->m_rule = this->m_rule->getNext();
	
	delete tag;
	return true;
}
void	dXmlxcas::clearRule( void )			//清除所有规则 
{
	this->m_root->clearallChild();
	this->m_rule = 0;
}

bool	dXmlxcas::setAttr( string attrnm, string value, bool inherit )	//为当前规则添加或设置一条属性
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( attrnm ) or !dXmlanal::confirmName( value ) )
		return false;
	
	dXmlnode* tag;
	for( tag = this->m_rule->getChild(); tag != 0; tag = tag->getNext() )
		if( tag->getAttr( "name" ) == attrnm )
			break;
	if( tag == 0 )
	{
		tag = dXmlnode::newnode( "attr", this->m_rule );
		tag->setAttr( "name", attrnm );
	}
	if( inherit )
		tag->setAttr( "inherit", "true" );
	else
		tag->setAttr( "inherit", "false" );
	tag->clearallChild();
	dXmlnode::newtext( value, tag );
	
}
string	dXmlxcas::getAttr( string attrnm )			//获取某属性规则的规则内容若没有此规则则返回空字符串 
{
	if( this->m_rule == 0 )
		return "";
	if( !dXmlanal::confirmName( attrnm ) )
		return "";
	for( dXmlnode* tag = this->m_rule->getChild( 0, "attr" ); tag != 0; tag = tag->getNext( "attr" ) )
		if( tag->getAttr( "name" ) == attrnm )
			return tag->getChild()->getText();
	return "";
		
}
bool	dXmlxcas::isInherit( string attrnm )		//判断某属性是否遗传,没有此属性则视为不遗传 
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( attrnm ) )
		return false;
	for( dXmlnode* tag = this->m_rule->getChild( 0, "attr" ); tag != 0; tag = tag->getNext( "attr" ) )
		if( tag->getAttr( "name" ) == attrnm )
			return (tag->getAttr( "inherit" ) == "true");
}
bool	dXmlxcas::removeAttr( string attrnm )				//删除某条属性规则 
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( attrnm ) )
		return false;
	for( dXmlnode* tag = this->m_rule->getChild( 0, "attr" ); tag != 0; tag = tag->getNext( "attr" ) )
		if( tag->getAttr( "name" ) == attrnm )
		{
			delete tag;
			return true;
		}
	return false;
}
void	dXmlxcas::clearAttr( void )				//清除属性规则 
{
	if( this->m_rule != 0 )
	{
		for( dXmlnode* tag = this->m_rule->getChild( 0, "attr" ); tag != 0; tag = this->m_rule->getChild( 0, "attr" ) )
			delete tag;
	}
}

bool	dXmlxcas::addNode( string nodenm )				//为当前规则添加必备节点 
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( nodenm ) )
		return false;
	if( this->isNode( nodenm ) )
		return false;
	
	dXmlnode *tag = dXmlnode::newnode( "node", this->m_rule );
	tag->setAttr( "name", nodenm );
	
	return true;
	
}
bool	dXmlxcas::isNode( string nodenm )				//判断节点规则是否存在 
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( nodenm ) )
		return false;
	for( dXmlnode* tag = this->m_rule->getChild( 0, "node" ); tag != 0; tag = this->m_rule->getChild( 0, "node" ) )
		if( tag->getAttr( "name" ) == nodenm )
			return true;
	return false;
}
bool	dXmlxcas::removeNode( string nodenm )			//删除某条节点规则 
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( nodenm ) )
		return false;
	for( dXmlnode* tag = this->m_rule->getChild( 0, "node" ); tag != 0; tag = this->m_rule->getChild( 0, "node" ) )
		if( tag->getAttr( "name" ) == nodenm )
		{
			delete tag;
			return true;
		}
	return false;
}
void	dXmlxcas::clearNode( void )				//清除所有节点规则 
{
		if( this->m_rule != 0 )
	{
		for( dXmlnode* tag = this->m_rule->getChild( 0, "node" ); tag != 0; tag = this->m_rule->getChild( 0, "node" ) )
			delete tag;
	}
}

const dXmlnode* dXmlxcas::getTree( void )		//获取整棵规则树  
{
	return this->m_root;
}
/********************dXmlanal类的成员函数********************/ 

string dXmlanal::cutwith( const char* str, const string& endms )	//用结束标志截取一段 
{
	string rtn;
	for( int i = 0; str[i] != '\0'; i++ )
	{
		for( int j = 0; endms[j] != '\0'; j++ )
			if( str[i] == endms[j] )
				return rtn;		
		rtn += str[i];		//返回值不包含结束标志 
	}
	throw invalid_argument( "invalid end" );	//不以标识符结束抛出异常 
}
string dXmlanal::seekfor( const char* str, const string& cntms )	//用容纳标志截取一段
{
	string rtn;
	for( int j,i = 0; str[i] != '\0'; i++ )
	{
		for( j = 0; cntms[j] != '\0'; j++ )
			if( str[i] == cntms[j] )
			{
				rtn += str[i];
				break;
			}
		if( cntms[j] == '\0' )
			break;
	}
	return rtn;
}
dXmlnode* dXmlanal::code2node( dXmlnode* base )	//从代码读取节点 
{
	dXmlnode *node = 0;
	if( base != 0 and base->isText() )
		throw invalid_argument( "dXmlanal::code2node(dXmlnode*):cannet set text node to father node" );
	//如果base不为零则一切非节点内容都被视为文本内容
	while( m_code[m_offs] != '\0' )
	{
		string text(dXmlanal::seekfor( m_code + m_offs, dxml_blanklist ));	//获取有效内容之前的空白 
		string value;
		m_offs += text.length();
		if( m_code[m_offs] == '\0' )	//解析结束 
			return node;
		else if( m_code[m_offs] != '<' )	//开始了一个文本节点
		{
			value = dXmlanal::cutwith( m_code + m_offs, "<" );	//获取到下一个标签开始之前的所有内容
			m_offs += value.length();
			text += value;
			if( base == 0 )
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):cannot read text node without father node" );
			else
				node = dXmlnode::newtext( dXmlanal::transtextC2M( text ), base );	//转换文本值并设置子节点 
		}
		else if( m_code[m_offs+1] == '/' )	//打开了一个结束标签
		{
			 break;	//把结束符号交给上层处理 
		}
		else if( m_code[m_offs+1] == '!' and m_code[m_offs+2] == '-' and m_code[m_offs+3] == '-' )	//开始了注释节点
		{
			for( m_offs += 4; m_code[m_offs] != 0; m_offs += 1 )
				if( m_code[m_offs-1] == '>' and m_code[m_offs-2] == '-' and m_code[m_offs-3] == '-' )
					break;	//当完全越过注释后跳出循环 
		}
		else //开始了一个节点
		{
			m_offs += 1;											//越过标签起始符 
			
			text = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//吞掉多余的空白
			m_offs += text.length();								//偏移指针跳过空白
			
			text = dXmlanal::cutwith( m_code + m_offs, dxml_blanklist + ">" );	//读取标签名
			m_offs += text.length();
			
			if( !dXmlanal::confirmName( text ) )					//对标签名查错并容错 
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of node: " + text );
			node = dXmlnode::newnode( text, base );					//建立节点 
			
			//开始读取属性列表 
			//<--此处应当补充xcas测试
			
			while( m_code[m_offs] != '\0' )
			{
				text = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//跳过空白
				m_offs += text.length();
				
				if( m_code[m_offs] == '>' )	//属性列表结束
					break; 
				else if( m_code[m_offs] == '\0' )	//文档错误的结束了
					invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid end of code" );
				else
				{
					text = dXmlanal::cutwith( m_code + m_offs, "=" );	//读取属性名
					m_offs += text.length();
					
					if( !dXmlanal::confirmName( text ) )	//对属性名查错并容错
						throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of attribute: " + text );
					
					m_offs += 1;	//越过等号
					value = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//读取空白
					m_offs += value.length();
					
					if( m_code[m_offs++] != '\'' )	//确认属性值以单引号开始
						throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid begin of attr value" );
					value = dXmlanal::cutwith( m_code + m_offs, "\'" );	//属性值以单引号开始以单引号结束
					m_offs += value.length();
					m_offs += 1;	//越过单引号 
					value = dXmlanal::transtextC2M( value );	//对属性值进行转换 
					if( !node->setAttr( text, value ) )		//为节点设置属性 
						throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid value of attr: " + value );
				}
			}
			m_offs += 1;	//越过标签结束符号
			this->code2node( node );	//为node读取子节点
			//检查结束标签
			if( m_code[m_offs++] != '<' or m_code[m_offs++] != '/' )
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid end of node" );
				
			text = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//读取多余的空白
			m_offs += text.length();
			
			text = dXmlanal::cutwith( m_code + m_offs, ">" );	//读取结束标签的名字
			m_offs += text.length();
			if( !dXmlanal::confirmName( text ) )	//对标签名查错
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of label" + text );
			if( text != node->getName() )	//判断结束标签的对应关系
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of EndLabel" );
			
			m_offs += 1;	//越过标签结束符 
			if( base == 0 )	//如果没有父节点则只读取一个节点
				break; 
		} 
	}
	return node;
}
bool	dXmlanal::confirmName( string& name )
{
	if( name.length() == 0 )	//名称长度不能为零
		return false;
	string token;
	bool bSpace = false;
	
	for( int i = 0; name[i] != '\0'; i++ ) switch( name[i] )
	{
		case ' ':case '\n':case '\r':case '\t':	//能被视为空白的字符
			if( token.length() != 0 )
				bSpace = true;	//如果已经录入了关键字则标记空白
			break;
		
		/***几乎所有标点都是非法字符***/ 
		case '`':case '~':case '!':case '@':case '#':case '$':
		case '%':case '^':case '&':case '*':case '(':case ')':
		case '=':case '+':case '[':case ']':case '{':case '}':
		case'\\':case '|':case ';':case ':':case'\'':case'\"':
		case ',':case '<':case '.':case '>':case '/':case '?':
			return false;
		
//		case '0' ... '9':
//		case 'a' ... 'z':
//		case 'A' ... 'Z':
//		case '-':case '_':
		default:	//包括数字,大小写英文字母,短线,下划线和任何其他字符集的字符都可以被用作名字
			token += name[i]; 
			break;
	}
	
	/***用过滤了空白的token替换name***/
	name = token;
	return true; 
	
}
string dXmlanal::transtextM2C( const string& text )
{
	string tag;
	for( int i = 0; text[i] != 0; i++ )
		switch( text[i] )
		{
			case'\'':	//apos
				tag += "&apos;";
				break;
			case'\"':
				tag += "&quot;";
				break;
			case'<':
				tag += "&lt;";
				break;
			case '>':
				tag += "&gt;";
				break;
			case '&':
				tag += "&amp;";
				break;
			case'\t':
				tag += "&tab;";
				break;
			case'\r':
				tag += "&rtrn;";
				break;
			case'\n':
				tag += "&endl;";
				break;
			case ' ':
				tag += "&spac;";
				break;
			default:
				tag += text[i];
				break;
		}
	return tag;
}
string dXmlanal::transtextC2M( const string& code )
{
	string value;
	for( int i = 0; code[i] != '\0'; i++ )
		if( code[i] == '&' )
		{
			string tk(dXmlanal::cutwith( code.data() + i, ";" ));
			if( tk == "&apos" )
				value += '\'';
			else if( tk == "&quot" )
				value += '\"';
			else if( tk == "&lt" )
				value += '<';
			else if( tk == "&gt" )
				value += '>';
			else if( tk == "&amp" )
				value += '&';
			else if( tk == "&rtrn" )
				value += '\r';
			else if( tk == "&endl" )
				value += '\n';
			else if( tk == "&tab" )
				value += '\t';
			else if( tk == "&spac" )
				value += ' ';
			else
				throw invalid_argument( "dXmlanal::transtextC2M(const string&):invalid translation key word: " + tk );
			i += tk.length();	//令指针指向分号等待跳跃 
		}
		else if( code[i] == '\'' or code[i] == '\"' or code[i] == ';' or code[i] == '<' or code[i] == '>' )
			throw invalid_argument( "dXmlanal::transtextC2M(const string&):invalid charactor in text or value: " + code[i] );
		else
			value += code[i];
	return value;
}
		
dXmlanal::dXmlanal( void ):		//解析器初始化 
m_version(1), m_code(0), m_offs(0), m_xcas(0)
{
	
}
dXmlanal::~dXmlanal( void )		//销毁时要清理空间
{
	if( this->m_xcas != 0 )
		delete this->m_xcas;
}

string		dXmlanal::generatCode( const dXmlnode* node )	//从节点生成代码
{
	if( node == 0 )
		throw invalid_argument( "dXmlanal::generatCode(const dXmlnode*):no node provided" );
	
	/***如果是文本节点则忽略tabs直接转意输出***/ 
	if( node->isText() )
		return dXmlanal::transtextM2C( node->getText() );
	/***普通节点则要构造完整的节点体系***/ 
	string code( "< " + node->getName() + " " );	//添加名字 
	
	//书写属性值
	int i = 0;
	dXmlattr* attr;
	for( attr = node->enumAttr( i++ ); attr != 0; attr = node->enumAttr( i++ ) )
		code += attr->getName() + "= \'" + dXmlanal::transtextM2C( attr->getValue() ) + "\' ";	//用单引号包括属性值
	
	code += ">";	//标签已经打开
	for( dXmlnode* cld = node->getChild(); cld != 0; cld = cld->getNext() )
		code += dXmlanal::generatCode( cld );	//书写所有子节点
	
	code += "</ " + node->getName() + " >";	//书写关闭标签 
	
	return code;
}
dXmlnode*	dXmlanal::analysisCode( const string& code )			//从代码生成节点,只能读取一个非文本节点
{
	if( code.length() == 0 )
		return 0;	//如果代码没有内容,理应不产生任何结果 
	
	this->clearReg();		//清除上次解析代码产生的寄存数据 
	//拷贝代码
	this->m_code = new char[code.length() + 1];
	this->m_code[code.length()] = '\0';
	strcpy( this->m_code, code.data() );
	
	return this->code2node( 0 );	//从代码读取节点 
}
int			dXmlanal::getOffs( void )
{
	return this->m_offs;
}
void		dXmlanal::clearReg( void )		//清除解析寄存器
{
	if( this->m_code != 0 )
	{
		delete[] this->m_code;
		this->m_code = 0;
	}
	this->m_offs = 0;
	this->m_xcas = 0;
}
}
#endif
