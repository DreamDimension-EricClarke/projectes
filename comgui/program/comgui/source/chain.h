#ifndef __CHAIN__
#define __CHAIN__
/*
chain��Ϊͬһ�������ṩ����ʽ�洢��ʽ
�������chain���´��һ������,�������Ҫ����
chain�����ݲ��ܿ�����Ӧ��������Ӧ�ĵ�ַָ�� 
*/
namespace dream{

template< typename Tpn >
class chain {
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
		node* find( int );
	public:
		/*���캯��*/
		chain( void );
		/*���������������������*/
		~chain( void );
		
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
		Tpn* focus( void );	//���ص�ǰ��עָ������� 
		Tpn& ifocus( void );//���ص�ǰ��ע���ݵ����� 
		Tpn* at( int );
		Tpn& operator[] ( int );	//�������׳��쳣
		/*���ص�ǰ��ע���λ��*/
		int locat( void );
		bool bHead( void );
		bool bTail( void );
		bool bEmpty( void );
		/*���ص�ǰ�ڵ�����*/
		int count( void ); 
		
		/*�޸���������*/
		bool set( const Tpn& );
		bool set( const Tpn&, int );
};

}
#endif
