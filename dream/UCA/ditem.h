#ifndef __DITEM___
#define __DITEM___

/*
	item_lexer���ܹ���һϵ��item
	��������item code������
	item code�����ж���item���� 
	item_lexer�ܴ�item code������������������
	Ҳ�ܴ�������������item code����
	ÿ�������п����ж��item����
	ÿ��item����ֻ��ӵ��һ����������,һ����ǩ,��һ������
	item_lexer֧�ֵ���������:( �������������������ִ�С )
	boolean ��ӦC++��bool���� ��ֵֻ����true��false��������,���ݲ����ִ�С 
	byte	��ӦC++��unsigned char���͵�����,����ÿ���ֽڶ�����д�� ���� 1E ����λ���ʮ����������,ÿ�������ּ�Ӧ���пո��϶ 
	hex		��ӦC++��unsigned long long����,��������һ���8λ��ʮ���������� 
	number	��ӦC++�е�double����,���Բ�Я��С���� 
	string 	��ӦC++�е�string�����char����,string���͵�item��item code�е���д�漰��ת���ַ�,ת���������:
		&& = &
		&; = ; 
	none	��Ӧ�����κ��������ݵ�item
	
	item code �﷨:
	( type )[ label ] value ;
	�κβ��ֵ����ݶ��ز�����,type��label��Ӧ�ñ��ո�ָ��� 
	label���Խ���Ϊ class:name.sub 
	һ��label�������һ��:��һ��.����������ᱻ��Ϊ�Ƿ�label
	һ��labelû��:����Ϊ����Ϊ������Ϊdefault��class
*/
#include <string>
using namespace std;
namespace dream {
/***item���������ͱ�־***/ 
enum lexerType {
	lexerTnone,		//������������Ӧû������ 
	lexerTboolean,	//boolean�������Ͷ�Ӧ��C++�е�bool�������� 
	lexerTnumber,	//number�������Ͷ�Ӧ��C++�е�double�������� 
	/***byte������Ψһ�޷��Զ��������ݳ��ȵ�����***/ 
	lexerTbyte,		//byte�������Ͷ�Ӧ��C++�е�unsigned char�������͵����ݶ� 
	lexerThex,		//hex�������Ͷ�Ӧ��C++�е�unsigned long long�������� 
	lexerTstring		//string�������Ͷ�Ӧ��C++�е�char�������͵����ݶ� 
};

/***item code��������־***/ 
enum errorCode_lexer {
	lexerEsuccess,		//û���κ����� 
	lexerEbadcode,		//����Դ�޷���ȡ 
	lexerEillegalNote,	//�����ע����ʼ 
	lexerEillegalItem,	//�������Ŀ��ʼ 
	lexerEillegalType,	//�Ƿ����������� 
	lexerErepeatType,	//�ظ������Ͷ��� 
	lexerElostType,		//ȱ�����Ͷ����� 
	lexerEillegalLabel,	//�Ƿ�����Ŀ��ǩ 
	lexerErepeatLabel,	//�ظ��ı�ǩ���� 
	lexerElostLabel,		//ȱ�ٱ�ǩ������ 
	lexerEillegalValue,	//�Ƿ����������� 
	lexerElostValue		//ȱ������������ 
};

/***item lexer�ж�token�����***/ 
enum lexerIdent {
	lexerIillegal,	//�Ƿ��ַ� 
	lexerIspace,		//���ַ� 
	lexerInewline,	//�س��� 
	lexerIletterR,	//a~f��Χ�ڵ���ĸ 
	lexerIletterO,	//g~z��Χ�ڵ���ĸ 
	lexerInumber,	//0~9��Χ�ڵ����� 
	lexerIother,		//�����Ϸ����ַ� 
	lexerItypeB,		//���Ͷ������俪ʼ�� 
	lexerItypeE,		//���Ͷ������������ 
	lexerIlabelB,	//��ǩ�������俪ʼ�� 
	lexerIlabelE,	//��ǩ�������������
	lexerInote,		//ע�ͻ���� 
	lexerInoteB,		//ע�������� 
	lexerInoteE,		//ע�ͽ����� 
	lexerItrans,		//ת��� 
	lexerIitemE,		//��Ŀ������ 
	lexerIend		//������� 
};

/***��������ʱ������״̬��־***/ 
enum LexerRegStatus {
	lexerSout,		//ջ���ⲿ,��Ҫ��ʼ�� 
	lexerSglobal,	//ȫ��״̬ ���ܻ���ע�� ��ʼ���� ����ִ��� 
	lexerSnoteB,	//ע�ͻ���״̬ ���ܿ�ʼע�� ���ܳ��ִ��� 
	lexerSnote,		//ע��״̬ ���ܻ���ע�� ���ܳ��ִ��� 
	lexerStype,		//���Ͷ���״̬ ���ܳ��ִ��� 
	lexerSitem,		//��Ŀ����״̬ ���ܻ���ע�� ���ܿ�ʼ��ǩ ���ܳ��ִ��� 
	lexerSlabel,	//��ǩ����״̬ ���ܳ��� 
	lexerSbool,		//boolean״̬  ���ܽ�����Ŀ ���ܳ��� 
	lexerSbyte,		//byte״̬ ���ܽ�����Ŀ ���ܳ��� 
	lexerShex,		//hex״̬ ���ܽ�����Ŀ ���ܳ��� 
	lexerSnone,		//none״̬ ���ܽ�����Ŀ ���ܳ��� 
	lexerSnumber,	//number״̬ ���ܽ�����Ŀ ���ܳ��� 
	lexerSstring,	//string״̬ ���ܿ�ʼת�� ���ܽ�����Ŀ ���ܳ��� 
	lexerStrans		//ת��״̬ ���ܳ��ִ��� 
};

/***���Ѱ��Ԫ�صı�־***/
enum lexerFind {
	lexerFprev,		//��һ�� 
	lexerFnext,		//��һ�� 
	lexerFfirst,		//��һ�� 
	lexerFlast		//���һ�� 
};

/***����������ṹ��***/ 
struct LexerCompRep {
	errorCode_lexer	error;	//�������
	int offb;			//����ƫ�� 
	int line;			//�����к�
	int offl;			//��������ƫ���� 
	int count;			//�ɹ�����Ľڵ���� 
};

class item_lexer {
	public:
		/***����������״̬��ģ��״̬ջ***/ 
		struct StatusStack {
			LexerRegStatus top;		//ջ��Ԫ�� 
			StatusStack* prev;	//ջ��ָ�� 
		};
		/***item���ݽṹ��***/
		struct item {
			lexerType mtType;	//item���� 
			string msLabel;		//item��ǩ 
			char *mpValue;		//item���ݿռ� 
			int miLength;		//item���ݳ��� 
			/***item��ָ��***/
			item *next;
			item *prev;
		};
	private:
		/***item lexer����������***/
		item *mpList = 0;	//item�б� 
		item *mpCurrent = 0;//item��ǰָ��
		string	msClass;	//��ǰ���� 
		
		/***item codeԴ����ͱ���Ĵ�����***/ 
		StatusStack	*msStack = 0;//״̬ջ
		errorCode_lexer	meError;//����״̬ 
		string 		msCode;	//Դ���� 
		string 		msToken;//�ؼ���
		bool		mbSpace;//�ؼ����Ƿ��Ѿ������� 
		bool		mbPoint;//���ֱ�������� 
		int 		miOffb;	//����ƫ�� 
		int 		miLine;	//�����к�
		int 		miOffl;	//��������ƫ���� 
		int			miRegCount;//���±���Ľڵ���� 
	private:
		LexerRegStatus pushReg( LexerRegStatus );	//��ջ״̬,������ջ���״̬ 
		LexerRegStatus popReg( void );		//��ջ״̬,���س�ջ���״̬ 
		LexerRegStatus getReg( int = 0 );	//��ȡ��n��Ķ�ջ���� 
		LexerRegStatus setReg( LexerRegStatus );	//���ö���ջ״̬ 
		bool initReg( const string& );	//��ʼ��һ���Ĵ�״̬ 
		char regNext( void );			//ָ����һ���ַ����Զ����к�ƫ�� 
		char regCode( void );			//���ص�ǰָ����ַ�
		LexerCompRep	regReport( void );		//���ر�����Ϣ
		lexerIdent regIdent( void );	//���Ե�ǰָ���ַ������ 
		lexerIdent identChar( char );	//���һ���ַ������ 
		bool regProc( void ) ;			//����ǰָ���ַ�,������ݻ򱨸���� 
		
		double string2number( string );
		unsigned long long string2hex( string );
		unsigned char string2byte( string );
		bool string2bool( string );	//�ַ���ת���ɲ�������ʧ���׳�invalid_argument�쳣 
		lexerType string2type( string );
		string type2string( lexerType );
		string item2string( item& );
		bool uniqLabel( const string& );//���Ա�ǩ��ӵ��������Ƿ�Ψһ
		bool correctClass( string& );//���������Ƿ���ȷ 
		bool correctLabel( string& );//���Ա�ǩ�Ƿ���ȷ,����ȫ������
		string label2class( const string& );//�ӱ�ǩ����ȡ����
		string label2name( const string& );//�ӱ�ǩ����ȡ���� 
		
		item* classItem( item* );	//��ָ���Ľڵ�����Ľڵ����ҳ���һ���ڵ�ǰ�����еĽڵ� 
		item* classPrev( item* );	//��ָ���Ľڵ����ǰ�Ľڵ����ҳ���һ���ڵ�ǰ�����еĽڵ� 
	public:
		item_lexer();
		~item_lexer();
		
		/***�����������***/ 
		void clearAll( void );
		/***��ձ���Ĵ���***/ 
		void clearComp( void );
		/***��յ�ǰָ���item***/ 
		bool clearItem( void );
		/***��ĩ������µ�item***/
		string pushItem( void );
		bool pushItem( string );
		bool pushItem( string, bool );
		bool pushItem( string, unsigned char*, int );
		bool pushItem( string, unsigned long long );
		bool pushItem( string, double );
		bool pushItem( string, string );
		/***�ڵ�ǰλ������µ�item,���ɹ���ָ�����item***/ 
		string insertItem( void );
		bool insertItem( string );
		bool insertItem( string, bool );
		bool insertItem( string, unsigned char*, int );
		bool insertItem( string, unsigned long long );
		bool insertItem( string, double );
		bool insertItem( string, string );
		/***ɾ��Ԫ��***/
		bool removeItem( void );	//ɾ����ǰָ���Ԫ�� 
		bool removeItem( string );	//ɾ���ɱ�ǩָ����Ԫ��
		/***���ñ�ǩ***/
		bool setLabel( string );
		/***��������***/ 
		bool setClass( string );
		/***��������***/
		bool setType( lexerType ); 
		/***��������,���Ͳ�����ʧ��***/
		bool setValue( bool );
		bool setValue( unsigned char*, int );
		bool setValue( unsigned long long );
		bool setValue( double );
		bool setValue( string );
		/***��ȡ����,���Ͳ������׳��쳣***/
		bool getBool( void );
		unsigned char* getByte( void );
		unsigned long long getHex( void );
		double getNumber( void );
		string getString( void );
		/***��ȡ����,ʧ���׳��쳣***/
		lexerType getType( void );
		/***��ȡ���ݳ���,ʧ�ܷ���-1***/
		int getLength( void );
		/***��ȡ��ǩ,ʧ���׳��쳣***/
		string getLabel( void ); 
		/***��ȡ����,ʧ���׳��쳣***/
		string getClass( void ); 
		/***��ȡitemλ��,ʧ���򷵻�-1***/
		int getPos( void ); 
		/***Ѱ��ȫ���е�Ԫ��,��ָ��***/
		bool findItem( string );		//���ձ�ǩ����,��ǩ���������������ڵ�ǰ�������� 
		bool findItem( lexerFind );		//��������λ���ϵ�Ԫ�� 
		bool findItem( lexerFind, lexerType );//�������ͼ��� 
		bool findItem( int );//���վ���λ�ü��� 
		/***Ѱ�ҵ�ǰ���е�Ԫ��,��ָ��***/ 
		bool classItem( lexerFind );	//��������λ���ϵ�Ԫ�� 
		bool classItem( lexerFind, lexerType );//�������ͼ��� 
		bool classItem( int );//���վ���λ�ü��� 
		/***���õ�ǰ����***/
		bool inClass( string ); //��Ѱ��ǩ��Ĭ��������ñ�ǩ��Ĭ���඼�Դ�Ϊ��׼ 
		/***ͳ��item����***/
		int countItem( void ); 
		/***�����ݱ����item code***/
		string generateCode( void );
		/***����ǰitem������ַ���,ʧ���׳��쳣***/
		string encodeItem( void );
		/***��item code��������ݼ��뵽���ݿ���***/
		LexerCompRep compilerCode( string );
};

}
#endif
