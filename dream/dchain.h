#ifndef __dchain__
#define __dchain__

/*
dchainl���ṩһ�ִ��ģ����������洢����
���ڵ��С��洢����СӦ��ѡ��dchains�����ۺ�Ч�ʻ��󽵵� 
dchain��ÿ���ڵ�ʹ�ÿ������캯��type( const type& );����ֵ 
*/

#include <stdexcept>
using namespace std;

namespace dream {
	
template< typename Tpn >
class dchainl {
	private:
		/***�ڵ�ṹ��***/
		struct node {
			Tpn vaule;
			node* prev;
			node* next;
			node( const Tpn& v):value(v){}
		}; 
	private:
		node*	mpHead;		//�׽ڵ�ָ�� 
		node*	mpTail;		//β�ڵ�ָ�� 
		node*	mpFocus;	//Ŀ��ڵ�ָ�� 
		int 	miCount;	//�ڵ��ͳ��
		int		miHelf;		//�ڵ��������� 
		int		miFocus;	//��ǰ����λ�� 
	private:
		/***Ѱ�ҽڵ�ָ��***/
		node* find( int location ) const
		{
			node* tag;
			int offs;
			
			if( location < 0 )
				location += miCount;
			if( location < 0 or location == miCount )
				return 0;
			else if( (offs = location - miFocus) < miHelf )
			{
				if( offs >= 0 )
				{
					for( tag = mpFocus; offs > 0; offs++ )
						tag = tag->next;
				}
				else
				{
					for( tag = mpFocus; offs < 0; offs-- )
						tag = tag->prev;
				}
				return tag;
			}
			else if( location <= miHelf )
			{
				for( tag = mpHead; location > 0; location-- )
					tag = tag->next;
				return tag;
			}
			else //if( location > miHelf )
			{
				offs = location - miCount;
				for( tag = mpTail; offs != -1; offs ++ )
					tag = tag->prev;
				return tag;
			}
		}
	public:
		//���캯�� 
		dchainl( ):
		mpHead(0),mpTail(0),mpFocus(0),
		miCount(0),miHelf(0),miFocus(0)
		{}
		
		//��������
		~dchain()
		{
			this->clear();
		}
		
		bool insert( const Tpn& value, int location = 0 )
		{
			if( location < 0 )
				location += miCount;
			if( miCount == 0 and (location == 0 or location == -1) )
			{
				mpHead = mpTail = mpFocus = new node(value);
				miCount = 1;
			}
				
		}
};

//end of file	
}
#endif
