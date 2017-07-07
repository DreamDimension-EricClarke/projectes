#ifndef __dchain__
#define __dchain__

/*
dchainl类提供一种大规模的数据链表存储方案
若节点很小或存储量很小应当选择dchains否则综合效率会大大降低 
dchain的每个节点使用拷贝构造函数type( const type& );赋初值 
*/

#include <stdexcept>
using namespace std;

namespace dream {
	
template< typename Tpn >
class dchainl {
	private:
		/***节点结构体***/
		struct node {
			Tpn vaule;
			node* prev;
			node* next;
			node( const Tpn& v):value(v){}
		}; 
	private:
		node*	mpHead;		//首节点指针 
		node*	mpTail;		//尾节点指针 
		node*	mpFocus;	//目标节点指针 
		int 	miCount;	//节点点统计
		int		miHelf;		//节点总量半数 
		int		miFocus;	//当前焦点位置 
	private:
		/***寻找节点指针***/
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
		//构造函数 
		dchainl( ):
		mpHead(0),mpTail(0),mpFocus(0),
		miCount(0),miHelf(0),miFocus(0)
		{}
		
		//析构函数
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
