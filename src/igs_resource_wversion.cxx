#include "igs_resource_wversion.h"
#include "igs_resource_msg_from_err.h"

/* for ver 5.0.0(Windows2000) or later */
void igs::resource::wversion(
	int&major_ver
	,int&minor_ver
	,int&build_number
	,std::basic_string<TCHAR>&ver_str
) {
	OSVERSIONINFOEX osv; ::ZeroMemory(&osv, sizeof(OSVERSIONINFOEX));
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osv.szCSDVersion[0] = 0;
	if (0 == ::GetVersionEx((LPOSVERSIONINFO)&osv)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("GetVersionEx(-)"),::GetLastError()
		));
	}
	major_ver = osv.dwMajorVersion;
	minor_ver = osv.dwMinorVersion;
	build_number = osv.dwBuildNumber;

	switch (osv.dwMajorVersion) {
	case 6:
		switch (osv.dwMinorVersion) {
		case 1:
			if (VER_NT_WORKSTATION == osv.wProductType) {
				ver_str += TEXT("Windows 7");
			} else {
				ver_str += TEXT("Windows Server 2008 R2");
			}
			break;
		case 0:
			if (VER_NT_WORKSTATION != osv.wProductType) {
				ver_str += TEXT("Windows Server 2008");
			} else {
				ver_str += TEXT("Windows Vista");
			}
			break;
		}
		break;
	case 5:
		switch (osv.dwMinorVersion) {
		case 2: {
			SYSTEM_INFO sysi;
			::ZeroMemory(&sysi,sizeof(SYSTEM_INFO));
			::GetSystemInfo(&sysi);

			if (0 != ::GetSystemMetrics(SM_SERVERR2)) {
				ver_str += TEXT("Windows Server 2003 R2");
/***
			} else if (osv.wSuiteMask & VER_SUITE_WH_SERVER) {
				ver_str += TEXT("Windows Home Server");
***/
			} else if (0 == ::GetSystemMetrics(SM_SERVERR2)) {
				ver_str += TEXT("Windows Server 2003");
			} else if (VER_NT_WORKSTATION == osv.wProductType
			&&   PROCESSOR_ARCHITECTURE_AMD64 ==
			sysi.wProcessorArchitecture) {
				ver_str += TEXT("Windows Xp Professional x64 Edition");
			}
		}
			break;
		case 1:
			ver_str += TEXT("Windows Xp");
			break;
		case 0:
			ver_str += TEXT("Windows 2000");
			break;
		}
		break;
	}
	if (0 != osv.szCSDVersion[0]) {
		ver_str += TEXT(' ');
		ver_str += osv.szCSDVersion;
	}
}
#if defined DEBUG_IGS_RESOURCE_WVERSION
#include <sstream>
#include <iostream>
#include "igs_resource_msg_from_err.cxx"
int main(void) {
	int major_ver = 0;
	int minor_ver = 0;
	int build_number = 0;
	std::basic_string<TCHAR> ver_str;

	igs::resource::wversion(major_ver,minor_ver,build_number,ver_str);

	std::basic_ostringstream<TCHAR> ost;
	ost	<< major_ver
		<< TEXT('.') << minor_ver
		<< TEXT('.') << build_number
		<< TEXT(' ') << ver_str
		;

#if defined UNICODE
	std::wcout
#else
	std::cout
#endif
		<< ost.str() << std::endl;

	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_WVERSION */
/*
rem vc2008sp1(32/64bits) :104,121 w! tes_msc_igs_rsc_wversion.bat
set _DEB=DEBUG_IGS_RESOURCE_WVERSION
set _SRC=igs_resource_wversion
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_wver
set _INC=.
set _LIB=user32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_wver_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
