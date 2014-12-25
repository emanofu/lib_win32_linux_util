#include <windows.h>
#include <tlhelp32.h> // CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

#include <stdexcept> // std::domain_error()
#include <string>
#include <vector>

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS 
# include <iostream> // std::TCOUT
# include <iomanip>  // std::setw()
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */

#include "igs_os_type.h"
#include "igs_resource_msg_from_err.h"

namespace {
 //-------------------------------------------------------------------
 void process_lists_win_(
	std::vector<DWORD>&ppid_lists
	,std::vector<DWORD>&pid_lists
	,std::vector< std::basic_string<TCHAR> >&pnam
 ) {
#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
std::TCOUT << TEXT("------ Process List ------\n");
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */

	HANDLE hwnd = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if ((HANDLE)-1 == hwnd) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("CreateToolhelp32Snapshot(-)"),::GetLastError()
		) );
	}
	PROCESSENTRY32 pe;
	::ZeroMemory(&pe,sizeof(pe));
	pe.dwSize = sizeof(pe);
	for (BOOL ret = ::Process32First(hwnd,&pe);
	ret;      ret = ::Process32Next( hwnd,&pe)) {
		ppid_lists.push_back( pe.th32ParentProcessID );
		pid_lists.push_back( pe.th32ProcessID );
		pnam.push_back( pe.szExeFile );

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
std::TCOUT
<< std::setw(3) << pid_lists.size()
<< TEXT(" ppid=") << std::setw(4) << ppid_lists.at(ppid_lists.size()-1)
<< TEXT(" pid=") << std::setw(4) << pid_lists.at(pid_lists.size()-1)
<< TEXT(" threads=") << std::setw(2) << pe.cntThreads
<< TEXT(" exe=") << pnam.at(pnam.size()-1)
<< std::endl;
/*
std::TCOUT
<< TEXT("szExeFile=")	<< pe.szExeFile
<< TEXT(" cntUsage=")	<< pe.cntUsage
<< TEXT(" th32ProcessID=")	<< pe.th32ProcessID
<< TEXT(" th32DefaultHeapID=")	<< pe.th32DefaultHeapID
<< TEXT(" th32ModuleID=")	<< pe.th32ModuleID
<< TEXT(" cntThreads=")		<< pe.cntThreads
<< TEXT(" th32ParentProcessID=")<< pe.th32ParentProcessID
<< TEXT(" pcPriClassBase=")	<< pe.pcPriClassBase
<< TEXT(" dwFlags=")	<< pe.dwFlags
<< std::endl;
*/
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
	}
	::CloseHandle(hwnd);
 }
 void process_posterity_win_(
	const std::vector<DWORD>&ppid
	,const std::vector<DWORD>&pid
	,const std::vector< std::basic_string<TCHAR> >&pnam
	,const DWORD parent
	,int&depth
	,std::vector<DWORD>&tree
	,std::vector< std::basic_string<TCHAR> >&name
	,std::vector<int>&depths
 ) {
	if (0 == parent) { return; } /* idゼロのprocessはOSが使用 */

	++depth;
	for (unsigned ii = 0; ii < ppid.size(); ++ii) {
	 if (ppid.at(ii) == parent) {
		process_posterity_win_(
		 ppid, pid, pnam, pid.at(ii), depth, tree, name, depths
		);
		tree.push_back( pid.at(ii) );
		name.push_back( pnam.at(ii) );
		depths.push_back( depth );
#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
std::TCOUT
<< std::setw(3) << ii
<< TEXT(" tree=") << std::setw(4) << tree.at(tree.size()-1)
<< TEXT(" depths=") << std::setw(1) << depths.at(depths.size()-1)
<< TEXT(" name=") << name.at(name.size()-1)
<< std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
	 }
	}
	--depth;
 }
 //-------------------------------------------------------------------
 void tree_of_process_win(
	DWORD top_pid
	,std::vector<DWORD>&tree
	,std::vector< std::basic_string<TCHAR> >&name
	,std::vector<int>&depths
 ) {
	std::vector<DWORD> ppid;
	std::vector<DWORD> pid;
	std::vector< std::basic_string<TCHAR> > pnam;
	process_lists_win_( ppid, pid, pnam );

	if (0 == top_pid) { /* idゼロのprocessはOSが占有 */
		return;
	}

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
std::TCOUT << TEXT("------ Process Posterity ------\n");
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
	int depth = 0;
	process_posterity_win_(
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
std::TCOUT
<< std::setw(3) << ii
<< TEXT(" tree=") << std::setw(4) << tree.at(tree.size()-1)
<< TEXT(" depths=") << std::setw(1) << depths.at(depths.size()-1)
<< TEXT(" name=") << name.at(name.size()-1)
<< std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
 }
 //-------------------------------------------------------------------
}

#if defined DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
# include "igs_resource_msg_from_err.cxx"
int main(int argc, char *argv[]) {
  try {
	DWORD top_pid = 4;
	if (2 == argc) {
		top_pid = atoi(argv[1]);
	}
	std::vector<DWORD> tree;
	std::vector< std::basic_string<TCHAR> > name;
	std::vector<int> depths;
	::tree_of_process_win( top_pid, tree, name, depths );

std::TCOUT << TEXT("------ Process Tree ------\n");
	for (unsigned ii=0;ii<tree.size();++ii) {
	 for (int jj=0; jj<depths.at(ii); ++jj) { std::TCOUT << TEXT(">"); }
	 std::TCOUT
		<< TEXT("pid ") << std::setw(4) << tree.at(ii)
		<< TEXT(" ") << name.at(ii) << std::endl;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_PROCESS */
/*
rem vc2008sp1(32/64bits) :171,188 w! tes_msc_igs_rsc_tree_of_p.bat
set _DEB=DEBUG_IGS_RESOURCE_TREE_OF_PROCESS
set _SRC=igs_resource_tree_of_process_win
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_tree_of_p_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_tree_of_p_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
