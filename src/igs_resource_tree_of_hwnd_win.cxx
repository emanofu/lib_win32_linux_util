#include <windows.h>
#include <stdexcept>	// std::domain_error()
#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
# include <iostream>	// std::cout
# include <iomanip>	// std::setw()
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */
#include <vector>
#include "igs_resource_msg_from_err.h"

namespace {
 struct pair_tree_ {
	pair_tree_(
		std::vector<HWND>&hwnds_
		,std::vector< std::basic_string<TCHAR> >&names_
		,std::vector<int>&depths_
		,DWORD pid_
	)
		:hwnds(hwnds_)
		,names(names_)
		,depths(depths_)
		,pid(pid_)
		,stack_depth(0)
		,next_counter(0)
	{}
	pair_tree_&operator=(const pair_tree_&){} // for c4512 waring
	std::vector<HWND>&hwnds;
	std::vector< std::basic_string<TCHAR> >&names;
	std::vector<int>&depths;
	DWORD pid;
	int stack_depth;
	int next_counter;
 };
 void check_child_(
	HWND parent_hwnd
	,pair_tree_& pa
 ) {
	++pa.stack_depth;
	for (HWND hWnd = ::GetTopWindow(parent_hwnd); NULL != hWnd;
	hWnd = ::GetNextWindow(hWnd,GW_HWNDNEXT)) {
		check_child_(hWnd,pa); /* さらに子供見る */
		DWORD pid = 0;
		DWORD tid = 0;
		tid = ::GetWindowThreadProcessId(hWnd, &pid);
#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
std::cout
<< std::setw(3) << pa.next_counter
<< " hwnd " << std::setw(4) << hWnd
<< " pid " << std::setw(4) << pid
<< " tid " << std::setw(4) << tid
<< " child " << pa.stack_depth
<< std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */
		++pa.next_counter;
		if (pid == pa.pid) {
			pa.hwnds.push_back(hWnd);
			pa.depths.push_back(pa.stack_depth);
			TCHAR buf[4000]; buf[0] = 0;
			if (0 == ::GetWindowText(hWnd,buf,4000)) {
				buf[0] = 0; // Error
			}
			pa.names.push_back(buf);
		}
	}
	--pa.stack_depth;
 }
//--------------------------------------------------------------------
 struct pair_top_level_ {
	pair_top_level_(
		std::vector<HWND>&hwnds_
		,std::vector< std::basic_string<TCHAR> >&names_
		,DWORD pid_
	)
		:hwnds(hwnds_)
		,names(names_)
		,pid(pid_)
		,next_counter(0)
	{}
	pair_top_level_&operator=(const pair_top_level_&){}//for c4512waring
	std::vector<HWND>&hwnds;
	std::vector< std::basic_string<TCHAR> >&names;
	DWORD pid;
	int next_counter;
 };
 BOOL CALLBACK EnumWindowsProc_(HWND hWnd,LPARAM lParam) {
	pair_top_level_ *pa = (pair_top_level_*)lParam;
	DWORD pid = 0;
	DWORD tid = 0;
	tid = ::GetWindowThreadProcessId( hWnd, &pid );
#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
std::cout
<< std::setw(3) << pa->next_counter
<< " hwnd " << std::setw(4) << hWnd
<< " pid " << std::setw(4) << pid
<< " tid " << std::setw(4) << tid
<< std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */
	++pa->next_counter;
	if (pid == pa->pid) {
		pa->hwnds.push_back(hWnd);
		TCHAR buf[4000];
		buf[0] = 0;
		if (0 == ::GetWindowText(hWnd,buf,4000)) {
			buf[0] = 0;
		}
		pa->names.push_back(buf);
	}
	return TRUE; /* 列挙を続行 */
 }
}
//--------------------------------------------------------------------

#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
#include "igs_resource_msg_from_err_win.cxx"
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */

namespace {
  void tree_of_hwnd_win(
	const DWORD pid
	,std::vector<HWND>&hwnds
	,std::vector< std::basic_string<TCHAR> >&names
	,std::vector<int>&depths
  ) {
	pair_tree_ pa(hwnds,names,depths,pid);
#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
std::cout << "--- ::GetTopWindow(-) pid=" << pid << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */
	check_child_(NULL,pa);
  }
  void top_level_hwnd_win(
	const DWORD pid
	,std::vector<HWND>&hwnds
	,std::vector< std::basic_string<TCHAR> >&names
  ) {
	pair_top_level_ pa(hwnds,names,pid);
#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
std::cout << "--- ::EnumWindows(-) pid=" << pid << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */
	if (0 == ::EnumWindows(EnumWindowsProc_,(LPARAM)&pa)) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("EnumWindows(-)"),::GetLastError()
		) );
	}
  }
}

#if defined DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
# if defined UNICODE
#  if !defined cout // define guard
#   define cout wcout /* "cout"文字列を全て"wcout"に変換する危険に注意 */
#  endif
# endif
int main(int argc,char *argv[]) {
  try {
	DWORD top_pid = 0;
	/* 引数0だと一致しないので全てのプロセスをサーチする */
	if (2 == argc) {
		top_pid = atoi(argv[1]);
	}
	/*--- Zオーダーが一番上の子ウィンドウ順ハンドルの木構造 ---*/
	std::vector<HWND> hwnds_tree;
	std::vector< std::basic_string<TCHAR> > names_tree;
	std::vector<int> depths_tree;
	::tree_of_hwnd_win(top_pid,hwnds_tree,names_tree,depths_tree);
	/*--- 画面上のすべてのトップレベルウィンドウを列挙 ---*/
	std::vector<HWND> hwnds_top;
	std::vector< std::basic_string<TCHAR> > names_top;
	::top_level_hwnd_win(top_pid,hwnds_top,names_top);
	/*--- Zオーダーが一番上の子ウィンドウ順ハンドルの木構造-表示 ---*/
	if (0<hwnds_tree.size()) {
std::cout
<< TEXT("---::tree_of_hwnd_win(-)")
<< TEXT(" top_pid=") << top_pid
<< std::endl;
	}
	for (unsigned ii=0;ii<hwnds_tree.size();++ii) {
	 for (int jj=0;jj<depths_tree.at(ii);++jj) {std::cout << TEXT(">");}
std::cout
<< std::setw(2) << ii
<< TEXT(" top_pid=") << std::setw(4) << top_pid
<< TEXT(" hwnd=") << std::setw(4) << hwnds_tree.at(ii)
<< TEXT(" name=") << names_tree.at(ii)
<< std::endl;
	}
	/*--- 画面上のすべてのトップレベルウィンドウを列挙-表示 ---*/
	if (0<hwnds_top.size()) {
std::cout
<< TEXT("---::top_level_hwnd_win(-)")
<< TEXT(" top_pid=") << top_pid
<< std::endl;
	}
	for (unsigned ii=0;ii<hwnds_top.size();++ii) {
std::cout
<< std::setw(2) << ii
<< TEXT(" top_pid=") << std::setw(4) << top_pid
<< TEXT(" hwnd=") << std::setw(4) << hwnds_top.at(ii)
<< TEXT(" name=") << names_top.at(ii)
<< std::endl;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN */
/*
rem vc2008sp1(32/64bits) :206,223 w! tes_msc_igs_rsc_tree_of_hw.bat
set _DEB=DEBUG_IGS_RESOURCE_TREE_OF_WHND_WIN
set _SRC=igs_resource_tree_of_hwnd_win
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_tree_of_hw
set _INC=.
set _LIB=User32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_tree_of_hw_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
