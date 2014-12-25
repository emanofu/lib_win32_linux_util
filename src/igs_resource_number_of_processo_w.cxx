#include <windows.h>
#include "igs_resource_number_of_processors.h"

unsigned int igs::resource::number_of_processors(void) {
	SYSTEM_INFO sysinfo; ::ZeroMemory(&sysinfo,sizeof(sysinfo));
	::GetSystemInfo(&sysinfo);  // Return nothing.
	/*
		typedef unsigned long DWORD; <-- from MSDN vs2008
		windows7 64bits,unsigned long is 32bits
		rhel5    64bits,unsigned long is 64bits
	*/
	return sysinfo.dwNumberOfProcessors;
}

#if defined DEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS
#include <iostream>
int main(void) {
	std::cout << "number_of_processors="
		<< igs::resource::number_of_processors()
		<< std::endl;
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS */
/*
rem vc2008sp1(32/64bits) :25,42 w! tes_msc_igs_rsc_n_of_p.bat
set _DEB=DEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS
set _SRC=igs_resource_number_of_processors
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_n_of_p_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_n_of_p_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
