#ifndef __DITEM_CPP__
#define __DITEM_CPP__
#include "item_lexer.h"
#include <exception>
#include <stdexcept>
#include <time.h>

namespace dream {
bool item_lexer::regProc( void ) 			//����ǰָ���ַ�,������ݻ򱨸���� 
{
	switch( this->getReg() )
	{
		case lexerSglobal://ȫ��״̬ ���ܻ���ע�� ��ʼ���� ����ִ��� 
			switch( this->regIdent() )
			{
				case lexerIend://��ȫ��״̬��������������ζ�Ŵ�����������
					break; 
				case lexerIspace://�����޺��Ŀո� 
				case lexerInewline:
					break;
				case lexerInote://����ע�ͷ���ʼ����ע�� 
					this->pushReg( lexerSnoteB );
					break;
				case lexerItypeB://���Ͷ�����ʼ����ʾ���Ͷ��忪ʼ
					this->msToken.clear();
					this->mbSpace = false;
					this->pushReg( lexerStype );
					break;
				default://�����ַ���Ӧ����Ϊ�Ƿ�����Ŀ
					this->meError = lexerEillegalItem;
					return false; 
			}
			break;
		case lexerSnoteB://ע�ͻ���״̬ ���ܿ�ʼע�� ���ܳ��ִ��� 
			if( this->regIdent() == lexerInoteB ) //���������������ע����ʼ��
			{
				if( this->getReg( 1 ) == lexerSnote )//���˵����ע��״̬��������ϵ�ע����ʼ��Ӧ������
					break;
				else //�����κ�����ֻҪ����ע�ͻ���״̬��˵������ע�� 
				{
					this->popReg();//�˳�ע�ͻ���׶� 
					this->pushReg( lexerSnote );//�������ע��״̬
					break; 
				}
			}
			else if( this->regIdent() == lexerInote )//��������ϵ�ע�ͽ�����
			{
				if( this->getReg( 1 ) == lexerSnote )//���˵��������״̬��������ϵ�ע�ͽ�����Ӧ������ע�� 
				{
					this->popReg();//�˳�ע�ͻ���׶�
					this->popReg();//�˳�ע�ͽ׶� 
					break;
				}
				else  //���˵�����κ�����������������ϵ�ע�ͽ�����Ӧ������ 
				{
					this->meError = lexerEillegalNote;
					return false;
				}
			}
			else//δ�ܳɹ�����ע�ͷ��� 
			{
				if( this->getReg( 1 ) == lexerSnote )//��ע��״̬��Ӧ������ 
					break;
				else	//����״̬��Ӧ������ 
				{
					this->meError = lexerEillegalNote;
					return false; 
				}
			}
			break;	
		case lexerSnote://ע��״̬ ���ܻ���ע�� ���ܳ��ִ��� 
			if( this->regIdent() == lexerIend )
			{
				this->meError = lexerEbadcode;
				return false;
			}
			else if( this->regIdent() == lexerInoteE )//����ע�ͽ����� 
			{
				this->pushReg( lexerSnoteB );//����ע�ͻ���״̬ 
				break;
			}
			else
				break; 
		case lexerStype://���Ͷ���״̬ ���ܳ��ִ��� 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					if( this->msToken.length() == 0 )
						this->mbSpace = true;
					break;
				case lexerIletterO:
				case lexerIletterR://type����Ӧ�����ִ�Сд 
						if( this->msCode[this->miOffb] >= 'A' and this->msCode[this->miOffb] <= 'Z' )
							this->msCode[this->miOffb] += 32;
				case lexerInumber:
				case lexerIother:
					if( this->mbSpace )
					{//���Ͷ��岻Ӧ�ñ��ո���� 
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
						//���˳��,�򴴽��˽ڵ��趨������ 
						this->pushItem();
						this->findItem( lexerFlast );
						this->setType( this->string2type( this->msToken ) );
						//���˳��,������token������ݲ�����item 
						this->msToken.clear();
						this->mbSpace = false;
						this->popReg();//�˳����Ͷ�����
						this->pushReg( lexerSitem );//����itemģʽ 
						break;
					}catch( invalid_argument e ) {
						this->meError = lexerEillegalType;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
				default:
					this->meError = lexerEillegalType;
					return false;
			}
			break;
		case lexerSitem://��Ŀ����״̬ ���ܻ���ע�� ���ܿ�ʼ��ǩ ���ܳ��ִ��� 
			switch( this->regIdent() )
			{
				case lexerIspace://�����޺��Ŀո� 
					break;
				case lexerInote://����ע�ͷ���ʼ����ע�� 
					this->pushReg( lexerSnoteB );
					break;
				case lexerIlabelB://���Ͷ�����ʼ����ʾ���Ͷ��忪ʼ
					this->msToken.clear();
					this->mbSpace = false;
					this->pushReg( lexerSlabel );
					break;
				default://�����ַ���Ӧ����Ϊ�Ƿ�����Ŀ
					this->meError = lexerEillegalItem;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false; 
			}
			break;
		case lexerSlabel://��ǩ����״̬ ���ܳ��� 
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
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIlabelE:
					if( !this->correctLabel( this->msToken ) )
					{
						this->meError = lexerEillegalLabel;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					if( !this->uniqLabel( this->msToken ) )
					{
						this->meError = lexerErepeatLabel;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->setLabel( this->msToken );

					this->msToken.clear();
					this->mbSpace = false;
					this->mbPoint = false;
					this->popReg();//�˳���ǩ���� 
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
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
		case lexerSbool://boolean״̬  ���ܽ�����Ŀ ���ܳ��� 
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
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
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
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					try {
						this->setValue( this->string2bool( this->msToken ) );
						this->msToken.clear();
						this->mbSpace = false;
						this->miRegCount += 1;
						this->popReg();//�˳�ֵ״̬
						this->popReg();//�˳�item״̬
						break; 
					} catch( invalid_argument e ) {
						this->meError = lexerEillegalValue;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
		case lexerSbyte://byte״̬ ���ܽ�����Ŀ ���ܳ��� 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					if( this->msToken.length() == 0 )
						break;//�����޺��Ŀո� 
					else
					{	//ÿ����һ���ո�����һ�ξ����һ������ 
						unsigned char t = this->string2byte( this->msToken );//��ȡ�µ��ֽ� 
						int len = this->getLength();	
						char *temp = new char[len+1];	//�����µĻ����� 
						((unsigned char*)temp)[len] = t;
						if( len > 0 ) 
						{	//�����,����ԭ���� 
							memcpy( temp, this->getByte(), len );
							delete[] this->mpCurrent->mpValue;
						}
						//�ֶ������µ����ݿռ� 
						this->mpCurrent->mpValue = temp;
						this->mpCurrent->miLength += 1;
						this->msToken.clear();
						break;
					}
				case lexerIletterR:
				case lexerInumber:
					//�˴�����ת����Сд,��Ϊstring2byte�����м�������
					if( this->msToken.length() >= 2 )//��֤ÿ���ֶ�ֻ����������ַ� 
					{	//��������ݸ�ʽ 
						this->meError = lexerEillegalValue;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() != 0 )
					{	//������һ�����ݺ���û�пո�,��Ӧ�ñ�©�� 
						unsigned char t = this->string2byte( this->msToken );//��ȡ�µ��ֽ� 
						int len = this->getLength();	
						char *temp = new char[len+1];	//�����µĻ����� 
						((unsigned char*)temp)[len] = t;
						if( len > 0 ) 
						{	//�����,����ԭ���� 
							memcpy( temp, this->getByte(), len );
							delete[] this->mpCurrent->mpValue;
						}
						//�ֶ������µ����ݿռ� 
						this->mpCurrent->mpValue = temp;
						this->mpCurrent->miLength += 1;
						this->msToken.clear();
					}
					this->miRegCount += 1;
					this->popReg();//�˳�ֵ״̬
					this->popReg();//�˳�item״̬
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
		case lexerShex://hex״̬ ���ܽ�����Ŀ ���ܳ��� 
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
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() == 0 )
					{
						this->meError = lexerElostValue;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->setValue( this->string2hex( this->msToken ) );
					this->msToken.clear();
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//�˳�ֵ״̬
					this->popReg();//�˳�item״̬
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
		case lexerSnone://none״̬ ���ܽ�����Ŀ ���ܳ��� 
			switch( this->regIdent() )
			{
				case lexerIspace:
				case lexerInewline:
					break;
				case lexerIitemE:
					this->msToken.clear();
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//�˳�ֵ״̬
					this->popReg();//�˳�item״̬
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
		case lexerSnumber://number״̬ ���ܽ�����Ŀ ���ܳ��� 
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
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->msToken += this->regCode();
					break;
				case lexerIitemE:
					if( this->msToken.length() == 0 )
					{
						this->meError = lexerElostValue;
						this->removeItem();	//ɾ�������Ч�Ľڵ� 
						return false;
					}
					this->setValue( this->string2number( this->msToken ) );
					this->msToken.clear();
					this->mbPoint = false;
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//�˳�ֵ״̬
					this->popReg();//�˳�item״̬
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
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
		case lexerSstring://string״̬ ���ܿ�ʼת�� ���ܽ�����Ŀ ���ܳ��� 
			switch( this->regIdent() )
			{
				case lexerIend:
					this->meError = lexerEillegalValue;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
				case lexerItrans:
					this->pushReg( lexerStrans );
					break;
				case lexerIitemE:
					this->setValue( this->msToken );
					this->msToken.clear();
					this->mbSpace = false;
					this->miRegCount += 1;
					this->popReg();//�˳�ֵ״̬
					this->popReg();//�˳�item״̬
					break;
				default:
					this->msToken += this->regCode();
					break;
					
			}
			break;
		case lexerStrans://ת��״̬ ���ܳ��ִ��� 
			switch( this->regCode() )
			{
				case '&':
					this->msToken += '&';
					this->popReg();//�˳�ת��ģʽ 
					break;
				case ';':
					this->msToken += ';';
					this->popReg();
					break;
				default:
					this->meError = lexerEillegalValue;
					this->removeItem();	//ɾ�������Ч�Ľڵ� 
					return false;
			}
			break;
	}
	return true;
}

LexerRegStatus item_lexer::pushReg( LexerRegStatus status )	//��ջ״̬,������ջ���״̬ 
{
	StatusStack *st = new StatusStack;
	st->top = status;
	st->prev = this->msStack;
	this->msStack = st;
	return this->msStack->top;
}
LexerRegStatus item_lexer::popReg( void )		//��ջ״̬,���س�ջ���״̬ 
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
LexerRegStatus item_lexer::getReg( int level )	//��ȡ��n��Ķ�ջ����
{
	if( 0 == this->msStack )
		return lexerSout;
	StatusStack *temp;
	for( temp = this->msStack; level != 0 and temp->prev != 0; level-- )
		temp = temp->prev;
	return temp->top;
}
LexerRegStatus item_lexer::setReg( LexerRegStatus status )	//���ö���ջ״̬ 
{
	if( 0 == this->msStack )
		return this->pushReg( status );
	this->msStack->top = status;
	return status;
}
bool item_lexer::initReg( const string& code )	//��ʼ��һ���Ĵ�״̬ 
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
char item_lexer::regNext( void )			//ָ����һ���ַ����Զ����к�ƫ�� 
{
	if( this->regIdent() == lexerIend )
		return '\0';	//��������Ѿ�������ֱ�ӷ���'\0' 
	this->miOffb += 1;
	if( this->regIdent() == lexerInewline )
	{	//�����һ���ַ��ǻ��з��򴥷����� 
		this->miLine += 1;
		this->miOffl = 0;
	}
	else
		this->miOffl += 1;
	return this->regCode();
}
char item_lexer::regCode( void )			//���ص�ǰָ����ַ�
{
	if( this->miOffb > this->msCode.length() )
		return '\0';	//���ָ�볬����Χ��ֱ�ӷ���'\0' 
	return this->msCode[this->miOffb];
}
LexerCompRep	item_lexer::regReport( void )	//���ر�����Ϣ
{
	LexerCompRep rep;
	rep.error = this->meError;	//�������
	rep.offb = this->miOffb;	//����ƫ�� 
	rep.line = this->miLine;	//�����к�
	rep.offl = this->miOffl;	//��������ƫ���� 
	rep.count = this->miRegCount;//�ɹ�����Ľڵ���� 
	return rep;
}
lexerIdent item_lexer::regIdent( void )	//���Ե�ǰָ���ַ������ 
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
bool item_lexer::string2bool( string str )	//�ַ���ת���ɲ�������ʧ���׳�invalid_argument�쳣
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
			/***�ֽ����ݱ�ת����2λ16��������***/ 
			for( int i = 0; i < tag.miLength; i++ )
			{
				sprintf( tempBuf, "%02X ", ((unsigned char*)tag.mpValue)[i] );
				s += tempBuf;
			}
			break;
		}
		case lexerThex:
		{
			/***hex���ݱ�ת����8λʮ����������***/ 
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
			/***�ַ�������Ҫ����ת��������������***/ 
			for( int i = 0; tag.mpValue[i] != '\0'; i++ )
			{
				if( '&' == tag.mpValue[i]  or ';' == tag.mpValue[i] )
					s += "&";
				s += tag.mpValue[i];
			}
			break;
		}	
	}
	/***item������***/ 
	s += ";";
	return s;
}

bool item_lexer::uniqLabel( const string& label )//���Ա�ǩ��ӵ��������Ƿ�Ψһ
{
	item *tp;
	for( tp = this->mpList; tp != 0; tp = tp->next )
		if( tp->msLabel == label )
			return false;
	return true;
}
bool item_lexer::correctClass( string& cls )//���������Ƿ���ȷ 
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
				if( ':' == cls[i] )//���������ֽ��־ 
					if( bClassed )//����ظ����������򱨸���� 
						return false;
					else if( !bToken )
						return false;	//������������ǰû���ַ�����Ϊû�б������ 
					else
						bClassed = true;
				tmp_cls += cls[i];//���ַ���ӵ��ִ��� 
				break;
			default:
				return false;
		}
	}
	cls = tmp_cls;
	return true;
}
bool item_lexer::correctLabel( string& label )//���Ա�ǩ�Ƿ���ȷ����ȫ���� 
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
				if( ':' == label[i] )//���������ֽ��־ 
					if( bClassed )//����ظ����������򱨸���� 
						return false;
					else if( !bToken )
						break;	//������������ǰû���ַ�����Ϊû�б������ 
					else
						bClassed = true;
				tmp_label += label[i];//���ַ���ӵ��ִ��� 
				break;
			default:
				return false;
		}
	}
	if( ':' == label[i-1] ) //����֮��û�����������
		return false; 
	if( !bClassed )
		tmp_label = this->msClass + ":" + tmp_label;
	label = tmp_label;
	return true;
}
/***������������������û���Զ������,����ʹ��***/ 
string item_lexer::label2class( const string& label )//�ӱ�ǩ����ȡ����
{
	string s;
	for( int i = 0; ':' != label[i]; i++ )
		s += label[i];
	return s;
}
string item_lexer::label2name( const string& label )//�ӱ�ǩ����ȡ���� 
{
	string s;
	int i;
	for( i = 0; ':' != label[i]; i++ );
	for( i += 1; '\0' != label[i]; i++ )
		s += label[i];
	return s;
}

item_lexer::item* item_lexer::classItem( item* ptr )	//��ָ���Ľڵ�����Ľڵ����ҳ���һ���ڵ�ǰ�����еĽڵ� 
{
	for( ; ptr != 0; ptr = ptr->next )
		if( this->msClass == this->label2class( ptr->msLabel ) )
			return ptr;
	return ptr;
}

item_lexer::item* item_lexer::classPrev( item* ptr )	//��ָ���Ľڵ����ǰ�Ľڵ����ҳ���һ���ڵ�ǰ�����еĽڵ� 
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

/***�����������***/ 
void item_lexer::clearAll( void )
{
	this->findItem( 0 );
	while( this->removeItem() );
	this->clearComp();
	this->msClass = "default";
}
/***��ձ���Ĵ���***/ 
void item_lexer::clearComp( void )
{
	while( this->popReg() != lexerSout );
	this->meError = lexerEsuccess;
	this->msCode.clear();		//Դ���� 
	this->msToken.clear();		//�ؼ��� 
	this->mbSpace = false;		//�ؼ����Ƿ��ѱ��ո���� 
	this->miOffb = 0;			//����ƫ�� 
	this->miLine = 1;			//�����к�
	this->miOffl = 0;			//��������ƫ���� 
	this->miRegCount = 0;		//���±���Ľڵ����
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
/***��ĩ������µ�item***/
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
	
	/***�Ȳ���һ�����ظ���Ĭ�ϱ�ǩ***/
	while( !this->uniqLabel( label ) )
	{
		sprintf( temp, "label-%d", ++tempI );
		label = this->msClass + ":" + temp;
	}
	one->msLabel = label;

	/***Ѱ��ĩ�˲����Ԫ��***/
	for(tempP = this->mpCurrent; tempP != 0 and tempP->next != 0; tempP = tempP->next );
	
	/***���ָ��Ϊ�����������Ϊ��***/ 
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
	/***����ǩ����Ч��***/ 
	if( !this->correctLabel( label ) )
		return false;
	/***����ǩΨһ��***/ 
	 if( !this->uniqLabel( label ) )
	 	return false;
	
	/***���item***/ 
	one = new item;
	one->msLabel = label;
	one->mtType = lexerTnone;
	one->mpValue = 0;
	one->miLength = 0;
	
	/***Ѱ����β***/ 
	for(tempP = this->mpCurrent; tempP != 0 and tempP->next != 0; tempP = tempP->next );
	
	/***���ָ��Ϊ�����������Ϊ��***/ 
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
	/***��ĩβ�����item***/
	if( !this->pushItem( label ) )
		return false;
	/***���浱ǰλ��***/
	temp_pos = this->getPos();
	/***ָ��ĩβ***/
	this->findItem( lexerFlast );
	/***��������***/
	this->setType( lexerTboolean );
	/***��������***/
	if( !this->setValue( value ) )
		return false;
	/***��ָ������***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, unsigned char* buf, int length )
{
	int temp_pos;
	/***��ĩβ�����item***/
	if( !this->pushItem( label ) )
		return false;
	/***���浱ǰλ��***/
	temp_pos = this->getPos();
	/***ָ��ĩβ***/
	this->findItem( lexerFlast );
	/***��������***/
	this->setType( lexerTbyte );
	/***��������***/
	if( !this->setValue( buf, length ) )
		return false;
	/***��ָ������***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, unsigned long long value )
{
	int temp_pos;
	/***��ĩβ�����item***/
	if( !this->pushItem( label ) )
		return false;
	/***���浱ǰλ��***/
	temp_pos = this->getPos();
	/***ָ��ĩβ***/
	this->findItem( lexerFlast );
	/***��������***/
	this->setType( lexerThex );
	/***��������***/
	if( !this->setValue( value ) )
		return false;
	/***��ָ������***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, double value )
{
	int temp_pos;
	/***��ĩβ�����item***/
	if( !this->pushItem( label ) )
		return false;
	/***���浱ǰλ��***/
	temp_pos = this->getPos();
	/***ָ��ĩβ***/
	this->findItem( lexerFlast );
	/***��������***/
	this->setType( lexerTnumber );
	/***��������***/
	if( !this->setValue( value ) )
		return false;
	/***��ָ������***/
	this->findItem( temp_pos );
	
	return true;
}
bool item_lexer::pushItem( string label, string value )
{
	int temp_pos;
	/***��ĩβ�����item***/
	if( !this->pushItem( label ) )
		return false;
	/***���浱ǰλ��***/
	temp_pos = this->getPos();
	/***ָ��ĩβ***/
	this->findItem( lexerFlast );
	/***��������***/
	this->setType( lexerTstring );
	/***��������***/
	if( !this->setValue( value ) )
		return false;
	/***��ָ������***/
	this->findItem( temp_pos );
	
	return true;
}
/***�ڵ�ǰλ������µ�item,���ɹ���ָ�����item***/ 
string item_lexer::insertItem( void )
{
	char temp[32] = "label-0";
	string label = this->msClass + ":" + temp;
	int tempI = 0;
	item *one = new item;
	
	one->mpValue = 0;
	one->miLength = 0;
	one->mtType = lexerTnone;
	
	/***�Ȳ���һ�����ظ���Ĭ�ϱ�ǩ***/
	while( !this->uniqLabel( label ) )
	{
		sprintf( temp, "label-%d", ++tempI );
		label = this->msClass + ":" + temp;
	}
	one->msLabel = label;
	
	/***���ָ��Ϊ�����������Ϊ��***/ 
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
	
	/***����ǩ����Ч��***/ 
	if( !this->correctLabel( label ) )
		return false;
	/***�ȼ���ǩΨһ��***/
	if( !this->uniqLabel( label ) )
		return false;
	
	one->mpValue = 0;
	one->miLength = 0;
	one->mtType = lexerTnone;
	
	/***���ָ��Ϊ�����������Ϊ��***/ 
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

/***ɾ��Ԫ��***/
bool item_lexer::removeItem( void )	//ɾ����ǰָ���Ԫ�� 
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
bool item_lexer::removeItem( string label )	//ɾ���ɱ�ǩָ����Ԫ��
{
	int temp_pos = this->getPos();	//���浱ǰλ�� 
 	/***��û��Ԫ����ʧ��***/ 
	if( temp_pos == -1 )
		return false;
	/***Ѱ��Ŀ��,��û����ʧ��***/ 
	if( !this->findItem( label ) )
		return false;
	/***ɾ��Ŀ��***/ 
	this->removeItem();
	/***�ָ�ԭ���Ķ�λ***/ 
	if( !this->findItem( temp_pos ) )
		this->findItem( temp_pos );
	return true;
}
/***���ñ�ǩ***/
bool item_lexer::setLabel( string label )
{
	/***û��Ԫ����ʧ��***/ 
	if( 0 == this->mpCurrent )
		return false;
	/***����ǩ����Ч��***/ 
	if( !this->correctLabel( label ) )
		return false;
	/***����ǩΨһ��***/
	if( !this->uniqLabel( label ) or label.length() <= 0 )
		return false;
	/***���ñ�ǩ***/
	this->mpCurrent->msLabel = label;
	return true; 
}
/***��������***/ 
bool item_lexer::setClass( string cls )
{
	/***û��Ԫ����ʧ��***/ 
	if( 0 == this->mpCurrent )
		return false;
	/***�������������ʧ��***/
	if( !this->correctClass( cls ) )
		return false; 
	this->mpCurrent->msLabel = cls + ":" + this->label2name( this->mpCurrent->msLabel );
	return true;
}
/***���õ�ǰ����***/
bool item_lexer::inClass( string cls )
{
	/***�������������ʧ��***/
	if( !this->correctClass( cls ) )
		return false; 
	this->msClass = cls;
	return true;
}
/***��������***/
bool item_lexer::setType( lexerType type )
{
	/***û��Ԫ����ʧ��***/ 
	if( 0 == this->mpCurrent )
		return false;
	/***��Ԫ��������Ҫ��������ͬ��ֱ�ӳɹ�***/ 
	if( type == this->mpCurrent->mtType )
		return true; 
	/***����ͬ�������Ԫ��***/ 
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
			break;	//byte��none�������������ݳ��ȵ���0������ 
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
/***��������,���Ͳ�����ʧ��***/
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
	//��Ӧ�����ԭ�пռ� 
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
	/***string���������ڴ��б���'\0'������,�������ĵ�����;����***/
	if( 0 == this->mpCurrent )
		return false;
	if( lexerTstring != this->mpCurrent->mtType )
		return false;
	//��Ӧ�����ԭ�пռ� 
	delete[] this->mpCurrent->mpValue;
	
	this->mpCurrent->mpValue = new char[value.length() + 1];
	memcpy( this->mpCurrent->mpValue, value.data(), value.length() );
	this->mpCurrent->mpValue[value.length()] = '\0';
	
	this->mpCurrent->miLength = value.length()+1;
	return true;
}
/***��ȡ����,���Ͳ������׳��쳣***/
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
/***��ȡ����,ʧ���׳��쳣***/
lexerType item_lexer::getType( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->mpCurrent->mtType;
}
/***��ȡ���ݳ���,ʧ�ܷ���-1***/
int item_lexer::getLength( void )
{
	if( 0 == this->mpCurrent )
		return -1;
	return this->mpCurrent->miLength;
}
/***��ȡ��ǩ,ʧ���׳��쳣***/
string item_lexer::getLabel( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->mpCurrent->msLabel;
} 
/***��ȡ����,ʧ���׳��쳣***/
string item_lexer::getClass( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->label2class( mpCurrent->msLabel );
}
/***��ȡitemλ��***/
int item_lexer::getPos( void )
{
	if( 0 == this->mpCurrent )
		return -1;
	int i = 0;
	for( item* temp = this->mpList; temp != this->mpCurrent; temp = temp->next )
		i += 1;
	return i;
}
/***Ѱ�����е�Ԫ��,��ָ��***/
bool item_lexer::findItem( string label )		//���ձ�ǩ���� 
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
bool item_lexer::findItem( lexerFind fd )		//��������λ���ϵ�Ԫ�� 
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
bool item_lexer::findItem( lexerFind fd, lexerType type )//�������ͼ��� 
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
bool item_lexer::findItem( int pos )//���վ���λ�ü��� 
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
/***Ѱ�ҵ�ǰ���е�Ԫ��,��ָ��***/ 
bool item_lexer::classItem( lexerFind fd )	//��������λ���ϵ�Ԫ�� 
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
bool item_lexer::classItem( lexerFind fd, lexerType type )//�������ͼ��� 
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
bool item_lexer::classItem( int pos )//���վ���λ�ü��� 
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
/***ͳ��item����***/
int item_lexer::countItem( void )
{
	int i = 0;
	for( item *temp = this->mpList; temp != 0; temp = temp->next )
		i += 1;
	return i;
}
/***�����ݱ����item code***/
string item_lexer::generateCode( void )
{
	time_t cusec = time( 0 );
	struct tm* cutime = localtime( &cusec );
	char tempBf[8];
	sprintf( tempBf, "%d", this->countItem() );
	string s = "$>�﷨��׼ item code v1.02\n**������ "+ string(asctime( cutime )) + "**������ " + tempBf + " �����ݵ�λ\n<$\n";
	for( item* pTemp = this->mpList; 0 not_eq pTemp; pTemp = pTemp->next )
		s += this->item2string( *pTemp ) + '\n';
	return s;
}
/***����ǰitem������ַ���,ʧ���׳��쳣***/ 
string item_lexer::encodeItem( void )
{
	if( 0 == this->mpCurrent )
		throw out_of_range( "there's no item" );
	return this->item2string( *this->mpCurrent );
}
/***��item code��������ݼ��뵽���ݿ���***/
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
