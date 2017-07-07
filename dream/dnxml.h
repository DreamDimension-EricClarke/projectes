#ifndef __dnxml__
#define __dnxml__

#include <iostream>
#include <stdexcept>

namespace dnxml {

/*
NodeType �����˽ڵ�����
*/
enum NodeType {
	
	
	TextNode,
	/*
	�ı��ڵ�:���� < nodename attr='attr' >text value</ nodename > ��
		"text value"����Ϊ nodename �ڵ��һ���ı��ӽڵ�
	�ı��ڵ�û���ӽڵ�,û������,û������,ʵ����,�ı��ڵ������ڱ������ֵı��������������ı���ֵ
	�ı��ڵ㲻����������,���ı��ڵ����ӵ�и��ڵ� 
	*/
	
	
	TreeNode,
	/*
	���ڵ�:����������nodename�ڵ�,���ڵ�ӵ������,�ӽڵ�,���������䵱���ڵ�. 
	*/
	
	
	ContentNode 
	/*
	���ݽڵ�:content�ڵ��,������һ�����ڵ�,���ڵ�ӵ��Ψһ���ı��ӽڵ��Ϊֵ�ڵ�
	���ݽڵ�ӵ��Ĭ�ϵ�Ψһ���ı��ӽڵ�,��װ������ӽڵ������ͼɾ��Ĭ�ϵ��ӽڵ㶼��ʧ��
	
	���ݽڵ�ӵ������xmlcontent = 'true'�Ա�֤content�ܴ��ĵ�����ȷ��ԭ 
	xml�ĵ��еĽڵ���ӵ�д�����,����ֻӵ��һ���ı��ӽڵ�,��ᱻʶ��Ϊһ��ContentNode
	�����κ����,�ڵ㶼�ᱻʶ��Ϊһ��TreeNode(����û�д�����,������ӵ������ֵ,��ڵ�û��Ψһ���ı��ӽڵ�) 
	�ڲ�ѯTreeNodeʱ,content�ڵ���Ա���Ϊһ��TreeNode��������,��������
	���ڲ�ѯContentNodeʱ,ͬ����TreeNode�ڵ�ᱻ����,��ʹ��ֻ��һ���ı��ӽڵ�
	*/
};

/*
xmlnode ��������xml�ĵ��е�һ���ڵ�,���һ���ڵ������ΪTreeNode��˽ڵ������Ϊһ������һ������ 
*/
class xmlnode {
	private:
		/*
		struct attr �ṹ�����ڱ���ڵ����Ե�������������ֵͬʱ������������֯��һ������������ 
		*/
		typedef struct xmlattr{
			std::string name;		//�������� 
			std::string value;		//����ֵ 
			xmlattr* next;			//�������е���һ������ 
			
			/*
			xmlattr �������͵Ĺ��캯�������ڿ����������б�������� 
			
			����:
				std::string name_ ���Խڵ㽫ӵ�е�����
				std::string value_���Խڵ㽫ӵ�е�ֵ
				std::string next_���Խڵ㽫ӵ�е�����ָ�� 
			*/
			xmlattr( std::string name_, std::string value_, xmlattr* next_ );
		}*xmlattrs;
	private:
		//�����������κβ����������һ�����������
		//ֻ�в��׳��쳣�ĺ����Ż��޸����������Ϣ������ϴ��󱨸� 
		static std::string	lasterror;
		
		const NodeType		m_type;	//�ڵ�������ڽڵ�������������ڲ��ɸı�
		
		std::string 		m_name;	//�ڵ�����,�˱���ֻ���ڽڵ�����Ϊ���ڵ�ʱ�ſ��� 
		std::string&		m_text;	//�ڵ���ı�����,ʵ������m_name������,ֻ�е��ڵ�Ϊ�ı��ڵ�ʱ���ʲ���Ч 
		xmlattrs			m_attrs;//�ڵ�������б�,ֻ�е��ڵ������ڵ�ʱ���ʲ���Ч 
		
		xmlnode*			m_next;	//�ڵ�������ֵܽڵ�ָ�� 
		xmlnode*			m_prev;	//�ڵ�������ֵܽڵ�ָ�� 
		xmlnode*			m_elder;//�ڵ�ĳ����ڵ�ָ�� 
		xmlnode*			m_child;//�ڵ���ӽڵ�ָ��ָ�����ӽڵ������е������νڵ� 
	private:
		
		
		/*
		trimname �ж�һ���ַ��������е������Ƿ���Գ�Ϊһ���Ϸ��Ľڵ����������
			���Ϸ�,trimnameֱ�ӽ�name�ж���Ŀհ׷�ȥ��,���ͳһ�����ʽ,�������ƱȽ�
		
		����: 
			std::string& name ��Ҫ���Ե����ֵ����� 
		
		����ֵ: bool
			��name����һ���Ϸ�����,��name�����ݲ����ܵ��κθı�,��������false
			��֮,name�����޸�Ϊ���״̬,���Һ�������true 
		*/
		bool trimname( std::string& name ); 
		
		
		/*
		memtext ��һ���ı����ݴ�raw״̬ת��Ϊmem״̬:
			raw״̬:��ָtextԭ����״̬,���е����ݶ���ϣ���������ֵ,���ܻ������ǩ��ֹ����ת�����
			mem״̬:��ָtext��xml�ĵ��е�״̬,���в��Ϸ����ַ���ʹ����ת�����ʼ��ת���ַ�������
			
			nodeʹ��mem״̬��text�洢�ı�����,����settext�����text��gettext������text����raw״̬��
			ʹ��tostring����encode���ɵõ�mem״̬��textֵ 
			
		����:
			std::string& text ������ת�����ַ�������
		����ֵ:
			bool ��text���ɹ�ת����������true����text�����ܵ��κθı�,����������false 
			����!!!��textΪ��,��������ʧ��,�����ڵ�д���ı�ʱ���ᶪʧһ���ı��ڵ� 
		*/
		bool memtext( std::string& text );
		
		
		/*
		rawtext ��һ���ı���mem״̬ת��Ϊraw״̬
			ʵ����,rawtext��������������һ���ַ������״���ʲô����״̬,������ʶ�������ת���ַ���ת����
			�ض����ַ�.
			
		����:
			std::string& text �������껨���ַ�������
		
		����ֵ:
			bool ��ת���ɹ�,��������true
			�������˷Ƿ���ת���ַ���������ԭ��,��������ʧ��,��ʱtext��ֵ�����ܵ��κθı�,���Һ�������false 
		*/
		bool rawtext( std::string& text );
	private:
		/*
		xmlnode ���캯��������Ϊ˽�к����Դ�ǿ��xmlnodeʵ����ʹ��newnode,newtext,newcontent��������
		���캯���Ὣ�ڵ����������������,����һ���ڵ���뵽ĳ������ĳ��λ�õ�������newnode,newtext,
		newcontent�Ⱥ�����ʵ��. 
		
		����:
			const std::string& name	�ڵ�����ƻ��ı�����
			xmlnode* elder			Ŀ��ڵ�ĸ��ڵ�
			NodeType type			Ŀ��ڵ������
		
		����ֵ:
			���ڵ㴴��ʧ�����׳��쳣runtime_error
			 
		*/
		xmlnode( const std::string& name, xmlnode* elder = 0, const NodeType type = TreeNode );
		
	public:
		
		
		/*
		xmlnode �����������ͷŽڵ������е�������Դ,���������Լ��Ӹ��ڵ����ͷ� 
		*/
		~xmlnode();
		
		
		/*
		xmlnode* newnode ������Ϊxmlnode��ľ�̬����,��������һ�����ڵ�,���Զ��ҽӵ�ĳ��������
		
		����: 
			cosnt std::string& name	���ڵ㼴��ӵ�е�����
			xmlnode* elder			���ڵ㼴��ӵ�еĸ��ڵ�
			
		����ֵ:
			�������ɹ������˽ڵ�,�򷵻��½ڵ��ָ��,�������������
			������ʧ��,�򷵻�0�����ô�������
		*/
		static xmlnode* newnode( const std::string& name, xmlnode* elder = 0 );
		
		
		/*
		xmlnode* newtext ������Ϊxmlnode��ľ�̬����,��������һ���ı��ڵ�,���Զ��ҽӵ�ĳ��������
		
		����:
			const std::string& text	�ı��ڵ������
			xmlnode* elder			�ı��ڵ�ĸ��ڵ�ָ��
			
		����ֵ:
			�������ɹ������˽ڵ�,�򷵻��½ڵ��ָ��,�������������
			������ʧ��,�򷵻�0�����ô�������
		*/
		static xmlnode* newtext( const std::string& text, xmlnode* elder );
		
		
		/*
		xmlnode* newcontent ������Ϊxmlnode��ľ�̬����,��������һ��content�ڵ��,���Զ��ҽӵ�ĳ��������
			content�ڵ��ָһ�����ڵ�(����Ϊ���ڵ�)����Ψһ���ı��ӽڵ�(����Ϊֵ�ڵ�)һ����ɵ�һ��ӵ����
			�ֺ����Ե��ı��ڵ����
			content�ڵ�ӵ��Ĭ�ϵ�����xmlcontent = 'true'��content�ڵ㱻�����xml�ĵ�ʱ���Զ�д��,�Ա�֤content
			�ڵ��ܴ�xml�ĵ�����ȷ�ػָ�,��xml�ĵ��ָ�һ��ӵ��xmlcontent���ԵĽڵ�ʱ,���ɹ�ת����Ϊcontent�ڵ�,��
			xmlcontent���Իᱻɾ��,�Ա��ⱻ�������޸�,����,xmlcontent���Իᱻ����,�Ա�������; 
			
			����:
				<head> 
					<text xmlcontent='true' color='red'>red text here</text> 
					<address xmlcontent='true' type='ip'>127.0.0.1</address> 
				</head>
			����,<text ...> ... </text> �� <address ... ... </address>����������head�ڵ��content�ڵ��
			
			��content�йصĲ������ϸ�ļ��,content�ڵ��ֻ����һ�����ڵ�,һ��ֵ�ڵ�,���ڵ�ֻ����Ψһ���ӽڵ�.
			������κεط�������Ҫ��,��ڵ�Ի�ڵ������ܱ���Ϊһ��content�ڵ�,��ᵼ������ڵ�Ի�ڵ�����content
			�����б�����,����ܻ�����content�����Ҳ���Ŀ��ڵ�,����
		
		����:
			const std::string& name	content�ڵ�Ե�����
			const std::string& text content�ڵ�Ե��ı�����
			xmlnode* elder			content�ڵ�Եĸ��ڵ�
		
		����ֵ:
			�������ɹ������˽ڵ�,�򷵻��½ڵ��ָ��,�������������
			������ʧ��,�򷵻�0�����ô������� 
		*/
		static xmlnode* newcontent( const std::string& name, const std::string& text, xmlnode* elder = 0 );
		
		
		/*
		is �������ڲ��Ա��ڵ��Ƿ����ڸ��ڵ�����
		
		����:
			NodeType type 	���������ԵĽڵ�����
		
		����ֵ:
			bool	��type��ڵ㱾��Ľڵ�������ͬ,��������true 
		*/
		bool is( NodeType type );
		
		
		/*
		gettype �������ڻ�ȡ��ǰ�ڵ�Ľڵ�����
		
		����ֵ:
			NodeType ��ǰ�ڵ�Ľڵ����� 
		*/
		NodeType gettype( void );
		
		
		/*
		bool setname ����Ϊ�ڵ���������,�����Ʋ��Ϸ���ڵ����Ͳ�����ڵ�ӵ��������������ʧ��
		
		����:
			cosnt std::string& name ��������Ϊ�ڵ����Ƶ��ַ��� 
		
		����ֵ:
			bool ������ʧ��,��������falseͬʱ,���������ô�������
			��֮,��������true������������� 
		*/
		bool setname( const std::string& name );
		
		
		/*
		std::string getname �������ڻ�ȡ��ǰ�ڵ������,���ڵ㲻ӵ�����Ƶĺ�������ʧ��
		
		����:
			std::string& name ���ڽ��շ��ص����Ƶ��ַ�������
			 
		����ֵ:
			bool �������ɹ�,�򷵻�true,name�������˵�ǰ�ڵ������
			������ʧ�����𷵻�false,����name�����ܵ��κθı�  
		*/
		bool getname( std::string& name );
		
		
		/*
		bool settext ��������Ϊ�ڵ������ı�����ֵ,���ڵ����Ͳ����ı����ͻ�Ŀ���ı��޷�ת����ʧ��
			����ǰ�ڵ�ĸ��ڵ���content�ڵ�,�����õ�Ŀ���ı�����Ϊ��������Ҳ��ʧ�� 
		
		����:
			const std::string& text �������õ��ı�����
		
		����ֵ:
			bool �����ɹ��򷵻�true,�������ô�������,������false; 
		*/
		bool settext( const std::string& text );
		
		
		/*
		bool gettext �������ڻ�ȡ��ǰ�ڵ���ı�����,����ǰ�ڵ㲻ӵ���ı�����,��������ʧ��
		
		����:
			std::string& text �����ں����ɹ�ʱ���浱ǰ�ڵ���ı����ݵ��ַ�������.
			
		����ֵ:
			bool �����ɹ��򷵻�true,�����������ô�������,������false 
		*/
		bool gettext( std::string& text );
		
		
		/*
		bool setcontent �������ڶ�content�ڵ���ı��ӽڵ㸳ֵ,���ڵ㲻��content�ڵ�,�����û�ʧ��
		
		����:
			const std::string& text	���������ı��ӽڵ���ı�����
			
		����ֵ:
			bool �������ɹ�,�򷵻�true,������ʧ���򷵻�false 
		*/
		bool setcontent( const std::string& text ); 
		
		
		/*
		bool setcontent ������������һ��content�ڵ�Ե��ı��ڵ���ı�����
			���������nameѰ�ҵ���һ�����ҵ���content�ڵ��,�������޸�
		
		����:
			const std::stirng& name		�����ڲ�ѯ��Ŀ��content������
			const std::string& text		�����õ�Ŀ��content���ı�����
			
		����ֵ:
			bool �����ɹ��򷵻�true,�����������ô�������������false 
		*/
		bool setcontent( const std::string& name, const std::string& text );
		
		
		/*
		bool setcontent ������������ָ����content�ڵ�Ե����ı��ڵ���ı�����
			������Ѱ�ҵ���pos+1������Ϊname��content�ڵ��,���޸����е��ı�����
			
		����: 
			const std::string& name		Ŀ��content������
			int pos						Ŀ��content���±�,���Ǹ�����������±�
										�������һ����ʵ��˳����ͬ,��ֻ����ӵ
										�ж�Ӧ���Ƶ�content���������.
			const std::string& text		�������õ�Ŀ��ڵ���е��ı�����
			
		 ����ֵ:
		 	bool ������ʧ���򷵻�false,�����ô�������,�������ɹ�,�򷵻�true 
		*/
		bool setcontent( const std::string& name, int pos, const std::string& text );
		
		
		/*
		bool getcontent �������ڻ�ȡcontent�ڵ�Ե��ı��ӽڵ���ı�����
			�����ڵ㲢��һ��content�ڵ�������ʧ��
		
		����:
			std::string& text	��������content���ı�����
		
		����ֵ:
			bool ������ʧ���򷵻�false,���򽫷���true 
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
