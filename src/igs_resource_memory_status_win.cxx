#include <stdexcept> // std::domain_error()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_memory_status.h"

void igs::resource::memory_status(
	DWORDLONG&commit_charge_total_bytes
	, DWORDLONG&commit_charge_limit_bytes
	, DWORDLONG&total_physical_bytes
) {
	MEMORYSTATUSEX stat; ::ZeroMemory(&stat,sizeof(stat));
	stat.dwLength = sizeof(stat);
	if (0 == ::GlobalMemoryStatusEx(&stat)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("GlobalMemoryStatusEx(-)"),::GetLastError()
		));
	}
	commit_charge_total_bytes = stat.ullTotalPageFile - stat.ullAvailPageFile;
	commit_charge_limit_bytes = stat.ullTotalPageFile;
	total_physical_bytes = stat.ullTotalPhys;
}

#if defined DEBUG_IGS_RESOURCE_MEMORY_STATUS
# include <iostream>
# include <iomanip>
# include <sstream>
# include "igs_resource_msg_from_err.cxx"
int main(void)
{
  try {
	DWORDLONG commit_charge_total_bytes  = 0;
	DWORDLONG commit_charge_limit_bytes  = 0;
	DWORDLONG total_physical_bytes = 0;
	igs::resource::memory_status(
		commit_charge_total_bytes
		,commit_charge_limit_bytes
		,total_physical_bytes
	);
	std::cout
		<< "Commit Charge Total\t"
		<< std::setw(13)
		<< commit_charge_total_bytes << "Bytes"

		<< std::setw(9)
		<< commit_charge_total_bytes/1024 << "KiB"

		<< std::setw(6)
		<< commit_charge_total_bytes/1024/1024 << "MiB\n"

		<< "Commit Charge Limit\t"
		<< std::setw(13)
		<< commit_charge_limit_bytes << "Bytes"

		<< std::setw(9)
		<< commit_charge_limit_bytes/1024 << "KiB"

		<< std::setw(6)
		<< commit_charge_limit_bytes/1024/1024 << "MiB\n"

		<< "Total Physical\t\t"
		<< std::setw(13)
		<< total_physical_bytes << "Bytes"

		<< std::setw(9)
		<< total_physical_bytes/1024 << "KiB"

		<< std::setw(6)
		<< total_physical_bytes/1024/1024 << "MiB\n"

		<< "Commit Charge Total / Total Physical\t"
		<< std::setw(3)
		<< commit_charge_total_bytes*100/total_physical_bytes
			<< "%"

		<< std::endl
		;
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_MEMORY_STATUS */
/*
rem vc2008sp1(32/64bits) :83,100 w! tes_msc_igs_rsc_memory_st.bat
set _DEB=DEBUG_IGS_RESOURCE_MEMORY_STATUS
set _SRC=igs_resource_memory_status
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_memory_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_memory_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
