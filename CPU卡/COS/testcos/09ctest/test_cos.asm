;***********************************************************************
;功能:test cos
;版本:V1.0
;作者:lht
;时间:2005/11/23
;***********************************************************************
$Include (Main.inc)



extrn	code	(init_param)
extrn	code	(Find_INS)
public	length_xx_response_data
public	incorrect_parameter_p3
public	command_success
;;******************************************************

	USING	0                                 ; Register-Bank 0
	CSEG	AT 0000H
	ljmp 	main
				
;;******************************************************

        CSEG 	AT 0003H                           ; IE0_VECTOR 0  /* 0x03 External Interrupt 0 */
                                
        jmp 	Eeprom_INT_0

        nop
        nop
        nop
        nop
        nop
;;******************************************************

	CSEG 	AT 000BH                           ; TF0_VECTOR 1  /* 0x0B Timer 0 */
				
	jmp 	Eeprom_INT_1
				
	nop
	nop
	nop
	nop
	nop
;;******************************************************

        CSEG 	AT 0013H                           ; IE1_VECTOR 2  /* 0x13 External Interrupt 1 */

        jmp 	Eeprom_INT_2

        nop
        nop
        nop
        nop
        nop
;;******************************************************

        CSEG 	AT 001BH                           ; TF1_VECTOR 3  /* 0x1B Timer 1 */
        
        jmp 	Eeprom_INT_3
        
        nop
        nop
        nop
        nop
        nop
;;******************************************************

        CSEG 	AT 0023H                           ; SIO_VECTOR 4  /* 0x23 Serial port */
        
        jmp 	Eeprom_INT_4
        
        nop
        nop
        nop
        nop
        nop
;;******************************************************

        CSEG 	AT 002Bh
        
        jmp 	Eeprom_INT_5
        
        nop
        nop
;;******************************************************
PROG_Main                   SEGMENT CODE

                                RSEG    PROG_Main

main:
        mov	P1, #0ch				;动态划分区12K数据，18K code	
	mov	097h,#00h				;第二页为程序		
	clr 	a
        mov 	sp, #(STACK-1)
        mov 	psw, #00h                           	; 选择工作寄存器组 0
IC_Reset_delay:
        clr 	a
        mov 	r0, #R0_Bank1				
        mov 	r2, #(Temp_byte-R0_Bank1)
Clr_Ram_loop:
        mov 	@r0, a
        inc 	r0
        djnz 	r2, Clr_Ram_loop

							;
	clr 	a					;
        mov 	r0, #low(byMem_Manage_start)
        mov 	P2, #high(byMem_Manage_start)
        mov 	r2, #(INT_Vector-byMem_Manage_start)
Clr_Thread_Para_loop:
        movx 	@r0, a
        inc 	r0
        djnz 	r2, Clr_Thread_Para_loop

        call 	init_param                         	; 初始化参数
						 	
send_ATR:						;环⑺虯TR
	mov 	r2, #(ATR_end-ATR_Start)            	; 发送的长度
       	mov 	dptr, #ATR_start                    	; ATR 的起始地址
send_ATR_loop:
       	clr 	a
	movc 	a, @a+dptr
       	call 	send_byte
	inc	dptr
     	djnz	r2, send_ATR_loop
							;准备接受指令
	mov 	b, #00h
        mov 	dptr, #(bySend_Receive_buf+40h)
receive_byte_start:
        mov 	TCON, #01h                          	; 设置下降沿触发
        mov 	IE, #81h                            	; 开中断
        orl 	PCON, #02h                          	; 进入休闲模式
        mov 	IE, #00h                            	; 关中断

        call 	receive_byte
        cjne 	a, #PTSS, no_PTS

        jmp 	save_PTS
no_PTS:
        mov 	byCLA, a
        mov 	r0, #byINS
        mov 	r2, #04h
        jmp 	receive_INS
receive_PTS_loop:
        call 	receive_byte
save_PTS:
        movx 	@dptr, a
        inc 	dptr

        xrl 	a, b
        mov 	b, a
        jnz 	receive_PTS_loop
send_PTS:
        mov 	r2, #38h
        djnz 	r2, $

        clr 	c
        mov 	a, dpl
        subb 	a, #40h
        mov 	r2, a
        mov 	dptr, #(bySend_Receive_buf+40h)
        call 	Send_xram_r2

        mov 	dptr, #(bySend_Receive_buf+40h+01h)
        movx 	a, @dptr
													anl a, #10h
        xrl 	a, #10h
	
        jnz 	receive_CLA

        inc 	dptr
        movx 	a, @dptr
        xrl 	a, #94h
        jnz 	receive_CLA

        MOV 	Baud_MR, #01H                       ; 初始化波特率寄存器9bh,9ch
        MOV 	Baud_BR, #0FFH                       ; ETU=CLK_PERD*8/512(Enhance BAUD)
        setb 	bEnhance_Speed
receive_CLA:
	clr 	INS_start_bit
	clr 	Super_W_eeprom

        mov 	r0, #byCLA
        mov 	r2, #05h
receive_APDU:
receive_INS:
	mov 	TCON, #01h                          ; 设置下降沿触发
        mov 	IE, #81h                            ; 开中断
        orl 	PCON, #02h                          ; 进入休闲模式
        mov 	IE, #00h                            ; 关中断
receive_APDU_loop:
      	call 	receive_byte				
      	mov 	@r0, a
      	inc 	r0
      	
      	djnz 	r2, receive_APDU_loop
      	
      	mov 	a, byCLA
	xrl 	a, #0f0h
	jz 	?_legality_INS
      	jmp 	Bad_CLA                  		; CLA 是否为 A0H
?_legality_INS:
	setb 	INS_start_bit


        mov 	Le_Length, #00h
        call 	Find_INS                   ; 是否为有效 INS
	jc 	Bad_INS_temp
	jmp	Bad_INS    			                           
Bad_INS_temp:
        push 	byINS
        pop 	byINS_Bak

        mov 	r0, #Write_Enable_2
        mov 	@r0, #02h

        jnb 	acc.1, has_lc_data_operate

        inc 	r0
        mov 	@r0, #03h
has_lc_data_operate:
        jnb 	acc.0, start_exec_INS               ; 是否有 Data

        mov 	a, byP3				    ;byp3数据长度	
        jz 	start_exec_INS
Normal_Receive_Lc_data:
        mov 	r2, a
        mov 	dptr, #bySend_Receive_buf

        mov 	a, byINS
        call 	Send_byte			  ;send ins
receive_Lc_loop:
        call 	receive_byte
        movx 	@dptr, a
        inc 	dptr

        djnz 	r2, receive_Lc_loop

start_exec_INS:
;****************************************************************
	mov 	IE, #82h
	setb 	TR0
	call 	Exec_INS_Operate                   ; 执行指令

clr_Write_Enable_flag:
	clr 	TR0			;关闭t0中断
        mov 	a, Le_Length
        jz 	Send_sw1_sw2

        mov 	a, byINS
        call 	Send_byte

        mov 	r2, Le_Length
        mov 	dptr, #bySend_Receive_buf
        call 	Send_xram_r2

Send_sw1_sw2:
        mov 	a, sw1
        call 	Send_byte
        
        mov 	a, sw2
        call 	Send_byte
        
        jmp 	receive_CLA                         ; 返回主程序重新开始循环
;******************************************************

Bad_INS:
        mov 	sw1, #6dh                           ; 错误的 INS
        jmp 	bad_CLA_INS_back
Bad_CLA:
        mov 	sw1, #6eh                           ; 错误的 CLA
bad_CLA_INS_back:
        mov 	sw2, #00h
        jmp 	clr_Write_Enable_flag               ; 发送 SW1, SW2

;******************************************************

Enable_Receive                  MACRO
        MOV SCON, #10100000B
        ENDM
;******************************************************
;******************************************************
receive_byte:
	Enable_Receive

	jnb	RI_SRF	,$
	JBC	TB8	,receive_byte

	jb	SCON.3	,receive_byte
	mov	a	,Send_Receive_SRF
	clr	RI_SRF

	ret
;;******************************************************
Send_byte:
 	push	02h
	mov	r2	,#08h
	djnz	r2	,$
	pop	02h

	mov	Send_Receive_SRF	,a
	mov	SCON	,#10010000B                    ; 开始发送数据
	jnb	TI_SRF	,$

	JBC	RB8	,$+3   				; Is OK ?
        Enable_Receive

	Nop
	Nop
	Nop
	Nop

	ret
;******************************************************

Send_xram_r2:
Send_xram_loop:
	movx	a	,@dptr
	call	Send_byte

	inc	dptr
	djnz	r2	,Send_xram_loop

	ret
;******************************************************        
;外部中断0，休眠唤醒     
Eeprom_INT_0:                                               
        reti                   
;******************************************************
;t1定时器中短，send 60                                                       
Eeprom_INT_1:                                          
        push acc               
	push psw                                       
                                                       
	mov a, #60h                                    
	call Send_byte                                 
                                                       
	clr a                                          
	mov c, bEnhance_Speed                          
	rrc a                                          
	mov TH0, a                                     
                                                       
	pop psw                                        
 	pop acc                                    
                                                       
	reti                                           
;******************************************************
                                                       
Eeprom_INT_2:
	reti                                          
;******************************************************
                                                       
Eeprom_INT_3:                                          
	reti                                          
;******************************************************
                                                       
Eeprom_INT_4:                                          
	reti                                          
;******************************************************
                                                       
Eeprom_INT_5:                                          
	reti                                          
	
Exec_INS_Operate:
        call 	find_INS                   		; 查找指令

	mov	a,#03h
        movc 	a, @a+dptr
        push 	acc                                	
 
	mov	a,#02h
        movc 	a, @a+dptr
        push 	acc                               	 

        mov 	P2, #high(bySend_Receive_buf)
call_command_ins:
	ret




length_xx_response_data:
	mov 	sw1, #9fh
        mov 	sw2, a
        ret
;;******************************************************
command_success:                                                                                                            	                                                                                                    
        mov 	a, XTK_sw2                                                                              
        mov 	sw2, a                                                                                  
        jz 	command_success_normal                                                                   
command_success_91XX:                                                                                                       
        mov 	sw1, #91h                                                                               
        ret                                                                                         
command_success_normal:                                                                                                     
Set_SW1_SW2_9000:                                                                                                           
        mov 	sw1, #90h                                                                               
        ret     

incorrect_parameter_p3:
	mov 	sw1, #67h
        mov 	sw2, a

         ret



;******************************************************
cseg		at	0ff00h


ATR_start:
        db	3bh, 3Dh                        ; 正向约定,发送 TA1, TB1, 8 个历史字符
        db 	94h, 00h                        ; F=512, D=8, 无协议类型T和后续接口字符
        db  	'DMT'                           ; 公司简写
        db  	11h                             ; V6.0 版
        db  	01h                             ; 2003 年 06 月
        db  	02h                             ; DTT4C03 芯片, B 版
        db  	00h                             ; 通用卡, 自检正常
	db	12h
	db	91H, 54H, 30H, 30H
	db	00h
ATR_end:
;******************************************************
cseg		at	0ffffh
		db 00h

	end       