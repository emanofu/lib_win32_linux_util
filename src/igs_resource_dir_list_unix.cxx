#include <iostream>
#include <stdexcept>	// std::domain_error()
#include <sys/types.h>	// opendir(), readdir(), closedir()
#include <dirent.h>	// opendir(), readdir(), closedir()
#include <errno.h>	// errno
#include <cstring>	// strerror()

#include "igs_os_type.h"
#include "igs_resource_msg_from_err.h"
#include "igs_resource_dir_list.h"

void igs::resource::dir_list(
	LPCTSTR dir_path
	,std::set< std::basic_string<TCHAR> >&set_list
) {
	DIR *directory_stream = ::opendir(
		igs::resource::mbs_from_ts(dir_path).c_str()
	);
	if (NULL == directory_stream) { /* エラー時はNULLが帰る */
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path ,errno 
		));
	}
 try {
	struct dirent *directory_entry=0;
	while (1) {
	  errno = 0;
	  directory_entry = ::readdir(directory_stream);
	  if (NULL == directory_entry) {/* エラーかファイル最後でNULL返す */
	    if (0 != errno) {	/* エラー */
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("readdir(-)") ,errno 
		));
	    } else { break; }	/* ファイルの最後に達した */
	  }
	  std::string file_name( directory_entry->d_name );
	  if (("."  != file_name) && (".." != file_name)) {
	/* d_type は、rhel4,darwin8 において、DT_DIR(4)ならdirである */
		std::basic_string<TCHAR> str(
			(DT_DIR&directory_entry->d_type)?TEXT("d"):TEXT("f")
		);
		str += igs::resource::ts_from_mbs(directory_entry->d_name);
						// p->d_name[NAME_MAX]
		set_list.insert(str);
	  }
	}
 }
 catch (...) {
	try { (void)::closedir(directory_stream); } catch(...) {}
	throw; /* try内のエラーを再投 */
 }
	if (0 != ::closedir(directory_stream)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("closedir()") ,errno
		));
	}
}
/* std::sort(vec_list.begin(),vec_list.end());すればstd::setと同じ順で並ぶ */
void igs::resource::dir_list(
	LPCTSTR dir_path
	,std::vector<std::basic_string<TCHAR> >&vec_list
) {
	DIR *directory_stream = ::opendir(
		igs::resource::mbs_from_ts(dir_path).c_str()
	);
	if (NULL == directory_stream) { /* エラー時はNULLが帰る */
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path ,errno 
		));
	}
 try {
	struct dirent *directory_entry=0;
	while (1) {
	  errno = 0;
	  directory_entry = ::readdir(directory_stream);
	  if (NULL == directory_entry) {/* エラーかファイル最後でNULL返す */
	    if (0 != errno) {	/* エラー */
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("readdir(-)") ,errno 
		));
	    } else { break; }	/* ファイルの最後に達した */
	  }
	  std::string file_name( directory_entry->d_name );
	  if (("."  != file_name) && (".." != file_name)) {
	/* d_type は、rhel4,darwin8 において、DT_DIR(4)ならdirである */
		std::basic_string<TCHAR> str(
			(DT_DIR&directory_entry->d_type)?TEXT("d"):TEXT("f")
		);
		str += igs::resource::ts_from_mbs(directory_entry->d_name);
						// p->d_name[NAME_MAX]
		vec_list.push_back(str);
	  }
	}
 }
 catch (...) {
	try { (void)::closedir(directory_stream); } catch(...) {}
	throw; /* try内のエラーを再投 */
 }
	if (0 != ::closedir(directory_stream)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("closedir()") ,errno
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_DIR_LIST_UNIX
# include <iostream>
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
#if defined UNICODE
		std::cout << igs::resource::mbs_from_ts(ost.str());
#else
		std::cout << ost.str();
#endif
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
#if defined UNICODE
		std::cout << igs::resource::mbs_from_ts(ost.str());
#else
		std::cout << ost.str();
#endif
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
#if defined UNICODE
		std::cout << igs::resource::mbs_from_ts(ost.str());
#else
		std::cout << ost.str();
#endif
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
#endif  /* !DEBUG_IGS_RESOURCE_DIR_LIST_UNIX */
/*
# gcc(32/64bits)  :200,202 w! tes_gcc_igs_rsc_dir_list.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_DIR_LIST_UNIX -I. igs_resource_dir_list.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_dir_list_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_DIR_LIST_UNIX -DUNICODE -I. igs_resource_dir_list.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_dir_list_wc
*/
