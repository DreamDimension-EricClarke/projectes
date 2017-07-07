#ifndef __CChain_H__
#define __CChain_H__

#include <stdexcept>
#include <functional>


/*CChain������Ӧ�κ����͵Ľڵ�����*/
/*��Ҫ��ͼ�ڶ���߳���ͬʱ����CChain��ʵ��,���Ǽ���Σ�յ�����,
���ж��߳�����,��ʹ��CChainMT*/
/*����CChain�Ǹ�ģ����,���Ե�������һ��cpp�ļ�����û�������*/
template <typename TData>
class CChain {

	private:
		//ÿ��chain�Ľڵ����Ͷ���chain��ģ�������й�,���Խṹ��������chain������
		struct SNode {
			TData        	data;       //�ڵ���Я��������
			SNode*          next;       //�ڵ�ĺ��ָ��
			SNode*          prev;       //�ڵ��ǰ��ָ��

			/*ӵ�г�ʼ���б�Ĺ��캯������chain����һϵ����Ҫ���캯����ʼ���Ľڵ�*/
			SNode( void ):
				next(0),prev(0) {
			}
			SNode( const TData& value ):
				data(value),next(0),prev(0) {
			}
		};

	private:
		/*
			Ϊ�˳�ַ���prevָ�������,CChain�Ľڵ����ӳ�Ϊ��������
			��һ������Ŀ���ַС��0ʱ,���������׽ڵ㿪ʼ,��ǰ(prev
			����)��ѯ
		    ����,��ʹʵ���������ǻ��ε�,����CChain�������ṩ������
			������,��һ���������ļ���Ŀ���ַ�����˽ڵ�����,������
			��ع�,���ǻᱨ��,ʧ��
		*/
		SNode* 			m_head;         //chain���׽ڵ�
		SNode*          m_target;    	//ѭ��������,���ڴ���Ľڵ�ָ��
		int        		m_count;        //chain��ǰ�Ľڵ�����
		volatile int    m_busy;         //�����ٲ�
	private:
		/*
		˽�е������������������һ���������ʹ��
		*/
		void operator=( const CChain& noneright ) {
			throw std::logic_error(" ");
		}
		
		/*�ٲú������ڵȴ��������Աʹ��Ȩ��*/
		void CatchBusy( ) {
			char temp;
			while( true ) {
				while( m_busy != 0 );   //�ȴ��ٲ��ͷ�
				m_busy += 1;            //�����ٲ�
				if( m_busy == 1 ) {
					return;             //������ɹ����˳�
				}
				//������������
			}
		}
	public:

		/*
		���캯�����ڼ򵥵ش���һ��CChainʵ��,��������������Ϊ
		��ֵ
		#####################################################
		*/
		CChain( void ):
			m_head(0), m_count(0), m_target(0), m_busy(0) {
		}
		
		/*
		�������캯������Ӧ�Ե�CChain������ʽ����ʱ�����
		��
		#####################################################
		����:
			const CChain<TData>& another        ���Կ�����Դ
			                                    ����������
												��
		*/
		CChain( CChain<TData>& another ):
		m_head(0), m_count(0), m_target(0) {
			another.ForEach( [&]( TData& td ) {
				Push( td );
				return true;
			} );
		}
		

		/*
		�������������CChain::Clear�����Զ������������еĽڵ�
		#####################################################
		*/
		~CChain( void ) {
			this->Clear( true );
		}

		/*
		Push���������������ĩβ���һ���ڵ�
		#####################################################
		����:
			const TData& data		Ҫ��ӵ�����ĩβ�Ľڵ㼴
									��Я��������
			-------------------------------------------------
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			bool                    �������ɹ�,��������true
			                        ����,����false
		=====================================================
		ע��:
		    ����ʹ�ÿ������캯�������ڵ�����,�뱣֤Ŀ��������
		    ���Ѿ���ȷ��������Щ����,��û��,��ȷ��Ŀ��������
		    ����һ��������������
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
		Insert���������������ĳ��λ�����һ���ڵ�
		#####################################################
		����:
			const TData& data    q	Ҫ��ӵ��ڵ��е���������
			-------------------------------------------------
			int       pos     		Ҫ���ڵ���ӵ������е�λ
			                        ��
			-------------------------------------------------
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			bool                    �ɹ�����ڵ��򷵻�true
			                        ʧ���򷵻�false
		=====================================================
		ע��:
			CChain�Խڵ������ֵ�����ɸ�,�����弰��Ϊ������ͼ
			CChainNodeMap.jpg���������Insert,Push,Remove����
		    Ϊ����
		*/
		bool Insert( const TData& data, int pos = 0, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			if( m_head == 0 )                    //������Ϊ��ʱ,�����⴦��
				if( pos == 0 or pos == -1 ) {
					m_head = new SNode( data );
					m_head->next = m_head->prev = m_head;
				} else
					return false;
			else if( pos > m_count ) {          //��pos > m_count ʱ,posΪ��Ч��Indexֵ
				return false;
			} else if( pos == m_count ) {       //��pos == m_count ʱ,Insert������ĩβ����һ���ڵ�,�൱��Push
				SNode* temp = new SNode( data );
				temp->prev = m_head->prev;
				temp->next = m_head;
				temp->prev->next = temp;
				temp->next->prev = temp;
			} else if( pos > 0 ) {             //�� 0 <= pos < m_count ʱ,Insert�������м����ڵ�,ԭ�ڵ���next����ƫ��
				SNode *temp, *tag;
				for( temp = m_head; pos > 0; pos-- )
					temp = temp->next;
				tag = new SNode( data );
				tag->next = temp;
				tag->prev = temp->prev;
				tag->next->prev = tag;
				tag->prev->next = tag;
			} else if( pos == 0 ) {             //�� pos == 0 ʱ,�������ײ���ӽڵ�,���޸�headָ��
				SNode* tag = new SNode( data );
				tag->next = m_head;
				tag->prev = m_head->prev;
				tag->prev->next = tag;
				tag->next->prev = tag;
				m_head = tag;
			} else if( pos >= -m_count ) {     //�� -m_count <= pos < 0 ʱ,Insert�������м����ڵ�,ԭ�ڵ���prev����ƫ��
				SNode *temp, *tag;
				for( temp = m_head; pos < 0; pos++ )
					temp = temp->prev;
				tag = new SNode( data );
				tag->prev = temp;
				tag->next = temp->next;
				tag->prev->next = tag;
				tag->next->prev = tag;
			} else if( pos == -1 - m_count ) {  //�� pos == -1 - m_count ʱ,Insert�������ײ�����ڵ�,���޸�headָ��
				SNode *tag = new SNode( data );
				tag->next = m_head;
				tag->prev = m_head->prev;
				tag->next->prev = tag;
				tag->prev->next = tag;
				m_head = tag;
			} else {                            //���������Ϊ��Ч��Indexֵ
				return false;
			}

			m_count += 1;       //��¼�ڵ������ı仯
			
			if( !bforce ) {
				m_busy = 0;
			}
			return true;
		}

		/*GetPointer��������Ѱ�ҽڵ�,�����ؽڵ����ݵ�ָ��
		#####################################################
		����:
			int       pos     		Ҫ��ѯ�Ľڵ��λ��
			-------------------------------------------------
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			TData*               	����ѯ�õ��˽��,����Ŀ��
			                        ��ָ��,����,����0
		*/
		TData* GetPointer( int pos, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			if( pos >= m_count or pos < -m_count )  //�������ֵ��Ч,ֱ�ӷ���0
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

		/*[]���������Ѱ�ҽڵ�,�����ؽڵ�Я�������ݵ�����
		#####################################################
		����:
			const int       pos     Ҫ��ѯ�Ľڵ��λ��
		=====================================================
		����ֵ:
			TData&               	����ѯ�õ��˽��,���ؽ��
			                        ������
			                        ����,�׳��쳣
									runtime_error
		*/
		TData& operator[]( int pos )throw(std::runtime_error) {
			if( pos >= m_count or pos < -m_count )  //�������ֵ��Ч,ֱ�ӷ���0
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

		/*GetIndex����������ȡһ���ڵ��������ֵ
		#####################################################
		����:
			cosnt TData& data    	Ŀ��ڵ���������ݵ�����
			-------------------------------------------------
			int&    		index   �������շ���ֵ������
			-------------------------------------------------
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			bool                    ���������ݲ��������ڱ���
			                        ��,���ѯʧ��,����false
			                        ���򷵻�true,ͬʱindex��
			                        �����˽ڵ������λ��
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

		/*Remove��������ɾ��һ��λ���ϵĽڵ�
		#####################################################
		����:
			int       pos     		����Ҫɾ���Ľڵ������ֵ
			-------------------------------------------------
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			bool                    �ɹ�ɾ���˽ڵ��򷵻�true
		                            ���򷵻�false
		=====================================================
		ע��:
		    ���ɾ����Ŀ����0λ��,m_headָ����next�����ƶ�
		    ���ɾ����Ŀ����-m_countλ��,m_headָ����prev����
		    �ƶ�
		    ��ѭ�������е���Remove�ᵼ��ʧ��
		*/
		bool Remove( int pos, const bool bforce = false ) {
			
			if( !bforce ) {
				CatchBusy();
			}
			
			if( pos >= m_count or pos < -m_count or m_target != 0 )  //�������ֵ��Ч,ֱ�ӷ���false
				return false;
			int ind(pos);   //����Ŀ��λ�õı���,�Ա�����ж�

			SNode* tag;     //�ҵ�Ŀ��ڵ�
			if( pos >= 0 ) {
				for( tag = m_head; pos > 0; pos-- )
					tag = tag->next;
			} else {
				for( tag = m_head; pos < 0; pos++ )
					tag = tag->prev;
			}

			//���ɾ���Ľڵ����׽ڵ�
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

		/*Remove��������ɾ��һ���������ڵĽڵ�
		#####################################################
		����:
			TData&		target  	��Ҫɾ���Ľڵ�Я��������
			                        ������
			-------------------------------------------------
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			bool                    ������ȷʵ�����ڱ������
			                        �ڵ���,��ɾ���ɹ�,����
			                        true,���򷵻�false
		=====================================================
		ע��:
		    ���ɾ����Ŀ����0λ��,m_headָ����next�����ƶ�
		    ��ѭ�������е���Remove�ᵼ��ʧ��
		*/
		bool Remove( TData& target, const bool bforce = false ) {

			if( !bforce ) {
				CatchBusy();
			}

			if( m_count == 0 )      //������Ϊ��,ֱ�ӷ���false
				return false;
			SNode* tag = m_head;     //�ҵ�Ŀ��ڵ�
			do {
				if( &tag->data == &target )
					break;
				tag = tag->next;
			} while( tag != m_head );

			if( &tag->data == &target ) {
				//���ɾ���Ľڵ����׽ڵ�
				if( tag == m_head )
					m_head = tag->next;

				//ɾ��Ŀ��ڵ�
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

		/*Clear����������������еĽڵ�
		#####################################################
        ����:
			const bool bforce       �Ƿ�ǿ��ִ��
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

		/*Count�������ڼ��������еĽڵ�����
		#####################################################
        ����:
			const bool bforce       �Ƿ�ǿ��ִ��
		=====================================================
		����ֵ:
			int                     �����нڵ������
		*/
		int Count( const bool bforce = false ) {
			if( !bforce ) {
				CatchBusy();
				m_busy = 0;
			}
			return m_count;
		}

		/*ForEach����ʹ���ض��Ļص������������нڵ�
		#####################################################
		����:
		    function<bool(TData&)	func	����ÿ���ڵ�ʹ��
											�Ļص�����
		=====================================================
		ע��:
			CChain���0��ʼ,��next��˳��Ϊÿ���ڵ����һ��
			func,���κ�һ�ε�����,func������false,������̻ᱻ
			��ֹ
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
