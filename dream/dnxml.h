#ifndef __dnxml__
#define __dnxml__

#include <iostream>
#include <stdexcept>

namespace dnxml {

/*
NodeType 描述了节点类型
*/
enum NodeType {
	
	
	TextNode,
	/*
	文本节点:例如 < nodename attr='attr' >text value</ nodename > 中
		"text value"被视为 nodename 节点的一个文本子节点
	文本节点没有子节点,没有属性,没有名字,实际上,文本节点中用于保存名字的变量被用来保存文本的值
	文本节点不允许单独存在,即文本节点必须拥有父节点 
	*/
	
	
	TreeNode,
	/*
	树节点:例如上例中nodename节点,树节点拥有属性,子节点,可以用来充当根节点. 
	*/
	
	
	ContentNode 
	/*
	内容节点:content节点对,包含了一个键节点,键节点拥有唯一的文本子节点称为值节点
	内容节点拥有默认的唯一的文本子节点,安装更多的子节点或者试图删除默认的子节点都会失败
	
	内容节点拥有属性xmlcontent = 'true'以保证content能从文档中正确还原 
	xml文档中的节点若拥有此属性,并且只拥有一个文本子节点,则会被识别为一个ContentNode
	其他任何情况,节点都会被识别为一个TreeNode(比如没有此属性,或属性拥有其他值,或节点没有唯一的文本子节点) 
	在查询TreeNode时,content节点可以被视为一个TreeNode而被发现,或重命名
	但在查询ContentNode时,同名的TreeNode节点会被忽略,即使它只有一个文本子节点
	*/
};

/*
xmlnode 类描述了xml文档中的一个节点,如果一个节点的类型为TreeNode则此节点可以视为一棵树或一棵子树 
*/
class xmlnode {
	private:
		/*
		struct attr 结构体用于保存节点属性的属性名和属性值同时将所有属性组织在一个单向链表中 
		*/
		typedef struct xmlattr{
			std::string name;		//属性名称 
			std::string value;		//属性值 
			xmlattr* next;			//属性链中的下一个属性 
			
			/*
			xmlattr 属性类型的构造函数有助于快速向属性列表添加属性 
			
			参数:
				std::string name_ 属性节点将拥有的名称
				std::string value_属性节点将拥有的值
				std::string next_属性节点将拥有的连接指针 
			*/
			xmlattr( std::string name_, std::string value_, xmlattr* next_ );
		}*xmlattrs;
	private:
		//在整个类中任何操作引起的上一个错误的描述
		//只有不抛出异常的函数才会修改这个错误信息用来配合错误报告 
		static std::string	lasterror;
		
		const NodeType		m_type;	//节点的类型在节点的整个生命周期不可改变
		
		std::string 		m_name;	//节点名称,此变量只有在节点类型为树节点时才可用 
		std::string&		m_text;	//节点的文本内容,实际上是m_name的引用,只有当节点为文本节点时访问才有效 
		xmlattrs			m_attrs;//节点的属性列表,只有当节点是树节点时访问才有效 
		
		xmlnode*			m_next;	//节点的下游兄弟节点指针 
		xmlnode*			m_prev;	//节点的上游兄弟节点指针 
		xmlnode*			m_elder;//节点的长辈节点指针 
		xmlnode*			m_child;//节点的子节点指针指向了子节点序列中的最上游节点 
	private:
		
		
		/*
		trimname 判断一个字符串中所有的内容是否可以成为一个合法的节点或属性名字
			若合法,trimname直接将name中多余的空白符去掉,变成统一的最简式,方便名称比较
		
		参数: 
			std::string& name 将要测试的名字的引用 
		
		返回值: bool
			若name不是一个合法名字,则name的内容不会受到任何改变,函数返回false
			反之,name将被修改为最佳状态,并且函数返回true 
		*/
		bool trimname( std::string& name ); 
		
		
		/*
		memtext 将一段文本内容从raw状态转化为mem状态:
			raw状态:即指text原本的状态,其中的内容都是希望被保存的值,可能会包涵标签起止符或转义符号
			mem状态:即指text在xml文档中的状态,其中不合法的字符会使用以转义符开始的转移字符串代替
			
			node使用mem状态的text存储文本内容,但是settext传入的text和gettext传出的text都是raw状态的
			使用tostring或者encode即可得到mem状态的text值 
			
		参数:
			std::string& text 即将被转化的字符串内容
		返回值:
			bool 若text被成功转化则函数返回true否则text不会受到任何改变,而函数返回false 
			当心!!!若text为空,则函数不会失败,但将节点写入文本时将会丢失一个文本节点 
		*/
		bool memtext( std::string& text );
		
		
		/*
		rawtext 将一段文本从mem状态转化为raw状态
			实际上,rawtext函数并不能区分一个字符串到底处于什么样的状态,它把能识别出来的转移字符串转化成
			特定的字符.
			
		参数:
			std::string& text 即将被钻花的字符串内容
		
		返回值:
			bool 若转化成功,则函数返回true
			若遇到了非法的转移字符串或其他原因,函数可能失败,此时text的值不会受到任何改变,并且函数返回false 
		*/
		bool rawtext( std::string& text );
	private:
		/*
		xmlnode 构造函数被设置为私有函数以此强制xmlnode实例被使用newnode,newtext,newcontent函数产生
		构造函数会将节点自身参数配置完整,但将一个节点加入到某棵树的某个位置的任务由newnode,newtext,
		newcontent等函数来实现. 
		
		参数:
			const std::string& name	节点的名称或文本内容
			xmlnode* elder			目标节点的父节点
			NodeType type			目标节点的类型
		
		返回值:
			若节点创建失败则抛出异常runtime_error
			 
		*/
		xmlnode( const std::string& name, xmlnode* elder = 0, const NodeType type = TreeNode );
		
	public:
		
		
		/*
		xmlnode 析构函数将释放节点所持有的所有资源,并主动将自己从父节点中释放 
		*/
		~xmlnode();
		
		
		/*
		xmlnode* newnode 被设置为xmlnode类的静态函数,用来创造一个树节点,并自动挂接到某个树根下
		
		参数: 
			cosnt std::string& name	树节点即将拥有的名称
			xmlnode* elder			树节点即将拥有的父节点
			
		返回值:
			若函数成功创建了节点,则返回新节点的指针,并清除错误描述
			若函数失败,则返回0并设置错误描述
		*/
		static xmlnode* newnode( const std::string& name, xmlnode* elder = 0 );
		
		
		/*
		xmlnode* newtext 被设置为xmlnode类的静态函数,用来创造一个文本节点,并自动挂接到某个树根下
		
		参数:
			const std::string& text	文本节点的内容
			xmlnode* elder			文本节点的父节点指针
			
		返回值:
			若函数成功创建了节点,则返回新节点的指针,并清除错误描述
			若函数失败,则返回0并设置错误描述
		*/
		static xmlnode* newtext( const std::string& text, xmlnode* elder );
		
		
		/*
		xmlnode* newcontent 被设置为xmlnode类的静态函数,用来创造一个content节点对,并自动挂接到某个树根下
			content节点对指一个树节点(被称为键节点)与其唯一的文本子节点(被称为值节点)一起组成的一个拥有名
			字和属性的文本节点组合
			content节点拥有默认的属性xmlcontent = 'true'当content节点被编码成xml文档时会自动写入,以保证content
			节点能从xml文档中正确地恢复,从xml文档恢复一个拥有xmlcontent属性的节点时,若成功转化成为content节点,则
			xmlcontent属性会被删除,以避免被无意中修改,否则,xmlcontent属性会被保留,以备其他用途 
			
			例如:
				<head> 
					<text xmlcontent='true' color='red'>red text here</text> 
					<address xmlcontent='true' type='ip'>127.0.0.1</address> 
				</head>
			其中,<text ...> ... </text> 和 <address ... ... </address>是两个属于head节点的content节点对
			
			与content有关的操作有严格的检查,content节点对只能有一个键节点,一个值节点,键节点只能有唯一的子节点.
			如果有任何地方不符合要求,则节点对或节点树不能被视为一个content节点,这会导致这个节点对或节点树在content
			操作中被忽略,这可能会引起content操作找不到目标节点,或者
		
		参数:
			const std::string& name	content节点对的名字
			const std::string& text content节点对的文本内容
			xmlnode* elder			content节点对的父节点
		
		返回值:
			若函数成功创建了节点,则返回新节点的指针,并清除错误描述
			若函数失败,则返回0并设置错误描述 
		*/
		static xmlnode* newcontent( const std::string& name, const std::string& text, xmlnode* elder = 0 );
		
		
		/*
		is 函数用于测试本节点是否属于个节点类型
		
		参数:
			NodeType type 	即将被测试的节点类型
		
		返回值:
			bool	若type与节点本身的节点类型相同,则函数返回true 
		*/
		bool is( NodeType type );
		
		
		/*
		gettype 函数用于获取当前节点的节点类型
		
		返回值:
			NodeType 当前节点的节点类型 
		*/
		NodeType gettype( void );
		
		
		/*
		bool setname 函数为节点设置名称,若名称不合法或节点类型不允许节点拥有名称则函数将会失败
		
		参数:
			cosnt std::string& name 将被设置为节点名称的字符串 
		
		返回值:
			bool 若函数失败,则函数返回false同时,函数将设置错误描述
			反之,函数返回true并清除错误描述 
		*/
		bool setname( const std::string& name );
		
		
		/*
		std::string getname 函数用于获取当前节点的名称,若节点不拥有名称的函数将会失败
		
		参数:
			std::string& name 用于接收返回的名称的字符串引用
			 
		返回值:
			bool 函数若成功,则返回true,name将保存了当前节点的名称
			若函数失败免责返回false,并且name不会受到任何改变  
		*/
		bool getname( std::string& name );
		
		
		/*
		bool settext 函数用于为节点设置文本内容值,若节点类型不是文本类型或目标文本无法转意则失败
			若当前节点的父节点是content节点,而设置的目标文本内容为空则设置也会失败 
		
		参数:
			const std::string& text 即将设置的文本内容
		
		返回值:
			bool 函数成功则返回true,否则将设置错误描述,并返回false; 
		*/
		bool settext( const std::string& text );
		
		
		/*
		bool gettext 函数用于获取当前节点的文本内容,若当前节点不拥有文本内容,则函数将会失败
		
		参数:
			std::string& text 用于在函数成功时保存当前节点的文本内容的字符串引用.
			
		返回值:
			bool 函数成功则返回true,否则函数将设置错误描述,并返回false 
		*/
		bool gettext( std::string& text );
		
		
		/*
		bool setcontent 函数用于对content节点的文本子节点赋值,若节点不是content节点,则设置会失败
		
		参数:
			const std::string& text	用于设置文本子节点的文本内容
			
		返回值:
			bool 若函数成功,则返回true,若函数失败则返回false 
		*/
		bool setcontent( const std::string& text ); 
		
		
		/*
		bool setcontent 函数用于设置一个content节点对的文本节点的文本内容
			函数会根据name寻找到第一个能找到的content节点对,并进行修改
		
		参数:
			const std::stirng& name		将用于查询的目标content的名字
			const std::string& text		将设置到目标content的文本内容
			
		返回值:
			bool 函数成功则返回true,否则函数将设置错误描述并返回false 
		*/
		bool setcontent( const std::string& name, const std::string& text );
		
		
		/*
		bool setcontent 函数用于设置指定的content节点对到的文本节点的文本内容
			函数会寻找到第pos+1个名称为name的content节点对,并修改其中的文本内容
			
		参数: 
			const std::string& name		目标content的名字
			int pos						目标content的下标,这是个虚拟的数组下标
										假设存在一个与实际顺序相同,但只包含拥
										有对应名称的content的数组存在.
			const std::string& text		即将设置到目标节点队中的文本内容
			
		 返回值:
		 	bool 若函数失败则返回false,并设置错误描述,若函数成功,则返回true 
		*/
		bool setcontent( const std::string& name, int pos, const std::string& text );
		
		
		/*
		bool getcontent 函数用于获取content节点对的文本子节点的文本内容
			若本节点并非一个content节点则函数会失败
		
		参数:
			std::string& text	用来保存content的文本内容
		
		返回值:
			bool 若函数失败则返回false,否则将返回true 
		*/
		bool getcontent( std::string& text );
		bool getcontent( const std::string& name, std::string& text );
		bool getcontent( const std::string& name, int pos, std::string& text );
		
		bool setattr( const std::string& name, const std::string& value );
		bool getattr( const std::string& name, std::string& value );
		bool unsetattr( const std::string& name );
};
	
}
#endif
