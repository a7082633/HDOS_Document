#ifndef SSSE32_H
#define SSSE32_H
#include <windows.h>


//状态码
#define IFD_OK	0					 //执行成功
#define IFD_ICC_TypeError	-1		 //卡片类型不对
#define IFD_ICC_NoExist	-2			 //无卡
#define IFD_ICC_NoPower	-3			 //有卡未上电
#define IFD_ICC_NoResponse	-4	     //卡片无应答
#define IFD_ConnectError	-11	     //读卡器连接错
#define IFD_UnConnected	-12	         //未建立连接(没有执行打开设备函数)
#define IFD_BadCommand	-13	         //(动态库)不支持该命令
#define IFD_ParameterError	-14	     //(发给动态库的)命令参数错
#define IFD_CheckSumError	-15	     //信息校验和出错



#ifdef __cplusplus
extern "C" {
#endif


long WINAPI ICC_Reader_Open(char* dev_Name); 
	
long WINAPI ICC_Reader_Close(long ReaderHandle);

long WINAPI ICC_Reader_pre_PowerOn(long ReaderHandle,unsigned char ICC_Slot_No,unsigned char* Response);//上电 冷 返回数据长度
long WINAPI ICC_Reader_PowerOn(long ReaderHandle,unsigned char ICC_Slot_No,unsigned char* Response);//上电 热 返回数据长度
/*
	大卡座ICC_Slot_No：0x01
	SAM1:0x11
	SAM2:0x12
	SAM3:0x13
	SAM4:0x14
	下同

*/
long WINAPI ICC_Reader_PowerOff(long ReaderHandle,unsigned char ICC_Slot_No);

long WINAPI ICC_Reader_GetStatus(long ReaderHandle,unsigned char ICC_Slot_No);//获取卡座状态

long WINAPI ICC_Reader_Application(long ReaderHandle,unsigned char ICC_Slot_No,
							 long Lenth_of_Command_APDU,unsigned char* Command_APDU,
							 unsigned char* Response_APDU);  //执行apdu命令  返回数据长度

//long WINAPI ICC_Reader_Libinfo(char* info);

long WINAPI ICC_PosBeep(long ReaderHandle, unsigned char time);//蜂鸣
//如果读取9600的卡则cpuetu应为96     如果读取38400的卡则cpuetu应为20
long WINAPI ICC_SetCpupara(long ReaderHandle,unsigned char ICC_Slot_No,unsigned char cpupro,unsigned char cpuetu);//设置速率

long WINAPI Rcard(long ReaderHandle,unsigned char ctime,int track,unsigned char *rlen,char *getdata);//读磁条卡


long WINAPI ICC_DispInfo(long ReaderHandle,unsigned char row,unsigned char list,char *data); //显示信息

long WINAPI ICC_DispSound(long ReaderHandle,unsigned char type,unsigned char nMode); //声音提示信息 type(1~14) nMode==0内置键盘语音，nMode==1外置键盘语音
/*
	1、请插卡 2、请刷卡 3、读卡错误 4、请输入密码  5、密码错误  6、操作成功 7、操作超时 8、操作失败 9、请取回卡 10、请重新输入密码 11、请再次输入密码 12、请输入新密码 13、请输入旧密码 14、请确认新密码
*/

long WINAPI ICC_CtlLight(long ReaderHandle);//清屏，关屏

long WINAPI ICC_ChangeLight(long ReaderHandle,unsigned char mode);//改变屏幕亮度 mode=1 调亮  mode=0调暗

long WINAPI ICC_GetInputPass(long ReaderHandle, unsigned char ctime, unsigned char *rlen, unsigned char start,unsigned char stop,unsigned char * cpass);//获取密码，带超时 start==0用内置键盘，start==1用外置键盘

long WINAPI ICC_GetInputPassID(long ReaderHandle, unsigned char ctime, unsigned char *rlen, unsigned char start,unsigned char stop,unsigned char * cardID,unsigned char * cpass);//获取密码，带超时,带ID

long WINAPI ICC_GetTimeOut(long ReaderHandle,unsigned char flag);//获取密码、读磁条超时


long WINAPI ICC_PassGet(long ReaderHandle,unsigned char *rlen,unsigned char time,unsigned char start,unsigned char stop,unsigned char * cpass);//获取密码

long WINAPI ICC_PassGetID(long ReaderHandle,unsigned char *rlen,unsigned char time,unsigned char start,unsigned char stop,unsigned char * cardID,unsigned char * cpass);//获取密码，带卡号

long WINAPI ICC_PassCheck(long ReaderHandle,unsigned char *rlen,unsigned char * cpass); //查下数据是否到了

long WINAPI ICC_PassSetDES(long ReaderHandle,unsigned char DES); //设置加密模式3des or des

long WINAPI ICC_PassSetDevID(long ReaderHandle,unsigned char *devID);//设置键盘设备ID

long WINAPI ICC_PassGetDevID(long ReaderHandle,unsigned char *devID);//获取键盘设备ID

long WINAPI ICC_PassSetKeyLen(long ReaderHandle,unsigned char len);//设置密码长度

long WINAPI ICC_PassDes(long ReaderHandle,unsigned char flag,unsigned char *InData,unsigned char *OutData); //对数据加解密 flag 0 加密 1 解密

long WINAPI ICC_PassSetFD(long ReaderHandle,unsigned char fd);//密码键盘设置波特率

long WINAPI ICC_GetInputPassACOUNT(long ReaderHandle, unsigned char ctime, unsigned char *rlen,unsigned char * cardID,unsigned char * cpass);//获取密码，带超时,带16 ID

long WINAPI ICC_PassGetACOUNT(long ReaderHandle,unsigned char *rlen,unsigned char time,unsigned char * cardID,unsigned char * cpass);//获取密码，带16卡号

long WINAPI ICC_PassMAC(long ReaderHandle,unsigned char *InData,unsigned char *OutData); //对数据算MAC flag 0 未结束 1 结束

long WINAPI ICC_PassECB(long ReaderHandle,unsigned char *rlen,unsigned char *InData,unsigned char *OutData); //对数据算ECB 

long WINAPI ICC_PassChangeMkey(long ReaderHandle,unsigned char MkeyId ,unsigned char *Newmkey); //修改主密钥 

long WINAPI ICC_PassChangeWkey(long ReaderHandle,unsigned char MkeyId ,unsigned char Wkeyid,unsigned char *Newmkey); //修改工作密钥 

long WINAPI ICC_PassActiveWkey(long ReaderHandle,unsigned char MkeyId ,unsigned char Wkeyid); //激活工作密钥 

/*############ 4442基本操作函数#################*/

long WINAPI ICC_Reader_4442_PowerOn(long ReaderHandle,unsigned char *data); //4442卡上电
long WINAPI ICC_Reader_4442_PowerOff(long ReaderHandle); //4442卡下电
long WINAPI ICC_Reader_4442_Read(long ReaderHandle,int offset,int len,unsigned char *data); //4442卡读数据
long WINAPI ICC_Reader_4442_Write(long ReaderHandle,int offset,int len,unsigned char *data); //4442卡写数据 
long WINAPI ICC_Reader_4442_Verify(long ReaderHandle,unsigned char *key); //4442卡认证密钥
long WINAPI ICC_Reader_4442_Change(long ReaderHandle,unsigned char *newkey); //4442卡修改密钥
long WINAPI ICC_Reader_4442_ReadProtect(long ReaderHandle,int offset,int len,unsigned char *data); //4442卡读保护数据
long WINAPI ICC_Reader_4442_WriteProtect(long ReaderHandle,int offset,int len,unsigned char *data); //4442卡写保护数据
long WINAPI ICC_Reader_4442_ReadCount(long ReaderHandle); //读取4442卡密钥剩余认证次数 返回次数0~3次

/*############ 24cXX基本操作函数#################*/

long WINAPI ICC_Reader_24c_PowerOn(long ReaderHandle,unsigned char *data,unsigned char flag); //24cxx卡上电 flag=0为24C01~16  flag=1为24C32~1024
long WINAPI ICC_Reader_24c_PowerOff(long ReaderHandle,unsigned char flag); //24cxx卡下电 flag=0为24C01~16  flag=1为24C32~1024
long WINAPI ICC_Reader_24c_Read(long ReaderHandle,int offset,int len,unsigned char *data,unsigned char flag); //24cxx卡读数据 flag=0为24C01~16  flag=1为24C32~1024
long WINAPI ICC_Reader_24c_Write(long ReaderHandle,int offset,int len,unsigned char *data,unsigned char flag); //24cxx卡写数据  flag=0为24C01~16  flag=1为24C32~1024

/*############ 4428 基本操作函数#################*/

long WINAPI ICC_Reader_4428_PowerOn(long ReaderHandle,unsigned char *data); //4428卡上电
long WINAPI ICC_Reader_4428_PowerOff(long ReaderHandle); //4428卡下电
long WINAPI ICC_Reader_4428_Read(long ReaderHandle,int offset,int len,unsigned char *data); //4428卡读数据
long WINAPI ICC_Reader_4428_Write(long ReaderHandle,int offset,int len,unsigned char *data); //4428卡写数据 
long WINAPI ICC_Reader_4428_Verify(long ReaderHandle,unsigned char *key); //4428卡认证密钥
long WINAPI ICC_Reader_4428_Change(long ReaderHandle,unsigned char *newkey); //4428卡修改密钥
long WINAPI ICC_Reader_4428_ReadProtect(long ReaderHandle,int offset,int len,unsigned char *data); //4428卡读保护数据
long WINAPI ICC_Reader_4428_WriteProtect(long ReaderHandle,int offset,int len,unsigned char *data); //4428卡写保护数据
long WINAPI ICC_Reader_4428_ReadCount(long ReaderHandle); //读取4428卡密钥剩余认证次数 返回次数0~8次

/*############非接基本操作函数#################*/

long WINAPI PICC_Reader_SetTypeA(long ReaderHandle);//设置读typeA

long WINAPI PICC_Reader_SetTypeB(long ReaderHandle);//设置读typeB

long WINAPI PICC_Reader_Select(long ReaderHandle,unsigned char cardtype);//选择卡片，41为typea,M1 42为typeb,TypeB卡片需先上电后选卡

long WINAPI PICC_Reader_Request(long ReaderHandle);//typea M1请求卡片

long WINAPI PICC_Reader_RFControl(long ReaderHandle);//关闭天线

long WINAPI PICC_Reader_anticoll(long ReaderHandle,unsigned char *uid);//防碰撞 typea M1卡片

/*############ M1卡操作函数#################*/
long WINAPI PICC_Reader_Authentication(long ReaderHandle,unsigned char Mode, unsigned char SecNr);//认证密钥 M1

long WINAPI PICC_Reader_Authentication_Pass(long ReaderHandle,unsigned char Mode, unsigned char SecNr,unsigned char *PassWord);//认证密钥M1 带密钥参数

long WINAPI PICC_Reader_Readval(long ReaderHandle,unsigned char Addr,unsigned long *value);	//读值

long WINAPI PICC_Reader_Initval(long ReaderHandle,unsigned char Addr,unsigned long value);//初始化值

long WINAPI PICC_Reader_Increment(long ReaderHandle,unsigned char Addr,unsigned long value);	//增值

long WINAPI PICC_Reader_Decrement(long ReaderHandle,unsigned char Addr,unsigned long value);//减值

long WINAPI PICC_Reader_Read(long ReaderHandle,unsigned char Addr,unsigned char *Data);	//读取卡片M1

long WINAPI PICC_Reader_Write(long ReaderHandle,unsigned char Addr,unsigned char *Data);//写入数据M1

long WINAPI PICC_Reader_LoadKey(long ReaderHandle,unsigned char Mode,unsigned char SecNr,unsigned char *Key);//装载密钥
/*
SecNr:扇区号
Mode：00 01 02 分别为第一、二、三套密钥KeyA
04 05 06 分别为第一、二、三套密钥KEYB
*/


/*############ TypeA CPU 卡操作函数 #################*/
long WINAPI PICC_Reader_PowerOnTypeA(long ReaderHandle,unsigned char* Response);//上电 返回数据长度 失败小于0

/*############ TypeB CPU 卡操作函数#################*/
long WINAPI PICC_Reader_PowerOnTypeB(long ReaderHandle,unsigned char* Response);//上电 返回数据长度 失败小于0



long WINAPI PICC_Reader_Application(long ReaderHandle,
							 long Lenth_of_Command_APDU,unsigned char* Command_APDU,
							 unsigned char* Response_APDU);  //type a/b执行apdu命令 返回数据长度 失败小于0

/*身份证读写函数*/
long WINAPI PICC_Reader_ID_Request(long ReaderHandle);//身份证寻卡
long WINAPI PICC_Reader_ID_Select(long ReaderHandle);//身份证选卡
long WINAPI PICC_Reader_ID_Read(long ReaderHandle,unsigned char* Response);//身份证读卡
long WINAPI Run(char* flag,char*data,char*err);

//*############## 102 卡#########################



#ifdef __cplusplus
}
#endif

#endif