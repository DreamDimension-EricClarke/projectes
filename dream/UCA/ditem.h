#ifndef __DITEM___
#define __DITEM___

/*
	item_lexer类能管理一系列item
	从中生成item code代码或从
	item code代码中读入item数据 
	item_lexer能从item code代码中生成数据序列
	也能从数据序列生成item code代码
	每个序列中可以有多个item对象
	每个item对象只能拥有一种数据类型,一个标签,和一份数据
	item_lexer支持的数据类型:( 数据类型描述符不区分大小 )
	boolean 对应C++中bool类型 其值只能是true或false两种类型,内容不区分大小 
	byte	对应C++中unsigned char类型的数组,其中每个字节都被书写成 类似 1E 的两位宽的十六进制数字,每两个数字间应当有空格间隙 
	hex		对应C++中unsigned long long类型,可以容纳一个最长8位的十六进制数字 
	number	对应C++中的double类型,可以不携带小数点 
	string 	对应C++中的string对象或char数组,string类型的item在item code中的书写涉及到转意字符,转意规则如下:
		&& = &
		&; = ; 
	none	不应该有任何数据内容的item
	
	item code 语法:
	( type )[ label ] value ;
	任何部分的内容都必不可少,type和label不应该被空格分隔开 
	label可以解释为 class:name.sub 
	一个label中最多有一个:和一个.多余此数量会被视为非法label
	一个label没有:则被视为则被视为基于名为default的class
*/
#include <string>
using namespace std;
namespace dream {
/***item中数据类型标志***/ 
enum lexerType {
	lexerTnone,		//空数据类型理应没有数据 
	lexerTboolean,	//boolean数据类型对应了C++中的bool数据类型 
	lexerTnumber,	//number数据类型对应了C++中的double数据类型 
	/***byte类型是唯一无法自动计算数据长度的类型***/ 
	lexerTbyte,		//byte数据类型对应了C++中的unsigned char数据类型的数据段 
	lexerThex,		//hex数据类型对应了C++中的unsigned long long数据类型 
	lexerTstring		//string数据类型对应了C++中的char数据类型的数据段 
};

/***item code编译错误标志***/ 
enum errorCode_lexer {
	lexerEsuccess,		//没有任何问题 
	lexerEbadcode,		//代码源无法读取 
	lexerEillegalNote,	//错误的注释起始 
	lexerEillegalItem,	//错误的项目起始 
	lexerEillegalType,	//非法的数据类型 
	lexerErepeatType,	//重复的类型定义 
	lexerElostType,		//缺少类型定义区 
	lexerEillegalLabel,	//非法的项目标签 
	lexerErepeatLabel,	//重复的标签定义 
	lexerElostLabel,		//缺少标签定义区 
	lexerEillegalValue,	//非法的数据内容 
	lexerElostValue		//缺少数据内容区 
};

/***item lexer中对token的理解***/ 
enum lexerIdent {
	lexerIillegal,	//非法字符 
	lexerIspace,		//空字符 
	lexerInewline,	//回车符 
	lexerIletterR,	//a~f范围内的字母 
	lexerIletterO,	//g~z范围内的字母 
	lexerInumber,	//0~9范围内的数字 
	lexerIother,		//其他合法的字符 
	lexerItypeB,		//类型定义区间开始符 
	lexerItypeE,		//类型定义区间结束符 
	lexerIlabelB,	//标签定义区间开始符 
	lexerIlabelE,	//标签定义区间结束符
	lexerInote,		//注释缓冲符 
	lexerInoteB,		//注释启动符 
	lexerInoteE,		//注释结束符 
	lexerItrans,		//转意符 
	lexerIitemE,		//项目结束符 
	lexerIend		//代码结束 
};

/***解析代码时编译器状态标志***/ 
enum LexerRegStatus {
	lexerSout,		//栈在外部,需要初始化 
	lexerSglobal,	//全局状态 可能缓冲注释 开始类型 或出现错误 
	lexerSnoteB,	//注释缓冲状态 可能开始注释 可能出现错误 
	lexerSnote,		//注释状态 可能缓冲注释 可能出现错误 
	lexerStype,		//类型定义状态 可能出现错误 
	lexerSitem,		//项目定义状态 可能缓冲注释 可能开始标签 可能出现错误 
	lexerSlabel,	//标签定义状态 可能出错 
	lexerSbool,		//boolean状态  可能结束项目 可能出错 
	lexerSbyte,		//byte状态 可能结束项目 可能出错 
	lexerShex,		//hex状态 可能结束项目 可能出错 
	lexerSnone,		//none状态 可能结束项目 可能出错 
	lexerSnumber,	//number状态 可能结束项目 可能出错 
	lexerSstring,	//string状态 可能开始转意 可能结束项目 可能出错 
	lexerStrans		//转意状态 可能出现错误 
};

/***相对寻找元素的标志***/
enum lexerFind {
	lexerFprev,		//上一个 
	lexerFnext,		//下一个 
	lexerFfirst,		//第一个 
	lexerFlast		//最后一个 
};

/***解释器报告结构体***/ 
struct LexerCompRep {
	errorCode_lexer	error;	//错误代码
	int offb;			//代码偏移 
	int line;			//代码行号
	int offl;			//代码行内偏移量 
	int count;			//成功编译的节点计数 
};

class item_lexer {
	public:
		/***解释器有限状态机模型状态栈***/ 
		struct StatusStack {
			LexerRegStatus top;		//栈顶元素 
			StatusStack* prev;	//栈链指针 
		};
		/***item数据结构体***/
		struct item {
			lexerType mtType;	//item类型 
			string msLabel;		//item标签 
			char *mpValue;		//item数据空间 
			int miLength;		//item数据长度 
			/***item链指针***/
			item *next;
			item *prev;
		};
	private:
		/***item lexer类数据内容***/
		item *mpList = 0;	//item列表 
		item *mpCurrent = 0;//item当前指向
		string	msClass;	//当前类域 
		
		/***item code源代码和编译寄存数据***/ 
		StatusStack	*msStack = 0;//状态栈
		errorCode_lexer	meError;//错误状态 
		string 		msCode;	//源代码 
		string 		msToken;//关键字
		bool		mbSpace;//关键字是否已经被隔断 
		bool		mbPoint;//数字被隔点隔开 
		int 		miOffb;	//代码偏移 
		int 		miLine;	//代码行号
		int 		miOffl;	//代码行内偏移量 
		int			miRegCount;//最新编译的节点计数 
	private:
		LexerRegStatus pushReg( LexerRegStatus );	//入栈状态,返回入栈后的状态 
		LexerRegStatus popReg( void );		//出栈状态,返回出栈后的状态 
		LexerRegStatus getReg( int = 0 );	//获取第n层的堆栈内容 
		LexerRegStatus setReg( LexerRegStatus );	//设置顶层栈状态 
		bool initReg( const string& );	//初始化一个寄存状态 
		char regNext( void );			//指向下一个字符并自动换行和偏移 
		char regCode( void );			//返回当前指向的字符
		LexerCompRep	regReport( void );		//返回报告信息
		lexerIdent regIdent( void );	//测试当前指向字符的身份 
		lexerIdent identChar( char );	//检测一个字符的身份 
		bool regProc( void ) ;			//处理当前指向字符,添加数据或报告错误 
		
		double string2number( string );
		unsigned long long string2hex( string );
		unsigned char string2byte( string );
		bool string2bool( string );	//字符串转换成布尔类型失败抛出invalid_argument异常 
		lexerType string2type( string );
		string type2string( lexerType );
		string item2string( item& );
		bool uniqLabel( const string& );//测试标签添加到链表中是否唯一
		bool correctClass( string& );//测试类名是否正确 
		bool correctLabel( string& );//测试标签是否正确,并补全作用域
		string label2class( const string& );//从标签中提取类域
		string label2name( const string& );//从标签中提取名称 
		
		item* classItem( item* );	//从指定的节点和其后的节点中找出第一个在当前类域中的节点 
		item* classPrev( item* );	//从指定的节点和其前的节点中找出第一个在当前类域中的节点 
	public:
		item_lexer();
		~item_lexer();
		
		/***清空所有数据***/ 
		void clearAll( void );
		/***清空编译寄存器***/ 
		void clearComp( void );
		/***清空当前指向的item***/ 
		bool clearItem( void );
		/***在末端添加新的item***/
		string pushItem( void );
		bool pushItem( string );
		bool pushItem( string, bool );
		bool pushItem( string, unsigned char*, int );
		bool pushItem( string, unsigned long long );
		bool pushItem( string, double );
		bool pushItem( string, string );
		/***在当前位置添加新的item,若成功则指向这个item***/ 
		string insertItem( void );
		bool insertItem( string );
		bool insertItem( string, bool );
		bool insertItem( string, unsigned char*, int );
		bool insertItem( string, unsigned long long );
		bool insertItem( string, double );
		bool insertItem( string, string );
		/***删除元素***/
		bool removeItem( void );	//删除当前指向的元素 
		bool removeItem( string );	//删除由标签指定的元素
		/***设置标签***/
		bool setLabel( string );
		/***设置类域***/ 
		bool setClass( string );
		/***设置类型***/
		bool setType( lexerType ); 
		/***设置内容,类型不符会失败***/
		bool setValue( bool );
		bool setValue( unsigned char*, int );
		bool setValue( unsigned long long );
		bool setValue( double );
		bool setValue( string );
		/***获取内容,类型不符则抛出异常***/
		bool getBool( void );
		unsigned char* getByte( void );
		unsigned long long getHex( void );
		double getNumber( void );
		string getString( void );
		/***获取类型,失败抛出异常***/
		lexerType getType( void );
		/***获取数据长度,失败返回-1***/
		int getLength( void );
		/***获取标签,失败抛出异常***/
		string getLabel( void ); 
		/***获取类域,失败抛出异常***/
		string getClass( void ); 
		/***获取item位置,失败则返回-1***/
		int getPos( void ); 
		/***寻找全局中的元素,并指向***/
		bool findItem( string );		//按照标签检索,标签若不包含类域则在当前类域搜索 
		bool findItem( lexerFind );		//检索特殊位置上的元素 
		bool findItem( lexerFind, lexerType );//按照类型检索 
		bool findItem( int );//按照绝对位置检索 
		/***寻找当前域中的元素,并指向***/ 
		bool classItem( lexerFind );	//检索特殊位置上的元素 
		bool classItem( lexerFind, lexerType );//按照类型检索 
		bool classItem( int );//按照绝对位置检索 
		/***设置当前类域***/
		bool inClass( string ); //搜寻标签的默认类和设置标签的默认类都以此为标准 
		/***统计item总数***/
		int countItem( void ); 
		/***将数据编码成item code***/
		string generateCode( void );
		/***将当前item编码成字符串,失败抛出异常***/
		string encodeItem( void );
		/***将item code编译成数据加入到数据库内***/
		LexerCompRep compilerCode( string );
};

}
#endif
