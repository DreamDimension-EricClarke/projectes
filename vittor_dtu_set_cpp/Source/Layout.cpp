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
	           (char*)"微拓DTU配置器 v20170806",
	           WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_POPUP,
	           128, 24, 1132, 730,
	           NULL, 0, m_hinst, 0 );

	if( hwnd == 0 )
		throw runtime_error( "failed to create main window" );

	m_widget.Push( SWidget( hwnd, 0, "主窗口" ) );
}
CLayout::~CLayout( void ) {
}

void CLayout::Init( HFONT hFont ) {
	HWND	hwnd;
	CreateGroup( 16, 0, 1090, 46, "", "组_串口配置", "主窗口" );
	CreateStatic( 20, 19, 55, 22, "串口号", "标题_串口号", "组_串口配置" );
	CreateCombo( 80, 16, 65, 22, "串口号", "组合框_串口号", "组_串口配置" );
	CreateStatic( 150, 19, 75, 22, "串口波特率", "标题_串口波特率", "组_串口配置" );
	CreateCombo( 230, 16, 80, 22, "串口波特率", "组合框_串口波特率", "组_串口配置" );
	CreateStatic( 315, 19, 60, 22, "数据位数", "标题_数据位数", "组_串口配置" );
	CreateCombo( 380, 16, 45, 22, "数据位数", "组合框_数据位数", "组_串口配置" );
	CreateStatic( 430, 19, 60, 22, "停止位数", "标题_停止位数", "组_串口配置" );
	CreateCombo( 495, 16, 45, 22, "停止位数", "组合框_停止位数", "组_串口配置" );
	CreateStatic( 545, 19, 60, 22, "校验方式", "标题_校验方式", "组_串口配置" );
	CreateCombo( 610, 16, 65, 22, "校验方式", "组合框_校验方式", "组_串口配置" );
	CreateStatic( 680, 19, 55, 22, "流控制", "标题_流控制", "组_串口配置" );
	CreateCombo( 740, 16, 65, 22, "流控制", "组合框_流控制", "组_串口配置" );
	CreateButton( 810, 16, 122, 24, "打开串口", "按钮_串口开关", "组_串口配置", OnCommSwitch );

	CreateGroup( 16, 48, 505, 128, "选择工作模式", "组_工作模式", "主窗口" );
	CreateRadio( 385, 16, 75, 22, "短信透传", "单选按钮__工作模式_短信透传", "组_工作模式", OnWorkMode );
	CreateRadio( 200, 16, 170, 22, "HTTPD", "单选按钮__工作模式_HTTPD", "组_工作模式", OnWorkMode );
	CreateRadio( 15, 16, 170, 22, "网络透传", "单选按钮__工作模式_网络透传", "组_工作模式", OnWorkMode );

	CreateGroup( 16, 178, 505, 240, "网络透传模式参数", "组_设备参数_网络透传", "主窗口" );

	CreateCheck( 15, 28, 75, 22, "服务器A", "复选框_服务器A", "组_设备参数_网络透传", OnFuncCheck );
	CreateGroup( 95, 16, 400, 46, "", "组_服务器A参数_网络透传", "组_设备参数_网络透传" );
	CreateStatic( 10, 16, 70, 22, "地址/端口号", "标题_服务器A地址", "组_服务器A参数_网络透传" );
	CreateEdit( 80, 16, 115, 22, "", "文本框_服务器地址_A", "组_服务器A参数_网络透传", ES_AUTOHSCROLL );
	CreateEdit( 200, 16, 35, 22, "", "文本框_服务器端口_A", "组_服务器A参数_网络透传", ES_AUTOHSCROLL| ES_NUMBER );
	CreateCombo( 240, 16, 55, 22, "连接协议", "组合框_连接协议_A", "组_服务器A参数_网络透传" );
	CreateCombo( 300, 16, 75, 22, "连接方式", "组合框_连接方式_A", "组_服务器A参数_网络透传" );

	CreateCheck( 15, 65, 75, 22, "服务器B", "复选框_服务器B", "组_设备参数_网络透传", OnFuncCheck );
	CreateGroup( 95, 53, 400, 46, "", "组_服务器B参数_网络透传", "组_设备参数_网络透传" );
	CreateStatic( 10, 16, 70, 22, "地址/端口号", "标题_服务器B地址", "组_服务器B参数_网络透传" );
	CreateEdit( 80, 16, 115, 22, "", "文本框_服务器地址_B", "组_服务器B参数_网络透传", ES_AUTOHSCROLL );
	CreateEdit( 200, 16, 35, 22, "", "文本框_服务器端口_B", "组_服务器B参数_网络透传", ES_AUTOHSCROLL| ES_NUMBER );
	CreateCombo( 240, 16, 55, 22, "连接协议", "组合框_连接协议_B", "组_服务器B参数_网络透传" );
	CreateCombo( 300, 16, 75, 22, "连接方式", "组合框_连接方式_B", "组_服务器B参数_网络透传" );

	CreateCheck( 15, 102, 75, 22, "心跳包", "复选框_心跳包", "组_设备参数_网络透传", OnFuncCheck );
	CreateGroup( 95, 90, 400, 46, "", "组_心跳包参数_网络透传", "组_设备参数_网络透传" );
	CreateStatic( 10, 16, 55, 22, "间隔(秒)", "标题_心跳包间隔", "组_心跳包参数_网络透传" );
	CreateEdit( 70, 16, 35, 22, "", "文本框_心跳包间隔", "组_心跳包参数_网络透传", ES_AUTOHSCROLL );
	CreateCombo( 110, 16, 95, 22, "", "组合框_心跳包类型", "组_心跳包参数_网络透传" );
	CreateEdit( 210, 16, 135, 22, "", "文本框_心跳包数据", "组_心跳包参数_网络透传", ES_AUTOHSCROLL );
	CreateCheck( 350, 16, 45, 22, "Hex", "复选框_十六进制心跳数据", "组_心跳包参数_网络透传" );

	CreateCheck( 15, 139, 75, 22, "注册包", "复选框_注册包", "组_设备参数_网络透传", OnFuncCheck );
	CreateGroup( 95, 127, 400, 70, "", "组_注册包参数_网络透传", "组_设备参数_网络透传" );
	CreateStatic( 10, 16, 50, 22, "发送方式", "标题_注册包发送方式", "组_注册包参数_网络透传" );
	CreateCombo( 65, 16, 145, 22, "发送方式", "组合框_注册包发送方式", "组_注册包参数_网络透传" );
	CreateStatic( 215, 16, 45, 22, "包类型", "标题_注册包类型", "组_注册包参数_网络透传" );
	CreateCombo( 265, 16, 130, 22, "包类型", "组合框_注册包类型", "组_注册包参数_网络透传" );
	CreateStatic( 10, 40, 30, 22, "内容", "标题_注册包内容", "组_注册包参数_网络透传" );
	CreateEdit( 40, 40, 305, 22, "", "文本框_注册包数据", "组_注册包参数_网络透传", ES_AUTOHSCROLL );
	CreateCheck( 350, 40, 45, 22, "Hex", "复选框_十六进制注册数据", "组_注册包参数_网络透传" );

	CreateCheck( 15, 200, 75, 22, "透传云", "复选框_透传云", "组_设备参数_网络透传", OnFuncCheck );
	CreateGroup( 95, 188, 400, 46, "", "组_透传云参数_网络透传", "组_设备参数_网络透传" );
	CreateStatic( 10, 16, 30, 22, "账户", "标题_透传云账户", "组_透传云参数_网络透传" );
	CreateEdit( 40, 16, 155, 22, "", "文本框_设备编号", "组_透传云参数_网络透传", ES_AUTOHSCROLL );
	CreateStatic( 205, 16, 30, 22, "密码", "标题_透传云密码", "组_透传云参数_网络透传" );
	CreateEdit( 235, 16, 155, 22, "", "文本框_通讯密码", "组_透传云参数_网络透传", ES_AUTOHSCROLL );

	CreateGroup( 16, 178, 505, 240, "HTTPD模式参数", "组_设备参数_HTTPD", "主窗口" );
	CreateStatic( 16, 32, 70, 22, "请求方法", "标题_HTTPD请求方法", "组_设备参数_HTTPD" );
	CreateCombo( 100, 32, 385, 22, "HTTPD方法", "组合框_HTTPD方法", "组_设备参数_HTTPD" );
	CreateStatic( 16, 64, 70, 22, "URL", "标题_HTTPD_URL", "组_设备参数_HTTPD" );
	CreateEdit( 100, 64, 385, 22, "", "文本框_HTTPDURL", "组_设备参数_HTTPD", ES_AUTOHSCROLL );
	CreateStatic( 16, 92, 70, 22, "服务器地址", "标题_HTTPD服务器地址", "组_设备参数_HTTPD" );
	CreateEdit( 100, 96, 385, 22, "", "文本框_HTTPD地址", "组_设备参数_HTTPD", ES_AUTOHSCROLL );
	CreateStatic( 16, 128, 70, 22, "服务器端口", "标题_HTTPD服务器端口", "组_设备参数_HTTPD" );
	CreateEdit( 100, 128, 385, 22, "", "文本框_HTTPD端口", "组_设备参数_HTTPD", ES_AUTOHSCROLL );
	CreateStatic( 16, 160, 70, 22, "请求头信息", "标题_HTTPD请求头信息", "组_设备参数_HTTPD" );
	CreateEdit( 100, 160, 385, 22, "", "文本框_HTTPD头信息", "组_设备参数_HTTPD", ES_AUTOHSCROLL );

	CreateGroup( 16, 178, 505, 240, "短信透传模式参数", "组_设备参数_短信透传", "主窗口" );
	CreateStatic( 16, 80, 80, 22, "目标电话号码", "标题_目标电话号", "组_设备参数_短信透传" );
	CreateEdit( 100, 80, 385, 22, "", "文本框_目标电话号", "组_设备参数_短信透传", ES_AUTOHSCROLL );

	CreateGroup( 16, 420, 505, 135, "全局参数", "组_全局参数", "主窗口" );
	CreateStatic( 5, 16, 70, 22, "运营商及APN", "标题_运营商及APN", "组_全局参数" );
	CreateCombo( 80, 16, 50, 22, "运营商及APN", "组合框_运营商及APN", "组_全局参数", OnApn );
	CreateEdit( 145, 16, 280, 22, "", "文本框_运营商及APN", "组_全局参数", ES_AUTOHSCROLL );
	CreateStatic( 5, 46, 50, 22, "串口参数", "标题_设备_串口参数", "组_全局参数" );
	CreateStatic( 60, 46, 40, 22, "波特率", "标题_设备_波特率", "组_全局参数" );
	CreateCombo( 105, 46, 65, 22, "波特率", "组合框_设备_波特率", "组_全局参数"  );
	CreateStatic( 175, 46, 25, 22, "流控", "标题_设备_流控", "组_全局参数" );
	CreateCombo( 205, 46, 55, 22, "流控", "组合框_设备_流控", "组_全局参数" );
	CreateStatic( 265, 46, 85, 22, "校验/数据/停止", "标题_设备_校验/数据/停止", "组_全局参数" );
	CreateCombo( 355, 46, 50, 22, "校验", "组合框_设备_校验", "组_全局参数" );
	CreateCombo( 410, 46, 30, 22, "数据", "组合框_设备_数据位数", "组_全局参数" );
	CreateCombo( 445, 46, 45, 22, "停止", "组合框_设备_停止位数", "组_全局参数" );
	CreateCheck( 5, 76, 80, 22, "指令回显", "复选框_指令回显", "组_全局参数" );
	CreateCheck( 85, 76, 80, 22, "类RFC2217", "复选框_类RFC2217", "组_全局参数" );
	CreateCheck( 165, 76, 65, 22, "通话功能", "复选框_通话功能", "组_全局参数" );
	CreateCheck( 5, 106, 80, 22, "串口AT指令", "复选框_串口AT指令", "组_全局参数" );
	CreateCheck( 85, 106, 80, 22, "网络AT指令", "复选框_网络AT指令", "组_全局参数" );
	CreateCheck( 165, 106, 65, 22, "允许缓存", "复选框_允许缓存", "组_全局参数" );
	CreateStatic( 235, 81, 50, 22, "命令密码", "标题_命令密码", "组_全局参数" );
	CreateEdit( 285, 76, 200, 22, "", "文本框_命令密码", "组_全局参数", ES_AUTOHSCROLL );
	CreateStatic( 235, 111, 50, 22, "启动信息", "标题_启动信息", "组_全局参数" );
	CreateEdit( 285, 106, 200, 22, "", "文本框_启动信息", "组_全局参数", ES_AUTOHSCROLL );

	CreateButton( 530, 56, 140, 36, "获取当前参数", "按钮_获取当前参数", "主窗口", OnQueryParam );
	CreateButton( 675, 56, 140, 36, "设置并保存所有参数", "按钮_设置并保存所有参数", "主窗口", OnSetParam );
	CreateButton( 820, 56, 140, 36, "进入配置状态", "按钮_进入配置状态", "主窗口", OnEnterConfigState );
	CreateButton( 965, 56, 140, 36, "进入通讯状态", "按钮_进入通讯状态", "主窗口", OnEnterCommunicateState );

	CreateButton( 530, 120, 140, 22, "帮助信息", "按钮_帮助信息", "主窗口", OnSubFunc );
	CreateButton( 675, 120, 140, 22, "模块重启", "按钮_模块重启", "主窗口", OnSubFunc );
	CreateButton( 820, 120, 140, 22, "查询版本", "按钮_查询版本", "主窗口", OnSubFunc );
	CreateButton( 965, 120, 140, 22, "查询本机号码", "按钮_查询本机号码", "主窗口", OnSubFunc );

	CreateButton( 530, 145, 140, 22, "存为用户默认", "按钮_存为用户默认", "主窗口", OnSubFunc );
	CreateButton( 675, 145, 140, 22, "恢复用户默认", "按钮_恢复用户默认", "主窗口", OnSubFunc );
	CreateButton( 820, 145, 140, 22, "恢复出厂设置", "按钮_恢复出厂设置", "主窗口", OnSubFunc );
	CreateButton( 965, 145, 140, 22, "保存参数", "按钮_保存参数", "主窗口" );

	CreateEdit( 535, 185, 570, 370, "", "文本框_运行日志", "主窗口", ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL );
	CreateEdit( 535, 560, 570, 95, "", "文本框_手动指令", "主窗口", ES_MULTILINE | ES_WANTRETURN );
	CreateCheck( 535, 658, 128, 22, "自动补全指令", "复选框_自动补全", "主窗口" );
	CreateButton( 977, 658, 128, 22, "发送", "按钮_发送指令", "主窗口", OnSendCmd );

	CreateGroup( 16, 555, 505, 125, "附加功能", "组_附加功能", "主窗口" );
	CreateButton( 16, 16, 128, 22, "智能柜配置模板", "按钮_电子柜配置模板", "组_附加功能", OnTemplate );
	CreateButton( 16, 46, 128, 22, "DTU配置模板", "按钮_DTU配置模板", "组_附加功能", OnTemplate );
	CreateButton( 16, 76, 128, 22, "校车通配置模板", "按钮_校车通配置模板", "组_附加功能", OnTemplate );

	m_widget.ForEach( [&](SWidget& widget ) {
		SendMessage( widget.hwnd, WM_SETFONT, (WPARAM)hFont, TRUE );
		return true;
	});

	hwnd = GetHandle( "组合框_串口波特率" );
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

	hwnd = GetHandle( "组合框_数据位数" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"8" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"7" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_停止位数" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1.5" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"2" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "按钮_串口开关" );
	SendMessage( hwnd, BM_SETSTYLE, BS_ICON | BS_LEFT, 0 );
	SendMessage( hwnd, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon( m_hinst, (LPCSTR)RES_BMP_RED ) );

	hwnd = GetHandle( "组合框_校验方式" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ODD" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"EVEN" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_流控制" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"CRTS" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_连接协议_A" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"TCP" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"UDP" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_连接方式_A" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"短连接" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"长连接" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_连接协议_B" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"TCP" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"UDP" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_连接方式_B" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"短连接" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"长连接" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_心跳包类型" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"向服务器发送" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"向串口发送" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_注册包发送方式" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"连接建立时发送一次" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"每次发送指令之前发送" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"同时启用以上两种" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_注册包类型" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ICCID" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"IMEI" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"注册ID" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"自定义数据" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );


	hwnd = GetHandle( "组合框_HTTPD方法" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"GET" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"POST" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_运营商及APN" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"移动" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"联通" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"其他" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_设备_波特率" );
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

	hwnd = GetHandle( "组合框_设备_流控" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"CRTS" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"RS485" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_设备_校验" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"NONE" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"ODD" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"EVEN" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_设备_数据位数" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"8" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"7" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "组合框_设备_停止位数" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"1" );
	SendMessage( hwnd, CB_ADDSTRING, 0, (LPARAM)"2" );
	SendMessage( hwnd, CB_SETCURSEL, 0, 0 );

	hwnd = GetHandle( "单选按钮__工作模式_网络透传" );
	SendMessage( hwnd, BM_SETCHECK, BST_CHECKED, 0 );
	OnWorkMode( 0, BN_CLICKED );
	
	hwnd = GetHandle( "复选框_服务器A" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "复选框_服务器B" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "复选框_心跳包" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "复选框_注册包" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	hwnd = GetHandle( "复选框_透传云" );
	SendMessage( hwnd, BM_SETCHECK, BST_UNCHECKED, 0 );
	
	OnFuncCheck( 0, BN_CLICKED );
	hwnd = GetHandle( "组合框_运营商及APN" );
	OnApn( hwnd, CBN_SELCHANGE );
	
	Log( "\
一般操作流程:\r\n\
1、模块连PC串口,上电;\r\n\
2、打开串口;\r\n\
3、获取当前参数;\r\n\
4、选择工作模式及配置相关参数;\r\n\
5、设置并保存所有参数;" );
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
		if( hwnd == layout.GetHandle( "组_工作模式" ) ) {
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
	HWND hWnd = GetHandle( "文本框_运行日志" );
	static string logstr;
	logstr += value + string("\r\n");
	SetWindowText( hWnd, logstr.data() );
	SendMessage( hWnd, EM_SCROLL, SB_BOTTOM, 0 );
}

#endif
