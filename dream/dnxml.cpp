
#include "dnxml.h"

namespace dnxml {
	
	
	xmlnode::xmlattr::xmlattr( std::string name_, std::string value_, xmlnode::xmlattr* next_ ):
	name(name_),value(value_),next(next_)
	{
	}
	
	xmlnode::xmlnode( const std::string& name, xmlnode* elder, const NodeType type ):
	m_type(type),m_name(name),m_text(m_name),m_elder(elder),m_next(0),m_prev(0),m_child(0),m_attrs(0)
	{
		if( m_type == TextNode )
			if( m_elder == 0 )
				throw std::runtime_error( "dnxml::node::node(...):�ı��ڵ㲻����Ϊ���ڵ����\n" );
			else if(  );
	}
}
