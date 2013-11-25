galera-msvc
===========
pthreads win32 library is required ( http://www.sourceware.org/pthreads-win32/ )
set temp_inst=c:\temp_inst\
if not exist %temp_inst%bin mkdir %temp_inst%bin
if not exist %temp_inst%lib mkdir %temp_inst%lib
if not exist %temp_inst%include mkdir %temp_inst%include
set LIB=%inst_temp%lib;%LIB%
set INCLUDE=%inst_temp%iinclude;%INCLUDE%

clone https://github.com/YannNayn/msvc_sup.git
cd msvc_sup
nmake CFG=dll-release
popd
copy /Y msvc_sup\msvc100\src\dll-release\*.dll %inst_temp%bin
copy /Y msvc_sup\msvc100\src\dll-release\*.lib %inst_temp%lib
copy /Y msvc_sup\include\*.h %inst_temp%include

