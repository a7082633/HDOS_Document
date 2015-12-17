copy  HD_WSC_CardInterface.dll		%windir%\system32
copy  HD300_V1.dll					%windir%\system32
copy  WSB_RWInterface.dll			%windir%\system32
copy  SSSE32.dll					%windir%\system32
copy  BankNo.dll					%windir%\system32
copy  HD_WSC_CardInterface.dll		%windir%\syswow64
copy  HD300_V1.dll					%windir%\syswow64
copy  WSB_RWInterface.dll			%windir%\syswow64
copy  SSSE32.dll					%windir%\syswow64
copy  BankNo.dll					%windir%\syswow64

%windir%\system32\regsvr32 /u HD_WSC_CardInterface.dll /s
%windir%\system32\regsvr32 HD_WSC_CardInterface.dll /s
%windir%\syswow64\regsvr32 /u HD_WSC_CardInterface.dll /s
%windir%\syswow64\regsvr32 HD_WSC_CardInterface.dll /s

pause