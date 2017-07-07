
#ifndef __UMFH_H__
#define __UMFH_H__


#include <windows.h>


#define DLLSPEC extern "C" __declspec(dllexport)

//===============================================
/*----------M1 Commond Functions--------*/
//===============================================
//1
DLLSPEC int _stdcall fw_init(int port,long baud); 
//2
DLLSPEC int _stdcall fw_exit(int icdev);
//3
DLLSPEC int _stdcall fw_card_hex(int icdev,unsigned char _Mode,unsigned char* Snrbuf);

DLLSPEC int _stdcall fw_card(int icdev,unsigned char _Mode,unsigned long *_Snr);
DLLSPEC int _stdcall fw_card_str(int icdev,unsigned char _Mode,unsigned char* strSnr);
//4
DLLSPEC int _stdcall fw_request(int icdev,unsigned char _Mode,unsigned int *TagType);
//5
DLLSPEC int _stdcall fw_anticoll(int icdev,unsigned char _Bcnt,unsigned long *_Snr);
//6
DLLSPEC int _stdcall fw_select(int icdev,unsigned long _Snr,unsigned char *_Size);
//7
DLLSPEC int _stdcall fw_load_key(int icdev,unsigned char _Mode,
				unsigned char _SecNr,unsigned char *_NKey);
//8
DLLSPEC int _stdcall fw_authentication(int icdev,unsigned char _Mode,
									   unsigned char _SecNr);

DLLSPEC int _stdcall fw_authentication_pass(int icdev, unsigned char _Mode, unsigned char Addr,unsigned char *passbuff);
//9
DLLSPEC int _stdcall fw_read(int icdev,unsigned char _Adr,unsigned char *_Data);
//9.1
DLLSPEC __int16 __stdcall fw_read_hex(int icdev,unsigned char _Adr,unsigned char *_Data);
//10
DLLSPEC int _stdcall fw_write(int icdev,unsigned char _Adr,unsigned char *_Data);
//10.1
DLLSPEC __int16 __stdcall fw_write_hex(int icdev,unsigned char _Adr,unsigned char *_Data);
//11
DLLSPEC int _stdcall fw_halt(int icdev);
//12
DLLSPEC int _stdcall fw_des(unsigned char *key,unsigned char *sour,
				   unsigned char *dest,__int16 m);
//13
DLLSPEC int _stdcall fw_changeb3(int icdev,unsigned char _SecNr,
				unsigned char *_KeyA,unsigned char *_CtrlW,unsigned char _Bk,
				unsigned char *_KeyB);
//13.2
DLLSPEC int _stdcall fw_changeb3_hex(int icdev,unsigned char _SecNr,
				unsigned char *_KeyA_hex,unsigned char *_CtrlW_hex,unsigned char _Bk,
				unsigned char *_KeyB_hex);
//15
DLLSPEC int _stdcall fw_initval(int icdev,unsigned char _Adr,unsigned long _Value);
DLLSPEC int _stdcall fw_initval_dec(int icdev,unsigned char _Adr,double _Value);
//16
DLLSPEC int _stdcall fw_increment(int icdev,unsigned char _Adr,unsigned long _Value);
DLLSPEC int _stdcall fw_increment_dec(int icdev,unsigned char _Adr,double _Value);
//17
DLLSPEC int _stdcall fw_readval(int icdev,unsigned char _Adr,unsigned long *_Value);
DLLSPEC int _stdcall fw_readval_dec(int icdev,unsigned char _Adr,double *_Value);
//18
DLLSPEC int _stdcall fw_decrement(int icdev,unsigned char _Adr,unsigned long _Value);
DLLSPEC int _stdcall fw_decrement_dec(int icdev,unsigned char _Adr,double _Value);

//19
DLLSPEC int _stdcall fw_HL_authentication(int icdev,unsigned char reqmode,
										  unsigned long snr,unsigned char authmode,
										  unsigned char secnr);
//20
DLLSPEC int _stdcall fw_HL_read(int icdev,unsigned char _Mode,unsigned char _Adr,
			   unsigned long _Snr,unsigned char *_Data, unsigned long *_NSnr);
//21
DLLSPEC int _stdcall fw_HL_write(int icdev,unsigned char _Mode,unsigned char _Adr,
				unsigned long *_Snr,unsigned char *_Data);
//22
DLLSPEC int _stdcall fw_restore(int icdev,unsigned char _Adr);
//23
DLLSPEC int _stdcall fw_transfer(int icdev,unsigned char _Adr);
//24
DLLSPEC int _stdcall fw_authentication_2(int icdev,unsigned char _Mode,unsigned char KeyNr,
						unsigned char Adr);
//25
DLLSPEC int _stdcall fw_authentication_pass(int icdev, unsigned char _Mode, 
											unsigned char Addr,
						   unsigned char *passbuff);
//26
DLLSPEC int _stdcall fw_card_double(int icdev,unsigned char _Mode,unsigned char *_Snr);

//30
DLLSPEC __int16 __stdcall fw_config_card(HANDLE icdev,unsigned char flags);

//===============================================
/*---------- device Functions--------*/
//===============================================
//1
DLLSPEC int _stdcall fw_beep(int icdev,unsigned int _Msec);
//2
DLLSPEC int _stdcall fw_disp_mode(int icdev,unsigned char mode);
//2.1
DLLSPEC int _stdcall fw_ctl_mode(int icdev,unsigned char mode);
//3
DLLSPEC int _stdcall fw_lcd_dispstr(int icdev,char *digit);
//4
DLLSPEC int _stdcall fw_gettime(int icdev,unsigned char *time);
//5
DLLSPEC int _stdcall fw_getver(int icdev,unsigned char *buff);
//6
DLLSPEC int _stdcall fw_high_disp(int icdev,unsigned char offset,
								  unsigned char disp_len,unsigned char *disp_str);
//7
DLLSPEC int _stdcall fw_lcd_setbright(int icdev,unsigned char bright);
//8
DLLSPEC int _stdcall fw_settime(int icdev,unsigned char *time);
//9
DLLSPEC int _stdcall fw_srd_eeprom(int icdev,int offset,int length,
								   unsigned char *rec_buffer);
//10
DLLSPEC int _stdcall fw_swr_eeprom(int icdev,int offset,int length,
								   unsigned char* buffer);
//11
DLLSPEC __int16 _stdcall a_hex(unsigned char *a,unsigned char *hex,__int16 len);
//12
DLLSPEC void _stdcall hex_a(unsigned char *hex,unsigned char *a,__int16 len);
//13
DLLSPEC __int16 __stdcall fw_reset(HANDLE icdev,unsigned __int16 _Msec);

//clear Mifare Pro LCD 
DLLSPEC int _stdcall fw_lcd_dispclear(int icdev);
//14 LED 8
DLLSPEC int _stdcall fw_LED_disp8(int icdev,unsigned char strlen,
								  unsigned char* dispstr);


//===============================================
//         4442 Commond Functions
//===============================================
//1
DLLSPEC int _stdcall fw_read_4442(int icdev,unsigned char _Adr,unsigned char *_Data,
								  int length);
//2
DLLSPEC int _stdcall fw_write_4442(int icdev,unsigned char _Adr,unsigned char *_Data,
								   int length);
//3
DLLSPEC int _stdcall fw_getProtectData_4442(int icdev,unsigned char _Adr,
											unsigned char *_Data,int length);
//4
DLLSPEC int _stdcall fw_setProtectData_4442(int icdev,unsigned char _Adr,
											unsigned char *_Data,int length);
//5
DLLSPEC int _stdcall fw_authentikey_4442(int icdev,unsigned char _Adr,int rlen,
										 unsigned char *key);
//6
DLLSPEC int _stdcall fw_changkey_4442(int icdev,unsigned char _Adr,int rlen,
									  unsigned char *key);
//7
DLLSPEC int _stdcall fw_cntReadError_4442(int icdev,unsigned char *cntReadError);


//===============================================
//         4428 Commond Functions
//===============================================
//1
DLLSPEC int _stdcall fw_read_4428(int icdev,unsigned int _Adr,unsigned char *_Data,
								  int length);
//2
DLLSPEC int _stdcall fw_write_4428(int icdev,unsigned int _Adr,unsigned char *_Data,
								   int length);
//3
DLLSPEC int _stdcall fw_getProtectData_4428(int icdev,unsigned int _Adr,
											unsigned char *_Data,int length);
//4
DLLSPEC int _stdcall fw_setProtectData_4428(int icdev,unsigned int _Adr,
											unsigned char *_Data,int length);
//5
DLLSPEC int _stdcall fw_authentikey_4428(int icdev,unsigned char *key);
//6
DLLSPEC int _stdcall fw_changkey_4428(int icdev,unsigned char *key);
//7
DLLSPEC int _stdcall fw_cntReadError_4428(int icdev,unsigned char *cntReadError);


//===============================================
//           cup(SAM)  Functions
//===============================================
//1
DLLSPEC __int16 __stdcall fw_cpureset(HANDLE ICDev,unsigned char *rlen, unsigned char *rbuff);
//2
DLLSPEC __int16 __stdcall fw_setcpu(HANDLE ICDev,unsigned char SAMID);
//3
DLLSPEC __int16 __stdcall fw_cpuapdu(HANDLE ICDev,unsigned char slen,unsigned char * sbuff,
						   unsigned char *rlen,unsigned char * rbuff);
//4
DLLSPEC __int16  __stdcall fw_cpuapdusource(HANDLE ICDev,unsigned char slen,
								 unsigned char * sbuff,unsigned char *rlen,
								 unsigned char * rbuff);
//5
DLLSPEC __int16 __stdcall fw_setcpupara(HANDLE ICDev,unsigned char cputype,
							  unsigned char cpupro,unsigned char cpuetu);

DLLSPEC __int16 __stdcall fw_cpudown(int icdev,unsigned char cardtype);

//================================================
//S70 Functions
//S70 operation command most the same as M1
//================================================
//1
DLLSPEC int _stdcall fw_read_S70(int icdev,unsigned char _Adr,unsigned char *_Data);
//2
DLLSPEC int _stdcall fw_write_S70(int icdev,unsigned char _Adr,unsigned char *_Data);




//================================================
//     UltraLight card  Functions
//================================================
#define PAGE16_19	16
#define PAGE20_23	20
#define PAGE24_27	24
#define PAGE28_31	28
#define PAGE32_35	32
#define PAGE36_39	36
#define PAGE44_47	44
//1 request
DLLSPEC int _stdcall fw_request_ultralt(int icdev,unsigned char _Mode);
//2 anticall
DLLSPEC int _stdcall fw_anticall_ultralt(int icdev,unsigned long *_Snr);
//3 select
DLLSPEC int _stdcall fw_select_ultralt(int icdev,unsigned long _Snr);
//4 reqa
DLLSPEC int _stdcall fw_reqa_ultralt(int icdev,unsigned char _Bcnt,unsigned long _Snr);
//5 wake up
DLLSPEC int _stdcall fw_wakeup_ultralt(int icdev,unsigned char _Snr);
//4 read
DLLSPEC int _stdcall fw_read_ultralt(int icdev,unsigned char iPage,unsigned char *redata);
//5 write
DLLSPEC int _stdcall fw_write_ultralt(int icdev,unsigned char iPage,unsigned char *sdata);
//6 Halt
DLLSPEC int _stdcall fw_halt_ultralt(int icdev);

DLLSPEC int _stdcall fw_ultralt_C_authen(int icdev, unsigned char* key);
/* 设定安全页
 * 参数：
	icdev: 设备句柄 
	ipage: 页号，这里只针对 4 ~ 41 页
	readSec: 读权限，0：读不需验证密码，1：读需验证密码; 注：写都需验证密码
 * 返回：0 成功
 */
DLLSPEC int _stdcall fw_ultralt_C_setSafePage(int icdev, int ipage, BOOL readSec);
/* 修改密码
 * 参数：
	icdev: 设备句柄 
	keyold: 旧密码，16字节
	keynew: 新密码，16字节， 输入输出
 * 返回：0 成功
 */
DLLSPEC int _stdcall fw_ultralt_C_changePwd(int icdev, unsigned char* keyold,unsigned char* keynew);
/* 锁页，此过程无法逆转，对应的页上锁后将不能再重写，也无法解锁
 * 参数：
   icdev: 设备句柄
   flag: 页标识
         取值：
		 3~15 分别对应 3~15 页
		 41~43 分别对应41~43
		 PAGE16_19 (值为16)：对应页16~19
		 PAGE20_23 (值为20)：对应页20~23
		 PAGE24_27 (值为24)：对应页24~27
		 PAGE28_31 (值为28)：对应页28~31
		 PAGE32_35 (值为32)：对应页32~35
		 PAGE36_39 (值为36)：对应页36~39
		 PAGE44_47 (值为44)：对应页44~47
 * 返回：成功返回0 
 */
DLLSPEC int _stdcall fw_ultralt_C_lockPage(int icdev, int flag);



//===============================================
//       MifarePro card  Functions
//===============================================
DLLSPEC __int16 _stdcall fw_reset_mifarepro(int icdev,unsigned char *rlen, unsigned char *rbuff);
DLLSPEC __int16 _stdcall fw_setcpu_mifarepro(int icdev,unsigned char SAMID);
DLLSPEC __int16 _stdcall fw_apdu_mifarepro(int icdev,unsigned char slen,unsigned char * sbuff,
						   unsigned char *rlen,unsigned char * rbuff);
DLLSPEC __int16 _stdcall fw_setpara_mifarepro(int icdev,unsigned char cputype,unsigned char cpupro,
							  unsigned char cpuetu);

//===============================================
//       Icode2 card  Functions
//===============================================

DLLSPEC __int16 __stdcall fw_inventory(HANDLE icdev,unsigned char flags,
							   unsigned char AFI, 
							   unsigned char masklen, 
							   unsigned char *rlen,unsigned char *rbuffer);
DLLSPEC __int16 __stdcall fw_stay_quiet(HANDLE icdev,unsigned char flags,unsigned char *UID);
DLLSPEC __int16 __stdcall fw_select_uid(HANDLE icdev,unsigned char flags,unsigned char *UID);
DLLSPEC __int16 __stdcall fw_reset_to_ready(HANDLE icdev,unsigned char flags,unsigned char *UID);
DLLSPEC __int16 __stdcall fw_readblock(HANDLE icdev,unsigned char flags,
							   unsigned char startblock,unsigned char blocknum, 
							   unsigned char *UID, 
							   unsigned char *rlen,unsigned char *rbuffer);
DLLSPEC __int16 __stdcall fw_writeblock(HANDLE icdev,unsigned char flags,
								unsigned char startblock,unsigned char blocknum, 
								unsigned char *UID, 
								unsigned char wlen,unsigned char *rbuffer);
DLLSPEC __int16 __stdcall fw_lock_block(HANDLE icdev,unsigned char flags,unsigned char block,
								unsigned char *UID);
DLLSPEC __int16 __stdcall fw_write_afi(HANDLE icdev,unsigned char flags,unsigned char AFI,
							   unsigned char *UID);
DLLSPEC __int16 __stdcall fw_lock_afi(HANDLE icdev,unsigned char flags,unsigned char AFI,
							  unsigned char *UID);
DLLSPEC __int16 __stdcall fw_write_dsfid(HANDLE icdev,unsigned char flags,unsigned char DSFID,
								 unsigned char *UID);
DLLSPEC __int16 __stdcall fw_lock_dsfid(HANDLE icdev,unsigned char flags,unsigned char DSFID,
								unsigned char *UID);
DLLSPEC __int16 __stdcall fw_get_systeminfo(HANDLE icdev,unsigned char flags,
									unsigned char *UID, 
									unsigned char *rlen,unsigned char *rbuffer);
DLLSPEC __int16 __stdcall fw_get_securityinfo(HANDLE icdev,unsigned char flags,
									  unsigned char startblock,unsigned char blocknum, 
									  unsigned char *UID, 
									  unsigned char *rlen,unsigned char *rbuffer);


//===============================================
//       typeB at88rf020  card  Functions
//===============================================

DLLSPEC __int16 __stdcall fw_request_b(HANDLE icdev,unsigned char _Mode,unsigned char AFI, 
		                       unsigned char N,unsigned char *ATQB);
DLLSPEC __int16 __stdcall fw_attrib(HANDLE icdev,unsigned char *PUPI, unsigned char CID);
DLLSPEC __int16 __stdcall fw_check_at(HANDLE icdev,unsigned char cid,unsigned char *key);
DLLSPEC __int16 __stdcall fw_read_at(HANDLE icdev,unsigned char Adr,unsigned char*key,
							 unsigned char* rbuffer);
DLLSPEC __int16 __stdcall fw_write_at(HANDLE icdev,unsigned char Adr,unsigned char* sbuffer);
DLLSPEC __int16 __stdcall fw_lock_at(HANDLE icdev,unsigned char Adr,unsigned char*sbuffer);

//////////////////////////////////////////////////////////////////////////
//功能：命令传输，类似CPU卡
//  参数：
// icdev  (输入):    设备句柄
// sBuffer(输入):    发送给卡片的命令信息
// sLen   (输入):    命令信息的长度
// rBuffer(输出):    命令返回的信息
// rLen   (输出):    命令返回信息的长度
//返回：0成功，非0失败
///////////////////////////////////////////////////////////////////////////
DLLSPEC __int16 __stdcall fw_CommandLink_at(HANDLE icdev,
										 unsigned char* sBuffer,
										 DWORD sLen,
										 unsigned char* rBuffer,
										 PDWORD rLen);

//取消选择卡
DLLSPEC __int16 __stdcall fw_halt_at(HANDLE icdev,unsigned char cid,unsigned char *key);

DLLSPEC __int16 __stdcall fw_count_at(HANDLE icdev,unsigned char cid,unsigned char* key);

//===============================================
//       ISO14443-4 卡
//===============================================

//1
DLLSPEC __int16 __stdcall
fw_pro_reset
(
 int ICDev,//端口标识符
 unsigned char *rlen,//返回复位信息的长度
 unsigned char * rbuff//存放返回的复位信息
 );

//2
DLLSPEC __int16 __stdcall
fw_pro_commandlink
(
 int ICDev,//端口标识符
 unsigned char slen,//发送的信息长度
 unsigned char * sbuff,//存放要发送的信息
 unsigned char *rlen,//返回信息的长度
 unsigned char * rbuff,//存放返回的信息
 unsigned char tt,//延迟时间，单位为：10ms
 unsigned char FG//分割长度。建议此值小于64
 );

//===============================================
//       ISO14443 desfire  card  Functions
//===============================================

DLLSPEC int _stdcall fw_anticoll2(int icdev,unsigned char _Bcnt,unsigned long *_Snr);
DLLSPEC int _stdcall fw_select2(int icdev,unsigned long _Snr);
DLLSPEC int __stdcall fw_reset_desfire(int icdev,unsigned char *rlen,unsigned char*rdata);
DLLSPEC int __stdcall fw_apdu_desfire(int icdev,unsigned char slen,unsigned char*sdata,
									  unsigned char *rlen,unsigned char*rdata);
//Applications 
DLLSPEC int __stdcall fw_authen_desfire(int icdev,unsigned char keyNo, char* 
										key,unsigned char* sessionKey);
DLLSPEC int __stdcall fw_authen_desfire_aes(int icdev,unsigned char keyNo, char* 
										key,unsigned char* sessionKey);
DLLSPEC int __stdcall fw_getver_desfire(int icdev,unsigned char* rlen,unsigned char* version);
DLLSPEC int __stdcall fw_getAIDs_desfire(int icdev,unsigned char* rlen,unsigned char* AIDS);
DLLSPEC int __stdcall fw_selectApp_desfire(int icdev,unsigned char* AID);
DLLSPEC int __stdcall fw_getKeySetting_desfire(int icdev,unsigned char* rlen,unsigned char*setbuf);
DLLSPEC int __stdcall fw_getKeyver_desfire(int icdev,unsigned char keyNo,unsigned char* keyVer);
DLLSPEC int __stdcall fw_createApp_desfire(int icdev,unsigned char*AID,unsigned char KeySetting,
										   unsigned char NumOfKey);
DLLSPEC int __stdcall fw_delAID_desfire(int icdev,unsigned char* AID);
DLLSPEC int __stdcall fw_changeKeySetting_desfire(int icdev,unsigned char newSet,char* sessionKey);
DLLSPEC int __stdcall fw_changeKey_desfire(int icdev,unsigned char* sessionKey,unsigned char* curKey,
										   unsigned char keyNo,unsigned char* newkey);
DLLSPEC int __stdcall fw_changeKey_desfire_aes(int icdev,unsigned char* sessionKey,unsigned char* curKey,
										   unsigned char keyNo,unsigned char* newkey,unsigned char keyVersion);
DLLSPEC int __stdcall fw_getFileIDs_desfire(int icdev,unsigned char* rlen,unsigned char* fileIDs);
DLLSPEC int __stdcall fw_getFileProper(int icdev,unsigned char fileNo,unsigned char* rlen,
									   unsigned char * fileProper);
DLLSPEC int __stdcall fw_changeFileSetting(int icdev,unsigned char fileNo,unsigned char comSet,
										   unsigned char* accessRight,char* sessionKey);
DLLSPEC int __stdcall fw_createDataFile_desfire(int icdev,unsigned char fileNo,unsigned char ComSet,
												unsigned char* AccessRight,unsigned char* FileSize);
DLLSPEC int __stdcall fw_createBackupDataFile_desfire(int icdev,unsigned char fileNo,unsigned char ComSet,
												unsigned char* AccessRight,unsigned char* FileSize);
DLLSPEC int __stdcall fw_createValueFile_desfire(int icdev,unsigned char fileNo,unsigned char ComSet,
												 unsigned char* AccessRight,unsigned char*
												 lowerLimit,unsigned char* upperLimit,unsigned
												 char* value,unsigned char creditEnabled);
DLLSPEC int __stdcall fw_createCsyRecord_desfire(int icdev,unsigned char fileNo,unsigned char comSet,
												 unsigned char* AccessRight,unsigned char* RecordSize,
												 unsigned char* MaxNum);
DLLSPEC int __stdcall fw_delFile_desfire(int icdev,unsigned char fileNo);
DLLSPEC int __stdcall fw_write_desfire(int icdev,unsigned char fileNo,unsigned int offset,unsigned int length,
									   unsigned char* data,char*sessionKey);
DLLSPEC int __stdcall fw_read_desfire(int icdev,unsigned char fileNo,unsigned int offset,unsigned int length,
									  unsigned char* revData,char*sessionKey);
DLLSPEC int __stdcall fw_getvalue_desfire(int icdev,unsigned char fileNo,
										  unsigned int* value,char* sessionKey);
DLLSPEC int __stdcall fw_credit_desfire(int icdev,unsigned char fileNo,
										unsigned int value,char*sessionKey);
DLLSPEC int __stdcall fw_debit_desfire(int icdev,unsigned char fileNo,
									   unsigned int value,char*sessionKey);
DLLSPEC int __stdcall fw_writeRecord_desfire(int icdev,unsigned char fileNo,
											 unsigned int offset,unsigned int length,
											 unsigned char* data,char*sessionKey);
DLLSPEC int __stdcall fw_readRecord_desfire(int icdev,unsigned char fileNo,
											unsigned int offset,unsigned int length,
											unsigned char* revData,unsigned int* SgRecordlen,
											unsigned int*rlen,char*sessionKey);
DLLSPEC int __stdcall fw_clearRecord_desfire(int icdev,unsigned char fileNo);
DLLSPEC int __stdcall fw_commitTransfer_desfire(int icdev);
DLLSPEC int __stdcall fw_abortTransfer_desfire(int icdev);
DLLSPEC int __stdcall fw_formatPICC_desfire(int icdev);

//===============================================
//         24C Commond Functions
//===============================================
DLLSPEC int __stdcall fw_read_24c64(int icdev,unsigned int offset,unsigned int length,
									unsigned char* rdata);
DLLSPEC int __stdcall fw_write_24c64(int icdev,unsigned int offset,unsigned int length,
									 unsigned char* wdata);
DLLSPEC int _stdcall  fw_check_24c64(int icdev);

//===============================================
/*----------ID card Functions--------*/
//===============================================

DLLSPEC int _stdcall fw_read_SerialNumberID(int icdev,
											unsigned int _Msec,
											unsigned char* snID);


//===============================================
//         Mifare Plus  Commond Functions
//===============================================
DLLSPEC int _stdcall fw_MFPlusL0_WritePerso(int ihic, unsigned char* key, unsigned short keyAddr);
DLLSPEC int _stdcall fw_MFPlusL0_CommitPerso(int ihic);
DLLSPEC int _stdcall fw_MFPlusL1_AuthenKeyL1(int ihic,unsigned char* key);
DLLSPEC int _stdcall fw_MFPlusL1_SwitchToL2(int ihic,unsigned char* key);
DLLSPEC int _stdcall fw_MFPlusL1_SwitchToL3(int ihic,unsigned char* key);
DLLSPEC int _stdcall fw_MFPlusL2_SwitchToL3(int ihic,unsigned char* key);
DLLSPEC	int _stdcall fw_MFPlusL3_AuthenL3Key(int ihic,unsigned char* key, unsigned short keyAddr);
DLLSPEC int _stdcall fw_MFPlusL3_AuthenSectorKey(int ihic,unsigned char keyType,unsigned int sectorNo,unsigned char* key);
DLLSPEC int _stdcall fw_MFPlusL3_UpdateKey(int ihic, unsigned int keyAddr,unsigned char* newKey);
DLLSPEC int _stdcall fw_MFPlusL3_ReadWithEncrypt(int ihic, unsigned int blkAddr,unsigned char blkNum,
												unsigned char* rdata,unsigned char flag);
DLLSPEC int _stdcall fw_MFPlusL3_WriteWithEncrypt(int ihic, unsigned int blkAddr,unsigned char blkNum,
											  unsigned char* wdata,unsigned char flag);
DLLSPEC int _stdcall fw_MFPlusL3_ReadWithPlain(int ihic, unsigned int blkAddr,unsigned char blkNum,
											  unsigned char* rdata);
DLLSPEC int _stdcall fw_MFPlusL3_WriteWithPlain(int ihic, unsigned int blkAddr,unsigned char blkNum,
											  unsigned char* wdata);
DLLSPEC int _stdcall fw_MFPlusL3_InitVal(int ihic, unsigned int blkAddr,unsigned long value);
DLLSPEC int _stdcall fw_MFPlusL3_ReadVal(int ihic, unsigned int blkAddr,unsigned long* value);
DLLSPEC int _stdcall fw_MFPlusL3_Increment(int ihic, unsigned int blkAddr,unsigned long value);
DLLSPEC int _stdcall fw_MFPlusL3_Decrement(int ihic, unsigned int blkAddr,unsigned long value);

#endif
