#include <stdexcept> // std::domain_error()
#include <shlwapi.h> // PathFileExists(),PathIsDirectory()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_exist.h"

#include <iostream>
namespace {
 const bool not_exist_or_throw_(LPCTSTR path,const DWORD err) {
	/* path�ؤΥ���������... */
	switch (err) {
	case NO_ERROR:             /* ���ｪλ                           */
	case ERROR_FILE_NOT_FOUND: /* ���ꤵ�줿�ե����뤬���Ĥ���ޤ��� */
	case ERROR_PATH_NOT_FOUND: /* ���ꤵ�줿�ѥ������Ĥ���ޤ���     */
	case ERROR_BAD_PATHNAME:   /* ���ꤵ�줿�ѥ���̵���Ǥ�
					("\\host1\"�ǵ�����)        */
	case ERROR_ACCESS_DENIED:  /* =5 ������������ */
		return true;	/* ¸�ߤ��ʤ� */
	}
	/* ����ʳ��ϥ��顼�Ȥ�throw���� */
	throw std::domain_error(igs_resource_msg_from_err(path,err));
 }
}
const bool igs::resource::exist::file_or_directory(LPCTSTR path) {
/* 
	struct _stat t_stat;
	return (0 == ::_stat(path.c_str(), &t_stat ))? true : false;
	�Ǥϡ�
	cp_path��dir path�ΤȤ��ǡ�
	��ʸ����'/'��'\'���դ��Ƥ����_stat()��Ư���ʤ�
	��������"n:/"�ξ������
*/
/*
	PathFileExists()��File���뤤��Directory��¸�ߤ��Τ��롣
	shlwapi.h,shlwapi.lib(shlwapi.dll version 4.71 or later)
	��ɬ�ס�
*/
	if (::PathFileExists(path) != TRUE) {/* File���뤤��Directory��? */
	 if (not_exist_or_throw_(path,::GetLastError())) {/* error���� */
		return false; /* File��Directory��¸�ߤ��ʤ� */
	 }
	}
	return true; /* File���뤤��Directory�Ǥ��� */
}
const bool igs::resource::exist::file(LPCTSTR path) {
	if (::PathFileExists(path) != TRUE) {/* File���뤤��Directory��? */
	 if (not_exist_or_throw_(path,::GetLastError())) { /* error���� */
		return false; /* File��Directory��¸�ߤ��ʤ� */
	 }
	}
	/* ���λ����ǡ�File���뤤��Directory�Ǥ��� */
	if (::PathIsDirectory(path) == FALSE) { /* Directory��? */
		return true; /* Directory�Ȥ���¸�ߤ��ʤ��Τ�File�Ǥ��� */
	}
	return false; /* Directory�Ǥ��� */
}
const bool igs::resource::exist::directory(LPCTSTR path) {
/*
	2012-03-06 Windows7 vc2008sp1�ˤơ�
	::PathIsDirectory(path)��
	1. path��ͭ���ʥǥ��쥯�ȥ�ʤ� FILE_ATTRIBUTE_DIRECTORY(16)��
	   ͭ���Ǥʤ���� FALSE(0) ���֤���TRUE(1)�ϻȤ��ʤ���
	2. ���顼�Ǥ��äƤ�::GetLastError()���ͤ򥻥åȤ��ʤ��餷����
*/
	if (::PathIsDirectory(path) == FALSE) { /* Directory��? */
		return false; /* Directory�Ȥ���¸�ߤ��ʤ� */
	}
	return true; /* Directory�Ǥ��� */
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
