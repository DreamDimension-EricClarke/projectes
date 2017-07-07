#ifndef __DITEM_CPP__
#define __DITEM_CPP__
#include "item_lexer.h"
#include <exception>
#include <stdexcept>
#include <time.h>

namespace dream {
bool item_lexer::regProc( void ) 			//处理当前指向字符,添加数据或报告错误 
{
	switch( this->getReg() )
	{
		case lexerSglobal://全局状态 可能缓冲注释 开始类型 或出现错误 
			switch( this->regIdent() )
			{
				case lexerIend://在全局状态下遇到结束符意味着代码正常结束
					break; 
				case lexerIspace://跳过无害的空格 
				case lexerInewline:
					break;
				case lexerInote://遇到注释符则开始缓冲注释 
					this->pushReg( lexerSnoteB );
					break;
				case lexerItypeB://类型定义起始符表示类型定义开始
					this->msToken.clear();
					this->mbSpace = false;
					this->pushReg( lexerStype );
					break;
				default://其他字符都应该视为非法的项目
					this->meError = lexerEillegalItem;
					return false; 
			}
			break;
		case lexerSnoteB://注释缓冲状态 可能开始注释 可能出现错误 
			if( this->regIdent() == lexerInoteB ) //遇到了组合起来的注释起始符
			{
				if( this->getReg( 1 ) == lexerSnote )//检测说明在注释状态遇到了组合的注释起始符应当忽略
					break;
				else //其他任何区域只要进入注释缓冲状态就说明接受注释 
				{
					this->popReg();//退出注释缓冲阶段 
					this->pushReg( lexerSnote );//进入进入注释状态
					break; 
				}
			}
			else if( this->regIdent() == lexerInote )//遇到了组合的注释结束符
			{
				if( this->getReg( 1 ) == lexerSnote )//检测说明在煮熟状态遇到了组合的注释结束符应当结束注释 
				{
					this->popReg();//退出注释缓冲阶段
					this->popReg();//退出注释阶段 
					break;
				}
				else  //检测说明在任何其他区域遇到了组合的注释结束符应当报错 
				{
					this->meError = lexerEillegalNote;
					return false;
				}
			}
			else//未能成功构成注释符号 
			{
				if( this->getReg( 1 ) == lexerSnote )//在注释状态内应当忽略 
					break;
				else	//其他状态则应当报错 
				{
					this->meError = lexerEillegalNote;
					return false; 
				}
			}
			break;	
		case lexerSnote://注释状态 可能缓冲注释 可能出现错误 
			if( this->regIdent() == lexerIend )
			{
				this->meError = lexerEbadcode;
				return false;
			}
			else if( this->regIdent() == lexerInoteE )//遇到注释结束符 
			{
				this->pushReg( lexerSnoteB );//进入注释缓冲状态 
				break;
			}
			else
				break; 
		case lexerStype://类型定义状态 可能出现错误 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					if( this->msToken.length() == 0 )
						this->mbSpace = true;
					break;
				case lexerIletterO:
				case lexerIletterR://type区理应不区分大小写 
						if( this->msCode[this->miOffb] >= 'A' and this->msCode[this->miOffb] <= 'Z' )
							this->msCode[this->miOffb] += 32;
				case lexerInumber:
				case lexerIother:
					if( this->mbSpace )
					{//类型定义不应该被空格隔断 
						this->meError = lexerEillegalType;
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerItypeE:
					if( this->msToken.length() == 0 )
					{
						this->meError = lexerElostType;
						return false;
					}
					try {
						//如果顺利,则创建了节点设定了类型 
						this->pushItem();
						this->findItem( lexerFlast );
						this->setType( this->string2type( this->msToken ) );
						//如果顺利,则清理token相关数据并进入item 
						this->msToken.clear();
						this->mbSpace = false;
						this->popReg();//退出类型定义区
						this->pushReg( lexerSitem );//进入item模式 
						break;
					}catch( invalid_argument e ) {
						this->meError = lexerEillegalType;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
				default:
					this->meError = lexerEillegalType;
					return false;
			}
			break;
		case lexerSitem://项目定义状态 可能缓冲注释 可能开始标签 可能出现错误 
			switch( this->regIdent() )
			{
				case lexerIspace://跳过无害的空格 
					break;
				case lexerInote://遇到注释符则开始缓冲注释 
					this->pushReg( lexerSnoteB );
					break;
				case lexerIlabelB://类型定义起始符表示类型定义开始
					this->msToken.clear();
					this->mbSpace = false;
					this->pushReg( lexerSlabel );
					break;
				default://其他字符都应该视为非法的项目
					this->meError = lexerEillegalItem;
					this->removeItem();	//删除这个无效的节点 
					return false; 
			}
			break;
		case lexerSlabel://标签定义状态 可能出错 
			switch( this->regIdent() )
			{
				case lexerIspace:
					if( this->msToken.length() != 0 )
						this->mbSpace = true;
					break;
				case lexerIletterO:
				case lexerIletterR:
				case lexerInumber:
				case lexerIother:
					if( this->mbSpace )
					{
						this->meError = lexerEillegalLabel;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIlabelE:
					if( !this->correctLabel( this->msToken ) )
					{
						this->meError = lexerEillegalLabel;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					if( !this->uniqLabel( this->msToken ) )
					{
						this->meError = lexerErepeatLabel;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->setLabel( this->msToken );

					this->msToken.clear();
					this->mbSpace = false;
					this->mbPoint = false;
					this->popReg();//退出标签定义 
					switch( this->getType() )
					{
						case lexerTboolean:
							this->pushReg( lexerSbool );
							break;
						case lexerTbyte:
							this->pushReg( lexerSbyte );
							break;
						case lexerThex:
							this->pushReg( lexerShex );
							break;
						case lexerTnone:
							this->pushReg( lexerSnone );
							break;
						case lexerTnumber:
							this->pushReg( lexerSnumber );
							break;
						case lexerTstring:
							this->pushReg( lexerSstring );
							break;
					}
					break;
				default:
					this->meError = lexerEillegalLabel;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
		case lexerSbool://boolean状态  可能结束项目 可能出错 
			switch( this->regIdent() )
			{
				case lexerIspace:
					if( this->msToken.length() != 0 )
						this->mbSpace = true;
					break;
				case lexerIletterO:
				case lexerIletterR:
					if( this->mbSpace )
					{
						this->meError = lexerEillegalValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					if( this->msCode[this->miOffb] >= 'A' and this->msCode[this->miOffb] <= 'Z' )
							this->msCode[this->miOffb] += 32;
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() == 0 )
					{
						this->meError = lexerElostValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					try {
						this->setValue( this->string2bool( this->msToken ) );
						this->msToken.clear();
						this->mbSpace = false;
						this->miRegCount += 1;
						this->popReg();//退出值状态
						this->popReg();//退出item状态
						break; 
					} catch( invalid_argument e ) {
						this->meError = lexerEillegalValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
		case lexerSbyte://byte状态 可能结束项目 可能出错 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					if( this->msToken.length() == 0 )
						break;//跳过无害的空格 
					else
					{	//每遇到一个空格就添加一次就添加一次数据 
						unsigned char t = this->string2byte( this->msToken );//获取新的字节 
						int len = this->getLength();	
						char *temp = new char[len+1];	//创造新的缓冲区 
						((unsigned char*)temp)[len] = t;
						if( len > 0 ) 
						{	//如果有,拷贝原数据 
							memcpy( temp, this->getByte(), len );
							delete[] this->mpCurrent->mpValue;
						}
						//手动设置新的数据空间 
						this->mpCurrent->mpValue = temp;
						this->mpCurrent->miLength += 1;
						this->msToken.clear();
						break;
					}
				case lexerIletterR:
				case lexerInumber:
					//此处不用转换大小写,因为string2byte函数有兼容能力
					if( this->msToken.length() >= 2 )//保证每个字段只有最多两个字符 
					{	//错误的数据格式 
						this->meError = lexerEillegalValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() != 0 )
					{	//如果最后一个数据后面没有空格,不应该被漏掉 
						unsigned char t = this->string2byte( this->msToken );//获取新的字节 
						int len = this->getLength();	
						char *temp = new char[len+1];	//创造新的缓冲区 
						((unsigned char*)temp)[len] = t;
						if( len > 0 ) 
						{	//如果有,拷贝原数据 
							memcpy( temp, this->getByte(), len );
							delete[] this->mpCurrent->mpValue;
						}
						//手动设置新的数据空间 
						this->mpCurrent->mpValue = temp;
						this->mpCurrent->miLength += 1;
						this->msToken.clear();
					}
					this->miRegCount += 1;
					this->popReg();//退出值状态
					this->popReg();//退出item状态
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
		case lexerShex://hex状态 可能结束项目 可能出错 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					if( this->msToken.length() != 0 )
						this->mbSpace = true;
					break;
				case lexerIletterR:
				case lexerInumber:
					if( this->mbSpace or this->msToken.length() >= 8 )
					{
						this->meError = lexerEillegalValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() == 0 )
					{
						this->meError = lexerElostValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->setValue( this->string2hex( this->msToken ) );
					this->msToken.clear();
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//退出值状态
					this->popReg();//退出item状态
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
		case lexerSnone://none状态 可能结束项目 可能出错 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					break;
				case lexerIitemE:
					this->msToken.clear();
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//退出值状态
					this->popReg();//退出item状态
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
		case lexerSnumber://number状态 可能结束项目 可能出错 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					if( this->msToken.length() != 0 )
						this->mbSpace = true;
					break;
				case lexerInumber:
					if( this->mbSpace )
					{
						this->meError = lexerEillegalValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() == 0 )
					{
						this->meError = lexerElostValue;
						this->removeItem();	//删除这个无效的节点 
						return false;
					}
					this->setValue( this->string2number( this->msToken ) );
					this->msToken.clear();
					this->mbPoint = false;
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//退出值状态
					this->popReg();//退出item状态
					break;
				case lexerIother:
					if( '.' == this->regCode() and !this->mbPoint )
					{
						this->msToken += this->regCode();
						this->mbPoint = true;
						break;
					}
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
		case lexerSstring://string状态 可能开始转意 可能结束项目 可能出错 
			switch( this->regIdent() )
			{
				case lexerIend:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
				case lexerItrans:
					this->pushReg( lexerStrans );
					break;
				case lexerIitemE:
					this->setValue( this->msToken );
					this->msToken.clear();
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//退出值状态
					this->popReg();//退出item状态
					break;
				default:
					this->msToken += this->regCode();
					break;
					
			}
			break;
		case lexerStrans://转意状态 可能出现错误 
			switch( this->regCode() )
			{
				case '&':
					this->msToken += '&';
					this->popReg();//退出转意模式 
					break;
				case ';':
					this->msToken += ';';
					this->popReg();
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//删除这个无效的节点 
					return false;
			}
			break;
	}
	return true;
}

LexerRegStatus item_lexer::pushReg( LexerRegStatus status )	//入栈状态,返回入栈后的状态 
{
	StatusStack *st = new StatusStack;
	st->top = status;
	st->prev = this->msStack;
	this->msStack = st;
	return this->msStack->top;
}
LexerRegStatus item_lexer::popReg( void )		//出栈状态,返回出栈后的状态 
{
	
	if( 0 == this->msStack )
		return lexerSout;
	StatusStack *temp = this->msStack;
	this->msStack = this->msStack->prev;
	delete temp;
	if( 0 == this->msStack )
		return lexerSout;
	else
		return this->msStack->top;
}
LexerRegStatus item_lexer::getReg( int level )	//获取第n层的堆栈内容
{
	if( 0 == this->msStack )
		return lexerSout;
	StatusStack *temp;
	for( temp = this->msStack; level != 0 and temp->prev != 0; level-- )
		temp = temp->prev;
	return temp->top;
}
LexerRegStatus item_lexer::setReg( LexerRegStatus status )	//设置顶层栈状态 
{
	if( 0 == this->msStack )
		return this->pushReg( status );
	this->msStack->top = status;
	return status;
}
bool item_lexer::initReg( const string& code )	//初始化一个寄存状态 
{
	this->clearComp();
	if( code.length() == 0 )
	{
		this->meError = lexerEbadcode; 
		return false;
	}
	this->msCode = code;
	this->pushReg( lexerSglobal );
	return true;
}
char item_lexer::regNext( void )			//指向下一个字符并自动换行和偏移 
{
	if( this->regIdent() == lexerIend )
		return '\0';	//如果代码已经结束则直接返回'\0' 
	this->miOffb += 1;
	if( this->regIdent() == lexerInewline )
	{	//如果下一个字符是换行符则触发换行 
		this->miLine += 1;
		this->miOffl = 0;
	}
	else
		this->miOffl += 1;
	return this->regCode();
}
char item_lexer::regCode( void )			//返回当前指向的字符
{
	if( this->miOffb > this->msCode.length() )
		return '\0';	//如果指针超出范围则直接返回'\0' 
	return this->msCode[this->miOffb];
}
LexerCompRep	item_lexer::regReport( void )	//返回报告信息
{
	LexerCompRep rep;
	rep.error = this->meError;	//错误代码
	rep.offb = this->miOffb;	//代码偏移 
	rep.line = this->miLine;	//代码行号
	rep.offl = this->miOffl;	//代码行内偏移量 
	rep.count = this->miRegCount;//成功编译的节点计数 
	return rep;
}
lexerIdent item_lexer::regIdent( void )	//测试当前指向字符的身份 
{
	return this->identChar( this->regCode() );
}
lexerIdent	item_lexer::identChar( char c )
{
	switch( c )
	{
		case '\0':
			return lexerIend;
		case ' ':case '\t':
			return lexerIspace;
		case '\n': case '\r':
			return lexerInewline;
		case '[':
			return lexerIlabelB;
		case ']':
			return lexerIlabelE;
		case '(':
			return lexerItypeB;
		case ')':
			return lexerItypeE;
		case '>':
			return lexerInoteB;
		case '<':
			return lexerInoteE;
		case '$':
			return lexerInote;
		case '&':
			return lexerItrans;
		case ';':
			return lexerIitemE;
		case 'a' ... 'f':
		case 'A' ... 'F':
			return lexerIletterR;
		case 'g' ... 'z':
		case 'G' ... 'Z':
			return lexerIletterO;
		case '0' ... '9':
			return lexerInumber;
		default:
			return lexerIother;
		case '~':case '`':case '{':
		case '}':case '|':case ',':
		case '=':case '+':case '^':
		case '!':case '?':case '\"':
			return lexerIillegal; 
	}
}
double item_lexer::string2number( string str )
{
	double r = 0.0;
	bool bPoint;
	double bit = 10.0;
	for( int i = 0; str[i] != 0; i++ )
	{
		switch( str[i] )
		{
			case '0' ... '9':
				if( bPoint )
				{
					r += ((double)(str[i]-'0'))/ bit;
					bit *= 10;
					break;
				}
				r *= 10;
				r += (double)(str[i] - '0');
				break;
			case '.':
				if( bPoint )
					return r;
				bPoint = true;
		}
	}
	return r;
}
unsigned long long item_lexer::string2hex( string str )
{
	unsigned long long r = 0;
	for( int i = 0; str[i] != 0; i++ )
	{
		r <<= 4;
		switch( str[i] )
		{
			case '0' ... '9':
				r += str[i] - '0';
				break;
			case 'a' ... 'f':
				r += 10 + str[i] - 'a';
				break;
			case 'A' ... 'F':
				r += 10 + str[i] - 'A';
				break;
		}
	}
	return r;
}
unsigned char item_lexer::string2byte( string str )
{
	unsigned char r = 0;
	if( str.length() == 0 )
		return 0;
	if( str.length() == 1 )
		switch( str[0] )
		{
			case '0' ... '9':
				r = str[0] - '0';
				break;
			case 'a' ... 'f':
				r = 10 + str[0] - 'a';
				break;
			case 'A' ... 'F':
				r = 10 + str[0] - 'A';
				break;
		}
	else if( str.length() == 2 )
	{
		switch( str[0] )
		{
			case '0' ... '9':
				r = str[0] - '0';
				break;
			case 'a' ... 'f':
				r = 10 + str[0] - 'a';
				break;
			case 'A' ... 'F':
				r = 10 + str[0] - 'A';
				break;
		}
		r <<= 4;
		switch( str[1] )
		{
			case '0' ... '9':
				r += str[1] - '0';
				break;
			case 'a' ... 'f':
				r += 10 + str[1] - 'a';
				break;
			case 'A' ... 'F':
				r += 10 + str[1] - 'A';
				break;
		}
	}
	
	return r;
		
}
bool item_lexer::string2bool( string str )	//字符串转换成布尔类型失败抛出invalid_argument异常
{
	if( str == "false" )
		return false;
	else if( str == "true" )
		return true;
	else
		throw invalid_argument( "wrong string" );
}

lexerType item_lexer::string2type( string type )
{
	if( type == "none" )
		return lexerTnone;
	if( type == "boolean" )
		return lexerTboolean;
	if( type == "byte" )
		return lexerTbyte;
	if( type == "hex" )
		return lexerThex;
	if( type == "number" )
		return lexerTnumber;
	if( type == "string" )
		return lexerTstring;
	
	throw invalid_argument( "wrong type" );
}
string item_lexer::type2string( lexerType type )
{
	switch( type )
	{
		case lexerTboolean:
			return "boolean";
		case lexerTbyte:
			return "byte";
		case lexerThex:
			return "hex";
		case lexerTnone:
			return "none";
		case lexerTnumber:
			return "number";
		case lexerTstring:
			return "string";
	}
}
string item_lexer::item2string( item& tag )
{
	
	std::string s = "(" + type2string( tag.mtType ) + ")";
	s += "[" + tag.msLabel + "]";
		
	switch( tag.mtType )
	{
		case lexerTboolean:
			if( *(bool*)tag.mpValue )
				s += "true";
			else
				s += "false";
			break;
		case lexerTbyte:
		{
			char tempBuf[4];
			/***字节数据被转换成2位16进制数字***/ 
			for( int i = 0; i < tag.miLength; i++ )
			{
				sprintf( tempBuf, "%02X ", ((unsigned char*)tag.mpValue)[i] );
				s += tempBuf;
			}
			break;
		}
		case lexerThex:
		{
			/***hex数据被转换成8位十六进制数字***/ 
			char tempBuf[16];
			sprintf( tempBuf, "%x", *(unsigned long long*)tag.mpValue );
			s += tempBuf;
			break;
		}
		case lexerTnone:
			break;
		case lexerTnumber:
		{
			char tempBuf[16];
			sprintf( tempBuf, "%lf", *(double*)tag.mpValue );
			s += tempBuf;
			break;
		}
		case lexerTstring:
		{
			/***字符串内容要经过转意才能置入代码中***/ 
			for( int i = 0; tag.mpValue[i] != '\0'; i++ )
			{
				if( '&' == tag.mpValue[i]  or ';' == tag.mpValue[i] )
					s += "&";
				s += tag.mpValue[i];
			}
			break;
		}	
	}
	/***item结束符***/ 
	s += ";";
	return s;
}

bool item_lexer::uniqLabel( const string& label )//测试标签添加到链表中是否唯一
{
	item *tp;
	for( tp = this->mpList; tp != 0; tp = tp->next )
		if( tp->msLabel == label )
			return false;
	return true;
}
bool item_lexer::correctClass( string& cls )//测试类名是否正确 
{
	if( cls.length() == 0 )
		return false;
	bool bToken = false;
	bool bSpace = false;
	bool bClassed = false;
	string tmp_cls = "";
	for( int i = 0; cls[i] != '\0'; i++ )
	{
		switch( this->identChar( cls[i] ) )
		{
			case lexerIspace:
			case lexerInewline:
				if( bToken )
					bSpace = true;
				break;
			case lexerIletterO:
			case lexerIletterR:
			case lexerInumber:
			case lexerIother:
				if( bSpace or bClassed )
					return false;
				bToken = true;
				if( ':' == cls[i] )//遇到类名分界标志 
					if( bClassed )//如果重复出现类名则报告错误 
						return false;
					else if( !bToken )
						return false;	//如果作用域符号前没有字符则视为没有标记类域 
					else
						bClassed = true;
				tmp_cls += cls[i];//将字符添加到字串中 
				break;
			default:
				return false;
		}
	}
	cls = tmp_cls;
	return true;
}
bool item_lexer::correctLabel( string& label )//测试标签是否正确并补全类域 
{
	if( label.length() == 0 )
		return false;
	bool bToken = false;
	bool bSpace = false;
	bool bClassed = false;
	string tmp_label = "";
	int i;
	for( i = 0; label[i] != '\0'; i++ )
	{
		switch( this->identChar( label[i] ) )
		{
			case lexerIspace:
			case lexerInewline:
				if( bToken )
					bSpace = true;
				break;
			case lexerIletterO:
			case lexerIletterR:
			case lexerInumber:
			case lexerIother:
				if( bSpace )
					return false;
				bToken = true;
				if( ':' == label[i] )//遇到类名分界标志 
					if( bClassed )//如果重复出现类名则报告错误 
						return false;
					else if( !bToken )
						break;	//如果作用域符号前没有字符则视为没有标记类域 
					else
						bClassed = true;
				tmp_label += label[i];//将字符添加到字串中 
				break;
			default:
				return false;
		}
	}
	if( ':' == label[i-1] ) //类域之后没有内容则错误
		return false; 
	if( !bClassed )
		tmp_label = this->msClass + ":" + tmp_label;
	label = tmp_label;
	return true;
}
/***接下来的两个函数都没有自动查错功能,谨慎使用***/ 
string item_lexer::label2class( const string& label )//从标签中提取类域
{
	string s;
	for( int i = 0; ':' != label[i]; i++ )
		s += label[i];
	return s;
}
string item_lexer::label2name( const string& label )//从标签中提取名称 
{
	string s;
	int i;
	for( i = 0; ':' != label[i]; i++ );
	for( i += 1; '\0' != label[i]; i++ )
		s += label[i];
	return s;
}

item_lexer::item* item_lexer::classItem( item* ptr )	//从指定的节点和其后的节点中找出第一个在当前类域中的节点 
{
	for( ; ptr != 0; ptr = ptr->next )
		if( this->msClass == this->label2class( ptr->msLabel ) )
			return ptr;
	return ptr;
}

item_lexer::item* item_lexer::classPrev( item* ptr )	//从指定的节点和其前的节点中找出第一个在当前类域中的节点 
{
	for( ; ptr != 0; ptr = ptr->prev )
		if( this->msClass == this->label2class( ptr->msLabel ) )
			return ptr;
	return ptr;
}

item_lexer::item_lexer( )
{
	this->clearAll();
}
item_lexer::~item_lexer( )
{
	this->clearAll();
}

/***清空所有数据***/ 
void item_lexer::clearAll( void )
{
	this->findItem( 0 );
	while( this->removeItem() );
	this->clearComp();
	this->msClass = "default";
}
/***清空编译寄存器***/ 
void item_lexer::clearComp( void )
{
	while( this->popReg() != lexerSout );
	this->meError = lexerEsuccess;
	this->msCode.clear();		//源代码 
	this->msToken.clear();		//关键字 
	this->mbSpace = false;		//关键字是否已被空格隔开 
	this->miOffb = 0;			//代码偏移 
	this->miLine = 1;			//代码行号
	this->miOffl = 0;			//代码行内偏移量 
	this->miRegCount = 0;		//最新编译的节点计数
}
bool item_lexer::clearItem( void )
{
	if( 0 == this->mpCurrent )
		return false;
	if( this->mpCurrent->mtType != lexerTnone )
	{
		if( this->mpCurrent->mpValue != 0 )
			delete[] this->mpCurrent->mpValue;
		this->mpCurrent->mpValue = 0;
		this->mpCurrent->miLength = 0;
		this->mpCurrent->mtType = lexerTnone;
	}
	return true;
}
/***在末端添加新的item***/
string item_lexer::pushItem( void )
{
	char temp[32] = "label-0";
	string label = this->msClass + ":" + temp;
	int tempI = 0;
	item *one = new item;
	item *tempP;
	
	one->mtType = lexerTnone;
	one->mpValue = 0;
	one->miLength = 0;
	
	/***先产生一个不重复的默认标签***/
	while( !this->uniqLabel( label ) )
	{
		sprintf( temp, "label-%d", ++tempI );
		label = this->msClass + ":" + temp;
	}
	one->msLabel = label;

	/***寻找末端并添加元素***/
	for(tempP = this->mpCurrent; tempP != 0 and tempP->next != 0; tempP = tempP->next );
	
	/***如果指针为空则表明链表为空***/ 
	if( 0 == tempP )
	{
		this->mpList = one;
		this->mpCurrent = one;
		this->mpList->next = 0;
		this->mpList->prev = 0;
	}
	else
	{
		one->next = 0;
		one->prev = tempP;
		tempP->next = one;
	}
	
	return temp;
}
bool item_lexer::pushItem( string label )
{
	item *tempP;
	item *one;
	/***检查标签的有效性***/ 
	if( !this->correctLabel( label ) )
		return false;
	/***检查标签唯一性***/ 
	 if( !this->uniqLabel( label ) )
	 	return false;
	
	/***填充item***/ 
	one = new item;
	one->msLabel = label;
	one->mtType = lexerTnone;
	one->mpValue = 0;
	one->miLength = 0;
	
	/***寻找链尾***/ 
	for(tempP = this->mpCurrent; tempP != 0 and tempP->next != 0; tempP = tempP->next );
	
	/***如果指针为空则表明链表为空***/ 
	if( 0 == tempP )
	{
		this->mpList = one;
		this->mpCurrent = one;
		this->mpList->next = 0;
		this->mpList->prev = 0;
	}
	else
	{
		one->next = 0;
		one->prev = tempP;
		tempP->next = one;
	}
	return true;
}
bool item_lexer::pushItem( string label, bool value )
{
	int temp_pos;
	/***在末尾添加新item***/
	if( !this->pushItem( label ) )
		return false;
	/***保存当前位置***/
	temp_pos = this->getPos();
	/***指向末尾***/
	this->findItem( lexerFlast );
	/***设置类型***/
	this->setType( lexerTboolean );
	/***设置数据***/
	if( !this->setValue( value ) )
		return false;
	/***将指针重置***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, unsigned char* buf, int length )
{
	int temp_pos;
	/***在末尾添加新item***/
	if( !this->pushItem( label ) )
		return false;
	/***保存当前位置***/
	temp_pos = this->getPos();
	/***指向末尾***/
	this->findItem( lexerFlast );
	/***设置类型***/
	this->setType( lexerTbyte );
	/***设置数据***/
	if( !this->setValue( buf, length ) )
		return false;
	/***将指针重置***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, unsigned long long value )
{
	int temp_pos;
	/***在末尾添加新item***/
	if( !this->pushItem( label ) )
		return false;
	/***保存当前位置***/
	temp_pos = this->getPos();
	/***指向末尾***/
	this->findItem( lexerFlast );
	/***设置类型***/
	this->setType( lexerThex );
	/***设置数据***/
	if( !this->setValue( value ) )
		return false;
	/***将指针重置***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, double value )
{
	int temp_pos;
	/***在末尾添加新item***/
	if( !this->pushItem( label ) )
		return false;
	/***保存当前位置***/
	temp_pos = this->getPos();
	/***指向末尾***/
	this->findItem( lexerFlast );
	/***设置类型***/
	this->setType( lexerTnumber );
	/***设置数据***/
	if( !this->setValue( value ) )
		return false;
	/***将指针重置***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, string value )
{
	int temp_pos;
	/***在末尾添加新item***/
	if( !this->pushItem( label ) )
		return false;
	/***保存当前位置***/
	temp_pos = this->getPos();
	/***指向末尾***/
	this->findItem( lexerFlast );
	/***设置类型***/
	this->setType( lexerTstring );
	/***设置数据***/
	if( !this->setValue( value ) )
		return false;
	/***将指针重置***/
	this->findItem( temp_pos );
	
	return true;
}
/***在当前位置添加新的item,若成功则指向这个item***/ 
string item_lexer::insertItem( void )
{
	char temp[32] = "label-0";
	string label = this->msClass + ":" + temp;
	int tempI = 0;
	item *one = new item;
	
	one->mpValue = 0;
	one->miLength = 0;
	one->mtType = lexerTnone;
	
	/***先产生一个不重复的默认标签***/
	while( !this->uniqLabel( label ) )
	{
		sprintf( temp, "label-%d", ++tempI );
		label = this->msClass + ":" + temp;
	}
	one->msLabel = label;
	
	/***如果指针为空则表明链表为空***/ 
	if( 0 == this->mpCurrent )
	{
		this->mpList = one;
		this->mpCurrent = one;
		this->mpList->next = 0;
		this->mpList->prev = 0;
	}
	else
	{
		one->next = this->mpCurrent;
		one->prev = this->mpCurrent->prev;
		if( this->mpCurrent->prev != 0 )
			this->mpCurrent->prev->next = one;
		else
			this->mpList = one;
		this->mpCurrent->prev = one;
		
		this->mpCurrent = one;
	}
	
	return temp;
}
bool item_lexer::insertItem( string label )
{
	item *one = new item;
	
	/***检查标签的有效性***/ 
	if( !this->correctLabel( label ) )
		return false;
	/***先检查标签唯一性***/
	if( !this->uniqLabel( label ) )
		return false;
	
	one->mpValue = 0;
	one->miLength = 0;
	one->mtType = lexerTnone;
	
	/***如果指针为空则表明链表为空***/ 
	if( 0 == this->mpCurrent )
	{
		this->mpList = one;
		this->mpCurrent = one;
		this->mpList->next = 0;
		this->mpList->prev = 0;
	}
	else
	{
		one->next = this->mpCurrent;
		one->prev = this->mpCurrent->prev;
		if( this->mpCurrent->prev != 0 )
			this->mpCurrent->prev->next = one;
		else
			this->mpList = one;
		this->mpCurrent->prev = one;
		
		this->mpCurrent = one;
	}
	
	return true;
}
bool item_lexer::insertItem( string label, bool value )
{
	if( !this->insertItem( label ) )
		return false;
	this->setType( lexerTboolean );
	return this->setValue( value );
}
bool item_lexer::insertItem( string label, unsigned char* value, int length )
{
	if( !this->insertItem( label ) )
		return false;
	this->setType( lexerTbyte );
	return this->setValue( value, length );
}
bool item_lexer::insertItem( string label, unsigned long long value )
{
	if( !this->insertItem( label ) )
		return false;
	this->setType( lexerThex );
	return this->setValue( value );
}
bool item_lexer::insertItem( string label, double value )
{
	if( !this->insertItem( label ) )
		return false;
	this->setType( lexerTnumber );
	return this->setValue( value );
}
bool item_lexer::insertItem( string label, string value )
{
	if( !this->insertItem( label ) )
		return false;
	this->setType( lexerTstring );
	return this->setValue( value );
}

/***删除元素***/
bool item_lexer::removeItem( void )	//删除当前指向的元素 
{
	if( 0 == this->mpCurrent )
		return false;
	item* target = this->mpCurrent;
	
	this->clearItem();
	
	if( this->mpCurrent->prev != 0 )
		this->mpCurrent->prev->next = this->mpCurrent->next;
	else
		this->mpList = this->mpCurrent->next;
	if( this->mpCurrent->next != 0 )
	{
		this->mpCurrent->next->prev = this->mpCurrent->prev;
		this->mpCurrent = this->mpCurrent->next;
	}
	else
		this->mpCurrent = this->mpCurrent->prev;

	delete target;
	return true;
}
bool item_lexer::removeItem( string label )	//删除由标签指定的元素
{
	int temp_pos = this->getPos();	//保存当前位置 
 	/***若没有元素则失败***/ 
	if( temp_pos == -1 )
		return false;
	/***寻找目标,若没有则失败***/ 
	if( !this->findItem( label ) )
		return false;
	/***删除目标***/ 
	this->removeItem();
	/***恢复原来的定位***/ 
	if( !this->findItem( temp_pos ) )
		this->findItem( temp_pos );
	return true;
}
/***设置标签***/
bool item_lexer::setLabel( string label )
{
	/***没有元素则失败***/ 
	if( 0 == this->mpCurrent )
		return false;
	/***检查标签的有效性***/ 
	if( !this->correctLabel( label ) )
		return false;
	/***检查标签唯一性***/
	if( !this->uniqLabel( label ) or label.length() <= 0 )
		return false;
	/***设置标签***/
	this->mpCurrent->msLabel = label;
	return true; 
}
/***设置类域***/ 
bool item_lexer::setClass( string cls )
{
	/***没有元素则失败***/ 
	if( 0 == this->mpCurrent )
		return false;
	/***类域错误则设置失败***/
	if( !this->correctClass( cls ) )
		return false; 
	this->mpCurrent->msLabel = cls + ":" + this->label2name( this->mpCurrent->msLabel );
	return true;
}
/***设置当前类域***/
bool item_lexer::inClass( string cls )
{
	/***类域错误则设置失败***/
	if( !this->correctClass( cls ) )
		return false; 
	this->msClass = cls;
	return true;
}
/***设置类型***/
bool item_lexer::setType( lexerType type )
{
	/***没有元素则失败***/ 
	if( 0 == this->mpCurrent )
		return false;
	/***若元素类型与要求类型相同则直接成功***/ 
	if( type == this->mpCurrent->mtType )
		return true; 
	/***若不同则先清空元素***/ 
	this->clearItem();
	
	this->mpCurrent->mtType = type;
	switch( type )
	{
		case lexerTboolean:
			this->mpCurrent->miLength = sizeof( bool );
			this->mpCurrent->mpValue = new char[sizeof( bool )];
			*(bool*)this->mpCurrent->mpValue = true;
			break;
		case lexerTbyte:
		case lexerTnone:
			break;	//byte和none是两个允许数据长度等于0的类型 
		case lexerThex:
			this->mpCurrent->miLength = sizeof(unsigned long long);
			this->mpCurrent->mpValue = new char[sizeof( unsigned long long )];
			*(unsigned long long*)this->mpCurrent->mpValue = 0;
			break;
		case lexerTnumber:
			this->mpCurrent->miLength = sizeof(double);
			this->mpCurrent->mpValue = new char[sizeof( double )];
			*(double*)this->mpCurrent->mpValue = 0.0;
			break;
		case lexerTstring:
			this->mpCurrent->mpValue = new char[1];
			this->mpCurrent->miLength = 1;
			*(char*)this->mpCurrent->mpValue = '\0';
			break;
	}
}
/***设置内容,类型不符会失败***/
bool item_lexer::setValue( bool value )
{
	if( 0 == this->mpCurrent )
		return false;
	if( lexerTboolean != this->mpCurrent->mtType )
		return false;
	*(bool*)this->mpCurrent->mpValue = value;
	return true;
}
bool item_lexer::setValue( unsigned char* buf, int length )
{
	if( 0 == this->mpCurrent )
		return false;
	if( lexerTbyte != this->mpCurrent->mtType )
		return false;
	//理应先清空原有空间 
	if( this->mpCurrent->mpValue != 0 )
		delete[] this->mpCurrent->mpValue;
	
	if( 0 == length )
		this->mpCurrent->mpValue = 0;
	else
	{
		this->mpCurrent->mpValue = new char[length];
		memcpy( this->mpCurrent->mpValue, buf, length );
	}
	
	this->mpCurrent->miLength = length;
	return true;
}
bool item_lexer::setValue( unsigned long long value )
{
	if( 0 == this->mpCurrent )
		return false;
	if( lexerThex != this->mpCurrent->mtType )
		return false;
	*(unsigned long long*)this->mpCurrent->mpValue = value;
	return true;
}
bool item_lexer::setValue( double value )
{
	if( 0 == this->mpCurrent )
		return false;
	if( lexerTnumber != this->mpCurrent->mtType )
		return false;
	*(double*)this->mpCurrent->mpValue = value;
	return true;
}
bool item_lexer::setValue( string value )
{
	/***string类数据在内存中保存'\0'结束符,但是在文档中以;结束***/
	if( 0 == this->mpCurrent )
		return false;
	if( lexerTstring != this->mpCurrent->mtType )
		return false;
	//理应先清空原有空间 
	delete[] this->mpCurrent->mpValue;
	
	this->mpCurrent->mpValue = new char[value.length() + 1];
	memcpy( this->mpCurrent->mpValue, value.data(), value.length() );
	this->mpCurrent->mpValue[value.length()] = '\0';
	
	this->mpCurrent->miLength = value.length()+1;
	return true;
}
/***获取内容,类型不符则抛出异常***/
bool item_lexer::getBool( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	if( lexerTboolean != this->mpCurrent->mtType )
		throw invalid_argument( "wrong item type" );
	return *(bool*)this->mpCurrent->mpValue;
}
unsigned char* item_lexer::getByte( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	if( lexerTbyte != this->mpCurrent->mtType )
		throw invalid_argument( "wrong item type" );
	return (unsigned char*)this->mpCurrent->mpValue;
}
unsigned long long item_lexer::getHex( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	if( lexerThex != this->mpCurrent->mtType )
		throw invalid_argument( "wrong item type" );
	return *(unsigned long long*)this->mpCurrent->mpValue;
}
double item_lexer::getNumber( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	if( lexerTnumber != this->mpCurrent->mtType )
		throw invalid_argument( "wrong item type" );
	return *(double*)this->mpCurrent->mpValue;
}
string item_lexer::getString( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	if( lexerTstring != this->mpCurrent->mtType )
		throw invalid_argument( "wrong item type" );
	return this->mpCurrent->mpValue;
}
/***获取类型,失败抛出异常***/
lexerType item_lexer::getType( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->mpCurrent->mtType;
}
/***获取数据长度,失败返回-1***/
int item_lexer::getLength( void )
{
	if( 0 == this->mpCurrent )
		return -1;
	return this->mpCurrent->miLength;
}
/***获取标签,失败抛出异常***/
string item_lexer::getLabel( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->mpCurrent->msLabel;
} 
/***获取类域,失败抛出异常***/
string item_lexer::getClass( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->label2class( mpCurrent->msLabel );
}
/***获取item位置***/
int item_lexer::getPos( void )
{
	if( 0 == this->mpCurrent )
		return -1;
	int i = 0;
	for( item* temp = this->mpList; temp != this->mpCurrent; temp = temp->next )
		i += 1;
	return i;
}
/***寻找链中的元素,并指向***/
bool item_lexer::findItem( string label )		//按照标签检索 
{
	if( !this->correctLabel( label ) )
		return false;
	for( item* temp = this->mpList; temp != 0; temp = temp->next )
		if( temp->msLabel == label )
		{
			this->mpCurrent = temp;
			return true;
		}
	return false;
}
bool item_lexer::findItem( lexerFind fd )		//检索特殊位置上的元素 
{
	if( 0 == this->mpCurrent )
		return false;
	
	switch( fd )
	{
		case lexerFfirst:
			this->mpCurrent = this->mpList;
			return true;
		case lexerFlast:
			for( ; this->mpCurrent->next != 0; this->mpCurrent = this->mpCurrent->next );
			return true;
		case lexerFnext:
			this->mpCurrent = this->mpCurrent->next;
			return true;
		case lexerFprev:
			this->mpCurrent = this->mpCurrent->prev;
			return true;
	}
}
bool item_lexer::findItem( lexerFind fd, lexerType type )//按照类型检索 
{
	if( 0 == this->mpCurrent )
	return false;
	item* temp;
	switch( fd )
	{
		case lexerFfirst:
			for( temp = this->mpList; temp != 0; temp = temp->next )
				if( temp->mtType == type )
				{
					this->mpCurrent = temp;
					return true;
				}
			return false;
		case lexerFlast:
			for( temp = this->mpList; temp->next != 0; temp = temp->next );
			for( ; temp != 0; temp = temp->prev )
				if( temp->mtType == type )
				{
					this->mpCurrent = temp;
					return true;
				}
			return false;
		case lexerFnext:
			for( temp = this->mpCurrent; temp != 0; temp = temp->next )
				if( temp->mtType == type )
				{
					this->mpCurrent = temp;
					return true;
				}
			return false;
		case lexerFprev:
			for( temp = this->mpCurrent; temp != 0; temp = temp->prev )
				if( temp->mtType == type )
				{
					this->mpCurrent = temp;
					return true;
				}
			return false;
	}
}
bool item_lexer::findItem( int pos )//按照绝对位置检索 
{
	if( pos < 0 )
		return false;
	if( 0 == this->mpList )
		return false;
	for( item* temp = this->mpList; temp != 0; temp = temp->next )
		if( pos-- == 0 )
		{
			this->mpCurrent = temp;
			return true;
		}
	return false;
}
/***寻找当前域中的元素,并指向***/ 
bool item_lexer::classItem( lexerFind fd )	//检索特殊位置上的元素 
{
	item* temp;
	if( 0 == this->mpCurrent )
		return false;
	switch( fd )
	{
		case lexerFfirst:
			temp = this->classItem( this->mpList );
			break;
		case lexerFnext:
			temp = this->classItem( this->mpCurrent->next );
			break;
		case lexerFprev:
			temp = this->classPrev( this->mpCurrent->prev );
			break;
		case lexerFlast:
			for( temp = this->classItem( this->mpList ); temp != 0 and this->classItem( temp->next ) != 0; temp = this->classItem( temp ) )
				temp = temp->next;
			break;
	}		
	if( temp != 0 )
	{
		this->mpCurrent = temp;
		return true;
	}
	return false;
}
bool item_lexer::classItem( lexerFind fd, lexerType type )//按照类型检索 
{
	item* temp;
	if( 0 == this->mpCurrent )
		return false;
	switch( fd )
	{
		case lexerFfirst:
			for( temp = this->classItem( this->mpList ); temp != 0; temp = this->classItem( temp->next ) )
				if( temp->mtType == type )
					break;
			break;
		case lexerFnext:
			for( temp = this->classItem( this->mpCurrent->next ); temp != 0; temp = this->classItem( temp->next ) )
				if( temp->mtType == type )
					break;
			break;
		case lexerFprev:
			for( temp = this->classPrev( this->mpCurrent->prev ); temp != 0; temp = this->classPrev( temp->prev ) )
				if( temp->mtType == type )
					break;
			break;
		case lexerFlast:
			for( temp = this->classItem( this->mpList ); temp != 0 ; temp = this->classItem( temp->next ) )
				if( temp->mtType == type and this->classItem( temp->next ) == 0 )
					break;
			break;
	}		
	if( temp != 0 )
	{
		this->mpCurrent = temp;
		return true;
	}
	return false;
}
bool item_lexer::classItem( int pos )//按照绝对位置检索 
{
	if( pos < 0 )
		return false;
	if( 0 == this->mpCurrent )
		return false;
	item* temp;
	for( temp = this->classItem( this->mpList ); temp != 0 and pos != 0 ; temp = this->classItem( temp->next ) )
		pos -= 1;
	if( temp != 0 )
	{
		this->mpCurrent = temp;
		return true;
	}
	return false;
}
/***统计item总数***/
int item_lexer::countItem( void )
{
	int i = 0;
	for( item *temp = this->mpList; temp != 0; temp = temp->next )
		i += 1;
	return i;
}
/***将数据编码成item code***/
string item_lexer::generateCode( void )
{
	time_t cusec = time( 0 );
	struct tm* cutime = localtime( &cusec );
	char tempBf[8];
	sprintf( tempBf, "%d", this->countItem() );
	string s = "$>语法标准 item code v1.02\n**生成于 "+ string(asctime( cutime )) + "**包含了 " + tempBf + " 个数据单位\n<$\n";
	for( item* pTemp = this->mpList; 0 not_eq pTemp; pTemp = pTemp->next )
		s += this->item2string( *pTemp ) + '\n';
	return s;
}
/***将当前item编码成字符串,失败抛出异常***/ 
string item_lexer::encodeItem( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->item2string( *this->mpCurrent );
}
/***将item code编译成数据加入到数据库内***/
LexerCompRep item_lexer::compilerCode( string code )
{
	if( !this->initReg( code ) )
		return this->regReport( );
	while( this->regIdent() != lexerIend )
	{
		if( !this->regProc() )
			break;
		this->regNext();
	}
	return this->regReport( );
}
}

#endif
