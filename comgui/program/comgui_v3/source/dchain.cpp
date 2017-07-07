#ifndef __dchain_cpp__
#define __dchain_cpp__

#include <stdexcept>
#include "dchain.h"
using namespace std;
namespace dream {
	
/***Ѱ�ҽڵ�ָ��***/ 
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
	this->mpHead = 0;		//�׽ڵ�ָ�� 
	this->mpTail = 0;		//β�ڵ�ָ�� 
	this->mpFocus = 0;	//��ǰ�ڵ�ָ�� 
}
/*���������������������*/
template< typename Tpn >
dchain<Tpn>::~dchain( void )
{
	this->clear();
}

/*��ĩ���������*/
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
/*��ĩ�˲���һ������ȫΪ0�Ľڵ�*/
template< typename Tpn >
bool dchain<Tpn>::push( void )
{
	Tpn tp;
	//memset( tp, 0, sizeof( tp ) );
	return this->push( tp );
}
/*�ڵ�ǰλ�ò�������*/
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
/*�ڵ�ǰλ�ò���һ������ȫΪ0�Ľڵ�*/
template< typename Tpn >
bool dchain<Tpn>::insert( void )
{
	Tpn tp;
	//memset( tp, 0, sizeof( tp ) );
	return this->insert( tp );
}
/*���ض�λ�ò�������*/
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

/*ɾ����ǰ�ڵ�*/ 
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
/*ɾ��ĳ���ڵ�*/
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
/*ɾ�����нڵ�*/
template< typename Tpn >
void dchain<Tpn>::clear( void )
{
	while( this->remove( 0 ) );
	return;
}

/*Ѱ��ĳ���ڵ�*/
template< typename Tpn >
bool dchain<Tpn>::focus( int location )	//�޸ĵ�ǰ��עָ���λ�� 
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
Tpn* dchain<Tpn>::focus( void )	const//���ص�ǰ��עָ������� 
{
	if( 0 == this->mpFocus )
		return 0;
	return &this->mpFocus->mValue;
}
template< typename Tpn >
Tpn& dchain<Tpn>::ifocus( void ) const//���ص�ǰ��ע���ݵ����� 
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
Tpn& dchain<Tpn>::operator[] ( int location ) const	//�������׳��쳣
{
	node* temp = this->find( location );
	if( 0 == temp )
		throw std::out_of_range( "location wrong" );
	return temp->mValue;
}
/*���ص�ǰ��ע���λ��*/
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

/*���ص�ǰ�ڵ�����*/
template< typename Tpn >
int dchain<Tpn>::count( void ) const
{
	int location = 0; 
	for( node* temp = this->mpHead; temp != 0; temp = temp->next )
		location += 1;
	return location;
}

/*�޸���������*/
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
