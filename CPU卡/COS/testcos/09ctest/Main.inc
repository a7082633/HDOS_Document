R0_Bank0                        equ     00h
R1_Bank0                        equ     01h
R2_Bank0                        equ     02h
R3_Bank0                        equ     03h
R4_Bank0                        equ     04h
R5_Bank0                        equ     05h
R6_Bank0                        equ     06h
R7_Bank0                        equ     07h

R0_Bank1                        equ     08h
R1_Bank1                        equ     09h
R2_Bank1                        equ     0Ah
R3_Bank1                        equ     0Bh
R4_Bank1                        equ     0Ch
R5_Bank1                        equ     0Dh
R6_Bank1                        equ     0Eh
R7_Bank1                        equ     0Fh

R0_Bank2                        equ     10h
R1_Bank2                        equ     11h
R2_Bank2                        equ     12h
R3_Bank2                        equ     13h
R4_Bank2                        equ     14h
R5_Bank2                        equ     15h
R6_Bank2                        equ     16h
R7_Bank2                        equ     17h

R0_Bank3                        equ     18h
R1_Bank3                        equ     19h
R2_Bank3                        equ     1Ah
R3_Bank3                        equ     1Bh
R4_Bank3                        equ     1Ch
R5_Bank3                        equ     1Dh
R6_Bank3                        equ     1Eh
R7_Bank3                        equ     1Fh

Edit_0                          equ     00h
Edit_1                          equ     01h
Edit_2                          equ     02h
Edit_3                          equ     03h
Edit_4                          equ     04h
Edit_5                          equ     05h
Edit_6                          equ     06h
Edit_7                          equ     07h
Edit_8                          equ     08h
Edit_9                          equ     09h
Edit_A                          equ     0Ah
Edit_B                          equ     0Bh
Edit_C                          equ     0Ch
Edit_D                          equ     0Dh
Edit_E                          equ     0Eh
Edit_F                          equ     0Fh
;;******************************************************

DIR_MF_Index                    equ     00h
DIR_Telecom_Index               equ     01h
DIR_GSM_Index                   equ     02h
DIR_CDMA_Index                  equ     03h

XTK_Path_Length                 equ     04h
Page_Disp_Number                equ     0bh
Page_Write_Size                 equ     20h
Page_Size                       equ     40h
PTSS                            equ     0ffh

Next_Page_Item                  equ     0FEh
Last_Page_Item                  equ     0FFh

GO_BACK                         equ     Last_Page_Item
GO_AHEAD                        equ     Next_Page_Item
;;******************************************************

DES_Encrypt_Flag                equ     00h
DES_Decrypt_Flag                equ     80h

SDES_Flag                       equ     00h
TDES_Flag                       equ     40h

DES_First_Flag                  equ     00h
DES_Not_First_Flag              equ     01h

None                            equ     00000000b
Have_Lc                         equ     00000001b
Have_Write_Eeprom               equ     00000010b
Have_CHV1_Limits                equ     00000100b
Have_CHV2_Limits                equ     00001000b
Have_ADM_Limits                 equ     00010000b
Have_Super_Key_Limits           equ     00100000b


CRC_accum_high                  equ     R6_Bank0
CRC_accum_low                   equ     R7_Bank0

VIP_F                           EQU     08H
F_FFS                           EQU     0AH
FFS_FAT                         EQU     0CH                             ;Number in FFS_FAT MUST be at bouadary od EEPROM_PAGE
FAT_CLUSTER                     EQU     0EH                             ;Number in FAT_CLUSTER MUST be at bouadary od EEPROM_PAGE

VIP_F_H                         equ     VIP_F+00h
VIP_F_L                         equ     VIP_F+01h

F_FFS_H                         equ     F_FFS+00h
F_FFS_L                         equ     F_FFS+01h

FFS_FAT_H                       equ     FFS_FAT+00h
FFS_FAT_L                       equ     FFS_FAT+01h

FAT_CLUSTER_H                   equ     FAT_CLUSTER+00h
FAT_CLUSTER_L                   equ     FAT_CLUSTER+01h


DIR                             EQU     10H
FILE                            EQU     11H
TYPE                            EQU     12H
OFFSET                          EQU     13H
;OFFSET+1                       EQU     14H


byDir                           equ     DIR                             ; current directory
byCurrent_File                  equ     FILE                            ; Current file.
byFile_Type                     equ     TYPE                            ; Type file BIT7=1/0: FAT/FIXED file
offset_high                     equ     OFFSET                          ; Save eeprom offset of current file
offset_low                      equ     OFFSET+01h


BDIR                            EQU     15h                             ;The following bytes are for Backups when do FSELECT().
BFILE                           EQU     16H
BTYPE                           EQU     17H
BOFFSET                         EQU     18H
;BOFFSET+1                      EQU     19H

byINS_Bak                       equ     1Ah
byCLA                           equ     1BH
byINS                           equ     1CH
byP1                            equ     1DH
byP2                            equ     1EH
byP3                            equ     1FH
Le_Length                       equ     byCLA

SW1                             equ     20h                             ; 应答信息第一个字节, Can be use temp
SW2                             equ     21h                             ; 应答信息第二个字节, Can be use temp

SUPER_BEGIN                     equ     22h
SUPER_BEGIN_H                   equ     SUPER_BEGIN+00h
SUPER_BEGIN_L                   equ     SUPER_BEGIN+01h

SUPER_END                       equ     24h
SUPER_END_H                     equ     SUPER_END+00h
SUPER_END_L                     equ     SUPER_END+01h

OTA_dir_Item            	equ     26h
OTA_Menu_Item                   equ     27h

Key_Bit                         equ     28h
bCHV1_Verify                    bit     Key_Bit.0                       ; CHV1 验证标志
bCHV2_Verify                    bit     Key_Bit.1                       ; CHV2 验证标志
bADM_Verify                     bit     Key_Bit.2                       ; ADM  验证标志
bSuper_Key                      bit     Key_Bit.3
bUIM                            bit     Key_Bit.4                       ; =0 For SIM , =1 For UIM
bEnhance_Speed                  bit     Key_Bit.5
bCOS_use_self                   bit     Key_Bit.6                       ; =0 Use normal, =1 Use by COS self
bHigh_Priority_Thread           bit     Key_Bit.7

bBase_Challenge                 Bit     29h.0
bCommit_SSD                     Bit     29h.1
bUpdate_SSD                     Bit     29h.2
bRun_CAVE                       Bit     29h.3

bNot_Support_Default_Text       bit     29h.4
bNot_Support_UCS2_Get_Input     bit     29h.5                           ; FOR 降级显示
bIn_Code                        bit     29h.6
bLow_half_byte                  bit     29h.7

bNot_Send_Menu_Setup            bit     2Ah.0

bCheck_by_alpha                 bit     2Ah.1
bFind_ascii_unicode_bit         bit     2Ah.2
bNew_SMS                        bit     2Ah.3
bShow_Item_Alpha                bit     2Ah.4
bAlreadySaveTerminalResp	bit	2Ah.5

Temp_bit                        equ     2Eh

XTK_Bit                         equ     2Fh
bFetch                          bit     XTK_Bit.0
bNot_INC_PC                     bit     XTK_Bit.1
bDisplay_Txt                    bit     XTK_Bit.2                       ; XTK Command End
bWant_UCS2_Input                bit     XTK_Bit.3
bRetry_XTK_INS                  bit     XTK_Bit.4
bBackward                  	bit     XTK_Bit.5
bOTA				BIT	XTK_Bit.6
bWait_Terminal_Response         bit     XTK_Bit.7

XTK_PATH1                       equ     30H                             ; 控制字节编号的第一个字节
XTK_PATH2                       equ     31H                             ; 控制字节编号的第二个字节
XTK_PATH3                       equ     32H                             ; 控制字节编号的第三个字节
XTK_PATH4                       equ     33H                             ; 控制字节编号的第三个字节
XTK_INS_PC                      equ     34H                             ; 菜单命令索引号
XTK_sw2                         equ     35h                             ; UTK return Data length
byFetch_INS                     equ     36h
byEDIT_INDEX                    equ     37H                             ; 编辑区索引号
byDisplay_Index                 equ     38h

byEdit_index_Menu		EQU	39h

Select_SMS_Number               equ     3Ch
Current_Send_SMS                equ     3Dh

Page_Address_high               equ     3Eh
Page_Address_low                equ     3Fh
SMS_page_address                equ     Page_Address_high
DCS_byte                        equ     Page_Address_low

Page_Count                      equ     40h                             ; 页显示计数器
Use_Select_Item                 equ     41h

OTA_FAT_HANDLEH			EQU	43H
OTA_FAT_HANDLEL			EQU	44H
OTA_DIR_NUMBER			EQU	45H

total_page_num			equ	4FH
Temp_byte                       equ     50h

STACK                           equ     0D0h                            ; 48 byte Stack can be use

Save_AUTHBS_buf                 equ     Save_RANDBS_buf-02h-04h-03h
Save_RANDBS_buf                 equ     STACK-02h-04h
Write_Enable_2                  equ     STACK-02h
Write_Enable_3                  equ     STACK-01h


Write_Enable_4                  equ     100h-01h
;;******************************************************
$if Simulator                           <> 0
Send_Receive_SRF                equ     0f9h
TI_SRF                          bit     0f9h
RI_SRF                          bit     0f8h

$else
Send_Receive_SRF                equ     099h
TI_SRF                          bit     099h
RI_SRF                          bit     098h

$endif

MODE_REG                        equ     9Ah
Baud_MR                         equ     9Bh
Baud_BR                         equ     9CH

RNG                             equ     0A1h
;;******************************************************

Dir_Start_Address               XData   0000h                           ; 目录起始地址
Fix_File_Start_Address          XData   0002h                           ; 标准文件、固定文件的起始地址

Bak_Control_Start_Address	XData	0004h
Bak_Data_Start_Address		XData	0006h
Bak_Data_End_Address		XData	0008h
Sector_64_Start_End_Address	XData	000Ah
Sector_128_Start_End_Address	XData	000Ch
Sector_256_Start_End_Address	XData	000Eh
Sector_512_Start_End_Address	XData	0010h

ADM_Address                     XData   0012h                           ; ADM
ADM_Number                      XData   001Ah                           ; ADM 剩余尝试次数

Cluster_FAT_Start_Address       XData   001Bh	; 簇管理的文件分配表起始地址
Cluster_FAT_End_Address		XData	001Dh   ; 簇管理的文件分配表结束地址
CLuster_Start_Address           XData   001Fh	; 簇管理的起始地址
CLuster_End_Address		XData	0021h	; 簇管理的结束地址

;********** added by yeq for flash ****************

Data_Recover_Tag_Address	XData	0023h	; 8 bytes
Bak_Control_Index		XData	0027h	

;********** added by yeq for flash end *************

;Key_Future_Address              XData   0011h

Key_Future_Address              XData   0030h

bySend_Receive_buf              XData   0FC00h                          ; 256 Byte Send Receive Buf
byXTK_Menu_buf                  XData   0FD00h                          ; 256 Byte XTK Menu Buf
byTemp_start                    XData   0FD00h                          ; 256 Temp Var Buf

byCard_Buf_start                XData   0FE00h                          ; 256 Temp Var Buf

byMem_Manage_start              XData   0FF00h                          ; 32 Byte Men Manager Block
byXTK_Thread_Para_Bak           XData   0FF20h                          ; 16 Byte
byXTK_Get_Input_buf             XData   0FF30h                          ; 208 Byte
INT_Vector                      XData   0FFF0h                          ; 16 Byte Int Vector
;;******************************************************

MF				equ	3F00h
Telecom_DF                      equ     7F10h
GSM_DF                          equ     7F20h
CDMA_DF                         equ     7F25h

XTK_Menu_DF                     equ     0101h
XTK_Menu_ID                     equ     0001h
Mobile_Model_EF			equ	0005h

Look_People_EF                  equ     0012h

CHV1_ID                         equ     0001h
CHV2_ID                         equ     0002h
UnBlock_CHV1                    equ     0003h
UnBlock_CHV2                    equ     0004h

Ki_ID                           equ     0005h
SMS_Center_ID                   equ     0F42h

A_Key_ID                        equ     0005h
SSD_ID                          equ     0006h
IMSI_M_ID                       equ     6F22h
UIM_ID_ID                       equ     6f31h

save_dptr_begin                 equ     byCard_Buf_start+0030h
;;******************************************************
return_mark                     equ     temp_byte+07h
temp_entrance1                  equ     temp_byte+11h
temp_entrance2                  equ     temp_byte+12h

judge_record_null               bit     temp_bit.0

MAIN_PROCESS_OUT_SP		EQU	0D1H


eeprom_manage_byte			equ		2ch
run_W_eeprom    	        bit     2ch.0
Super_W_eeprom		        bit     2ch.1
INS_start_bit           	bit     2ch.2
Sector_Size			equ	40h
;;*************************************************************
;;定义DES模块特殊寄存器
;;09L
;;added by laiht for 09L des module end 2005.10.20
PLAINTEXT_ADDR	equ	09dh	;明文或密文的基址
KEY_ADDR	equ	09eh	;密钥的基址
DES_CON		equ	09fh	;des 控制寄存器

;;DES_CON [0]：操作模式选择：
;	;0 ：进行加密运算
;	;1 ：进行解密运算;
;;DES_CON [1]：DES模块门控时钟，须在运算开始之前打开
;;DES_CON [2]：DES运算使能
;;初始化内部RAM ，将明文和密钥写入内部RAM DES数据区，明文和密钥各占8Byte空间，注意：数据低位写入低地址。
;;开启时钟，即将DES_CON [1]置为1
;;设置加解密模式及明文，密钥基址，即根据内部RAM初始化情况设置DES_CON[0]、PLAINTEXT_ADDR和PLAINTEXT_ADDR；
;;设置DES运算使能，即将DES_CON[2]置为1，开始DES运算，在执行完一次DES运算后，DES_CON [2]会硬件清"0"，表示DES运算结束
;;运算结束后，将DES_CON [1] 置为0，关断时钟，以节约功耗


;****************************************************************
;;******************************************************
Define_INS	MACRO   INS_Index, INS_Address, INS_attribute

Extrn	Code    (INS_Address)
        db      INS_Index
        db      INS_attribute
        dw      INS_Address

        ENDM
