;******************************************************
				Name    init
$Include (Main.inc)
public	init_param
?PR?PROG_init                       SEGMENT CODE

                                RSEG    ?PR?PROG_init

init_param:
        mov 	PCON, #0Ch                          ; 初始化 PCON
        mov 	TCON, #00h                          ; 关闭定时器
        mov 	SCON, #80h                          ; 关闭串行通讯
        mov 	IE, #00h                            ; 禁止所有中断

	mov 	TMOD, #01h				; 0定时器01模式

        ;mov 	sw1, #9fh
        ;mov 	sw2, #16h                           ; 设置初始响应数据长度

        MOV 	Baud_MR, #0BH                       ; 初始化波特率寄存器9bh,9ch
        MOV 	Baud_BR, #13H                       ; ETU=CLK_PERD/372(SLOWEST BAUD)
init_param_out:
        ret
;cseg		at	07fffh
;		db 00h
        
        
        end