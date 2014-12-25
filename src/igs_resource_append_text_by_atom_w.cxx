#include <stdexcept>	// std::domain_error
#include <sstream>	// std::basic_ostringstream<TCHAR>
#include "igs_resource_msg_from_err.h"
#include "igs_resource_append_text_by_atom.h"
namespace {
 void append_text_by_atom_(
	const std::basic_string<TCHAR>&file_path
	,HANDLE handle
	,const LPCVOID lpBuffer
	,const DWORD nNumberOfBytesToWrite
 ) {
	/* �ɲä��뤿��ե�����κǸ�ذ�ư */
	LONG file_seek_low = 0;/* 2GB�ʲ��ΤȤ��Ϥ����ͤ�����ߤ�Ф褤 */
	LONG file_seek_high = 0;/* 2GB�ʾ�ξ�̥��֥��� */
	file_seek_low = ::SetFilePointer(
		handle ,file_seek_low ,&file_seek_high ,FILE_END
	);
	if (INVALID_SET_FILE_POINTER == file_seek_low) {
		const DWORD error_code = ::GetLastError();
		if (NO_ERROR != error_code) {
			throw std::domain_error(igs_resource_msg_from_err(
				file_path,error_code
			));
		}
	}

	DWORD bytes_write = 0;
	if (0 == ::WriteFile(
		handle		//HANDLE hFile
		,lpBuffer
		,nNumberOfBytesToWrite
		,&bytes_write	//,LPDWORD lpNumberOfBytesWritten
		,NULL		//,LPOVERLAPPED lpOverlapped
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
	if (nNumberOfBytesToWrite != bytes_write) {
		std::basic_ostringstream<TCHAR> ost;
		ost	<< TEXT("planed ") << nNumberOfBytesToWrite
			<< TEXT("bytes ,but Wrote") << bytes_write
			<< TEXT("bytes ")
			;
		throw std::domain_error(igs_resource_msg_from_er(
			ost.str()
		));
	}
 }
}
namespace igs {
 namespace resource {
  void append_text_by_atom(
	const std::basic_string<TCHAR>&file_path
	,const std::string&str
	,const DWORD sleep_milliseconds		// =3  3...7msec
	,const DWORD time_out_milli_seconds	//=1000  =1sec
  ) {
	const LPCVOID lpBuffer = static_cast<LPCVOID>(str.c_str());
	const DWORD nNumberOfBytesToWrite = static_cast<const DWORD>(
						str.length() );

	/* WindowsXp Windows7�Ǥϡ�
		igs_log.cxx�ˤơ�"std::ofstream"��Ȥ���
		��process�ǣ��ս꤫��(�ɲ�)��¸����ȡ�
		�ɤ���⡢���顼�ˤʤ餺�����ｪλ���롣
		�Ĥޤꡢ"std::ofstream"�ϥե����붦ͭ���Ƥ��롣
		����������¸���Ƥϡ�
		�����ߥ󥰤ˤ�äơ���ʬŪ�ˡ���񤭡��������Ƥ��ޤ���
		����ϡ���¸���֤Υݥ��󥿤�¾process����¸��λ���ΰ��֤�
		���äƤ��ޤ�����ȹͤ����롣
		�Ĥޤꡢ
		"std::ofstream"�ϥޥ�����������ɲ���¸�ˤ��б����Ƥ��ʤ���
		��äơ��ե����붦ͭ���ʤ��褦���̤ν����򤹤�ɬ�פ����롣
		2011-05-09
	*/
	HANDLE handle=0;
	for (DWORD time_accum=0; ; time_accum+=sleep_milliseconds) {
		/* ����λ��ֲ᤮�Ƥ�¾����ζ�ͭ���򤱤ʤ��Ȥ��ϥ��顼 */
		if (time_out_milli_seconds <= time_accum) {
			std::basic_ostringstream<TCHAR> ost;
			ost	<< TEXT("append_no_sharing_file_binary(-)")
				<< TEXT(" time out over ")
				<< time_out_milli_seconds
				<< TEXT("msec");
			throw std::domain_error( igs_resource_msg_from_er(
				ost.str()
			));
		}
		/* ��ͭ���ʤ��⡼�ɤǡ�������¸�ߤ��ʤ���п������� */
		handle = ::CreateFile(
		file_path.c_str()	// LPCTSTR lpFileName
		,GENERIC_WRITE		//,DWORD dwDesiredAccess
		,0/* =��ͭ���ʤ� */	//,DWORD dwShareMode
		,NULL	//,LPSECURITY_ATTRIBUTES lpSecurityAttributes
		,OPEN_ALWAYS		//,DWORD dwCreationDisposition
		,FILE_ATTRIBUTE_NORMAL	//,DWORD dwFlagsAndAttributes
		,NULL			//,HANDLE hTemplateFile
		);
		/* �ե�����򳫤������뤤�Ͽ����������������� */
		if (INVALID_HANDLE_VALUE != handle) {
			break;
		}
		/* ���顼��������� */
		const DWORD error_code = ::GetLastError();
		if (ERROR_SHARING_VIOLATION != error_code) { // =32
			/* "�̤Υץ�����������"�ʳ��Υ��顼 */
			throw std::domain_error( igs_resource_msg_from_err(
				file_path,error_code
			));
		}
		/* "�̤Υץ�����������"�ΤȤ���Sleep��˺Ƥӳ��� */
		::Sleep(sleep_milliseconds);
	}
	/*
	(��ͭ���ʤ����֤�)�ե�����򳫤���"::CloseHandle()"��������
	�������餤���ʤ�"taskkill /f /pid id"
	���뤤��"TerminateProcess()"���Ƥ⡢
	���Υե�����Ϻ��ٳ������Ȥ��Ǥ��뤳�Ȥ��ǧ���Ƥ���2011-05-07
	*/

 try {
	append_text_by_atom_(
		file_path
		,handle
		,lpBuffer
		,nNumberOfBytesToWrite
	);
 }
 catch(...) {
	/* �ե����볫�������ȡ��Ĥ���ޤǤν����ǥ��顼�ξ�硢
		close���Ƥ��饨�顼���throw���� */
	::CloseHandle(handle);
	throw;
 }

	if (0 == ::CloseHandle(handle)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
  }
 }
}
#if defined DEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM
# include <iostream>
# include <vector>
# include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] ) {
	if (6 != argc) {
		std::cout
		<< "Usage : "<< argv[0]
		<< " file wstr loop sleep timeout\n"
		<< "wstr    : save string\n"
		<< "loop    : 500 loop count\n"
		<< "sleep   : 3    milli seconds of waiting already opend\n"
		<< "timeout : 1000 milli seconds of timeout already opend\n"
		;
		return 1;
	}
 try {
	/*
		binary��¸�ʤΤǡ����ԥ����ɤ�'\n'���Τޤ���¸�����
		Windows�ˤƥƥ�������¸������ϡ�
		UNIX���ԥ�����'\n'
		�Ǥʤ�
		DOS���ԥ�����"\r\n"(CRLF)(0x0d0x0a)"
		������뤳�ȡ�
	*/
	std::string str(argv[2]); str += '\n';
	const int loop_count			= atoi(argv[3]);
	const DWORD sleep_milliseconds		= atoi(argv[4]);
	const DWORD time_out_milli_seconds	= atoi(argv[5]);
	for (int ii=0; ii < loop_count; ++ii) {
		igs::resource::append_text_by_atom(
			igs::resource::ts_from_mbs(argv[1])
			,str
			, sleep_milliseconds
			, time_out_milli_seconds
		);
	}
 }
 catch (std::exception& e) { std::cerr << e.what() << std::endl; }
 catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM */
/*
rem vc2010(32/64bits) :189,204 w! tes_msc16_igs_rsc_append_text_by_atom.bat
set _DEB=DEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM
set _SRC=igs_resource_append_text_by_atom
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_append_text_by_atom_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_append_text_by_atom_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :229,246 w! tes_msc15_igs_rsc_append_text_by_atom.bat
set _DEB=DEBUG_IGS_RESOURCE_APPEND_TEXT_BY_ATOM
set _SRC=igs_resource_append_text_by_atom
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_append_text_by_atom_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_append_text_by_atom_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
