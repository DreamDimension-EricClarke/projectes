#ifndef __dxml_cpp__
#define __dxml_cpp__

#include <string>
#include <string.h>
#include "dxml.h"

namespace dream {

/********************dxml����********************/
static const string dxml_blanklist( " \n\t\r" );

/********************dxmlattr��ĳ�Ա����********************/ 

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

/********************dxmlnode��ĳ�Ա����********************/
/***���캯���ܵ�����,���нڵ㶼�ڶѿռ�������***/
dXmlnode::dXmlnode( string name, dXmlnode* father, bool bText ):	//���캯���趨�˻������� 
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

dXmlattr*	dXmlnode::findAttr( string name ) const		//��������Ѱ�����Խڵ� 
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

/***��������������������,��ɢ����***/
dXmlnode::~dXmlnode( void )
{
	if( this->m_father != 0 )
		this->m_father->pickoutChild( this );
	this->clearallAttr();
	this->clearallChild();
	return;
}

/***��̬�Ľڵ㴴������***/ 
dXmlnode*	dXmlnode::newnode( string name, dXmlnode* father )	//����һ���ڵ� 
{
	return new dXmlnode( name, father, false );
}
dXmlnode*	dXmlnode::newtext( string text, dXmlnode* father )	//����һ���ı� 
{
	return new dXmlnode( text, father, true );
}

/***��λ����***/ 
dXmlnode*	dXmlnode::getFather( void )	const					//��ȡ���ڵ�ָ�� 
{
	return this->m_father;
}
dXmlnode*	dXmlnode::getPrevious( string name ) const				//��������ͨ�ڵ���ɸѡӵ���ض����ƵĽڵ� 
{
	if( name.length() == 0 )	//���������ַ����򷵻���һ���ڵ� 
		return this->m_previous;
		
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getPrevious( string ):invalid node name: " + name );
	
	/***�����׷������ָ���ڵ�***/ 
	for( dXmlnode* tag = this->m_previous; tag != 0; tag = tag->m_previous )
		if( !tag->isText() and tag->m_name == name )
			return tag;
	return 0;
}
dXmlnode*	dXmlnode::getNext( string name ) const				//�������õ��ַ���Ϊ��ʱ�������нڵ�
{
	if( name.length() == 0 )	//���������ַ����򷵻���һ���ڵ� 
		return this->m_next;
	
	if( !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getNext( string ):invalid node name: " + name );
	
	/***����β�������ָ���ڵ�***/ 
	for( dXmlnode* tag = this->m_next; tag != 0; tag = tag->m_next )
		if( !tag->isText() and tag->m_name == name )
			return tag;
	return 0;
}
dXmlnode*	dXmlnode::getBrother( int pos, string name )	//�������������Ƶ��ֵܽڵ�
{
	dXmlnode* tag;
	if( pos >= 0 )
		for( tag = this; tag->m_previous != 0; tag = tag->m_previous );	//�ҵ��׽ڵ�
	else
		for( tag = this; tag->m_next != 0; tag = tag->m_next );	//�ҵ�β�ڵ�
	
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
dXmlnode*	dXmlnode::getChild( int pos , string name ) const	//��ȡ�ӽڵ�����ָ��,�ı��ڵ���׳��쳣
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getChild( int,string ):invalid node type" );
		
	if( name.length() != 0 and !dXmlanal::confirmName( name ) )
		throw invalid_argument( "dXmlnode::getChild( int, string ):invalid node name: " + name );
	
	if( this->m_child == 0 )
		return 0;
	return this->m_child->getBrother( pos, name );
}
/***�ӽڵ��������,�ı��ڵ���׳��쳣***/ 
bool		dXmlnode::installChild( dXmlnode* node, int pos )	//��ĳλ�ð�װ�ӽڵ�,��Ŀ��ڵ��и��ڵ���ʧ��
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::installChild( dXmlnode*, int ):invalid node type" );
	if( node == 0 or node->m_father != 0 )
		return false;
	dXmlnode* tag;
	if( this->m_child == 0 )
		if( pos == 0 or pos == -1 )		//���ӽڵ���Ϊ��ʱֻ����ĩ�˻��׶˰�װ�ڵ� 
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
bool		dXmlnode::pickoutChild( dXmlnode* node )			//��Ŀ��ڵ����ڱ��ڵ�,���߳�
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
void		dXmlnode::clearallChild( void )				//��������ӽڵ�
{
	while( this->m_child != 0 )
		delete this->m_child;
}

/***���ڵ��������***/
bool		dXmlnode::isText( void ) const						//�жϱ��ڵ��Ƿ�Ϊ�ı��ڵ� 
{
	return this->m_btext;
}

bool		dXmlnode::setName( string name )				//���ñ��ڵ������,�ı��ڵ���׳��쳣 
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::setName( string ):invalid node type" );
	if( !dXmlanal::confirmName( name ) )	//�����ֲ��
		return false;
	
	this->m_name = name;
	return true; 
}
string		dXmlnode::getName( void ) const					//��ȡ���ڵ������,�ı��ڵ���׳��쳣 
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getName( string ):invalid node type" );
	return this->m_name;
}
bool		dXmlnode::setText( const string& text )				//�����ı�����,������ͨ�ڵ���׳��쳣 
{
	if( !this->m_btext )
		throw invalid_argument( "dXmlnode::setText( const string& ):invalid node type" );
	if( text.length() == 0 )
		return false;
	this->m_text = text;
	return true;
}
string		dXmlnode::getText( void ) const					//��ȡ�ı�����,������ͨ�ڵ���׳��쳣 
{
	if( !this->m_btext )
		throw invalid_argument( "dXmlnode::getText( void ):invalid node type" );
	return this->m_text;
}

/***�κι������ԵĲ��������ı��ڵ㶼���׳��쳣***/
bool		dXmlnode::setAttr( string name, const string& value )	//�����ض����ԵĲ���,û�д����Ի��Զ�����
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::setAttr( string, string ):invalid node type" );
	
	if( !dXmlanal::confirmName( name ) )
		return false;
	if( value.length() == 0 )	//���ַ����ᵼ�����Ա�ɾ��
		return this->removeAttr( name );
	dXmlattr* tag = this->findAttr( name );
	if( tag == 0 )
		this->m_attr.push( dXmlattr( name, value ) );
	else
		tag->setValue( value );
	return true;
}

string		dXmlnode::getAttr( string name ) const				//��ȡ�ض����ԵĲ���,û�д����Ի��׳��쳣 
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
dXmlattr*	dXmlnode::enumAttr( int pos ) const				//����˳���оٿɼ�������
{
	if( this->m_btext )
		throw invalid_argument( "dXmlnode::getAttr( string ):invalid node type" );
	return this->m_attr.at( pos );
}
bool		dXmlnode::removeAttr( string name )				//ɾ���ض�����
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
void		dXmlnode::clearallAttr( void )				//�����������
{
	this->m_attr.clear();
}
/***��������***/
dXmlnode* dXmlnode::copy( dXmlnode* base ) const						//�����ڵ���������ݿ������µĽڵ�
{
	if( this->m_btext )	//������ڵ����ı��ڵ� 
		return dXmlnode::newtext( this->m_text, base );	//��¡ͬ�����ı��ڵ㼴�� 
	//�������µĽڵ� 
	dXmlnode* tag = dXmlnode::newnode( this->m_name, base );
	
	int i = 0;
	dXmlattr* atp;
	for( atp = this->m_attr.at(i++); atp != 0; atp = this->m_attr.at(i++) )
		tag->setAttr( atp->getName(), atp->getValue() );//Ϊ�½��Ľڵ��������� 
	//�������е��ӽڵ� 
	for( dXmlnode* cd = this->m_child; cd != 0; cd = cd->m_next )
		cd->copy( tag );
	return tag;
}
/********************dXmlxcas��ĳ�Ա����********************/
dXmlxcas::dXmlxcas( void ):	//���ƹ��캯���Ա�֤ÿ���ڵ��ĵ����Ƕ�����
	m_rule(0)
{
	this->m_root = dXmlnode::newnode( "xml-case-cading-attribute-sheet" );
}
dXmlxcas::~dXmlxcas( void )
{
	delete this->m_root;
}

bool	dXmlxcas::addRule( string label, bool bRefuse )//���һ������
{	//label��Ӧ�˹�����Եı�ǩ�� bRefuse��Ӧ�˹����Ƿ�ܾ��ϲ��Ŵ� 
	if( !dXmlanal::confirmName( label ) )
		return false;	//��Եı�ǩ����Ӧ��һ���Ϸ��ı�ǩ�� 
	if( this->isRule( label ) )
		return false;	//�����ظ�����ͬһ���� 
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
bool	dXmlxcas::findRule( string label )			//Ѱ��һ������ 
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
bool	dXmlxcas::isRefuse( void )				//�Ƿ�ܾ�ǰ�� 
{
	if( this->m_rule == 0 or this->m_rule->getAttr( "refuse" ) == "false" );
		return false;
	return true;
}
bool	dXmlxcas::removeRule( void )			//ɾ����ǰ���� 
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
void	dXmlxcas::clearRule( void )			//������й��� 
{
	this->m_root->clearallChild();
	this->m_rule = 0;
}

bool	dXmlxcas::setAttr( string attrnm, string value, bool inherit )	//Ϊ��ǰ������ӻ�����һ������
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
string	dXmlxcas::getAttr( string attrnm )			//��ȡĳ���Թ���Ĺ���������û�д˹����򷵻ؿ��ַ��� 
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
bool	dXmlxcas::isInherit( string attrnm )		//�ж�ĳ�����Ƿ��Ŵ�,û�д���������Ϊ���Ŵ� 
{
	if( this->m_rule == 0 )
		return false;
	if( !dXmlanal::confirmName( attrnm ) )
		return false;
	for( dXmlnode* tag = this->m_rule->getChild( 0, "attr" ); tag != 0; tag = tag->getNext( "attr" ) )
		if( tag->getAttr( "name" ) == attrnm )
			return (tag->getAttr( "inherit" ) == "true");
}
bool	dXmlxcas::removeAttr( string attrnm )				//ɾ��ĳ�����Թ��� 
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
void	dXmlxcas::clearAttr( void )				//������Թ��� 
{
	if( this->m_rule != 0 )
	{
		for( dXmlnode* tag = this->m_rule->getChild( 0, "attr" ); tag != 0; tag = this->m_rule->getChild( 0, "attr" ) )
			delete tag;
	}
}

bool	dXmlxcas::addNode( string nodenm )				//Ϊ��ǰ������ӱر��ڵ� 
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
bool	dXmlxcas::isNode( string nodenm )				//�жϽڵ�����Ƿ���� 
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
bool	dXmlxcas::removeNode( string nodenm )			//ɾ��ĳ���ڵ���� 
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
void	dXmlxcas::clearNode( void )				//������нڵ���� 
{
		if( this->m_rule != 0 )
	{
		for( dXmlnode* tag = this->m_rule->getChild( 0, "node" ); tag != 0; tag = this->m_rule->getChild( 0, "node" ) )
			delete tag;
	}
}

const dXmlnode* dXmlxcas::getTree( void )		//��ȡ���ù�����  
{
	return this->m_root;
}
/********************dXmlanal��ĳ�Ա����********************/ 

string dXmlanal::cutwith( const char* str, const string& endms )	//�ý�����־��ȡһ�� 
{
	string rtn;
	for( int i = 0; str[i] != '\0'; i++ )
	{
		for( int j = 0; endms[j] != '\0'; j++ )
			if( str[i] == endms[j] )
				return rtn;		
		rtn += str[i];		//����ֵ������������־ 
	}
	throw invalid_argument( "invalid end" );	//���Ա�ʶ�������׳��쳣 
}
string dXmlanal::seekfor( const char* str, const string& cntms )	//�����ɱ�־��ȡһ��
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
dXmlnode* dXmlanal::code2node( dXmlnode* base )	//�Ӵ����ȡ�ڵ� 
{
	dXmlnode *node = 0;
	if( base != 0 and base->isText() )
		throw invalid_argument( "dXmlanal::code2node(dXmlnode*):cannet set text node to father node" );
	//���base��Ϊ����һ�зǽڵ����ݶ�����Ϊ�ı�����
	while( m_code[m_offs] != '\0' )
	{
		string text(dXmlanal::seekfor( m_code + m_offs, dxml_blanklist ));	//��ȡ��Ч����֮ǰ�Ŀհ� 
		string value;
		m_offs += text.length();
		if( m_code[m_offs] == '\0' )	//�������� 
			return node;
		else if( m_code[m_offs] != '<' )	//��ʼ��һ���ı��ڵ�
		{
			value = dXmlanal::cutwith( m_code + m_offs, "<" );	//��ȡ����һ����ǩ��ʼ֮ǰ����������
			m_offs += value.length();
			text += value;
			if( base == 0 )
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):cannot read text node without father node" );
			else
				node = dXmlnode::newtext( dXmlanal::transtextC2M( text ), base );	//ת���ı�ֵ�������ӽڵ� 
		}
		else if( m_code[m_offs+1] == '/' )	//����һ��������ǩ
		{
			 break;	//�ѽ������Ž����ϲ㴦�� 
		}
		else if( m_code[m_offs+1] == '!' and m_code[m_offs+2] == '-' and m_code[m_offs+3] == '-' )	//��ʼ��ע�ͽڵ�
		{
			for( m_offs += 4; m_code[m_offs] != 0; m_offs += 1 )
				if( m_code[m_offs-1] == '>' and m_code[m_offs-2] == '-' and m_code[m_offs-3] == '-' )
					break;	//����ȫԽ��ע�ͺ�����ѭ�� 
		}
		else //��ʼ��һ���ڵ�
		{
			m_offs += 1;											//Խ����ǩ��ʼ�� 
			
			text = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//�̵�����Ŀհ�
			m_offs += text.length();								//ƫ��ָ�������հ�
			
			text = dXmlanal::cutwith( m_code + m_offs, dxml_blanklist + ">" );	//��ȡ��ǩ��
			m_offs += text.length();
			
			if( !dXmlanal::confirmName( text ) )					//�Ա�ǩ������ݴ� 
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of node: " + text );
			node = dXmlnode::newnode( text, base );					//�����ڵ� 
			
			//��ʼ��ȡ�����б� 
			//<--�˴�Ӧ������xcas����
			
			while( m_code[m_offs] != '\0' )
			{
				text = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//�����հ�
				m_offs += text.length();
				
				if( m_code[m_offs] == '>' )	//�����б����
					break; 
				else if( m_code[m_offs] == '\0' )	//�ĵ�����Ľ�����
					invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid end of code" );
				else
				{
					text = dXmlanal::cutwith( m_code + m_offs, "=" );	//��ȡ������
					m_offs += text.length();
					
					if( !dXmlanal::confirmName( text ) )	//������������ݴ�
						throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of attribute: " + text );
					
					m_offs += 1;	//Խ���Ⱥ�
					value = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//��ȡ�հ�
					m_offs += value.length();
					
					if( m_code[m_offs++] != '\'' )	//ȷ������ֵ�Ե����ſ�ʼ
						throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid begin of attr value" );
					value = dXmlanal::cutwith( m_code + m_offs, "\'" );	//����ֵ�Ե����ſ�ʼ�Ե����Ž���
					m_offs += value.length();
					m_offs += 1;	//Խ�������� 
					value = dXmlanal::transtextC2M( value );	//������ֵ����ת�� 
					if( !node->setAttr( text, value ) )		//Ϊ�ڵ��������� 
						throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid value of attr: " + value );
				}
			}
			m_offs += 1;	//Խ����ǩ��������
			this->code2node( node );	//Ϊnode��ȡ�ӽڵ�
			//��������ǩ
			if( m_code[m_offs++] != '<' or m_code[m_offs++] != '/' )
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid end of node" );
				
			text = dXmlanal::seekfor( m_code + m_offs, dxml_blanklist );	//��ȡ����Ŀհ�
			m_offs += text.length();
			
			text = dXmlanal::cutwith( m_code + m_offs, ">" );	//��ȡ������ǩ������
			m_offs += text.length();
			if( !dXmlanal::confirmName( text ) )	//�Ա�ǩ�����
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of label" + text );
			if( text != node->getName() )	//�жϽ�����ǩ�Ķ�Ӧ��ϵ
				throw invalid_argument( "dXmlanal::code2node(dXmlnode*):invalid name of EndLabel" );
			
			m_offs += 1;	//Խ����ǩ������ 
			if( base == 0 )	//���û�и��ڵ���ֻ��ȡһ���ڵ�
				break; 
		} 
	}
	return node;
}
bool	dXmlanal::confirmName( string& name )
{
	if( name.length() == 0 )	//���Ƴ��Ȳ���Ϊ��
		return false;
	string token;
	bool bSpace = false;
	
	for( int i = 0; name[i] != '\0'; i++ ) switch( name[i] )
	{
		case ' ':case '\n':case '\r':case '\t':	//�ܱ���Ϊ�հ׵��ַ�
			if( token.length() != 0 )
				bSpace = true;	//����Ѿ�¼���˹ؼ������ǿհ�
			break;
		
		/***�������б�㶼�ǷǷ��ַ�***/ 
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
		default:	//��������,��СдӢ����ĸ,����,�»��ߺ��κ������ַ������ַ������Ա���������
			token += name[i]; 
			break;
	}
	
	/***�ù����˿հ׵�token�滻name***/
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
			i += tk.length();	//��ָ��ָ��ֺŵȴ���Ծ 
		}
		else if( code[i] == '\'' or code[i] == '\"' or code[i] == ';' or code[i] == '<' or code[i] == '>' )
			throw invalid_argument( "dXmlanal::transtextC2M(const string&):invalid charactor in text or value: " + code[i] );
		else
			value += code[i];
	return value;
}
		
dXmlanal::dXmlanal( void ):		//��������ʼ�� 
m_version(1), m_code(0), m_offs(0), m_xcas(0)
{
	
}
dXmlanal::~dXmlanal( void )		//����ʱҪ����ռ�
{
	if( this->m_xcas != 0 )
		delete this->m_xcas;
}

string		dXmlanal::generatCode( const dXmlnode* node )	//�ӽڵ����ɴ���
{
	if( node == 0 )
		throw invalid_argument( "dXmlanal::generatCode(const dXmlnode*):no node provided" );
	
	/***������ı��ڵ������tabsֱ��ת�����***/ 
	if( node->isText() )
		return dXmlanal::transtextM2C( node->getText() );
	/***��ͨ�ڵ���Ҫ���������Ľڵ���ϵ***/ 
	string code( "< " + node->getName() + " " );	//������� 
	
	//��д����ֵ
	int i = 0;
	dXmlattr* attr;
	for( attr = node->enumAttr( i++ ); attr != 0; attr = node->enumAttr( i++ ) )
		code += attr->getName() + "= \'" + dXmlanal::transtextM2C( attr->getValue() ) + "\' ";	//�õ����Ű�������ֵ
	
	code += ">";	//��ǩ�Ѿ���
	for( dXmlnode* cld = node->getChild(); cld != 0; cld = cld->getNext() )
		code += dXmlanal::generatCode( cld );	//��д�����ӽڵ�
	
	code += "</ " + node->getName() + " >";	//��д�رձ�ǩ 
	
	return code;
}
dXmlnode*	dXmlanal::analysisCode( const string& code )			//�Ӵ������ɽڵ�,ֻ�ܶ�ȡһ�����ı��ڵ�
{
	if( code.length() == 0 )
		return 0;	//�������û������,��Ӧ�������κν�� 
	
	this->clearReg();		//����ϴν�����������ļĴ����� 
	//��������
	this->m_code = new char[code.length() + 1];
	this->m_code[code.length()] = '\0';
	strcpy( this->m_code, code.data() );
	
	return this->code2node( 0 );	//�Ӵ����ȡ�ڵ� 
}
int			dXmlanal::getOffs( void )
{
	return this->m_offs;
}
void		dXmlanal::clearReg( void )		//��������Ĵ���
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
