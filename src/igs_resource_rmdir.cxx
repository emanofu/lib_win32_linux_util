/* ディレクトリ1個削除
directoryがない、removeできない、空でないディレクトリが消せない
とき、エラー例外を投げる */
#if defined _WIN32 //-------------------------------------------------
# include <stdexcept>	// std::domain_error
# include "igs_resource_msg_from_err.h"
# include "igs_resource_rmdir.h"
void igs::resource::rmdir(LPCTSTR dir_path) {
	if (0 == ::RemoveDirectory( dir_path )) { // kernel32.lib
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path,::GetLastError()
		));
	}
}
#else //--------------------------------------------------------------
# include <unistd.h> // rmdir(-)
# include <cerrno>
# include <stdexcept>	// std::domain_error
# include "igs_resource_msg_from_err.h"
# include "igs_resource_rmdir.h"
void igs::resource::rmdir(LPCTSTR dir_path) {
	/* rmdir()は成功すれば0を返し、失敗すれば-1を返す */
	if (0 != ::rmdir(igs::resource::mbs_from_ts(dir_path).c_str())) {
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path,errno
		));
	}
}
#endif //-------------------------------------------------------------

#if defined DEBUG_IGS_RESOURCE_RMDIR
# include "igs_resource_msg_from_err.cxx"
# include <iostream>
int main( int argc, char *argv[] )
{
	if (2 != argc) {
		std::cout << "Usage " << argv[0] << " dirpath\n";
		return 1;
	}
  try {
	igs::resource::rmdir(igs::resource::ts_from_mbs(argv[1]).c_str());
  }
  catch (std::exception& e) {
	std::cerr << "exception=\"" << e.what() << "\"\n";
  }
  catch (...) {
	std::cerr << "other exception\n";
  }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_RMDIR */
/*
rem vc2008sp1(32/64bits) :66,83 w! tes_msc_igs_rsc_rmdir.bat
set _DEB=DEBUG_IGS_RESOURCE_RMDIR
set _SRC=igs_resource_rmdir
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_rmdir_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_rmdir_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :86,88 w! tes_gcc_igs_rsc_rmdir.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_RMDIR -I. igs_resource_rmdir.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_rmdir_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_RMDIR -DUNICODE -I. igs_resource_rmdir.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_rmdir_wc
*/
