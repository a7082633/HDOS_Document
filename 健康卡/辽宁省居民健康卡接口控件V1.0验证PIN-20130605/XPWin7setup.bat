copy  LL_WSC_CardInterface.ocx		%windir%\system32
copy  HD300_V1.dll			%windir%\system32
copy  WSB_RWInterface.dll		%windir%\system32
copy  LL_WSC_CardInterface.ocx		%windir%\syswow64
copy  HD300_V1.dll			%windir%\syswow64
copy  WSB_RWInterface.dll		%windir%\syswow64

%windir%\system32\regsvr32 /u LL_WSC_CardInterface.ocx
%windir%\system32\regsvr32 LL_WSC_CardInterface.ocx
%windir%\syswow64\regsvr32 /u LL_WSC_CardInterface.ocx
%windir%\syswow64\regsvr32 LL_WSC_CardInterface.ocx