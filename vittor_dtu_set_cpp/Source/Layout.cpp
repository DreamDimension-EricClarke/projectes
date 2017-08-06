#ifndef __Layout_CPP__
#define __Layout_CPP__
#include <iostream>
#include "Layout.h"
#include "main.h"

using namespace std;

volatile WNDPROC DefGroupProc = 0;
volatile WNDPROC DefButtonProc = 0;

SWidget::SWidget( HWND _hwnd, HMENU _ID, string _name, WDGPROC _proc ):
	hwnd(_hwnd),ID(_ID),name(_name), proc(_proc) {

}

SWidget::SWidget( const SWidget& another):
	hwnd(another.hwnd), ID(another.ID), name(another.name), proc(another.proc) {
}

CLayout::CLayout( WNDPROC proc ):
	m_proc( proc ) {
	HWND	hwnd;
	WNDCLASSEX wndclass;
	m_hinst = GetModuleHandle( 0 );

	memset( &wndclass, 0, sizeof(wndclass) );
	wndclass.cbSize = sizeof( wndclass );
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndclass.hIcon = LoadIcon( m_hinst, (LPCSTR)0 );
	wndclass.hIconSm = LoadIcon( m_hinst, (LPCSTR)0 );
	wndclass.hInstance = m_hinst;
	wndclass.lpfnWndProc = proc;
	wndclass.lpszClassName = (char*)"vittor_dtu_set_cpp";
	wndclass.lpszMenuName = 0;
	wndclass.style = CS_ENABLE;

	if( !RegisterClassEx( &wndclass ) )
		throw runtime_error( "failed to register class" );

	hwnd = CreateWindowEx(
	           WS_EX_CLIENTEDGE,
	           (char*)"vittor_dtu_set_cpp",
	           (char*)"΢��DTU������ v20170806",
	           WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_POPUP,
	           128, 24, 1132, 730,
	           NULL, 0, m_hinst, 0 );

	if( hwnd == 0 )
		throw runtime_error( "failed to create main window" );

	m_widget.Push( SWidget( hwnd, 0, "������" ) );
}
CLayout::~CLayout( void ) {
}

void CLayout::Init( HFONT hFont ) {
	HWND	hwnd;
	CreateGroup( 16, 0, 1090, 46, "", "��_��������", "������" );
	CreateStatic( 20, 19, 55, 22, "���ں�", "����_���ں�", "��_��������" );
	CreateCombo( 80, 16, 65, 22, "���ں�", "��Ͽ�_���ں�", "��_��������" );
	CreateStatic( 150, 19, 75, 22, "���ڲ�����", "����_���ڲ�����", "��_��������" );
	CreateCombo( 230, 16, 80, 22, "���ڲ�����", "��Ͽ�_���ڲ�����", "��_��������" );
	CreateStatic( 315, 19, 60, 22, "����λ��", "����_����λ��", "��_��������" );
	CreateCombo( 380, 16, 45, 22, "����λ��", "��Ͽ�_����λ��", "��_��������" );
	CreateStatic( 430, 19, 60, 22, "ֹͣλ��", "����_ֹͣλ��", "��_��������" );
	CreateCombo( 495, 16, 45, 22, "ֹͣλ��", "��Ͽ�_ֹͣλ��", "��_��������" );
	CreateStatic( 545, 19, 60, 22, "У�鷽ʽ", "����_У�鷽ʽ", "��_��������" );
	CreateCombo( 610, 16, 65, 22, "У�鷽ʽ", "��Ͽ�_У�鷽ʽ", "��_��������" );
	CreateStatic( 680, 19, 55, 22, "������", "����_������", "��_��������" );
	CreateCombo( 740, 16, 65, 22, "������", "��Ͽ�_������", "��_��������" );
	CreateButton( 810, 16, 122, 24, "�򿪴���", "��ť_���ڿ���", "��_��������", OnCommSwitch );

	CreateGroup( 16, 48, 505, 128, "ѡ����ģʽ", "��_����ģʽ", "������" );
	CreateRadio( 385, 16, 75, 22, "����͸��", "��ѡ��ť__����ģʽ_����͸��", "��_����ģʽ", OnWorkMode );
	CreateRadio( 200, 16, 170, 22, "HTTPD", "��ѡ��ť__����ģʽ_HTTPD", "��_����ģʽ", OnWorkMode );
	CreateRadio( 15, 16, 170, 22, "����͸��", "��ѡ��ť__����ģʽ_����͸��", "��_����ģʽ", OnWorkMode );

	CreateGroup( 16, 178, 505, 240, "����͸��ģʽ����", "��_�豸����_����͸��", "������" );

	CreateCheck( 15, 28, 75, 22, "������A", "��ѡ��_������A", "��_�豸����_����͸��", OnFuncCheck );
	CreateGroup( 95, 16, 400, 46, "", "��_������A����_����͸��", "��_�豸����_����͸��" );
	CreateStatic( 10, 16, 70, 22, "��ַ/�˿ں�", "����_������A��ַ", "��_������A����_����͸��" );
	CreateEdit( 80, 16, 115, 22, "", "�ı���_��������ַ_A", "��_������A����_����͸��", ES_AUTOHSCROLL );
	CreateEdit( 200, 16, 35, 22, "", "�ı���_�������˿�_A", "��_������A����_����͸��", ES_AUTOHSCROLL| ES_NUMBER );
	CreateCombo( 240, 16, 55, 22, "����Э��", "��Ͽ�_����Э��_A", "��_������A����_����͸��" );
	CreateCombo( 300, 16, 75, 22, "���ӷ�ʽ", "��Ͽ�_���ӷ�ʽ_A", "��_������A����_����͸��" );

	CreateCheck( 15, 65, 75, 22, "������B", "��ѡ��_������B", "��_�豸����_����͸��", OnFuncCheck );
	CreateGroup( 95, 53, 400, 46, "", "��_������B����_����͸��", "��_�豸����_����͸��" );
	CreateStatic( 10, 16, 70, 22, "��ַ/�˿ں�", "����_������B��ַ", "��_������B����_����͸��" );
	CreateEdit( 80, 16, 115, 22, "", "�ı���_��������ַ_B", "��_������B����_����͸��", ES_AUTOHSCROLL );
	CreateEdit( 200, 16, 35, 22, "", "�ı���_�������˿�_B", "��_������B����_����͸��", ES_AUTOHSCROLL| ES_NUMBER );
	CreateCombo( 240, 16, 55, 22, "����Э��", "��Ͽ�_����Э��_B", "��_������B����_����͸��" );
	CreateCombo( 300, 16, 75, 22, "���ӷ�ʽ", "��Ͽ�_���ӷ�ʽ_B", "��_������B����_����͸��" );

	CreateCheck( 15, 102, 75, 22, "������", "��ѡ��_������", "��_�豸����_����͸��", OnFuncCheck );
	CreateGroup( 95, 90, 400, 46, "", "��_����������_����͸��", "��_�豸����_����͸��" );
	CreateStatic( 10, 16, 55, 22, "���(��)", "����_���������", "��_����������_����͸��" );
	CreateEdit( 70, 16, 35, 22, "", "�ı���_���������", "��_����������_����͸��", ES_AUTOHSCROLL );
	CreateCombo( 110, 16, 95, 22, "", "��Ͽ�_����������", "��_����������_����͸��" );
	CreateEdit( 210, 16, 135, 22, "", "�ı���_����������", "��_����������_����͸��", ES_AUTOHSCROLL );
	CreateCheck( 350, 16, 45, 22, "Hex", "��ѡ��_ʮ��������������", "��_����������_����͸��" );

	CreateCheck( 15, 139, 75, 22, "ע���", "��ѡ��_ע���", "��_�豸����_����͸��", OnFuncCheck );
	CreateGroup( 95, 127, 400, 70, "", "��_ע�������_����͸��", "��_�豸����_����͸��" );
	CreateStatic( 10, 16, 50, 22, "���ͷ�ʽ", "����_ע������ͷ�ʽ", "��_ע�������_����͸��" );
	CreateCombo( 65, 16, 145, 22, "���ͷ�ʽ", "��Ͽ�_ע������ͷ�ʽ", "��_ע�������_����͸��" );
	CreateStatic( 215, 16, 45, 22, "������", "����_ע�������", "��_ע�������_����͸��" );
	CreateCombo( 265, 16, 130, 22, "������", "��Ͽ�_ע�������", "��_ע�������_����͸��" );
	CreateStatic( 10, 40, 30, 22, "����", "����_ע�������", "��_ע�������_����͸��" );
	CreateEdit( 40, 40, 305, 22, "", "�ı���_ע�������", "��_ע�������_����͸��", ES_AUTOHSCROLL );
	CreateCheck( 350, 40, 45, 22, "Hex", "��ѡ��_ʮ������ע������", "��_ע�������_����͸��" );

	CreateCheck( 15, 200, 75, 22, "͸����", "��ѡ��_͸����", "��_�豸����_����͸��", OnFuncCheck );
	CreateGroup( 95, 188, 400, 46, "", "��_͸���Ʋ���_����͸��", "��_�豸����_����͸��" );
	CreateStatic( 10, 16, 30, 22, "�˻�", "����_͸�����˻�", "��_͸���Ʋ���_����͸��" );
	CreateEdit( 40, 16, 155, 22, "", "�ı���_�豸���", "��_͸���Ʋ���_����͸��", ES_AUTOHSCROLL );
	CreateStatic( 205, 16, 30, 22, "����", "����_͸��������", "��_͸���Ʋ���_����͸��" );
	CreateEdit( 235, 16, 155, 22, "", "�ı���_ͨѶ����", "��_͸���Ʋ���_����͸��", ES_AUTOHSCROLL );

	CreateGroup( 16, 178, 505, 240, "HTTPDģʽ����", "��_�豸����_HTTPD", "������" );
	CreateStatic( 16, 32, 70, 22, "���󷽷�", "����_HTTPD���󷽷�", "��_�豸����_HTTPD" );
	CreateCombo( 100, 32, 385, 22, "HTTPD����", "��Ͽ�_HTTPD����", "��_�豸����_HTTPD" );
	CreateStatic( 16, 64, 70, 22, "URL", "����_HTTPD_URL", "��_�豸����_HTTPD" );
	CreateEdit( 100, 64, 385, 22, "", "�ı���_HTTPDURL", "��_�豸����_HTTPD", ES_AUTOHSCROLL );
	CreateStatic( 16, 92, 70, 22, "��������ַ", "����_HTTPD��������ַ", "��_�豸����_HTTPD" );
	CreateEdit( 100, 96, 385, 22, "", "�ı���_HTTPD��ַ", "��_�豸����_HTTPD", ES_AUTOHSCROLL );
	CreateStatic( 16, 128, 70, 22, "�������˿�", "����_HTTPD�������˿�", "��_�豸����_HTTPD" );
	CreateEdit( 100, 128, 385, 22, "", "�ı���_HTTPD�˿�", "��_�豸����_HTTPD", ES_AUTOHSCROLL );
	CreateStatic( 16, 160, 70, 22, "����ͷ��Ϣ", "����_HTTPD����ͷ��Ϣ", "��_�豸����_HTTPD" );
	CreateEdit( 100, 160, 385, 22, "", "�ı���_HTTPDͷ��Ϣ", "��_�豸����_HTTPD", ES_AUTOHSCROLL );

	CreateGroup( 16, 178, 505, 240, "����͸��ģʽ����", "��_�豸����_����͸��", "������" );
	CreateStatic( 16, 80, 80, 22, "Ŀ��绰����", "����_Ŀ��绰��", "��_�豸����_����͸��" );
	CreateEdit( 100, 80, 385, 22, "", "�ı���_Ŀ��绰��", "��_�豸����_����͸��", ES_AUTOHSCROLL );

	CreateGroup( 16, 420, 505, 135, "ȫ�ֲ���", "��_ȫ�ֲ���", "������" );
	CreateStatic( 5, 16, 70, 22, "��Ӫ�̼�APN", "����_��Ӫ�̼�APN", "��_ȫ�ֲ���" );
	CreateCombo( 80, 16, 50, 22, "��Ӫ�̼�APN", "��Ͽ�_��Ӫ�̼�APN", "��_ȫ�ֲ���", OnApn );
	CreateEdit( 145, 16, 280, 22, "", "�ı���_��Ӫ�̼�APN", "��_ȫ�ֲ���", ES_AUTOHSCROLL );
	CreateStatic( 5, 46, 50, 22, "���ڲ���", "����_�豸_���ڲ���", "��_ȫ�ֲ���" );
	CreateStatic( 60, 46, 40, 22, "������", "����_�豸_������", "��_ȫ�ֲ���" );
	CreateCombo( 105, 46, 65, 22, "������", "��Ͽ�_�豸_������", "��_ȫ�ֲ���"  );
	CreateStatic( 175, 46, 25, 22, "����", "����_�豸_����", "��_ȫ�ֲ���" );
	CreateCombo( 205, 46, 55, 22, "����", "��Ͽ�_�豸_����", "��_ȫ�ֲ���" );
	CreateStatic( 265, 46, 85, 22, "У��/����/ֹͣ", "����_�豸_У��/����/ֹͣ", "��_ȫ�ֲ���" );
	CreateCombo( 355, 46, 50, 22, "У��", "��Ͽ�_�豸_У��", "��_ȫ�ֲ���" );
	CreateCombo( 410, 46, 30, 22, "����", "��Ͽ�_�豸_����λ��", "��_ȫ�ֲ���" );
	CreateCombo( 445, 46, 45, 22, "ֹͣ", "��Ͽ�_�豸_ֹͣλ��", "��_ȫ�ֲ���" );
	CreateCheck( 5, 76, 80, 22, "ָ�����", "��ѡ��_ָ�����", "��_ȫ�ֲ���" );
	CreateCheck( 85, 76, 80, 22, "��RFC2217", "��ѡ��_��RFC2217", "��_ȫ�ֲ���" );
	CreateCheck( 165, 76, 65, 22, "ͨ������", "��ѡ��_ͨ������", "��_ȫ�ֲ���" );
	CreateCheck( 5, 106, 80, 22, "����ATָ��", "��ѡ��_����ATָ��", "��_ȫ�ֲ���" );
	CreateCheck( 85, 106, 80, 22, "����ATָ��", "��ѡ��_����ATָ��", "��_ȫ�ֲ���" );
	CreateCheck( 165, 106, 65, 22, "������", "��ѡ��_������", "��_ȫ�ֲ���" );
	CreateStatic( 235, 81, 50, 22, "��������", "����_��������", "��_ȫ�ֲ���" );
	CreateEdit( 285, 76, 200, 22, "", "�ı���_��������", "��_ȫ�ֲ���", ES_AUTOHSCROLL );
	CreateStatic( 235, 111, 50, 22, "������Ϣ", "����_������Ϣ", "��_ȫ�ֲ���" );
	CreateEdit( 285, 106, 200, 22, "", "�ı���_������Ϣ", "��_ȫ�ֲ���", ES_AUTOHSCROLL );

	CreateButton( 530, 56, 140, 36, "��ȡ��ǰ����", "��ť_��ȡ��ǰ����", "������", OnQueryParam );
	CreateButton( 675, 56, 140, 36, "���ò��������в���", "��ť_���ò��������в���", "������", OnSetParam );
	CreateButton( 820, 56, 140, 36, "��������״̬", "��ť_��������״̬", "������", OnEnterConfigState );
	CreateButton( 965, 56, 140, 36, "����ͨѶ״̬", "��ť_����ͨѶ״̬", "������", OnEnterCommunicateState );

	CreateButton( 530, 120, 140, 22, "������Ϣ", "��ť_������Ϣ", "������", OnSubFunc );
	CreateButton( 675, 120, 140, 22, "ģ������", "��ť_ģ������", "������", OnSubFunc );
	CreateButton( 820, 120, 140, 22, "��ѯ�汾", "��ť_��ѯ�汾", "������", OnSubFunc );
	CreateButton( 965, 120, 140, 22, "��ѯ��������", "��ť_��ѯ��������", "������", OnSubFunc );

	CreateButton( 530, 145, 140, 22, "��Ϊ�û�Ĭ��", "��ť_��Ϊ�û�Ĭ��", "������", OnSubFunc );
	CreateButton( 675, 145, 140, 22, "�ָ��û�Ĭ��", "��ť_�ָ��û�Ĭ��", "������", OnSubFunc );
	CreateButton( 820, 145, 140, 22, "�ָ���������", "��ť_�ָ���������", "������", OnSubFunc );
	CreateButton( 965, 145, 140, 22, "�������", "��ť_�������", "������" );

	CreateEdit( 535, 185, 570, 370, "", "�ı���_������־", "������", ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL );
	CreateEdit( 535, 560, 570, 95, "", "�ı���_�ֶ�ָ��", "������", ES_MULTILINE | ES_WANTRETURN );
	CreateCheck( 535, 658, 128, 22, "�Զ���ȫָ��", "��ѡ��_�Զ���ȫ", "������" );
	CreateButton( 977, 658, 128, 22, "����", "��ť_����ָ��", "������", OnSendCmd );

	CreateGroup( 16, 555, 505, 125, "���ӹ���", "��_���ӹ���", "������" );
	CreateButton( 16, 16, 128, 22, "���ܹ�����ģ��", "��ť_���ӹ�����ģ��", "��_���ӹ���", OnTemplate );
	CreateButton( 16, 46, 128, 22, "DTU����ģ��", "��ť_DTU����ģ��", "��_���ӹ���", OnTemplate );
	CreateButton( 16, 76, 128, 22, "У��ͨ����ģ��", "��ť_У��ͨ����ģ��", "��_���ӹ���", OnTemplate );

	m_widget.ForEach( [&](SWidget& widget ) {
		SendMessage( widget.hwnd, WM_SETFONT, (WPARAM)hFont, TRUE );
		return true;
	});

	hwnd = GetHandle( "��Ͽ�_���ڲ�����" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1200" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"2400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"4800" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"9600" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"14400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"19200" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"38400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"57600" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"115200" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"128000" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"230400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"460800" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"921600" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_����λ��" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"8" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"7" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_ֹͣλ��" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1.5" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"2" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��ť_���ڿ���" );
	SendMessage( hwnd, BM_SETSTYLE, BS_ICON | BS_LEFT, 0 );
	SendMessage( hwnd, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon( m_hinst, (LPCSTR)RES_BMP_RED ) );

	hwnd = GetHandle( "��Ͽ�_У�鷽ʽ" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ODD" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"EVEN" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"CRTS" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_����Э��_A" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"TCP" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"UDP" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_���ӷ�ʽ_A" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"������" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_����Э��_B" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"TCP" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"UDP" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_���ӷ�ʽ_B" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"������" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_����������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"�����������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"�򴮿ڷ���" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_ע������ͷ�ʽ" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"���ӽ���ʱ����һ��" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ÿ�η���ָ��֮ǰ����" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ͬʱ������������" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_ע�������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ICCID" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"IMEI" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ע��ID" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"�Զ�������" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );


	hwnd = GetHandle( "��Ͽ�_HTTPD����" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"GET" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"POST" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_��Ӫ�̼�APN" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"�ƶ�" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"��ͨ" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"����" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_�豸_������" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1200" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"2400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"4800" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"9600" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"14400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"19200" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"38400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"57600" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"115200" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"128000" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"230400" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"460800" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"921600" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_�豸_����" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"CRTS" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"RS485" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_�豸_У��" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ODD" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"EVEN" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_�豸_����λ��" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"8" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"7" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��Ͽ�_�豸_ֹͣλ��" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"2" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "��ѡ��ť__����ģʽ_����͸��" );
	SendMessage( hwnd, BM_SETCHECK, BST_CHECKED, 0 );
	OnWorkMode( 0, BN_CLICKED );
	
	hwnd = GetHandle( "��ѡ��_������A" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "��ѡ��_������B" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "��ѡ��_������" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "��ѡ��_ע���" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "��ѡ��_͸����" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	
	OnFuncCheck( 0, BN_CLICKED );
	hwnd = GetHandle( "��Ͽ�_��Ӫ�̼�APN" );
	OnApn( hwnd, CBN_SELCHANGE );
	
	Log( "\
һ���������:\r\n\
1��ģ����PC����,�ϵ�;\r\n\
2���򿪴���;\r\n\
3����ȡ��ǰ����;\r\n\
4��ѡ����ģʽ��������ز���;\r\n\
5�����ò��������в���;" );
}

HWND CLayout::GetHandle( HMENU ID ) {
	HWND hwnd = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			hwnd = widget.hwnd;
			return false;
		}
		return true;
	} );
	return hwnd;
}
HWND CLayout::GetHandle( string name ) {
	HWND hwnd = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			hwnd = widget.hwnd;
			return false;
		}
		return true;
	} );
	return hwnd;
}

HMENU CLayout::GetID( HWND handle ) {
	HMENU ID = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			ID = widget.ID;
			return false;
		}
		return true;
	} );
	return ID;
}
HMENU CLayout::GetID( string name ) {
	HMENU ID = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			ID = widget.ID;
			return false;
		}
		return true;
	} );
	return ID;
}

bool CLayout::GetName( HWND handle, string& name ) {
	bool bfound = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			name = widget.name;
			bfound = true;
			return false;
		}
		return true;
	} );
	return bfound;
}
bool CLayout::GetName( HMENU ID, string& name ) {
	bool bfound = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			name = widget.name;
			bfound = true;
			return false;
		}
		return true;
	} );
	return bfound;
}

bool CLayout::SetProc( HWND handle, WDGPROC proc ) {
	bool bSet = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			widget.proc = proc;
			bSet = true;
			return false;
		}
		return true;
	} );
	return bSet;
}
bool CLayout::SetProc( HMENU ID, WDGPROC proc ) {
	bool bSet = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			widget.proc = proc;
			bSet = true;
			return false;
		}
		return true;
	} );
	return bSet;
}
bool CLayout::SetProc( string name, WDGPROC proc ) {
	bool bSet = false;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			widget.proc = proc;
			bSet = true;
			return false;
		}
		return true;
	} );
	return bSet;
}

WDGPROC CLayout::GetProc( HWND handle ) {
	WDGPROC proc = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.hwnd == handle ) {
			proc = widget.proc;
			return false;
		}
		return true;
	} );
	return proc;
}
WDGPROC CLayout::GetProc( HMENU ID ) {
	WDGPROC proc = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.ID == ID ) {
			proc = widget.proc;
			return false;
		}
		return true;
	} );
	return proc;
}
WDGPROC CLayout::GetProc( string name ) {
	WDGPROC proc = 0;
	m_widget.ForEach( [&](SWidget& widget ) {
		if( widget.name == name ) {
			proc = widget.proc;
			return false;
		}
		return true;
	} );
	return proc;
}

bool CLayout::CallProc( HWND handle, WORD notify_code, LRESULT& ret ) {
	if( bRunning )
		return false;
	WDGPROC proc = GetProc( handle );
	if( proc == 0 ) {
		ret = 0;
		return false;
	} else {
		TaskFunc = proc;
		TaskWnd = handle;
		TaskNCode = notify_code;
		ResumeThread( TaskThread );
		ret = 0;
		return true;
	}
}
bool CLayout::CallProc( HMENU ID, WORD notify_code, LRESULT& ret ) {
	if( bRunning )
		return false;
	WDGPROC proc = GetProc( ID );
	if( proc == 0 ) {
		ret = 0;
		return false;
	} else {
		TaskFunc = proc;
		TaskWnd = GetHandle( ID );
		TaskNCode = notify_code;
		ResumeThread( TaskThread );
		ret = 0;
		return true;
	}
}
bool CLayout::CallProc( string name, WORD notify_code, LRESULT& ret ) {
	if( bRunning )
		return false;
	WDGPROC proc = GetProc( name );
	if( proc == 0 ) {
		ret = 0;
		return false;
	} else {
		TaskFunc = proc;
		TaskWnd = GetHandle( name );
		TaskNCode = notify_code;
		ResumeThread( TaskThread );
		ret = 0;
		return true;
	}
}

LRESULT CALLBACK GroupProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_PAINT:
		if( hwnd == layout.GetHandle( "��_����ģʽ" ) ) {
			OnWorkMode( hwnd, BN_CLICKED );
		}
		break;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
		//return (LRESULT)COLOR_WINDOW;
	case WM_COMMAND:
	case WM_NOTIFY:
		SendMessage( (HWND)GetWindowLongPtr( hwnd, GWLP_HWNDPARENT ), message, wParam, lParam );
		break;
	default:
		break;
	}
	if( DefGroupProc != 0 )
		return CallWindowProc(DefGroupProc, hwnd, message, wParam, lParam );
	else
		return DefWindowProc( hwnd, message, wParam, lParam );
}

LRESULT CALLBACK ButtonProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	default:
		break;
	}
	if( DefButtonProc != 0 )
		return CallWindowProc(DefButtonProc, hwnd, message, wParam, lParam );
	else
		return DefWindowProc( hwnd, message, wParam, lParam );
}


bool CLayout::CreateGroup( DWORD x, DWORD y, DWORD width, DWORD height, const string& caption,
                           const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption.data(),
	                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
	                x, y, width, height,
	                GetHandle( parent ),
	                (HMENU)ID,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;
	DefGroupProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)GroupProc );
	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateStatic( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                            const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"STATIC",
	                caption,
	                WS_CHILD | WS_VISIBLE,
	                x, y, width, height,
	                hparent,
	                (HMENU)ID,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateCombo( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                           const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"COMBOBOX",
	                caption,
	                WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateButton( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                            const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption,
	                WS_CHILD | WS_VISIBLE | BS_NOTIFY,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	DefButtonProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)ButtonProc );

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateRadio( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                           const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption,
	                WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_AUTORADIOBUTTON,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	DefButtonProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)ButtonProc );

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateCheck( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                           const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"BUTTON",
	                caption,
	                WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_AUTOCHECKBOX,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	DefButtonProc = (WNDPROC)GetWindowLongPtr( hwnd, GWLP_WNDPROC );
	SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)ButtonProc );

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateEdit( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                          const string& name, const string& parent, UINT addition,  WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"EDIT",
	                caption,
	                WS_CHILD | WS_VISIBLE | WS_BORDER | addition,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

bool CLayout::CreateScroll( DWORD x, DWORD y, DWORD width, DWORD height, const char* caption,
                            const string& name, const string& parent, WDGPROC proc ) {

	long long ID = (long long)m_widget.Count();
	HWND hparent = GetHandle( parent );
	HWND hwnd = CreateWindowEx(
	                0,
	                (char*)"SCROLLBOX",
	                caption,
	                WS_CHILD | WS_VISIBLE | SS_NOTIFY,
	                x, y, width, height,
	                hparent,
	                0,
	                m_hinst,
	                0
	            );

	if( hwnd == 0 )
		return false;

	return m_widget.Push( SWidget( hwnd, (HMENU)ID, name, proc ) );
}

void CLayout::Log( const string& value ) {
	HWND hWnd = GetHandle( "�ı���_������־" );
	static string logstr;
	logstr += value + string("\r\n");
	SetWindowText( hWnd, logstr.data() );
	SendMessage( hWnd, EM_SCROLL, SB_BOTTOM, 0 );
}

#endif
