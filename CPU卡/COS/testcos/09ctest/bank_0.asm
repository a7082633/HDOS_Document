;******************************************************
				Name    init
$Include (Main.inc)
public	init_param
?PR?PROG_init                       SEGMENT CODE

                                RSEG    ?PR?PROG_init

init_param:
        mov 	PCON, #0Ch                          ; ��ʼ�� PCON
        mov 	TCON, #00h                          ; �رն�ʱ��
        mov 	SCON, #80h                          ; �رմ���ͨѶ
        mov 	IE, #00h                            ; ��ֹ�����ж�

	mov 	TMOD, #01h				; 0��ʱ��01ģʽ

        ;mov 	sw1, #9fh
        ;mov 	sw2, #16h                           ; ���ó�ʼ��Ӧ���ݳ���

        MOV 	Baud_MR, #0BH                       ; ��ʼ�������ʼĴ���9bh,9ch
        MOV 	Baud_BR, #13H                       ; ETU=CLK_PERD/372(SLOWEST BAUD)
init_param_out:
        ret
;cseg		at	07fffh
;		db 00h
        
        
        end