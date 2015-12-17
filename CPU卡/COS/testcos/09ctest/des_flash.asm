				Name    des_flash
$Include (Main.inc)

public	des_encrypt_or_decrypt
public	erase_sector
public	acc_add_dptr
public	fill_xram_A
public	WriteProcess
prog_eeprom                      SEGMENT CODE

                                RSEG    prog_eeprom

;;;******************************************************
;des encrypt/dencrypt
;入口参数：
;a:a=0 encrypt;a=1;dencrypt
;PLAINTEXT_ADDR:明文或密文的基址
;KEY_ADDR:密钥的基址
;出口参数：
;PLAINTEXT_ADDR:encrypt/decrypt result initial address
;********************************************************
des_encrypt_or_decrypt:
$if	simulator<>0
	ret
$else 
	mov	b,#00000110B 
	orl	a,b
	mov	DES_CON,a	;开DES协处理器
open_des_coprocessor:		                                                                        
	mov	a,DES_CON
	anl	a,#04h
	jnz	open_des_coprocessor
des_coprocessor_end:                                                                 
        mov	DES_CON,#00h
        ret                                                    
 endif             
;;;******************************************************
;erase_sector
;入口参数：dptr:flash initial address
;出口参数：c=1;success ;c=0;fail
;********************************************************              
erase_sector:
	Push	02		;如果失败，最多重复3次
	
	Mov	r2,#03h
SECTOR_ERASE:
				; initial addr, the first section
	Push	DpH
	Push	DpL
	

	MOV    96H, #80H
	MOV    DPTR, #0AAAAH
	MOV    A,  #0AAH
	MOVX   @DPTR, A  	;write 1st com, addr = 0AAAAH, data = AAH
	MOV    DPTR, #1554H
	MOV    A, #55H
	MOVX   @DPTR, A  	;write 2nd com, addr = 1554H,  data = 55H
	MOV    DPTR, #0AAAAH
	MOV    A, #80H
	MOVX   @DPTR, A  	;write 3rd com, addr = 0AAAAH, data = 80H
	MOV    DPTR, #0AAAAH
	MOV    A,  #0AAH
	MOVX   @DPTR, A  	;write 4st com, addr = 0AAAAH, data = AAH
	MOV    DPTR, #1554H
	MOV    A, #55H
	MOVX   @DPTR, A  	;write 5th com, addr = 1554H,  data = 55H


	POP    DPL
	POP    DPH
	MOV    A,  #30H
	MOVX   @DPTR, A 	 ;erase  section (64 byte once time)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
VREAD_VERIFY:
	MOV    	a,  96H   		;judge whether Erase success or not
	CJNE   	a, #90H,  EraseSuccess	; Erase success ,point next operation
	Djnz	r2, SECTOR_ERASE	; Erase fail ,DPTR is not change , re_erase
	Clr	C
	Pop	02
	Ret

EraseSuccess:
	Setb 	C
	Pop	02
	Ret

;;******************************************************       
                                                               
fill_xram_A:                                                                                                                           
	cjne 	r2, #00h, fill_xram_A_loop                   
	ret                            
fill_xram_A_loop:                                              
	movx 	@r0, a                    
	inc 	r0                         
                                                               
	djnz 	r2, fill_xram_A_loop      
                                                               
	ret      

;;******************************************************

acc_add_dptr:
        add 	a, dpl
        mov 	dpl, a

        mov 	a, dph
        addc	a, #00h
        mov 	dph, a

        jc 	$

        ret

;****************************************************************
;*	Write Process
;*	Source Byte:	A					*
;*	Target Byte:	B					*
;****************************************************************

WriteProcess:
	push dph 
	push dpl 
	push acc 

$if simulator <> 0
$else
	MOV     96H, #08H
	MOV     DPTR, #0AAAAH
	;MOV     DPTR, #0AAAH
	MOV     A, #0AAH
	MOVX    @DPTR, A   ;write 1st command, addr = 0AAAAH, data= AAH
	MOV     DPTR, #1554H
	MOV     A, #55H
	MOVX    @DPTR ,A   ;write 2nd command, addr = 1554H,  data = 55H
	MOV     DPTR, #0AAAAH
	;MOV     DPTR, #0AAAH
	MOV     A, #0A0H
	MOVX    @DPTR ,A   ;write 3rd command, addr = 0AAAAH, data = A0H
$endif
	pop acc
	pop dpL
	pop dpH

	MOVX	@DPTR,	A
	Nop
	Nop
	Nop
	Nop

	Ret


	end









