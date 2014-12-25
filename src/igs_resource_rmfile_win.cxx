/*
ファイル1個削除
ファイルがない、削除できない、とき、エラー例外を投げる
*/
# include <stdexcept> // std::domain_error
# include "igs_resource_msg_from_err.h"
# include "igs_resource_rmfile.h"

/* fileがなくてもremoveできなくてもエラー例外を投げる */
void igs::resource::rmfile(LPCTSTR file_path) {
	if (0 == ::DeleteFile(file_path)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_RMFILE
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (2 != argc) {
		std::cout << "Usage " << argv[0] << " file_path\n";
		return 1;
	}
  try {
	igs::resource::rmfile(igs::resource::ts_from_mbs(argv[1]).c_str());
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_RMFILE */
/*
rem vc2010(32/64bits) :41,56 w! tes_msc16_igs_rsc_rmfile.bat
set _DEB=DEBUG_IGS_RESOURCE_RMFILE
set _SRC=igs_resource_rmfile
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_rmfile_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_rmfile_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
/*
rem vc2008sp1(32/64bits) :67,84 w! tes_msc15_igs_rsc_rmfile.bat
set _DEB=DEBUG_IGS_RESOURCE_RMFILE
set _SRC=igs_resource_rmfile
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_rmfile_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_rmfile_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
