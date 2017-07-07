#ifndef __dchain__
#define __dchain__
/*
dchain��Ϊͬһ�������ṩ����ʽ�洢��ʽ
�������dchain���´��һ������,�������Ҫ����
dchain�����ݲ��ܿ�����Ӧ��������Ӧ�ĵ�ַָ�� 
*/

#include "dream.h"

namespace dream{

template< typename Tpn >
class dchain {
	private:
		/***�ڵ�����***/ 
		struct node {
			Tpn		mValue;	//����Ŀ�����ݵ�ֵ 
			node	*prev;
			node	*next;		//����ָ�� 
		};
	private:
		node*		mpHead;		//�׽ڵ�ָ�� 
		node*		mpTail;		//β�ڵ�ָ�� 
		node*		mpFocus;	//��ǰ�ڵ�ָ�� 
	private:
		/***Ѱ�ҽڵ�ָ��***/ 
		node* find( int ) const;
	public:
		/*���캯��*/
		dchain( void );
		/*���������������������*/
		~dchain( void );
		
		/*��ĩ���������*/
		bool push( const Tpn& ); 
		bool push( void );
		/*�ڵ�ǰλ�ò�������*/
		bool insert( const Tpn& );
		bool insert( void );
		/*���ض�λ�ò�������*/
		bool insert( const Tpn&, int );
		
		/*ɾ����ǰ�ڵ�*/ 
		bool remove( void );
		/*ɾ��ĳ���ڵ�*/
		bool remove( int ); 
		/*ɾ�����нڵ�*/
		void clear( void ); 
		
		/*Ѱ��ĳ���ڵ�*/
		bool focus( int );	//�޸ĵ�ǰ��עָ���λ�� 
		bool next( void );
		bool prev( void );
		Tpn* focus( void ) const;	//���ص�ǰ��עָ������� 
		Tpn& ifocus( void ) const;//���ص�ǰ��ע���ݵ����� 
		Tpn* at( int ) const;
		Tpn& operator[] ( int ) const;	//�������׳��쳣
		/*���ص�ǰ��ע���λ��*/
		int locat( void ) const;
		bool bHead( void ) const;
		bool bTail( void ) const;
		bool bEmpty( void ) const;
		/*���ص�ǰ�ڵ�����*/
		int count( void ) const; 
		
		/*�޸���������*/
		bool set( const Tpn& );
		bool set( const Tpn&, int );
};

}
#endif
