#include <stdexcept>	// std::domain_error
#include <string>	// std::string
#include <fstream>	// std::ofstream
#include "igs_resource_msg_from_err.h"
#include "igs_resource_append_text_by_atom.h"
namespace igs {
 namespace resource {
  void append_text_by_atom(
	const std::basic_string<TCHAR>&file_path
	,const std::string&str
  ) {
	/* Linux(rhel4)では、
		UNIXにおける操作の不可分性により、
		"std::ofstream"で、別processで２箇所から(追加)保存は、
		問題ない。すべて正常に保存される。
	*/
	std::ofstream ofs;
	const std::string filepath( igs::resource::mbs_from_ts(file_path) );
	ofs.open( filepath.c_str(), std::ios::out|std::ios::app );
	if (!ofs) {
		std::basic_string<TCHAR> err;
		err += file_path; err += TEXT(":can not open");
		throw std::domain_error(igs_resource_msg_from_er( err ));
	}
	ofs << str;
	if (!ofs) {
		ofs.close();
		std::basic_string<TCHAR> err;
		err += file_path; err += TEXT(":can not write");
		throw std::domain_error(igs_resource_msg_from_er( err ));
	}
	/* ファイルはこのmethodを抜けると自動的に閉じるが意識的に閉る */
	ofs.close();
  }
 }
}
#if defined DEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM
# include <iostream>
# include <vector>
# include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] ) {
	if (4 != argc) {
		std::cout
		<< "Usage : " << argv[0]
		<< " file wstr loop\n"
		<< "wstr    : save string\n"
		<< "loop    : 500 loop count\n"
		;
		return 1;
	}
 try {
	/*
		binary保存なので、改行コードは'\n'そのまま保存される
		Windowsにてテキスト保存する時は、
		UNIX改行コード'\n'
		でなく
		DOS改行コード"\r\n"(CRLF)(0x0d0x0a)"
		を入れること。
	*/
	std::string str(argv[2]); str += '\n';
	const int loop_count			= atoi(argv[3]);
	for (int ii=0; ii < loop_count; ++ii) {
		igs::resource::append_text_by_atom(
			igs::resource::ts_from_mbs(argv[1])
			,str
		);
	}
 }
 catch (std::exception& e) { std::cerr << e.what() << std::endl; }
 catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM */
/*
# gcc(32/64bits) :75,77 w! tes_gcc_igs_rsc_append_text_by_atom.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM -I. igs_resource_append_text_by_atom.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_append_text_by_atom_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM -DUNICODE -I. igs_resource_append_text_by_atom.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_append_text_by_atom_wc
*/
