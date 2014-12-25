#include <sys/stat.h>	// ::mkdir(-)
#include <sys/types.h>	// ::mkdir(-)
#include <cerrno>	// errno
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
	/* ::mkdir()は成功すれば0を返し、失敗すれば-1を返す */
	if (0!=::mkdir(igs::resource::mbs_from_ts(dir_path).c_str(),0777)) {
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path,errno
		));
	}
	/* 作成したdirectoryの存在を確認する */
	for (int retry_exist = 0 ;retry_exist<wait_times ;++retry_exist) {
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
	std::vector< std::basic_string<TCHAR> > dir_part;

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
		if (0 < ii) { dpath += TEXT("/"); }
		dpath += dir_part[ii];
		if (dpath.size() <= 0) { continue; }

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
		0	作成した
		EEXIST	既に存在している
*/
DWORD igs::resource::mkdir_for_lock(LPCTSTR dir_path) {
	if (0==::mkdir(igs::resource::mbs_from_ts(dir_path).c_str(),0777)) {
		return 0;
	}
	if (EEXIST == errno /* =17 EEXIST File exists (POSIX.1) */
	||  EACCES == errno /*     EACCES Permission denied (POSIX.1) */
	) {
		return errno;
	}
	throw std::domain_error(igs_resource_msg_from_err(
		dir_path,errno
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
# gcc(32/64bits)  :136,138 w! tes_gcc_igs_rsc_mkdir.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_MKDIR -I. igs_resource_mkdir.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_mkdir_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_MKDIR -DUNICODE -I. igs_resource_mkdir.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_mkdir_wc
*/
