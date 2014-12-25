#include <iostream>	// std::cout
#include <vector>	// std::vector
#include <string>	// std::basic_string<TCHAR>
#include <sstream>	// std::basic_ostringstream<TCHAR>
#include <stdexcept>	// std::domain_error
#include "igs_resource_msg_from_err.h"
#include "igs_resource_exist.h"
#include "igs_resource_mkdir.h"
#include "igs_resource_sleep.h"

/* ディレクトリ1個作成し存在も確認して返る、孫は直接はつくれない */
void igs::resource::mkdir_certain(
	LPCTSTR dir_path
	,DWORD wait_milli_seconds	/* =  10(0.01秒) */
	,int wait_times			/* =6000(60秒) */
) {
	if (0 == ::CreateDirectory(dir_path ,NULL)) {
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path ,::GetLastError()
		));
	}
/*
	CreateDirectory(-)のあと実際に存在したか確認をとろうとすると、
	その間の瞬間に、他のプロセスのRemoveDirectoy(-)を実行してしまうと、
	存在確認Loopが終了しなくなり、エラーを投げてしまう、、のでは、、
	今のところエラーは起きないので保留
	2012-10-19
*/
	/* 作成したdirectoryの存在を確認する */
	for (int retry_exist=0 ;retry_exist<wait_times ;++retry_exist) {
		if (igs::resource::exist::directory(dir_path)) {
			return;
		}
		igs::resource::sleep_m(wait_milli_seconds);
	}
	/* 回数(wait_times)繰り返して、それでもだめならTimeOut
	エラーとして例外を投げる */
	std::basic_ostringstream<TCHAR> ost;
	ost	<< TEXT("mkdir_certain() but not mkdir")
		<< TEXT(" \"") << dir_path << TEXT('\"')
		<< TEXT(' ') << wait_times << TEXT("times/loop")
		<< TEXT(' ') << wait_milli_seconds << TEXT("msec/time")
		;
	throw std::domain_error(igs_resource_msg_from_er(ost.str()));
}

/* ディレクトリを掘り進む、孫以下も指定によりすべて作る */
void igs::resource::mkdir_descendant(LPCTSTR dir_path)
{
	std::basic_string<TCHAR> dirpath(dir_path);
	std::vector<std::basic_string<TCHAR>> dir_part;

	std::basic_string<TCHAR>::size_type before_index = 0;
	std::basic_string<TCHAR>::size_type index = 0;
	for (;std::basic_string<TCHAR>::npos!=index ;before_index=++index) {
		index = dirpath.find_first_of(TEXT("/\\"),before_index);
		if (index == before_index) { /* 区切り文字が連続した */
			dir_part.push_back(TEXT(""));
		} else if (std::basic_string<TCHAR>::npos == index) {
			dir_part.push_back(dirpath.substr(before_index));
			break; /* 区切り文字がもうないなら最後の部分 */
		} else {
			dir_part.push_back(dirpath.substr(
				before_index,index-before_index
			));
		}
	}

	std::basic_string<TCHAR> dpath;
	for (unsigned ii = 0; ii < dir_part.size(); ++ii) {
		if (0 < ii) { dpath += TEXT("\\"); }
		dpath += dir_part[ii];
		if (dpath.size() <= 0) { continue; }

		/* "\\host1"を存在チェックしないように... */
		if ((2 == ii) &&
		(0 == dir_part[0].size()) &&
		(0 == dir_part[1].size())) { continue; }

		if (igs::resource::exist::file_or_directory(dpath.c_str())){
#if defined DEBUG_IGS_RESOURCE_MKDIR
std::TCOUT << TEXT("exist=\"") << dpath << TEXT("\"") << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_MKDIR */
			continue;
		}
#if defined DEBUG_IGS_RESOURCE_MKDIR
std::TCOUT << TEXT("mkdir=\"") << dpath << TEXT("\"") << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_MKDIR */
		igs::resource::mkdir_certain(dpath.c_str());
	}
}
/* lockするためにディレクトリ1個作成(孫は直接はつくれない)
	戻り値は
		ERROR_SUCCESS		=0   作成した
		ERROR_ALREADY_EXISTS	=183 既に存在する
		ERROR_ACCESS_DENIED	=5   アクセス拒否
		ERROR_NETWORK_ACCESS_DENIED = 64
			指定されたネットワーク名は利用できません
	の三つ。
	これ以外のエラーは例外をthrowする */
DWORD igs::resource::mkdir_for_lock(LPCTSTR dir_path)
{
	/*
		MSDNより
		パス名の文字列の最大の長さは、既定では 248 文字です。
		この制限は、関数がパスを分析する方法にも関係しています。
		この制限をほぼ 32,000 ワイド文字へ拡張するには、
		この関数の Unicode 版を呼び出し、
		パスの前に "\\?\" という接頭辞を追加してください。
		shlwapi.h
		shlwapi.lib
	*/
	if (0 != ::CreateDirectory(dir_path ,NULL)) {
		return ERROR_SUCCESS;		/* =0   作成した	*/
	}
	const DWORD err = ::GetLastError();
	if ((ERROR_ALREADY_EXISTS == err)	/* =183 既に存在する	*/
	||  (ERROR_ACCESS_DENIED  == err)	/* =5   アクセス拒否	*/
	||  (ERROR_NETWORK_ACCESS_DENIED == err)
					/* =64  ネットワーク名利用不可	*/
	) {
		return err;
	}
	throw std::domain_error(igs_resource_msg_from_err(
		dir_path ,err
	));
}

#if defined DEBUG_IGS_RESOURCE_MKDIR
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
# include "igs_resource_exist.cxx"
# include "igs_resource_sleep.cxx"
int main( int argc, char *argv[] )
{
	if (2 != argc) {
		std::cout << "Usage " << argv[0] << " newdirpath\n";
		std::cout << "Exsample\n";
		std::cout << "  C:\\Users\\user1\\tmp\n";
		std::cout << "  C:/Users/user1/tmp\n";
		std::cout << "  \\\\host1\\Users\\user1\\tmp\n";
		std::cout << "  //host1/Users/user1/tmp\n";
		return 1;
	}
  try {
	igs::resource::mkdir_descendant(igs::resource::ts_from_mbs(argv[1]).c_str());
  }
  catch (std::exception& e) {
	std::cerr << "exception=\"" << e.what() << "\"\n";
  }
  catch (...) {
	std::cerr << "other exception\n";
  }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_MKDIR */
/*
rem vc2010(32/64bits) :163,178 w! tes_msc16_igs_rsc_mkdir.bat
set _DEB=DEBUG_IGS_RESOURCE_MKDIR
set _SRC=igs_resource_mkdir
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :148,165 w! tes_msc15_igs_rsc_mkdir.bat
set _DEB=DEBUG_IGS_RESOURCE_MKDIR
set _SRC=igs_resource_mkdir
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
