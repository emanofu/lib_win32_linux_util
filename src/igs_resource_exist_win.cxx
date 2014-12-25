#include <stdexcept> // std::domain_error()
#include <shlwapi.h> // PathFileExists(),PathIsDirectory()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_exist.h"

#include <iostream>
namespace {
 const bool not_exist_or_throw_(LPCTSTR path,const DWORD err) {
	/* pathへのアクセスが... */
	switch (err) {
	case NO_ERROR:             /* 正常終了                           */
	case ERROR_FILE_NOT_FOUND: /* 指定されたファイルが見つかりません */
	case ERROR_PATH_NOT_FOUND: /* 指定されたパスが見つかりません     */
	case ERROR_BAD_PATHNAME:   /* 指定されたパスは無効です
					("\\host1\"で起きる)        */
	case ERROR_ACCESS_DENIED:  /* =5 アクセス拒否 */
		return true;	/* 存在しない */
	}
	/* それ以外はエラーとしthrowする */
	throw std::domain_error(igs_resource_msg_from_err(path,err));
 }
}
const bool igs::resource::exist::file_or_directory(LPCTSTR path) {
/* 
	struct _stat t_stat;
	return (0 == ::_stat(path.c_str(), &t_stat ))? true : false;
	では、
	cp_pathがdir pathのときで、
	末文字に'/'か'\'が付いていると_stat()は働かない
	ただし、"n:/"の場合を除く
*/
/*
	PathFileExists()はFileあるいはDirectoryの存在を感知する。
	shlwapi.h,shlwapi.lib(shlwapi.dll version 4.71 or later)
	が必要。
*/
	if (::PathFileExists(path) != TRUE) {/* FileあるいはDirectoryか? */
	 if (not_exist_or_throw_(path,::GetLastError())) {/* error処理 */
		return false; /* FileもDirectoryも存在しない */
	 }
	}
	return true; /* FileあるいはDirectoryである */
}
const bool igs::resource::exist::file(LPCTSTR path) {
	if (::PathFileExists(path) != TRUE) {/* FileあるいはDirectoryか? */
	 if (not_exist_or_throw_(path,::GetLastError())) { /* error処理 */
		return false; /* FileもDirectoryも存在しない */
	 }
	}
	/* この時点で、FileあるいはDirectoryである */
	if (::PathIsDirectory(path) == FALSE) { /* Directoryか? */
		return true; /* Directoryとして存在しないのでFileである */
	}
	return false; /* Directoryである */
}
const bool igs::resource::exist::directory(LPCTSTR path) {
/*
	2012-03-06 Windows7 vc2008sp1にて、
	::PathIsDirectory(path)は
	1. pathが有効なディレクトリなら FILE_ATTRIBUTE_DIRECTORY(16)、
	   有効でなければ FALSE(0) を返す。TRUE(1)は使えない。
	2. エラーであっても::GetLastError()に値をセットしないらしい。
*/
	if (::PathIsDirectory(path) == FALSE) { /* Directoryか? */
		return false; /* Directoryとして存在しない */
	}
	return true; /* Directoryである */
}

#if defined DEBUG_IGS_RESOURCE_EXIST
#include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (3 != argc) {
		std::cout << "Usage : " << argv[0]
			<< " file_or_directory" << " file/dir/filedir"
			<< "\n";
		return 1;
	}
  try {
	const char* path = argv[1];
	std::basic_string<TCHAR> str(igs::resource::ts_from_mbs(path));
	if (!strcmp("file",argv[2])) {
	 if (igs::resource::exist::file(str.c_str())) {
		std::cout << path << " is exist as file\n";
	 } else {
		std::cout << path << " is NOT exist as file\n";
	 }
	} else
	if (!strcmp("dir",argv[2])) {
	 if (igs::resource::exist::directory(str.c_str())) {
		std::cout << path << " is exist as directory\n";
	 } else {
		std::cout << path << " is NOT exist as directory\n";
	 }
	} else
	if (!strcmp("filedir",argv[2])) {
	 if (igs::resource::exist::file_or_directory(str.c_str())) {
		std::cout << path << " is exist as file or directory\n";
	 } else {
		std::cout << path << " is NOT exist as file or directory\n";
	 }
	} else {
		std::cout << "bad argument \"" << argv[2] << "\"\n";
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_EXIST */
/*
rem vc2010(32/64bits) :113,128 w! tes_msc16_igs_rsc_exist.bat
set _DEB=DEBUG_IGS_RESOURCE_EXIST
set _SRC=igs_resource_exist
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_exist_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_exist_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
/*
rem vc2008sp1(32/64bits) :112,129 w! tes_msc15_igs_rsc_exist.bat
set _DEB=DEBUG_IGS_RESOURCE_EXIST
set _SRC=igs_resource_exist
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_exist_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_exist_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
