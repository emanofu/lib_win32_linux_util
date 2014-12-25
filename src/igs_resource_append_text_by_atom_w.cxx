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
	/* 追加するためファイルの最後へ移動 */
	LONG file_seek_low = 0;/* 2GB以下のときはこの値だけをみればよい */
	LONG file_seek_high = 0;/* 2GB以上の上位ダブルワード */
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

	/* WindowsXp Windows7では、
		igs_log.cxxにて、"std::ofstream"を使い、
		別processで２箇所から(追加)保存すると、
		どちらも、エラーにならずに正常終了する。
		つまり、"std::ofstream"はファイル共有している。
		しかし、保存内容は、
		タイミングによって、部分的に、上書き、削除されてしまう。
		これは、保存位置のポインタが他processの保存完了前の位置を
		拾ってしまうためと考えられる。
		つまり、
		"std::ofstream"はマルチタスクの追加保存には対応していない。
		よって、ファイル共有しないように別の処理をする必要がある。
		2011-05-09
	*/
	HANDLE handle=0;
	for (DWORD time_accum=0; ; time_accum+=sleep_milliseconds) {
		/* 指定の時間過ぎても他からの共有が解けないときはエラー */
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
		/* 共有しないモードで、開く、存在しなければ新規作成 */
		handle = ::CreateFile(
		file_path.c_str()	// LPCTSTR lpFileName
		,GENERIC_WRITE		//,DWORD dwDesiredAccess
		,0/* =共有しない */	//,DWORD dwShareMode
		,NULL	//,LPSECURITY_ATTRIBUTES lpSecurityAttributes
		,OPEN_ALWAYS		//,DWORD dwCreationDisposition
		,FILE_ATTRIBUTE_NORMAL	//,DWORD dwFlagsAndAttributes
		,NULL			//,HANDLE hTemplateFile
		);
		/* ファイルを開く、あるいは新規作成に成功した */
		if (INVALID_HANDLE_VALUE != handle) {
			break;
		}
		/* エラー情報を得る */
		const DWORD error_code = ::GetLastError();
		if (ERROR_SHARING_VIOLATION != error_code) { // =32
			/* "別のプロセスが使用中"以外のエラー */
			throw std::domain_error( igs_resource_msg_from_err(
				file_path,error_code
			));
		}
		/* "別のプロセスが使用中"のときはSleep後に再び開く */
		::Sleep(sleep_milliseconds);
	}
	/*
	(共有しない状態で)ファイルを開き、"::CloseHandle()"する前に
	外部からいきなり"taskkill /f /pid id"
	あるいは"TerminateProcess()"しても、
	そのファイルは再度開くことができることを確認している2011-05-07
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
	/* ファイル開いたあと、閉じるまでの処理でエラーの場合、
		closeしてからエラーを再throwする */
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
		binary保存なので、改行コードは'\n'そのまま保存される
		Windowsにてテキスト保存する時は、
		UNIX改行コード'\n'
		でなく
		DOS改行コード"\r\n"(CRLF)(0x0d0x0a)"
		を入れること。
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
