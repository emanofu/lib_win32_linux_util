#include "igs_resource_sleep.h"

/* 1second = 1,000milli_seconds(•ﬂ•Í…√) */
void igs::resource::sleep_m(const DWORD milli_seconds) {
	::Sleep(milli_seconds);
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_RESOURCE_SLEEP
#include <iostream>
# include <ctime>	// ::time(-), ctime(-)
int main(int argc, char *argv[])
{
	if (2 != argc) {
		std::cout << "Usage : " << argv[0]
			<< " milli_seconds"
			<< std::endl;
		return 1;
	}
	time_t tt=0; char bb[80];
	tt=::time(0); ::ctime_s(bb,sizeof(bb),&tt); std::cout << bb;
	igs::resource::sleep_m(atoi(argv[1]));
	tt=::time(0); ::ctime_s(bb,sizeof(bb),&tt); std::cout << bb;
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_SLEEP */
/*
rem vc2010md(32/64bits) :27,42 w! tes_msc16_igs_rsc_sleep.bat
set _DEB=DEBUG_IGS_RESOURCE_SLEEP
set _SRC=igs_resource_sleep
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_sleep_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_sleep_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
/*
rem vc2008sp1(32/64bits) :84,101 w! tes_msc15_igs_rsc_sleep.bat
set _DEB=DEBUG_IGS_RESOURCE_SLEEP
set _SRC=igs_resource_sleep
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_sleep_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_sleep_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
