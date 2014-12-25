#include <string>	// std::basic_string<TCHAR>

#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
# include <iostream>	// std::cout()
# include <iomanip>	// std::setw()
# include "igs_resource_tree_of_hwnd_win.cxx"
# include "igs_resource_tree_of_process_win.cxx"
# if defined UNICODE
#  if !defined cout // define guard
#   define cout wcout /* "cout"ʸ���������"wcout"���Ѵ����������� */
#  endif
# endif
namespace {
 void pr_pid_win_(
	const int depth
	, const int now
	, const size_t all
	, const DWORD pid
	, const std::basic_string<TCHAR>&pname
 ) {
	for (int ii = 0; ii < depth; ++ii) { std::cout << TEXT(">"); }
	std::cout
		<< std::setw(3) << now
		<< std::setw(3) << all
		<< TEXT(" pid=") << std::setw(4) << pid
		<< TEXT(" pname=\"") << pname << TEXT("\"")
		<< std::endl
		;
 }
 void pr_hwnd_win_(
	const int depth
	, const int now
	, const size_t all
	, const DWORD pid
	, const HWND hwnd
	, const std::basic_string<TCHAR>&wname
	, LPCTSTR mname
 ) {
	for(int ii = 0; ii < depth; ++ii) { std::cout << TEXT(">"); }
	std::cout
		<< std::setw(3) << now
		<< std::setw(3) << all
		<< TEXT(" pid=") << std::setw(4) << pid
		<< TEXT(" hwnd=") << std::setw(4) << hwnd
		<< TEXT(" wname=\"") << wname << TEXT("\"")
		<< TEXT(" PostMessage(-)=") << mname
		<< std::endl
		;
 }
}
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */

#include <vector>	// std::vector()
#include <stdexcept>	// std::domain_error()
#include "igs_resource_kill_process.h"
#include "igs_resource_msg_from_err.h"
namespace {
 void post_message_win_(
	const DWORD top_pid
	,const UINT msg
	,LPCTSTR mname
 ) {
	std::vector<DWORD> ptree;
	std::vector< std::basic_string<TCHAR> > pname;
	std::vector<int> pdept;
	::tree_of_process_win( top_pid, ptree, pname, pdept);

	/* �ץ���tree�λҤ���Ƥν� */
	//for (unsigned pnum = 0; pnum < ptree.size(); ++pnum) {

	/* �ץ���tree�οƤ���Ҥν� */
	for (int pnum=static_cast<int>(ptree.size())-1; 0<=pnum; --pnum) {

#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
	  pr_pid_win_(
		pdept.at(pnum),pnum+1,ptree.size(),ptree.at(pnum)
		,pname.at(pnum)
	  );
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */

	  std::vector<HWND> wtree;
	  std::vector< std::basic_string<TCHAR> > wname;
	  //std::vector<int> wdept;

	  /* �ƥץ��������ƤΥ�����ɥ� */
	  //::tree_of_hwnd_win( ptree.at(pnum), wtree, wname, wdept );

	  /* �ƥץ�����top������ɥ��Τ� */
	  ::top_level_hwnd_win(ptree.at(pnum), wtree, wname);

	  /* windows tree�θ����?window����Ҥ�
	  �����ʡ�window���饪���ʡ���window�� */
	  //for (unsigned wnum=0; wnum<wtree.size(); ++wnum) {

	  /* windows tree�θ����?window����Ƥ�
	  �����ʡ���window���饪���ʡ�window�� */
	  for (int wnum=static_cast<int>(wtree.size())-1; 0<=wnum; --wnum) {

#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
		pr_hwnd_win_(
			0,wnum+1,wtree.size(),ptree.at(pnum),wtree.at(wnum)
			,wname.at(wnum),mname
		);
//getchar();
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */

		/* SendMessage()�Ͻ�������λ���Ƥ������ */
		//const LRESULT ret = ::SendMessage(wtree.at(wnum),msg,0,0);

		/* PostMessage()�ϥ��塼�򥻥åȤ����������� */
		const BOOL ret = ::PostMessage(wtree.at(wnum),msg,0,0);
/*
MSDN(vs2008)
  PostMessage �ؿ���Ȥä� WM_QUIT ��å�������ݥ��Ȥ��뤳�Ȥ��򤱤Ƥ�������������ˡ�PostQuitMessage �ؿ���ȤäƤ���������
�������������äƤɤ��������ȡ�����Ū�˲�������ʤΤ���������
--> ¾�Υ��ץ��λ��������ˤϴط��ʤ�...�餷��...
�������������ץ�ν�λ��ˡ�ϡ�
A. ���٤Ƥ�hwnd��WM_DESTORY��PostMessage()�����ꥢ�ץ�ν�λ���Ԥ�
B. ���٤Ƥ�hwnd��WM_QUIT��SendMessage()������
�ɤ���?
*/
		if (!ret) {
			if (wnum <= 0) {
			throw std::domain_error(igs_resource_msg_from_err(
				mname,::GetLastError()
			));
			} else {
#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
std::cout
	<< TEXT("PostMessage() error but 2nd or after, then normal end???")
	<< std::endl;
std::cout << TEXT("--- error message start") << std::endl;
std::cout <<	igs::resource::ts_from_mbs(
			igs_resource_msg_from_err(mname,::GetLastError())
		) << std::endl;
std::cout << TEXT("--- error message end") << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */
				return;
			}
		}
	  }
	}
 }
}
void igs::resource::post_close_to_process(const DWORD pid)
{	post_message_win_(pid,WM_CLOSE  ,TEXT("WM_CLOSE"));
}
void igs::resource::post_destroy_to_process(const DWORD pid)
{	post_message_win_(pid,WM_DESTROY,TEXT("WM_DESTROY"));
}
void igs::resource::post_quit_to_process(const DWORD pid)
{	post_message_win_(pid,WM_QUIT   ,TEXT("WM_QUIT"));
}
void igs::resource::terminate_process(const DWORD pid) {
	std::vector<DWORD> ptree;
	std::vector< std::basic_string<TCHAR> > pname;
	std::vector<int> pdept;
	::tree_of_process_win( pid, ptree, pname, pdept );

	/* �ץ���tree�λҤ���Ƥν� */
	//for (unsigned pnum=0; pnum<ptree.size(); ++pnum) {

	/* �ץ���tree�οƤ���Ҥν� */
	for (int pnum=static_cast<int>(ptree.size())-1; 0<=pnum; --pnum) {

#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
	pr_pid_win_(
	 pdept.at(pnum),pnum+1,ptree.size(),ptree.at(pnum),pname.at(pnum)
	);
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */
		const HANDLE hdl = ::OpenProcess(
			PROCESS_TERMINATE, FALSE, ptree.at(pnum) );
		if (NULL == hdl) {
			throw std::domain_error(igs_resource_msg_from_err(
				TEXT("::OpenProcess(-)"),::GetLastError()
			));
		}
/*
(��)
MSDN���...
TerminateProcess()�ؿ��ϡ��ץ�����̵���˽�λ�����ޤ���
���̤ʾ����ǤΤߤ��δؿ���ȤäƤ���������
ExitProcess �ؿ��ǤϤʤ� TerminateProcess �ؿ���Ȥ��ȡ�
DLL�ʥ����ʥߥå���󥯥饤�֥��ˤ��������Ƥ��륰���Х�ǡ����ϡ�
�������Τʤ����֤˴٤��ǽ��������ޤ���

���δؿ��ϡ����ꤷ���ץ����˽�°���뤹�٤ƤΥ���åɤ�λ������
���Υץ����⽪λ�����ޤ���
����������λ�������ʹ���Ǥ��뤳�Ȥ�
���Υץ����˥����å����Ƥ��� DLL �����Τ��ޤ���

�������������äƤɤ��������ȡ�����Ū�˲�������ʤΤ���������
*/
		if (0 == ::TerminateProcess(hdl,1)) {
			throw std::domain_error(igs_resource_msg_from_err(
				TEXT("TerminateProcess(-)"),::GetLastError()
			));
		}
		::CloseHandle(hdl);
	}
}
#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
# include "igs_resource_msg_from_err_win.cxx"
namespace {
 enum POST_TYPE {
	DESTROY=0
	, QUIT
	, CLOSE
	, TERMINATE
 };
}
int main(int argc,char *argv[])
{
  try {
	if ((2 != argc) && (3 != argc)) {
		std::cout << "Usage : " << argv[0]
			<< " pid [-d/-q/-c/-t]\n"
			<< "-d is destroy\n"
			<< "-q is quit\n"
			<< "-c is close\n"
			<< "-t is terminate\n"
			<< "default is -d\n"
			;
		return 1;
	}

	POST_TYPE type = DESTROY;
	DWORD pid = 0;

	if (2 == argc) {
		pid = atoi(argv[1]);
	} else if (3 == argc) {
		pid = atoi(argv[1]);
		if      (!strcmp("-d", argv[2])) { type = DESTROY; }
		else if (!strcmp("-q", argv[2])) { type = QUIT; }
		else if (!strcmp("-c", argv[2])) { type = CLOSE; }
		else if (!strcmp("-t", argv[2])) { type = TERMINATE; }
	} else {
		return 2;
	}

	switch (type) {
	case DESTROY:	igs::resource::post_destroy_to_process(pid); break;
	case QUIT:	igs::resource::post_quit_to_process(pid); break;
	case CLOSE:	igs::resource::post_close_to_process(pid); break;
	case TERMINATE:	igs::resource::terminate_process(pid); break;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */
/*
rem vc2008sp1(32/64bits) :255,272 w! tes_msc_igs_rsc_kill_proc.bat
set _DEB=DEBUG_IGS_RESOURCE_KILL_PROCESS
set _SRC=igs_resource_kill_process
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_kill_proc
set _INC=.
set _LIB=User32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_kill_p_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
