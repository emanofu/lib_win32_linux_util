#include <fstream>	// std::ifstream
#include <string>
#include <vector>
#include <set>
#include "igs_os_type.h"

#include "igs_resource_dir_list.h"
#include "igs_resource_msg_from_err.h"

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
#include <iostream>	// std::cout
#include <iomanip>	// std::setw()
#include <sstream>	// std::basic_ostringsteam<TCHAR>
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */

/*
man proc
/proc/[number]/stat
*/
namespace {
 //-------------------------------------------------------------------
 bool isdigitstr_(const std::basic_string<TCHAR>&str) {
	for (unsigned ii = 0; ii < str.size(); ++ii) {
		/* vc2008 
			_UNICODE および _MBCS が未定義の場合 isdigit 
			_UNICODE が定義されている場合 iswdigit 
		*/ 
#if defined UNICODE
		if (!iswdigit(str.at(ii))) { return false; }
#else
		if (!isdigit(str.at(ii))) { return false; }
#endif
	}
	return true;
 }
 void process_lists_unix_(
	std::vector<pid_t>&ppid_lists
	,std::vector<pid_t>&pid_lists
	,std::vector< std::basic_string<TCHAR> >&pnam
 ) {
	LPCTSTR head_dir = TEXT("/proc");
	std::set< std::basic_string<TCHAR> > sorting_list;
	igs::resource::dir_list( head_dir, sorting_list );

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
std::cout << "------ Process List ------\n";
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */

	for (std::set< std::basic_string<TCHAR> >::iterator
		it = sorting_list.begin();
		sorting_list.end() != it; ++it
	) {
		if (TEXT('d') != it->at(0)) { continue; }
		if (!isdigitstr_(&(it->at(1)))) { continue; }

		std::basic_string<TCHAR> file_path( head_dir );
		file_path += TEXT("/");
		file_path += &(it->at(1));
		file_path += TEXT("/stat");

		int pid = 0;
		char comm[1024]; comm[0] = 0;
		char state = 0;
		int ppid = 0;

		std::ifstream ifs(
			igs::resource::mbs_from_ts(file_path).c_str()
		);
		ifs >> pid >> comm >> state >> ppid;
		ifs.close();

		ppid_lists.push_back( ppid );
		pid_lists.push_back( pid );
		pnam.push_back( igs::resource::ts_from_mbs(comm) );

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
{
std::basic_ostringstream<TCHAR> ost;
ost
<< std::setw(3) << pid_lists.size()
<< TEXT(" ppid=") << std::setw(4) << ppid_lists.at(ppid_lists.size()-1)
<< TEXT(" pid=")  << std::setw(4) << pid_lists.at(pid_lists.size()-1)
<< TEXT("path=") << std::setw(16) << file_path
<< TEXT(" state=") << state
<< TEXT(" comm=") << pnam.at(pnam.size()-1)
<< std::endl;
std::cout << igs::resource::mbs_from_ts(ost.str());
}
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
	}
 }
 void process_posterity_unix_(
	const std::vector<pid_t>&ppid
	,const std::vector<pid_t>&pid
	,const std::vector< std::basic_string<TCHAR> >&pnam
	,const pid_t parent
	,int&depth
	,std::vector<pid_t>&tree
	,std::vector< std::basic_string<TCHAR> >&name
	,std::vector<int>&depths
 ) {
	if (0 == parent) { return; } /* idゼロのprocessはOSが使用 */

	++depth;
	for (unsigned ii = 0; ii < ppid.size(); ++ii) {
	 if (ppid.at(ii) == parent) {
		process_posterity_unix_(
		 ppid, pid, pnam, pid.at(ii), depth, tree, name, depths
		);
		tree.push_back( pid.at(ii) );
		name.push_back( pnam.at(ii) );
		depths.push_back( depth );
#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
{
std::basic_ostringstream<TCHAR> ost;
ost
<< std::setw(3) << ii
<< TEXT(" tree=") << std::setw(4) << tree.at(tree.size()-1)
<< TEXT(" depths=") << std::setw(1) << depths.at(depths.size()-1)
<< TEXT(" name=") << name.at(name.size()-1)
<< std::endl;
std::cout << igs::resource::mbs_from_ts(ost.str());
}
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
	 }
	}
	--depth;
 }
 //-------------------------------------------------------------------
 void tree_of_process_unix(
	pid_t top_pid
	,std::vector<pid_t>&tree
	,std::vector< std::basic_string<TCHAR> >&name
	,std::vector<int>&depths
 ) {
	std::vector<pid_t> ppid;
	std::vector<pid_t> pid;
	std::vector< std::basic_string<TCHAR> > pnam;
	process_lists_unix_( ppid, pid, pnam );

	if (0 == top_pid) { return; } /* idゼロのprocessはOSが占有 */

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
std::cout << "------ Process Posterity ------\n";
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
	int depth = 0;
	process_posterity_unix_(
		ppid, pid, pnam, top_pid, depth, tree, name, depths );
	tree.push_back(top_pid);
	unsigned ii = 0;
	for (; ii < pid.size(); ++ii) {
		if (pid.at(ii) == top_pid) {
			name.push_back(pnam.at(ii));
			break;
		}
	}
	if (pid.size() == ii) { name.push_back(TEXT("")); }
	depths.push_back(depth);
#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
{
std::basic_ostringstream<TCHAR> ost;
ost
<< std::setw(3) << ii
<< TEXT(" tree=") << std::setw(4) << tree.at(tree.size()-1)
<< TEXT(" depths=") << std::setw(1) << depths.at(depths.size()-1)
<< TEXT(" name=") << name.at(name.size()-1)
<< std::endl;
std::cout << igs::resource::mbs_from_ts(ost.str());
}
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
 }
 //-------------------------------------------------------------------
}
#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
#include "igs_resource_dir_list.cxx"
#include "igs_resource_msg_from_err.cxx"
int main(int argc, char *argv[]) {
 try {
	pid_t top_pid = 1;
	if (2 == argc) {
		top_pid = atoi(argv[1]);
	}
	std::vector<pid_t> tree;
	std::vector< std::basic_string<TCHAR> > name;
	std::vector<int> depths;
	::tree_of_process_unix( top_pid, tree, name, depths );

std::cout << "------ Process Tree ------\n";
	for (unsigned ii=0; ii<tree.size(); ++ii) {
	 for (int jj=0; jj<depths.at(ii); ++jj) { std::cout << ">"; }
		std::basic_ostringstream<TCHAR> ost;
		ost
		<< TEXT(" pid ") << std::setw(4) << tree.at(ii)
		<< TEXT(" ") << name.at(ii) << std::endl;
		std::cout << igs::resource::mbs_from_ts(ost.str());
	}
 }
 catch (std::exception& e) {
	std::cerr << "exception=\"" << e.what() << "\"" << std::endl;
	return 1;
 }
 catch (...) {
	std::cerr << "other exception" << std::endl;
	return 2;
 }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
/*
# gcc(32/64bits) :210,212 w! tes_gcc_igs_rsc_t_of_p.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_TREE_OF_PROCESS -I. igs_resource_tree_of_process_unix.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_t_of_p_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_TREE_OF_PROCESS -DUNICODE -I. igs_resource_tree_of_process_unix.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_t_of_p_wc
*/
