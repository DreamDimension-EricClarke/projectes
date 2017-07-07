#ifndef __CChain_H__
#define __CChain_H__

#include <stdexcept>
#include <functional>


/*CChain类能适应任何类型的节点内容*/
/*不要试图在多个线程中同时访问CChain类实例,这是极其危险的事情,
若有多线程需求,请使用CChainMT*/
/*由于CChain是个模板类,所以单独放在一个cpp文件中是没有意义的*/
template <typename TData>
class CChain {

	private:
		//每个chain的节点类型都和chain的模板类型有关,所以结构体内置在chain的类中
		struct SNode {
			TData        	data;       //节点所携带的数据
			SNode*          next;       //节点的后继指针
			SNode*          prev;       //节点的前驱指针

			/*拥有初始化列表的构造函数允许chain持有一系列需要构造函数初始化的节点*/
			SNode( void ):
				next(0),prev(0) {
			}
			SNode( const TData& value ):
				data(value),next(0),prev(0) {
			}
		};

	private:
		/*
			为了充分发挥prev指针的作用,CChain的节点连接成为环形链表
			当一个检索目标地址小于0时,检索将从首节点开始,向前(prev
			方向)查询
		    但是,即使实际上链表是环形的,但是CChain不向外提供环形特
			征比如,当一个正整数的检索目标地址超过了节点总量,检索不
			会回滚,而是会报错,失败
		*/
		SNode* 			m_head;         //chain的首节点
		SNode*          m_target;    	//循环历遍中,正在处理的节点指针
		int        		m_count;        //chain当前的节点总数
		volatile int    m_busy;         //操作仲裁
	private:
		/*
		私有的运算符重载用来禁用一种运算符的使用
		*/
		void operator=( const CChain& noneright ) {
			throw std::logic_error(" ");
		}
		
		/*仲裁函数用于等待并抢夺成员使用权限*/
		void CatchBusy( ) {
			char temp;
			while( true ) {
				while( m_busy != 0 );   //等待仲裁释放
				m_busy += 1;            //抢夺仲裁
				if( m_busy == 1 ) {
					return;             //若抢夺成功则退出
				}
				//否则重新抢夺
			}
		}
	public:

		/*
		构造函数用于简单地创造一个CChain实例,并将所有内容置为
		空值
		#####################################################
		*/
		CChain( void ):
			m_head(0), m_count(0), m_target(0), m_busy(0) {
		}
		
		/*
		拷贝构造函数用于应对当CChain对象被隐式拷贝时的情况
		况
		#####################################################
		参数:
			const CChain<TData>& another        用以拷贝的源
			                                    链表对象的引
												用
		*/
		CChain( CChain<TData>& another ):
		m_head(0), m_count(0), m_target(0) {
			another.ForEach( [&]( TData& td ) {
				Push( td );
				return true;
			} );
		}
		

		/*
		析构函数会调用CChain::Clear函数自动清理链中所有的节点
		#####################################################
		*/
		~CChain( void ) {
			this->Clear( true );
		}

		/*
		Push函数用于在链表的末尾添加一个节点
		#####################################################
		参数:
			const TData& data		要添加到链表末尾的节点即
									将携带的内容
			-------------------------------------------------
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			bool                    若创建成功,则函数返回true
			                        否则,返回false
		=====================================================
		注意:
		    函数使用拷贝构造函数创建节点内容,请保证目标数据类
		    型已经正确重载了这些函数,若没有,请确保目标数据类
		    型是一个基础数据类型
		*/
		bool Push( const TData& data, const bool bforce = false ) {
			
			if( !bforce )
				CatchBusy();
			
			if( m_head == 0 ) {
				m_head = new SNode( data );
				m_head->next = m_head->prev = m_head;
			} else {
				SNode* temp = new SNode( data );
				temp->prev = m_head->prev;
				temp->next = m_head;
				temp->prev->next = temp;
				temp->next->prev = temp;
			}
			m_count += 1;
			
			if( !bforce )
				m_busy = 0;
			return true;
		}

		/*
		Insert函数用于在链表的某个位置添加一个节点
		#####################################################
		参数:
			const TData& data    q	要添加到节点中的数据内容
			-------------------------------------------------
			int       pos     		要将节点添加到链表中的位
			                        置
			-------------------------------------------------
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			bool                    成功加入节点则返回true
			                        失败则返回false
		=====================================================
		注意:
			CChain对节点的索引值可正可负,其意义及行为特征见图
			CChainNodeMap.jpg其中描绘了Insert,Push,Remove的行
		    为特征
		*/
		bool Insert( const TData& data, int pos = 0, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			if( m_head == 0 )                    //当链表为空时,做特殊处理
				if( pos == 0 or pos == -1 ) {
					m_head = new SNode( data );
					m_head->next = m_head->prev = m_head;
				} else
					return false;
			else if( pos > m_count ) {          //当pos > m_count 时,pos为无效的Index值
				return false;
			} else if( pos == m_count ) {       //当pos == m_count 时,Insert在链表末尾插入一个节点,相当于Push
				SNode* temp = new SNode( data );
				temp->prev = m_head->prev;
				temp->next = m_head;
				temp->prev->next = temp;
				temp->next->prev = temp;
			} else if( pos > 0 ) {             //当 0 <= pos < m_count 时,Insert在链表中间插入节点,原节点向next方向偏移
				SNode *temp, *tag;
				for( temp = m_head; pos > 0; pos-- )
					temp = temp->next;
				tag = new SNode( data );
				tag->next = temp;
				tag->prev = temp->prev;
				tag->next->prev = tag;
				tag->prev->next = tag;
			} else if( pos == 0 ) {             //当 pos == 0 时,在链表首部添加节点,并修改head指针
				SNode* tag = new SNode( data );
				tag->next = m_head;
				tag->prev = m_head->prev;
				tag->prev->next = tag;
				tag->next->prev = tag;
				m_head = tag;
			} else if( pos >= -m_count ) {     //当 -m_count <= pos < 0 时,Insert在链表中间插入节点,原节点向prev方向偏移
				SNode *temp, *tag;
				for( temp = m_head; pos < 0; pos++ )
					temp = temp->prev;
				tag = new SNode( data );
				tag->prev = temp;
				tag->next = temp->next;
				tag->prev->next = tag;
				tag->next->prev = tag;
			} else if( pos == -1 - m_count ) {  //当 pos == -1 - m_count 时,Insert在链表首部插入节点,并修改head指针
				SNode *tag = new SNode( data );
				tag->next = m_head;
				tag->prev = m_head->prev;
				tag->next->prev = tag;
				tag->prev->next = tag;
				m_head = tag;
			} else {                            //其他情况视为无效的Index值
				return false;
			}

			m_count += 1;       //记录节点总量的变化
			
			if( !bforce ) {
				m_busy = 0;
			}
			return true;
		}

		/*GetPointer函数用于寻找节点,并返回节点内容的指针
		#####################################################
		参数:
			int       pos     		要查询的节点的位置
			-------------------------------------------------
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			TData*               	若查询得到了结果,返回目标
			                        的指针,否则,返回0
		*/
		TData* GetPointer( int pos, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			if( pos >= m_count or pos < -m_count )  //如果索引值无效,直接返回0
				return 0;
			SNode* tag;
			if( pos >= 0 ) {
				for( tag = m_head; pos > 0; pos-- )
					tag = tag->next;
			} else {
				for( tag = m_head; pos < 0; pos++ )
					tag = tag->prev;
			}
			
			if( !bforce ) {
				m_busy = 0;
			}
			
			return &(tag->data);
		}

		/*[]运算符用来寻找节点,并返回节点携带的数据的引用
		#####################################################
		参数:
			const int       pos     要查询的节点的位置
		=====================================================
		返回值:
			TData&               	若查询得到了结果,返回结果
			                        的引用
			                        否则,抛出异常
									runtime_error
		*/
		TData& operator[]( int pos )throw(std::runtime_error) {
			if( pos >= m_count or pos < -m_count )  //如果索引值无效,直接返回0
				throw std::runtime_error("cannot find node \n");
			SNode* tag;
			if( pos >= 0 ) {
				for( tag = m_head; pos > 0; pos-- )
					tag = tag->next;
			} else {
				for( tag = m_head; pos < 0; pos++ )
					tag = tag->prev;
			}
			return tag->data;
		}

		/*GetIndex函数用来获取一个节点的正索引值
		#####################################################
		参数:
			cosnt TData& data    	目标节点的数据内容的引用
			-------------------------------------------------
			int&    		index   用来接收反馈值得引用
			-------------------------------------------------
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			bool                    若数据内容并不存在于本链
			                        表,则查询失败,返回false
			                        否则返回true,同时index中
			                        保存了节点的所在位置
		*/
		bool GetIndex( const TData& data, int& index, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			int pos = 0;
			for( SNode* tag = m_head; pos < m_count; pos++ )
				if( &tag->data == &data ) {
					index = pos;
					return true;
				} else {
					tag = tag->next;
				}
				
			if( !bforce ) {
				m_busy = 0;
			}

			return false;
		}

		/*Remove函数用于删除一个位置上的节点
		#####################################################
		参数:
			int       pos     		即将要删除的节点的索引值
			-------------------------------------------------
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			bool                    成功删除了节点则返回true
		                            否则返回false
		=====================================================
		注意:
		    如果删除的目标在0位置,m_head指针向next方向移动
		    如果删除的目标在-m_count位置,m_head指针向prev方向
		    移动
		    在循环历遍中调用Remove会导致失败
		*/
		bool Remove( int pos, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			if( pos >= m_count or pos < -m_count or m_target != 0 )  //如果索引值无效,直接返回false
				return false;
			int ind(pos);   //保留目标位置的备份,以便后续判断

			SNode* tag;     //找到目标节点
			if( pos >= 0 ) {
				for( tag = m_head; pos > 0; pos-- )
					tag = tag->next;
			} else {
				for( tag = m_head; pos < 0; pos++ )
					tag = tag->prev;
			}

			//如果删除的节点是首节点
			if( tag == m_head ) {
				if( ind >= 0 )
					m_head = tag->next;
				else
					m_head = tag->prev;
			}

			tag->prev->next = tag->next;
			tag->next->prev = tag->prev;
			delete tag;
			m_count -= 1;

			if( m_count == 0 )
				m_head = 0;

			if( !bforce ) {
				m_busy = false;
			}

			return true;
		}

		/*Remove函数用于删除一个数据所在的节点
		#####################################################
		参数:
			TData&		target  	将要删除的节点携带的数据
			                        的引用
			-------------------------------------------------
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			bool                    若数据确实保存在本链表的
			                        节点中,则删除成功,返回
			                        true,否则返回false
		=====================================================
		注意:
		    如果删除的目标在0位置,m_head指针向next方向移动
		    在循环历遍中调用Remove会导致失败
		*/
		bool Remove( TData& target, const bool bforce = false ) {

			if( !bforce ) {
				CatchBusy();
			}

			if( m_count == 0 )      //若链表为空,直接返回false
				return false;
			SNode* tag = m_head;     //找到目标节点
			do {
				if( &tag->data == &target )
					break;
				tag = tag->next;
			} while( tag != m_head );

			if( &tag->data == &target ) {
				//如果删除的节点是首节点
				if( tag == m_head )
					m_head = tag->next;

				//删除目标节点
				tag->prev->next = tag->next;
				tag->next->prev = tag->prev;
				delete tag;
				if( --m_count == 0 )
					m_head = 0;
				return true;
			}
			
			if( !bforce ) {
				m_busy = 0;
			}

			return false;
		}

		/*Clear函数用于清空链表中的节点
		#####################################################
        参数:
			const bool bforce       是否强制执行
		*/
		void Clear( const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			SNode* tag;
			while( m_head != 0 ) {
				tag = m_head;
				m_head = m_head->next;

				tag->next->prev = tag->prev;
				tag->prev->next = tag->next;
				delete tag;
				if( --m_count == 0 )
					m_head = 0;
			}
			
			if( !bforce ) {
				m_busy = 0;
			}
			
		}

		/*Count函数用于计算链表中的节点总数
		#####################################################
        参数:
			const bool bforce       是否强制执行
		=====================================================
		返回值:
			int                     链表中节点的总数
		*/
		int Count( const bool bforce = false ) {
			if( !bforce ) {
				CatchBusy();
				m_busy = 0;
			}
			return m_count;
		}

		/*ForEach函数使用特定的回调函数历遍所有节点
		#####################################################
		参数:
		    function<bool(TData&)	func	历遍每个节点使用
											的回调函数
		=====================================================
		注意:
			CChain会从0开始,以next的顺序为每个节点调用一次
			func,若任何一次调用中,func返回了false,历遍过程会被
			终止
		*/
		void ForEach( std::function<bool(TData&)> func ) {
			if( m_count == 0 )
				return;
			SNode* tag = m_head;
			
			do {
				if( !func( tag->data ) )
					break;
				tag = tag->next;
			} while( tag != m_head );
			return;
		}
};

#endif
