#include <iostream>	// std::cout
#include <iomanip>	// std::setw()
#include <sstream>	// std::ostringstream
#include <vector>	// std::vector
#include "igs_resource_msg_from_err.h"

#include "igs_resource_process.h"
//--------------------------------------------------------------------
igs::resource::process::parameters::parameters()
	: hStdRead(0)
	, hStdWrite(0)
	, hErrRead(0)
	, hErrWrite(0)
{
	::ZeroMemory(&this->pi,sizeof(this->pi));
}
igs::resource::process::parameters::~parameters()
{
	try { 
		static_cast<void>(this->close());
	} catch(...) {}/* destructor�����㳰�ꤲ�ʤ� */
/*
�����ǥ��ȥ饯������close()�Υ��顼���������㳰���ꤲ�Ƥ��ޤ��ȡ�
¾���㳰�ˤ��ƤӽФ��줿����2���㳰�ξ��֤Ȥʤꡢ
�ץ����϶�����λ���Ƥ��ޤ���
�Τ��ꤲ�ʤ���
�Τ�close()�Υ��顼�������Ƥ��å�������̵��Ȥʤ�
*/
}
const std::string igs::resource::process::parameters::close( void ) {
/*
(��)
�ץ�����λ�����Ƥ⡢ɬ�����⤽����б�����ץ������֥������Ȥ�
�����ƥफ���������櫓�ǤϤ���ޤ���
�ץ����� 1 �Ĥޤ���ʣ���Υϥ�ɥ뤬������Ƥ��Ƥ����硢
�Ǹ�Υϥ�ɥ���Ĥ���ȡ��ץ������֥������Ȥ���ưŪ�˺������ޤ���
*/
	std::string em; // =error message

	if (0!= this->pi.hThread) { if (0 == ::CloseHandle(
		this->pi.hThread)) { if (!em.empty()) { em += '\n'; }
					em += igs_resource_msg_from_err(
       TEXT("CloseHandle(hThread)") ,::GetLastError()); }
		this->pi.hThread = 0;
	}
	if (0!= this->pi.hProcess) { if (0 == ::CloseHandle(
		this->pi.hProcess)) { if (!em.empty()) { em += '\n'; }
					em += igs_resource_msg_from_err(
       TEXT("CloseHandle(hProcess)") ,::GetLastError()); }
		this->pi.hProcess = 0;
	}
	if (0!= this->hErrRead) { if (0 == ::CloseHandle(
		this->hErrRead)) { if (!em.empty()) { em += '\n'; }
					em += igs_resource_msg_from_err(
    TEXT("CloseHandle(hErrRead)") ,::GetLastError()); }
		this->hErrRead = 0;
	}
	if (0!= this->hErrWrite) { if (0 == ::CloseHandle(
		this->hErrWrite)) { if (!em.empty()) { em += '\n'; }
					em += igs_resource_msg_from_err(
    TEXT("CloseHandle(hErrWrite)") ,::GetLastError()); }
		this->hErrWrite = 0;
	}
	if (0!= this->hStdRead) { if (0 == ::CloseHandle(
		this->hStdRead)) { if (!em.empty()) { em += '\n'; }
					em += igs_resource_msg_from_err(
    TEXT("CloseHandle(hStdRead)") ,::GetLastError()); }
		this->hStdRead = 0;
	}
	if (0!= this->hStdWrite) { if (0 == ::CloseHandle(
		this->hStdWrite)) { if (!em.empty()) { em += '\n'; }
					em += igs_resource_msg_from_err(
    TEXT("CloseHandle(hStdWrite)") ,::GetLastError()); }
		this->hStdWrite = 0;
	}
	return em;
}
//--------------------------------------------------------------------
void igs::resource::process::execute(
	igs::resource::process::parameters&pa
	,LPTSTR command_line /* LPCTSTR���ȥ��顼�������� */
	,DWORD add_priority_flag
) {
	SECURITY_ATTRIBUTES sa; ::ZeroMemory(&sa,sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	/* Stdout��̾���ʤ��ѥ��׺��� */
	if (0 == ::CreatePipe( &pa.hStdRead ,&pa.hStdWrite ,&sa ,0 )) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("CreatePipe(Std)"),::GetLastError()
		) );
	}

	/* Stderr��̾���ʤ��ѥ��׺��� */
	if (0 == ::CreatePipe( &pa.hErrRead ,&pa .hErrWrite ,&sa ,0 )) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("CreatePipe(Err)"),::GetLastError()
		) );
	}

	/* �ץ������� */
	STARTUPINFO si; ::ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = pa.hStdWrite;
	si.hStdError  = pa.hErrWrite;
#if defined DEBUG_IGS_RESOURCE_PROCESS
std::cout << "<" << command_line << ">" << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_PROCESS */
/*------
BOOL CreateProcess(
  LPCTSTR lpApplicationName,                 // �¹Բ�ǽ�⥸�塼���̾��
  LPTSTR lpCommandLine,                      // ���ޥ�ɥ饤���ʸ����
  LPSECURITY_ATTRIBUTES lpProcessAttributes, // �������ƥ����һ�
  LPSECURITY_ATTRIBUTES lpThreadAttributes,  // �������ƥ����һ�
  BOOL bInheritHandles,                      // �ϥ�ɥ�ηѾ����ץ����
  DWORD dwCreationFlags,                     // �����Υե饰
  LPVOID lpEnvironment,                      // �������Ķ��֥�å�
  LPCTSTR lpCurrentDirectory,                // �����ȥǥ��쥯�ȥ��̾��
  LPSTARTUPINFO lpStartupInfo,               // �������ȥ��å׾���
  LPPROCESS_INFORMATION lpProcessInformation // �ץ�������
);
------*/
	if (0 == ::CreateProcess(
		NULL,command_line,NULL,NULL
		,TRUE
		,CREATE_NO_WINDOW|add_priority_flag
		,NULL,NULL,&si,&pa.pi
	)) {
		throw std::domain_error( igs_resource_msg_from_err(
			command_line,::GetLastError()
		));
	}

	/* �ҥץ���������Ԥ�,Console���ץ�ǤϾ�˥��顼(-1)���֤� */
	::WaitForInputIdle(pa.pi.hProcess, INFINITE);
}

namespace {
  template <class type>
  const std::basic_string<TCHAR> form_(
	LPCTSTR title
	,LPCTSTR key
	,const type val
	,const int key_wide=9
  ) {
	std::basic_ostringstream<TCHAR> ost;
	ost
		<< title
		<< TEXT(' ') << std::setw(key_wide) << std::left << key
		<< TEXT(' ') << val;
	return ost.str();
  }
}
void igs::resource::process::form(
	const igs::resource::process::parameters&pa
	,std::vector< std::basic_string<TCHAR> >&lines
) {
  lines.push_back(form_(TEXT("pipe"),TEXT("hStdRead") ,pa.hStdRead	));
  lines.push_back(form_(TEXT("pipe"),TEXT("hStdWrite"),pa.hStdWrite	));
  lines.push_back(form_(TEXT("pipe"),TEXT("hErrRead") ,pa.hErrRead	));
  lines.push_back(form_(TEXT("pipe"),TEXT("hErrWrite"),pa.hErrWrite	));
  lines.push_back(form_(TEXT("proc"),TEXT("hProcess") ,pa.pi.hProcess	));
  lines.push_back(form_(TEXT("proc"),TEXT("hThread")  ,pa.pi.hThread	));
  lines.push_back(form_(TEXT("proc"),TEXT("ProcessID"),pa.pi.dwProcessId));
  lines.push_back(form_(TEXT("proc"),TEXT("ThreadID") ,pa.pi.dwThreadId	));
}

bool igs::resource::process::wait(
	const igs::resource::process::parameters&pa
	,const int time_out_msec
	,std::basic_string<TCHAR>&std_str
	,std::basic_string<TCHAR>&err_str
) {
	/*
	���ꤵ�줿Process�������ʥ����(�¹Խ�λ����)�ˤʤä�����
	�����ॢ���Ȼ��֤��вᤷ����������֤��ޤ�
	*/
	const DWORD dwWait = ::WaitForSingleObject(
		pa.pi.hProcess, time_out_msec
	);
	if (WAIT_FAILED == dwWait) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("WaitForSingleObject(-)"),::GetLastError()
		));
	}

	/* Std Pipe�����ɼ�Ǥ�����Х��ȿ� */
	DWORD std_bytes = 0;
	if (0 == ::PeekNamedPipe(pa.hStdRead,NULL,0,NULL,&std_bytes,NULL)) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("PeekNamedPipe(Std)"),::GetLastError()
		) );
	}

	/* Std �ѥ��פ����̤��ɤ߽Ф�������length-1ʸ���Ǥ��뤳�Ȥ���� */
	if (0 < std_bytes) {
	  std::vector<char> buf(std_bytes*3); /* ǰ�Τ���3�����̳��ݤ��� */
	  DWORD dmy = 0; /* �ºݤ��ɤ߼�ä��Х��ȿ� */
	  DWORD siz = static_cast<DWORD>(buf.size()) - 1;
	  if (0==::ReadFile( pa.hStdRead ,&buf.at(0) ,siz ,&dmy ,NULL )) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("ReadFile(Std)"),::GetLastError()
		) );
	  }
	  std_str = igs::resource::ts_from_mbs( &buf.at(0) );
	}

	/* Err Pipe�����ɼ�Ǥ�����Х��ȿ� */
	DWORD err_bytes = 0;
	if (0 == ::PeekNamedPipe(pa.hErrRead,NULL,0,NULL,&err_bytes,NULL)) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("PeekNamedPipe(Err)"),::GetLastError()
		) );
	}

	/* Err �ѥ��פ����̤��ɤ߽Ф�������length-1ʸ���Ǥ��뤳�Ȥ���� */
	if (0 < err_bytes) {
	  std::vector<char> buf(err_bytes*3); /* ǰ�Τ���3�����̳��ݤ��� */
	  DWORD dmy = 0; /* �ºݤ��ɤ߼�ä��Х��ȿ� */
	  DWORD siz = static_cast<DWORD>(buf.size()) - 1;
	  if (0==::ReadFile( pa.hErrRead ,&buf.at(0) ,siz ,&dmy ,NULL )) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("ReadFile(Err)"),::GetLastError()
		));
	  }
	  err_str = igs::resource::ts_from_mbs( &buf.at(0) );
	}

	/* �����ʥ���֤Ǥʤ�(��λ���Ƥʤ�)��
	   ���뤤�ϡ��ɤ߹��ߤ��ꡢ�ʤ�true���֤� */
	return	(WAIT_OBJECT_0 != dwWait)
		|| (0 < std_bytes)
		|| (0 < err_bytes)
		;
}

DWORD igs::resource::process::id(
	const igs::resource::process::parameters&pa
) {
	return pa.pi.dwProcessId;
}

/* ���ꤵ�줿�ץ����ν�λ���ơ�������������ޤ� */
DWORD igs::resource::process::exit_code(
	const igs::resource::process::parameters&pa
) {
	DWORD exitcode=0;
	if (0 == ::GetExitCodeProcess(pa.pi.hProcess, &exitcode)) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("GetExitCodeProcess(-)"),::GetLastError()
		) );
	}
	return exitcode;
}
void igs::resource::process::close(
	igs::resource::process::parameters&pa
) {
	std::string str(pa.close());
	if (!str.empty()) {
		throw std::domain_error( str );
	}
}

#if defined DEBUG_IGS_RESOURCE_PROCESS
#include "igs_resource_msg_from_err.cxx"
namespace {
 void pr_text_(DWORD nStdHandle, const std::basic_string<TCHAR>&name) {
	//::AllocConsole();
	DWORD ret=0;
	::WriteConsole(
		::GetStdHandle(nStdHandle)
		,name.c_str() ,static_cast<DWORD>(name.size())
		,&ret ,NULL
	);
	//::FreeConsole();
 }
 void pr_out_(const std::basic_string<TCHAR>&name) {
	pr_text_( STD_OUTPUT_HANDLE, name );
 }
 void pr_err_(const std::basic_string<TCHAR>&name) {
	pr_text_( STD_ERROR_HANDLE, name );
 }
}
int main(int argc, char *argv[])
{
	if (1 == argc) {
		std::cout << "Usage : " << argv[0] << " commandline\n";
		return 1;
	}
	int ret = 0;
  try {
	igs::resource::process::parameters pa;
	{
		std::string str;
		for (int ii=1;ii<argc;++ii) {
			if (1<ii) { str += ' '; }
			str += argv[ii];
		}
		igs::resource::process::execute(
			pa
			,const_cast<LPTSTR>(
			 igs::resource::ts_from_mbs(str.c_str()).c_str()
			)
		);
	}
	std::cout << "pid=" << igs::resource::process::id(pa) << std::endl;

	std::vector< std::basic_string<TCHAR> > lines;
	igs::resource::process::form( pa,lines );
	for (unsigned ii=0;ii<lines.size(); ++ii) {
#if defined UNICODE
		std::wcout
#else
		std::cout
#endif
		<< lines.at(ii) << std::endl;
	}

	/* Process�¹���(�󥷥��ʥ����)��message������ʤ�true���֤롣
	���뤤�ϡ��¹���ʤ�millisecond�Ԥ� */
	std::basic_string<TCHAR> sout,serr,msg;
	while (igs::resource::process::wait(pa,1000,sout,serr)) {
		/* job����Υ�å�������������� */
		if (!sout.empty()){
			std::basic_string<TCHAR> str(TEXT("sout=\""));
			str += sout; str += TEXT("\"");
			pr_out_(str);

			msg += TEXT("sout "); msg += sout;

			sout.clear();
		}
		if (!serr.empty()){
			std::basic_string<TCHAR> str(TEXT("serr=\""));
			str += serr; str += TEXT("\"");
			pr_err_(str);

			msg += TEXT("serr "); msg += serr;

			serr.clear();
		}
	}
	{
		std::basic_string<TCHAR> str;
		str += TEXT("------------\n");
		str += msg;
		str += TEXT("------------\n");
		pr_out_(str);
	}
		;
	DWORD exitcode = igs::resource::process::exit_code(pa);
	ret = exitcode;
	std::cout << "exit_code=" << exitcode << std::endl;
	igs::resource::process::close(pa);
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }

	/* Process�ν�λ�����ɤ��֤� */
	return ret;
}
#endif  /* !DEBUG_IGS_RESOURCE_PROCESS */
/*
rem vc2008sp1(32/64bits) :369,387 w! tes_msc_igs_rsc_process.bat
set _DEB=DEBUG_IGS_RESOURCE_PROCESS
set _SRC=igs_resource_process
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_process
set _INC=.
set _LIB=user32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_process_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem
// :351,406 w! tes_msc_process.cxx
// cl /W4 /WX /MD /EHa /O2 /wd4819 tes_msc_process.cxx /Fetes_msc_process
// mt -manifest tes_msc_process.exe.manifest -outputresource:tes_msc_process.exe;1
// del tes_msc_process.obj tes_msc_process.exe.manifest
// cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE tes_msc_process.cxx /Fetes_msc_process_uc
// mt -manifest tes_msc_process_uc.exe.manifest -outputresource:tes_msc_process_uc.exe;1
// del tes_msc_process.obj tes_msc_process_uc.exe.manifest
#include <windows.h> // ::Sleep(-)
#include <iostream>
namespace {
 void pr_text_(DWORD nStdHandle, const std::basic_string<TCHAR>&name) {
	//::AllocConsole();
	DWORD ret=0;
	::WriteConsole(
		::GetStdHandle(nStdHandle)
		,name.c_str() ,static_cast<DWORD>(name.size())
		,&ret ,NULL
	);
	//::FreeConsole();
 }
 void pr_out_(const std::basic_string<TCHAR>&name) {
	pr_text_( STD_OUTPUT_HANDLE, name );
 }
 void pr_err_(const std::basic_string<TCHAR>&name) {
	pr_text_( STD_ERROR_HANDLE, name );
 }
}
int main(int argc,char *argv[]) {
	unsigned loop=50;
	int msec=1;
	if (1 != argc && 3 != argc) {
		std::cout << "Usage : " << argv[0] << " loop msec\n";
		return 1;
	}
	if (3 == argc) {
		loop = atoi(argv[1]);
		msec = atoi(argv[2]);
	}
#if defined UNICODE
	for (unsigned ii=0;ii<loop;++ii) {
		std::wcout << TEXT("message about cout ") << ii;
		pr_out_(TEXT("time�Ǥ�\n"));
		std::wcerr << TEXT("message about cerr ") << ii;
		pr_err_(TEXT("time��\n"));
		::Sleep(msec);
	}
#else
	for (unsigned ii=0;ii<loop;++ii) {
		std::cout << "message about cout " << ii << "time�Ǥ�\n";
		std::cerr << "message about cerr " << ii << "time��\n";
		::Sleep(msec);
	}
#endif
	return 0;
}
*/
