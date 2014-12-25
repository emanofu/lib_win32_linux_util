#include <windows.h>
#include <stdexcept> // std::domain_error(-)
#include <algorithm> // std::sort
#include "igs_resource_msg_from_err.h"
#include "igs_resource_dir_list.h"

namespace {
 void set_list_(
	WIN32_FIND_DATA&fd
	, std::set< std::basic_string<TCHAR> > &set_list
 ) {
	std::basic_string<TCHAR> file_name( fd.cFileName );
	if ((TEXT(".")  != file_name) && (TEXT("..") != file_name)
	  && !(fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN) /* for admin */
	  && !(fd.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM) /* for OS */
	) {
		std::basic_string<TCHAR> str(
			(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			? TEXT("d")
			: TEXT("f")
		);
		str += fd.cFileName;
		set_list.insert(str);
	}
 }
}
void igs::resource::dir_list(
	LPCTSTR dir_path
	,std::set< std::basic_string<TCHAR> >&set_list
) {
	HANDLE hSearch=0;
	WIN32_FIND_DATA fd; ::ZeroMemory(&fd,sizeof(fd));
	std::basic_string<TCHAR> find_path(dir_path);
	find_path += TEXT("\\*");
	hSearch = ::FindFirstFile( find_path.c_str(), &fd );
	if (INVALID_HANDLE_VALUE == hSearch) {
		throw std::domain_error(igs_resource_msg_from_err(
			find_path,::GetLastError()
		));
	}
	set_list_( fd, set_list );
  try {
   for (;;) {
	if ( !::FindNextFile( hSearch, &fd ) ) {
	 const DWORD dw = ::GetLastError();
	 if (ERROR_NO_MORE_FILES == dw) { break; }
	 else {
		throw std::domain_error(igs_resource_msg_from_err(
			find_path,dw
		));
	 }
	}
	set_list_( fd, set_list );
   }
  }
  catch(...) {
	::FindClose( hSearch ); /* このエラーは無視 */
	throw; /* try内のエラーを再投 */
  }
	if (!::FindClose( hSearch )) {
		throw std::domain_error(igs_resource_msg_from_err(
			find_path,::GetLastError()
		));
	}
}
namespace {
 void set_list_(
	WIN32_FIND_DATA&fd
	,std::vector< std::basic_string<TCHAR> >&vec_list
 ) {
	std::basic_string<TCHAR> file_name( fd.cFileName );
	if ((TEXT(".")  != file_name) && (TEXT("..") != file_name)
	  && !(fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN) /* for admin */
	  && !(fd.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM) /* for OS */
	) {
		std::basic_string<TCHAR> str(
			(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			? TEXT("d")
			: TEXT("f")
		);
		str += fd.cFileName;
		vec_list.push_back(str);
	}
 }
}
/* std::sort(vec_list.begin(),vec_list.end());すればstd::setと同じ順で並ぶ */
void igs::resource::dir_list(
	LPCTSTR dir_path
	,std::vector< std::basic_string<TCHAR> >&vec_list
) {
	HANDLE hSearch=0;
	WIN32_FIND_DATA fd; ::ZeroMemory(&fd,sizeof(fd));
	std::basic_string<TCHAR> find_path(dir_path);
	find_path += TEXT("\\*");
	hSearch = ::FindFirstFile( find_path.c_str(), &fd );
	if (INVALID_HANDLE_VALUE == hSearch) {
		throw std::domain_error(igs_resource_msg_from_err(
			find_path,::GetLastError()
		));
	}
	set_list_( fd, vec_list );
 try {
   for (;;) {
	if ( !::FindNextFile( hSearch, &fd ) ) {
	 const DWORD dw = ::GetLastError();
	 if (ERROR_NO_MORE_FILES == dw) { break; }
	 else {
		throw std::domain_error(igs_resource_msg_from_err(
			find_path,dw
		));
	 }
	}
	set_list_( fd, vec_list );
   }
 }
 catch(...) {
	::FindClose( hSearch ); /* このエラーは無視 */
	throw; /* try内のエラーを再投 */
 }
	if (!::FindClose( hSearch )) {
		throw std::domain_error(igs_resource_msg_from_err(
			find_path,::GetLastError()
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_DIR_LIST_WIN
# include <iostream>
# include <string>
# include "igs_resource_msg_from_err.cxx"
int main(int argc,char *argv[])
{
	if (2 != argc) {
		std::cout << "Usage : " << argv[0] << " dir_path\n";
		return 1;
	}
 try {
	igs::resource::locale_to_jp();

	std::cout << "dir_path " << '\"' << argv[1] << '\"' << std::endl;

	std::set< std::basic_string<TCHAR> > set_list;
	igs::resource::dir_list(
		igs::resource::ts_from_mbs(argv[1]).c_str()
		,set_list
	);
	std::cout << "std::set size=" << set_list.size() << std::endl;

	int ii=0;
	for (std::set< std::basic_string<TCHAR> >::iterator
	it = set_list.begin(); set_list.end() != it; ++it ,++ii
	) {
		std::basic_ostringstream<TCHAR> ost;
		ost	<< ii << TEXT('/') << set_list.size()
			<< TEXT('\t') << it->at(0)
			<< TEXT(' ') << it->size()
			<< TEXT('\t') << TEXT('\"')
			<< &(it->at(1))
			<< TEXT("\"\n")
			;
		std::TCOUT << ost.str();
	}

	std::vector< std::basic_string<TCHAR> > vec_list;
	igs::resource::dir_list(
		igs::resource::ts_from_mbs(argv[1]).c_str()
		,vec_list
	);
	std::cout << "std::vector size=" << vec_list.size() << std::endl;
	for (unsigned ii = 0 ;ii < vec_list.size() ;++ii) {
		const std::basic_string<TCHAR>&str( vec_list.at(ii) );
		std::basic_ostringstream<TCHAR> ost;
		ost	<< ii << TEXT('/') << vec_list.size()
			<< TEXT('\t') << str.at(0)
			<< TEXT(' ') << str.size()
			<< TEXT('\t') << TEXT('\"')
			<< &(str.at(1))
			<< TEXT("\"\n")
			;
		std::TCOUT << ost.str();
	}

	std::sort( vec_list.begin(),vec_list.end() );
	std::cout << "std::vector std::sort size=" << vec_list.size()
							<< std::endl;
	for (unsigned ii = 0 ;ii < vec_list.size() ;++ii) {
		const std::basic_string<TCHAR>&str( vec_list.at(ii) );
		std::basic_ostringstream<TCHAR> ost;
		ost	<< ii << TEXT('/') << vec_list.size()
			<< TEXT('\t') << str.at(0)
			<< TEXT(' ') << str.size()
			<< TEXT('\t') << TEXT('\"')
			<< &(str.at(1))
			<< TEXT("\"\n")
			;
		std::TCOUT << ost.str();
	}
 }
 catch (std::exception& e) {
	std::cerr << "exception=" << e.what() << std::endl;
	return 2;
 }
 catch (...) {
	std::cerr << "other exception" << std::endl;
	return 3;
 }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_DIR_LIST_WIN */
/*
rem vc2010(32/64bits) :210,225 w! tes_msc16_igs_rsc_dir_list.bat
set _DEB=DEBUG_IGS_RESOURCE_DIR_LIST_WIN
set _SRC=igs_resource_dir_list
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_dir_list_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_dir_list_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
/*
rem vc2008sp1(32/64bits) :210,227 w! tes_msc15_igs_rsc_dir_list.bat
set _DEB=DEBUG_IGS_RESOURCE_DIR_LIST_WIN
set _SRC=igs_resource_dir_list
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_dir_list_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_dir_list_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
