				Name    exe
$Include (Main.inc)

public	find_INS
;public	Exec_INS_Operate
extrn	code	(Command_INS_End)
extrn	code	(Command_INS)
extrn	code	(Number_INS)

?PR?PROG_exe                       SEGMENT CODE

                                RSEG    ?PR?PROG_exe
find_INS:
	mov 	r2, #Number_INS
        mov 	dptr, #Command_INS                  	; INS ָ�����ʼ��?
find_INS_table_start:
        mov 	a,r2
        jz 	not_find_INS_in_table
find_INS_table_loop:
        clr 	a
        movc 	a,@a+dptr
        xrl 	a,byINS
        jz 	find_INS_in_table                   	; �Ƿ�Ϊ INS

	inc 	dptr
	inc 	dptr
	inc 	dptr
	inc 	dptr
        djnz 	r2,find_INS_table_loop            	; ������һ��
not_find_INS_in_table:
	clr 	c
	ret
find_INS_in_table:
        mov 	a, #01h
        movc 	a, @a+dptr
                                
find_INS_Success:
        setb 	c
        ret                                     	; �ҵ�, ����

;cseg		at	07fffh
;		db 00h



	end