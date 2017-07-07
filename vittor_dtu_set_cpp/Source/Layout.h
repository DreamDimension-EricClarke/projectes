#ifndef __Layout_H__
#define __Layout_H__
#include <windows.h>
#include "CChain.h"

using namespace std;

typedef LRESULT(*WDGPROC)(HWND,WORD);			//�ؼ��ص�����  �ؼ����ھ�� ֪ͨ��

/*SWidget�ṹ��������һ������*/
struct SWidget {
	HWND			hwnd;                   		//���ھ��
	HMENU			ID;                     		//�ؼ�ID
	string			name;                   		//�ؼ�����
	WDGPROC			proc;                   		//���̻ص�����
	
	SWidget( HWND _hwnd, HMENU _ID, string _name, WDGPROC _proc = 0 );
	SWidget( const SWidget& );      				//��������������ȷ����string name����
};

/*CLayout�ཫ���ڲ��ֳ����ʵ��*/
class CLayout {
	private:
		CChain<SWidget>     m_widget;   		//�ؼ��б�,��һ��Ԫ����Զ��������
		WNDPROC             m_proc;     		//������Ӧ�е���Ϣ����
		HINSTANCE           m_hinst;    		//Ӧ�ó���ʵ��
	private:
		friend LRESULT CALLBACK GroupProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
	public:
		CLayout( WNDPROC );
		~CLayout( void );
		void Init( HFONT hFont );
		
		HWND GetHandle( HMENU ID );
		HWND GetHandle( string name );
		
		HMENU GetID( HWND handle );
		HMENU GetID( string name );
		
		bool GetName( HWND handle, string& name );
		bool GetName( HMENU ID, string& name );
		
		bool SetProc( HWND handle, WDGPROC proc );
		bool SetProc( HMENU ID, WDGPROC proc );
		bool SetProc( string name, WDGPROC proc );
		
		WDGPROC GetProc( HWND handle );
		WDGPROC GetProc( HMENU ID );
		WDGPROC GetProc( string name );
		
		bool CallProc( HWND handle, WORD notify_code, LRESULT& ret );
		bool CallProc( HMENU ID, WORD notify_code, LRESULT& ret );
		bool CallProc( string name, WORD notify_code, LRESULT& ret );
		
		bool CreateGroup( DWORD x, DWORD y, DWORD width, DWORD height, const string& caption, const string& name, const string& parent, WDGPROC proc = 0 );
		bool CreateStatic( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent, WDGPROC proc = 0 );
		bool CreateCombo( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent, WDGPROC proc = 0 );
		bool CreateButton( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent, WDGPROC proc = 0 );
		bool CreateRadio( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent, WDGPROC proc = 0 );
		bool CreateCheck( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent, WDGPROC proc = 0 );
		bool CreateEdit( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent,UINT addition = 0, WDGPROC proc = 0 );
		bool CreateScroll( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption, const string& name, const string& parent, WDGPROC proc = 0 );
		
		void Log( const string& value );
};

#endif
