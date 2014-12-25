#include "igs_path_separation.h"

/* 文字列を分離する文字の定義(固定) */
igs::path::separation::separation()
	:path(TEXT("/"))
	,file(TEXT("."))
	,encl(TEXT("\""))
	//,body(TEXT("_"))
{}
//------------------------------------------------------------------
#if defined DEBUG_IGS_PATH_SEPARATION
# include <iostream>
# include <sstream>
int main(void) { 
	igs::path::separation psep;
	std::basic_ostringstream<TCHAR> ost;
	ost
	 << TEXT("igs::path::separation\n")

			<< TEXT(" path=")
	 << static_cast<int>(psep.path[0])
	 << TEXT("=\"")   << psep.path << TEXT("\"\n")

			<< TEXT(" file=")
	 << static_cast<int>(psep.file[0])
	 << TEXT("=\"")   << psep.file << TEXT("\"\n")

			<< TEXT(" encl=")
	 << static_cast<int>(psep.encl[0])
	 << TEXT("=\"")   << psep.encl << TEXT("\"\n")

	 ;
	std::TCOUT << ost.str();
	return 0;
}
#endif  /* !DEBUG_IGS_PATH_SEPARATION */
/*
rem vc2010(32/64bits) :38,54 w! tes_msc16_igs_path_sepa.bat
set _DEB=DEBUG_IGS_PATH_SEPARATION
set _SRC=igs_path_separation
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_path_sepa_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_path_sepa_wc
rem Need _UNICODE for ::_tcslen()
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D_UNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :51,69 w! tes_msc15_igs_path_sepa.bat
set _DEB=DEBUG_IGS_PATH_SEPARATION
set _SRC=igs_path_separation
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_path_sepa_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_path_sepa_wc
rem Need _UNICODE for ::_tcslen()
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D_UNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :74,76 w! tes_gcc_igs_path_sepa.csh
g++ -Wall -O2 -g -DDEBUG_IGS_PATH_SEPARATION -I. igs_path_separation.cxx -o tes_gcc_${HOSTTYPE}_igs_path_sepa_mb
g++ -Wall -O2 -g -DDEBUG_IGS_PATH_SEPARATION -DUNICODE -I. igs_path_separation.cxx -o tes_gcc_${HOSTTYPE}_igs_path_sepa_wc
*/
