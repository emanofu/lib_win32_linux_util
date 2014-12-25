#include <windows.h>	/* DWORD,GetUserName() */
#include <Lmcons.h>	/* UNLEN */
#include <stdexcept> /* std::domain_error */
#include "igs_resource_msg_from_err.h"
#include "igs_resource_user_name.h"

/* アプリ内の共通文字定義 */

void igs::resource::user_name(std::basic_string<TCHAR>&name)
{
	/****** 環境変数からユーザー名をとる
	const char *const cp_user_env = "USERNAME";
	igs::resource::getenv(cp_user_env,name);
	******/

	/*	現在のスレッドのユーザー名を取得します。
		この名前は、システムに既にログオンしているユーザー
		の名前を表します。
	*/
	/*	loginせず(?)実行すると(?)
		環境変数からuser_nameがみつからない
		Advapi32.libをリンクすること
	*/
	DWORD nSize =  UNLEN+1;
	TCHAR lpBuffer[UNLEN+1]; /* UNLEN は Lmcons.h 内で定義 */
	lpBuffer[0] = '\0';
	if ((0 == ::GetUserName(lpBuffer,&nSize)) || (nSize <= 1)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("GetUserName(-)"),::GetLastError()
		));
	}
	name = lpBuffer;
}
#if defined DEBUG_IGS_RESOURCE_USER_NAME
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (1 != argc) {
		std::cout << "Usage " << argv[0] << std::endl;
		return 1;
	}
  try {
	std::basic_string<TCHAR> name;
	igs::resource::user_name(name);
	std::TCOUT << TEXT("user_name \"") << name << TEXT("\"\n");
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_USER_NAME */
/*
rem vc2010(32/64bits) :54,69 w! tes_msc16_igs_rsc_user_name.bat
set _DEB=DEBUG_IGS_RESOURCE_USER_NAME
set _SRC=igs_resource_user_name
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_user_name_mb
set _INC=.
set _LIB=advapi32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_user_name_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :110,127 w! tes_msc15_igs_rsc_user_name.bat
set _DEB=DEBUG_IGS_RESOURCE_USER_NAME
set _SRC=igs_resource_user_name
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_user_name_mb
set _INC=.
set _LIB=advapi32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_user_name_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
