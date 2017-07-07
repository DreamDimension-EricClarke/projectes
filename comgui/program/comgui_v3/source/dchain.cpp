#ifndef __dchain_cpp__
#define __dchain_cpp__

#include <stdexcept>
#include "dchain.h"
using namespace std;
namespace dream {
	
/***寻找节点指针***/ 
template< typename Tpn >
typename dchain<Tpn>::node* dchain<Tpn>::find( int location ) const
{
	if( location >= 0 )
	{
		node *temp;
		for( temp = this->mpHead; temp != 0 and location != 0; temp = temp->next )
			location -= 1;
		return temp;
	}
	else
	{
		node *temp;
		for( temp = this->mpTail; temp != 0 and location != -1; temp = temp->prev )
			location += 1;
		return temp;
	}
}
	
template< typename Tpn >
dchain<Tpn>::dchain( void )
{
	this->mpHead = 0;		//首节点指针 
	this->mpTail = 0;		//尾节点指针 
	this->mpFocus = 0;	//当前节点指针 
}
/*析构函数将清除链中数据*/
template< typename Tpn >
dchain<Tpn>::~dchain( void )
{
	this->clear();
}

/*在末端添加数据*/
template< typename Tpn >
bool dchain<Tpn>::push( const Tpn& value )
{
	if( 0 == this->mpTail )
	{
		this->mpHead = this->mpTail = this->mpFocus = new node;
		this->mpFocus->next = this->mpFocus->prev = 0;
		this->mpFocus->mValue = value;
		return true;
	}
	this->mpTail->next = new node;
	this->mpTail->next->prev = this->mpTail;
	this->mpTail = this->mpTail->next;
	this->mpTail->next = 0;
	this->mpTail->mValue = value;
}
/*在末端插入一个数据全为0的节点*/
template< typename Tpn >
bool dchain<Tpn>::push( void )
{
	Tpn tp;
	//memset( tp, 0, sizeof( tp ) );
	return this->push( tp );
}
/*在当前位置插入数据*/
template< typename Tpn >
bool dchain<Tpn>::insert( const Tpn& value )
{
	if( 0 == this->mpFocus )
	{
		this->mpHead = this->mpTail = this->mpFocus = new node;
		this->mpFocus->next = this->mpFocus->prev = 0;
		this->mpFocus->mValue = value;
		return true;
	}
	node *temp = new node;
	temp->next = this->mpFocus;
	temp->prev = this->mpFocus->prev;
	if( this->mpFocus->prev != 0 )
		this->mpFocus->prev->next = temp;
	else
		this->mpHead = temp;
	this->mpFocus->prev = temp;
	this->mpFocus->mValue = value;
	return true;
}
/*在当前位置插入一个数据全为0的节点*/
template< typename Tpn >
bool dchain<Tpn>::insert( void )
{
	Tpn tp;
	//memset( tp, 0, sizeof( tp ) );
	return this->insert( tp );
}
/*在特定位置插入数据*/
template< typename Tpn >
bool dchain<Tpn>::insert( const Tpn& value, int location )
{
	node* temp = this->find( location );
	if( 0 == temp )
		return false;
	node *tag = new node;
	tag->next = temp;
	tag->prev = temp->prev;
	if( temp->prev != 0 )
		temp->prev->next = tag;
	else
		this->mpHead = tag;
	temp->prev = tag;
	tag->mValue = value;
	return true;
}

/*删除当前节点*/ 
template< typename Tpn >
bool dchain<Tpn>::remove( void )
{
	if( 0 == this->mpFocus )
		return false;
	node* target = this->mpFocus;
	if( 0 == target->next )
	{
		this->mpTail = this->mpFocus->prev;
		this->mpFocus = target->prev;
	}
	else
	{
		target->next->prev = target->prev;
		this->mpFocus = target->next;
	}
	
	if( 0 == target->prev )
		this->mpHead = target->next;
	else
		target->prev->next = target->next;
	
	delete target;
	return true;
}
/*删除某个节点*/
template< typename Tpn >
bool dchain<Tpn>::remove( int location )
{
	node* target = this->find( location );
	if( 0 == target )
		return false;
	if( this->mpFocus == target )
		return this->remove();
	if( 0 == target->next )
	{
		this->mpTail = this->mpFocus->prev;
		this->mpFocus = target->prev;
	}
	else
	{
		target->next->prev = target->prev;
		this->mpFocus = target->next;
	}
	
	if( 0 == target->prev )
		this->mpHead = target->next;
	else
		target->prev->next = target->next;
	
	delete target;
	return true;
}
/*删除所有节点*/
template< typename Tpn >
void dchain<Tpn>::clear( void )
{
	while( this->remove( 0 ) );
	return;
}

/*寻找某个节点*/
template< typename Tpn >
bool dchain<Tpn>::focus( int location )	//修改当前关注指针的位置 
{
	node* temp = this->find( location );
	if( 0 == temp )
		return false;
	this->mpFocus = temp;
	return true;
}
template< typename Tpn >
bool dchain<Tpn>::next( void )
{
	if( 0 == this->mpFocus or 0 == this->mpFocus->next )
		return false;
	this->mpFocus = this->mpFocus->next;
	return true;
}
template< typename Tpn >
bool dchain<Tpn>::prev( void )
{
	if( 0 == this->mpFocus or 0 == this->mpFocus->prev )
		return false;
	this->mpFocus = this->mpFocus->prev;
	return true;
}
template< typename Tpn >
Tpn* dchain<Tpn>::focus( void )	const//返回当前关注指针的内容 
{
	if( 0 == this->mpFocus )
		return 0;
	return &this->mpFocus->mValue;
}
template< typename Tpn >
Tpn& dchain<Tpn>::ifocus( void ) const//返回当前关注内容的引用 
{
	if( 0 == this->mpFocus )
		throw out_of_range( "no node existing" );
	return this->mpFocus->mValue;
}
template< typename Tpn >
Tpn* dchain<Tpn>::at( int location ) const
{
	node* temp = this->find( location );
	if( 0 == temp )
		return 0;
	return &temp->mValue;
}
template< typename Tpn >
Tpn& dchain<Tpn>::operator[] ( int location ) const	//错误则抛出异常
{
	node* temp = this->find( location );
	if( 0 == temp )
		throw std::out_of_range( "location wrong" );
	return temp->mValue;
}
/*返回当前关注点的位置*/
template< typename Tpn >
int dchain<Tpn>::locat( void ) const
{
	if( 0 == this->mpFocus )
		return -1;
	int location = 0; 
	for( node* temp = this->mpHead; temp != this->mpFocus; temp = temp->next )
		location += 1;
	return location;
}
template< typename Tpn >
bool dchain<Tpn>::bHead( void ) const
{
	if( this->mpHead != this->mpFocus )
		return false;
	else if( this->mpHead == 0 )
		return false;
	else
		return true; 
}
template< typename Tpn >
bool dchain<Tpn>::bTail( void ) const
{
	if( this->mpTail != this->mpFocus )
		return false;
	else if( this->mpTail == 0 )
		return false;
	else
		return true; 
}
template< typename Tpn >
bool dchain<Tpn>::bEmpty( void ) const
{
	if( this->mpHead == 0 )
		return true;
	return false;
}

/*返回当前节点数量*/
template< typename Tpn >
int dchain<Tpn>::count( void ) const
{
	int location = 0; 
	for( node* temp = this->mpHead; temp != 0; temp = temp->next )
		location += 1;
	return location;
}

/*修改内容数据*/
template< typename Tpn >
bool dchain<Tpn>::set( const Tpn& value )
{
	if( 0 == this->mpFocus )
		return false;
	this->mpFocus->mValue = value;
	return true;
}
template< typename Tpn >
bool dchain<Tpn>::set( const Tpn& value, int location )
{
	node* temp = this->find( location );
	if( 0 == temp )
		return false;
	temp->mValue = value;
	return true;
}


}
#endif
