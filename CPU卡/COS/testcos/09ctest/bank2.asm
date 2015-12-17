				Name    ins

$Include (Main.inc)

?PR?PROG_ins                    SEGMENT CODE

                                RSEG    ?PR?PROG_ins
extrn	code	(length_xx_response_data)
extrn	code	(command_success)
extrn	CODE	(incorrect_parameter_p3)
extrn	CODE	(des_encrypt_or_decrypt)
extrn	CODE	(erase_sector)
extrn	CODE	(Fill_Xram_A)
extrn	CODE	(acc_add_dptr)
extrn	CODE	(WriteProcess)

public	Read_Flash	
public	Write_Flash	
public	EraseFlash	
public	testP2	
public	get_response
public	test_des
public	test_trim

public	Read_Flash_1	
public	Write_Flash_1	
public	EraseFlash_1	



public	Read_Flash_2	
public	Write_Flash_2	
public	EraseFlash_2	
public	get_rgn
public	test_t0
Nop_space:
	ds	02000h
;****************************************************************
;testread  flash
;
Read_Flash:
	mov	dph,byP1	;高地址
	mov	dpl,byP2	;低地址
	mov	r2,byP3		;长度	
	mov	P2,#high(bySend_Receive_buf)
	mov	r0,#0
read_loop:
	movx	a,@dptr
	movx	@r0,a
	inc 	r0
	inc 	dptr
	djnz	r2,read_loop
	mov	Le_Length,byP3
	jmp	command_success


;****************************************************************

Write_Flash:
	Mov	DpH,byP1
	Mov	DpL,byP2
	Mov	r1,#High(bySend_Receive_Buf)
	Mov	r0,#Low(bySend_Receive_Buf)
	Mov	r2,byP3

	Call	XRamToFlashEeprom
	Jnc	memory_problem
	Jmp	command_success
memory_problem:
        mov sw1, #92h
        mov sw2, #40h

        ret


;****************************************************************
EraseFlash:
	mov	a,byp1
	mov	dph,a
	mov	a,byp2
	mov	dpl,a
$IF Simulator <> 0 
	push	00H
	push	dph
	push	dpl
Erase_Loop_512:	
	Mov	P2,DpH
	Mov	r0,DpL
	Mov	A,#0FFh

	mov	r2,#Sector_Size
	Call	Fill_Xram_A

	mov	a,#Sector_Size
	call	acc_add_dptr	
	
	Mov	A,B
	Jz	BisZero
	Djnz	B,Erase_Loop_512
BisZero:
	pop	dpl
	pop	dph
	pop	00H
	;SetB	C
	;ret
$else
	call	erase_sector	
	;ret
$endif
        jmp 	command_success


;****************************************************************
;测试P2读写COS命令
;取某些命令结果	
testP2:
	mov	dptr,#bySend_Receive_buf
	movx	a,@dptr
	
	mov	p2,a
	mov	a,p2
	movx	@dptr,a

        mov 	a, #01h
        jmp 	length_xx_response_data


;****************************************************************
;cos命令
;取某些命令结果
Get_response:
        mov 	a, sw1                              ; 是否为9fh
        xrl 	a, #9fh
        jz 	have_response_data
        mov 	sw2, #00h                           ; 清响应数据长度
        jmp 	has_not_enough_data
have_response_data:
        mov 	a, byP3
        jz 	has_not_enough_data                  ; 返回数据是否太长

        mov 	Le_Length, byP3
        jmp 	command_success
;;******************************************************

has_not_enough_data:
	mov 	a, sw2
	jmp 	incorrect_parameter_p3
	
;;******************************************************
;测试des 协处理器	
test_des:
	;source data
	mov	dptr,#bySend_Receive_buf
	mov	r2,#08h
	mov	r0,#060h
	call	xram_to_ram	
	;key
	mov	dptr,#(bySend_Receive_buf+08h)
	mov	r2,#08h
	mov	r0,#070h
	call	xram_to_ram	

	mov	PLAINTEXT_ADDR,#060h
	mov	KEY_ADDR,#070h
	mov	a,byp2
	call	des_encrypt_or_decrypt
	mov	dptr,#bySend_Receive_buf
	mov	r2,#08h
	mov	r0,#060h
	call	ram_to_xram	
	
        mov 	a, #08h
        jmp 	length_xx_response_data

  
;*****************************************************
;
xram_to_ram:
	push	00h
	push	dph
	push	dpl
xram_to_ram_loop:
	movx	a,@dptr
	mov	@r0,a
	inc	dptr
	inc	r0
	djnz	r2,xram_to_ram_loop
	pop	dpl
	pop	dph
	pop	00h
	ret


ram_to_xram:
	push	00h
	push	dph
	push	dpl
ram_to_xram_loop:
	mov	a,@r0
	movx	@dptr,a
	inc	dptr
	inc	r0
	djnz	r2,ram_to_xram_loop
	pop	dpl
	pop	dph
	pop	00h
	ret
      
;*****************************************************************
;* high :r1
;* low	: r0
;* len :r2
;* addr: dptr
;*****************************************************************

XRamToFlashEeprom:
RamToFlashEeprom:
	push	00h
	push	02h
	push	01h
	;Judge Xram or Ram to Flash by r1
RamOrXram:
	Mov	a,r1
	Jz	RamToFlash
XramToFlash:
	Mov	P2,r1
	Movx	A,@r0
	Jmp	ByteCheckStep
RamToFlash:
	Mov	A,@r0
ByteCheckStep:
	Call	WriteProcess
LoopSet:
	Inc	Dptr
	Inc	r0
	
	Mov	A	,r0
	Jnz	LoopSetStep
	Mov	A	,r1
	Jz	LoopSetStep
	Inc	r1
	Mov	P2,	r1
LoopSetStep:
	Djnz	r2,	RamOrXram

;Check Write Result
	Pop	01h
	Pop	02h
	Pop	00h
	
	Mov	A,	r2
	Call	Dptr_Sub_Acc
	
CheckWriteResult:
	Mov	A	,r1
	Jz	RamCheck
;Xram Check
	Mov	P2	,01h
	Movx	A	,@r0
	Mov	B	,A
	Jmp	CompareByte
RamCheck:
	Mov	B	,@r0
CompareByte:
	Movx	A,	@Dptr
	Xrl	A,	B
	Jnz	TestError
	Inc	Dptr
	Inc	r0

	Mov	A	,r0
	Jnz	CheckNextStep
	Mov	A	,r1
	Jz	CheckNextStep
	Inc	r1
	Mov	P2,	r1
CheckNextStep:	
	Djnz	r2,	CheckWriteResult

	SetB	C
	Ret

ByteError:
	Pop	01h
	Pop	02h
	Pop	00h
TestError:
	Clr	C
	Ret

;;******************************************************
;;    入口参数: DPTR = 源地址, acc = 偏移量
;;    出口参数: DPTR = 目标地址
;;破坏的寄存器: acc
;;******************************************************

dptr_sub_acc:
	push b

        clr c
        mov b, a
        mov a, dpl
        subb a, b
        mov dpl, a

        mov a, dph
        subb a, #00h
        mov dph, a

        jc $

        pop b

        ret
;cseg		at	07fffh
;		db 00h





;testread  flash
;
Read_Flash_1:
	mov	dph,byP1	;高地址
	mov	dpl,byP2	;低地址
	mov	r2,byP3		;长度	
	mov	P2,#high(bySend_Receive_buf)
	mov	r0,#0

	mov	a,p1
	anl	a,#01fh
	orl	a,#020h
	mov	p1,a

read_loop_1:
	movx	a,@dptr
	movx	@r0,a
	inc 	r0
	inc 	dptr
	djnz	r2,read_loop_1
	mov	Le_Length,byP3
	jmp	command_success


;****************************************************************

Write_Flash_1:
	Mov	DpH,byP1
	Mov	DpL,byP2
	Mov	r1,#High(bySend_Receive_Buf)
	Mov	r0,#Low(bySend_Receive_Buf)
	Mov	r2,byP3

	mov	a,p1
	anl	a,#01fh
	orl	a,#020h
	mov	p1,a

	Call	XRamToFlashEeprom
	Jnc	memory_problem_1
	Jmp	command_success
memory_problem_1:
        mov sw1, #92h
        mov sw2, #40h

        ret


;****************************************************************
EraseFlash_1:
	mov	a,byp1
	mov	dph,a
	mov	a,byp2
	mov	dpl,a

	mov	a,p1
	anl	a,#01fh
	orl	a,#020h
	mov	p1,a

$IF Simulator <> 0 
	push	00H
	push	dph
	push	dpl
Erase_Loop_512_1:	
	Mov	P2,DpH
	Mov	r0,DpL
	Mov	A,#0FFh

	mov	r2,#Sector_Size
	Call	Fill_Xram_A

	mov	a,#Sector_Size
	call	acc_add_dptr	
	
	Mov	A,B
	Jz	BisZero_1
	Djnz	B,Erase_Loop_512_1
BisZero_1:
	pop	dpl
	pop	dph
	pop	00H
	;SetB	C
	;ret
$else
	call	erase_sector	
	;ret
$endif
        jmp 	command_success




;testread  flash
;
Read_Flash_2:
	mov	dph,byP1	;高地址
	mov	dpl,byP2	;低地址
	mov	r2,byP3		;长度	
	mov	P2,#high(bySend_Receive_buf)
	mov	r0,#0

	mov	a,p1
	anl	a,#01fh
	orl	a,#040h
	mov	p1,a

read_loop_2:
	movx	a,@dptr
	movx	@r0,a
	inc 	r0
	inc 	dptr
	djnz	r2,read_loop_2
	mov	Le_Length,byP3
	jmp	command_success


;****************************************************************

Write_Flash_2:
	Mov	DpH,byP1
	Mov	DpL,byP2
	Mov	r1,#High(bySend_Receive_Buf)
	Mov	r0,#Low(bySend_Receive_Buf)
	Mov	r2,byP3

	mov	a,p1
	anl	a,#01fh
	orl	a,#040h
	mov	p1,a

	Call	XRamToFlashEeprom
	Jnc	memory_problem_2
	Jmp	command_success
memory_problem_2:
        mov sw1, #92h
        mov sw2, #40h

        ret


;****************************************************************
EraseFlash_2:
	mov	a,byp1
	mov	dph,a
	mov	a,byp2
	mov	dpl,a

	mov	a,p1
	anl	a,#01fh
	orl	a,#020h
	mov	p1,a

$IF Simulator <> 0 
	push	00H
	push	dph
	push	dpl
Erase_Loop_512_2:	
	Mov	P2,DpH
	Mov	r0,DpL
	Mov	A,#0FFh

	mov	r2,#Sector_Size
	Call	Fill_Xram_A

	mov	a,#Sector_Size
	call	acc_add_dptr	
	
	Mov	A,B
	Jz	BisZero_2
	Djnz	B,Erase_Loop_512_2
BisZero_2:
	pop	dpl
	pop	dph
	pop	00H
	;SetB	C
	;ret
$else
	call	erase_sector	
	;ret
$endif
        jmp 	command_success



;testread  flash
;
get_rgn:
	mov	r2,byP3		;长度	
	mov	P2,#high(bySend_Receive_buf)
	mov	r0,#0
read_rgn_loop:
	mov	a,rng
	movx	@r0,a
	inc 	r0
	djnz	r2,read_rgn_loop
	mov	Le_Length,byP3
	jmp	command_success




test_trim:
           MOV 		0a5H, #01H
           MOV 		DPTR , #0200H                    
           MOVX 	A, @DPTR

           PUSH   	ACC
           MOV    	DPTR, #0AAAAH
           MOV    	A,  #0AAH
           MOVX   	@DPTR, A  ;write 1st com, addr = 555H,data = AAH
           MOV    	DPTR, #1554H
           MOV    	A, #55H
           MOVX   	@DPTR, A  ;write 2nd com, addr = AAAH,data = 55H
           MOV    	DPTR, #0AAAAH
           MOV    	A, #0B8H
           MOVX   	@DPTR, A  ;write 3rd com, addr = 555H,data = B8H 
           MOV    	DPTR, #0010H
           POP   	 ACC

           MOVX   	@DPTR, A  ;write 4st com, addr = 008H,data = CBD
           MOV   	DPTR, #0000H
           MOV    	A, #0FFH
           MOVX   	@DPTR, A  ;write 5th com, addr = 000H,data = FFH
           CLR 		P3.0
           ;LCALL TRIM
           MOV 		0a5H, #00H
           SETB 	P3.0
           ;Ajmp  $
	jmp 		command_success


test_t0:
	jmp	$

        end                                                                                    