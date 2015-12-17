                                Name    INS_Table

$Include (main.inc)


Public                          Number_INS
Public                          Command_INS
Public                          Command_INS_End

Number_INS                 equ     (Command_INS_End-Command_INS)/4

PROG_INS_Table                  Segment Code
                                RSEG    PROG_INS_Table
;;******************************************************

Command_INS:
	Define_INS      011h, Read_Flash, 	      None
	Define_INS      012h, Write_Flash,            Have_Lc+Have_Write_Eeprom
	Define_INS      013h, EraseFlash, 	      Have_Write_Eeprom
	Define_INS      015h, testP2, 	      	      Have_Lc
        Define_INS      016h, test_des, 	      Have_Lc
        Define_INS      0C0h, Get_Response, 	      None

	Define_INS      017h, Read_Flash_1, 	      None
	Define_INS      018h, Write_Flash_1,            Have_Lc+Have_Write_Eeprom
	Define_INS      019h, EraseFlash_1, 	      Have_Write_Eeprom

	Define_INS      01ah, Read_Flash_2, 	      None
	Define_INS      01bh, Write_Flash_2,            Have_Lc+Have_Write_Eeprom
	Define_INS      01ch, EraseFlash_2, 	      Have_Write_Eeprom

	Define_INS      01dh, get_rgn, 	      None

	Define_INS      01eh, test_trim, 	      None

	Define_INS      01fh, test_t0, 	      None

Command_INS_End:


	end