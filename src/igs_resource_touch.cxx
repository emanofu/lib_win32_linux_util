#include <stdexcept>
#include "igs_resource_msg_from_err.h"
#include "igs_resource_touch.h"

#if defined _WIN32
void igs::resource::touch(LPCTSTR file_path) {
	HANDLE handle = ::CreateFile(
		file_path ,GENERIC_WRITE ,0/* =共有しない */ ,NULL
		,OPEN_EXISTING ,FILE_ATTRIBUTE_NORMAL ,NULL
	);
	if (handle == INVALID_HANDLE_VALUE) {
		const DWORD err = ::GetLastError();
/*
	2012-09-01
	ERROR_SHARING_VIOLATION
	"プロセスはファイルにアクセスできません。別のプロセスが使用中です。"
	hosts.txtのtouchのみで使用している。
	このエラーが頻繁に起きる
if (ERROR_SHARING_VIOLATION == err) { return; }
*/
		throw std::domain_error( igs_resource_msg_from_err(
			file_path ,err
		));
	}
	SYSTEMTIME stFileTime;
	::GetSystemTime( &stFileTime );
	FILETIME ftFileTime;
	if (0 == ::SystemTimeToFileTime( &stFileTime ,&ftFileTime )) {
		throw std::domain_error( igs_resource_msg_from_err(
			file_path ,::GetLastError()
		));
	}
	/*
		modify時刻を更新する
		第2引数	Creation	作成日時
		第3引数	LastAccess	最終アクセス日時
		第4引数	LastWrite	更新日時
	*/
	if (0 == ::SetFileTime( handle ,NULL ,NULL ,&ftFileTime )) {
		throw std::domain_error( igs_resource_msg_from_err(
			file_path ,::GetLastError()
		));
	}

	if (0 == ::CloseHandle(handle)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path ,::GetLastError()
		));
	}
}
#else
#include <sys/types.h>
#include <utime.h>
#include <cerrno>
void igs::resource::touch(const char*file_path) {
	/* accessとmodify時刻を更新する */
	if (::utime( file_path, NULL ) < 0) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path ,errno
		));
	}
}
#endif

#if defined DEBUG_IGS_RESOURCE_TOUCH
#include <iostream>
#include "igs_resource_sleep.cxx"
#include "igs_resource_msg_from_err.cxx"
int main(int argc, char *argv[]) {
	if (2 != argc) {
		std::cout
			<< "Usage : " << argv[0] << " file_path\n"
			<< std::endl;
		return 1;
	}
# if defined _WIN32
	std::basic_string<TCHAR> file_path(
		igs::resource::ts_from_mbs(argv[1])
	);
# else
	std::string file_path( argv[1] );
# endif

#  if defined UNICODE
	std::wcout
#  else
	std::cout
#  endif
		<< file_path << std::endl;
 try {
	for (unsigned ii=0; ii<60; ++ii) {
#  if defined UNICODE
	std::wcout << TEXT(".");
#  else
	std::cout << '.' << std::flush;
#  endif
		igs::resource::touch( file_path.c_str() );
		igs::resource::sleep_m(1000); // 1sec
	}
 }
 catch (std::exception&e) {
	std::cout << e.what() << std::endl;
 }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_TOUCH */
/*
rem vc2008sp1(32/64bits) :102,119 w! tes_msc_igs_rsc_touch.bat
set _DEB=DEBUG_IGS_RESOURCE_TOUCH
set _SRC=igs_resource_touch
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_touch
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_touch_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :120,121 w! tes_gcc_igs_rsc_touch.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_TOUCH -I. igs_resource_touch.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_touch
*/
