#include <sys/types.h>	// stat()
#include <sys/stat.h>	// stat()
#include <unistd.h>	// stat()
#include <cerrno>	// errno
#include <cstring>	// memset()
#include <stdexcept>	// std::domain_error()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_exist.h"

namespace {
 const bool exist_file_or_directory_(LPCTSTR path,struct stat&t_stat) {
	if (0 != ::stat(igs::resource::mbs_from_ts(path).c_str(),&t_stat)) {
	 if (ENOENT == errno) {return false;}/* No such file or directory */
	 throw std::domain_error(igs_resource_msg_from_err( path,errno ));
	}
	return true;
 }
}
const bool igs::resource::exist::file_or_directory(LPCTSTR path) {
	struct stat t_stat; ::memset(&t_stat,0,sizeof(t_stat));
 	if (!exist_file_or_directory_(path,t_stat)) { return false; }
	return (
		S_ISREG(t_stat.st_mode) || S_ISDIR(t_stat.st_mode)
	)? true: false; /* ファイルあるいはディレクトリならtrue */
}
const bool igs::resource::exist::file(LPCTSTR path) {
	struct stat t_stat; ::memset(&t_stat,0,sizeof(t_stat));
 	if (!exist_file_or_directory_(path,t_stat)) { return false; }
	return (S_ISREG(t_stat.st_mode))? true: false;/* 通常のファイルか */
}
const bool igs::resource::exist::directory(LPCTSTR path) {
	struct stat t_stat; memset(&t_stat,0,sizeof(t_stat));
 	if (!exist_file_or_directory_(path,t_stat)) { return false; }
	return (S_ISDIR(t_stat.st_mode))? true: false;/* ディレクトリか */
}

#if defined DEBUG_IGS_RESOURCE_EXIST
#include <iostream>
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
  catch (std::exception& e) {
	std::cerr << "exception=\"" << e.what() << "\"\n";
  }
  catch (...) {
	std::cerr << "other exception\n";
  }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_EXIST */
/*
# gcc(32/64bits)  :85,87 w! tes_gcc_igs_rsc_exist.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_EXIST -I. igs_resource_exist.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_exist_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_EXIST -DUNICODE -I. igs_resource_exist.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_exist_wc
*/
