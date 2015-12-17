@echo Ð¶ÔØHD_WSC_CardInterface¾ÓÃñ½¡¿µ¿¨¿Ø¼þ
@echo begin

%windir%\system32\regsvr32 /u HD_WSC_CardInterface.dll /s
del %windir%\system32\HD_WSC_CardInterface.dll
del %windir%\system32\SSSE32.dll
del %windir%\system32\HD300_V1.dll
del %windir%\system32\WSB_RWInterface.dll
del %windir%\system32\BankNo.dll

%windir%\syswow64\regsvr32 /u HD_WSC_CardInterface.dll /s
del %windir%\syswow64\HD_WSC_CardInterface.dll
del %windir%\syswow64\SSSE32.dll
del %windir%\syswow64\HD300_V1.dll
del %windir%\syswow64\WSB_RWInterface.dll
del %windir%\syswow64\BankNo.dll

@echo pause