#ifndef __CHAIN_CPP__
#define __CHAIN_CPP__

#include "chain.h"
#include <stdexcept>
using namespace std;
namespace dream {
	
/***Ѱ�ҽڵ�ָ��***/ 
template< typename Tpn >
typename chain<Tpn>::node* chain<Tpn>::find( int location )
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
chain<Tpn>::chain( void )
{
	this->mpHead = 0;		//�׽ڵ�ָ�� 
	this->mpTail = 0;		//β�ڵ�ָ�� 
	this->mpFocus = 0;	//��ǰ�ڵ�ָ�� 
}
/*���������������������*/
template< typename Tpn >
chain<Tpn>::~chain( void )
{
	this->clear();
}

/*��ĩ���������*/
template< typename Tpn >
bool chain<Tpn>::push( const Tpn& value )
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
/*��ĩ�˲���һ������ȫΪ0�Ľڵ�*/
template< typename Tpn >
bool chain<Tpn>::push( void )
{
	Tpn tp;
	//memset( tp, 0, sizeof( tp ) );
	return this->push( tp );
}
/*�ڵ�ǰλ�ò�������*/
template< typename Tpn >
bool chain<Tpn>::insert( const Tpn& value )
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
/*�ڵ�ǰλ�ò���һ������ȫΪ0�Ľڵ�*/
template< typename Tpn >
bool chain<Tpn>::insert( void )
{
	Tpn tp;
	//memset( tp, 0, sizeof( tp ) );
	return this->insert( tp );
}
/*���ض�λ�ò�������*/
template< typename Tpn >
bool chain<Tpn>::insert( const Tpn& value, int location )
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

/*ɾ����ǰ�ڵ�*/ 
template< typename Tpn >
bool chain<Tpn>::remove( void )
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
/*ɾ��ĳ���ڵ�*/
template< typename Tpn >
bool chain<Tpn>::remove( int location )
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
/*ɾ�����нڵ�*/
template< typename Tpn >
void chain<Tpn>::clear( void )
{
	while( this->remove( 0 ) );
	return;
}

/*Ѱ��ĳ���ڵ�*/
template< typename Tpn >
bool chain<Tpn>::focus( int location )	//�޸ĵ�ǰ��עָ���λ�� 
{
	node* temp = this->find( location );
	if( 0 == temp )
		return false;
	this->mpFocus = temp;
	return true;
}
template< typename Tpn >
bool chain<Tpn>::next( void )
{
	if( 0 == this->mpFocus or 0 == this->mpFocus->next )
		return false;
	this->mpFocus = this->mpFocus->next;
	return true;
}
template< typename Tpn >
bool chain<Tpn>::prev( void )
{
	if( 0 == this->mpFocus or 0 == this->mpFocus->prev )
		return false;
	this->mpFocus = this->mpFocus->prev;
	return true;
}
template< typename Tpn >
Tpn* chain<Tpn>::focus( void )	//���ص�ǰ��עָ������� 
{
	if( 0 == this->mpFocus )
		return 0;
	return &this->mpFocus->mValue;
}
template< typename Tpn >
Tpn& chain<Tpn>::ifocus( void )//���ص�ǰ��ע���ݵ����� 
{
	if( 0 == this->mpFocus )
		throw out_of_range( "no node existing" );
	return this->mpFocus->mValue;
}
template< typename Tpn >
Tpn* chain<Tpn>::at( int location )
{
	node* temp = this->find( location );
	if( 0 == temp )
		return 0;
	return &temp->mValue;
}
template< typename Tpn >
Tpn& chain<Tpn>::operator[] ( int location )	//�������׳��쳣
{
	node* temp = this->find( location );
	if( 0 == temp )
		throw std::out_of_range( "location wrong" );
	return temp->mValue;
}
/*���ص�ǰ��ע���λ��*/
template< typename Tpn >
int chain<Tpn>::locat( void )
{
	if( 0 == this->mpFocus )
		return -1;
	int location = 0; 
	for( node* temp = this->mpHead; temp != this->mpFocus; temp = temp->next )
		location += 1;
	return location;
}
template< typename Tpn >
bool chain<Tpn>::bHead( void )
{
	if( this->mpHead != this->mpFocus )
		return false;
	else if( this->mpHead == 0 )
		return false;
	else
		return true; 
}
template< typename Tpn >
bool chain<Tpn>::bTail( void )
{
	if( this->mpTail != this->mpFocus )
		return false;
	else if( this->mpTail == 0 )
		return false;
	else
		return true; 
}
template< typename Tpn >
bool chain<Tpn>::bEmpty( void )
{
	if( this->mpHead == 0 )
		return true;
	return false;
}

/*���ص�ǰ�ڵ�����*/
template< typename Tpn >
int chain<Tpn>::count( void )
{
	int location = 0; 
	for( node* temp = this->mpHead; temp != 0; temp = temp->next )
		location += 1;
	return location;
}

/*�޸���������*/
template< typename Tpn >
bool chain<Tpn>::set( const Tpn& value )
{
	if( 0 == this->mpFocus )
		return false;
	this->mpFocus->mValue = value;
	return true;
}
template< typename Tpn >
bool chain<Tpn>::set( const Tpn& value, int location )
{
	node* temp = this->find( location );
	if( 0 == temp )
		return false;
	temp->mValue = value;
	return true;
}


}
#endif
