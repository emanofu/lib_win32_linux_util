#if defined _WIN32 //-------------------------------------------------
// #include <windows.h>
#include <stdexcept>	// std::domain_error
#include <sstream>	// std::basic_ostringstream<TCHAR>
#include "igs_resource_msg_from_err.h"
#include "igs_resource_afbinary.h"
namespace igs {
 namespace resource {
/*
void append_text_with_atomically(
void append_text_by_atom(
*/
  void append_no_sharing_file_binary(
	LPCTSTR file_path

	,LPCVOID lpBuffer
	,const DWORD nNumberOfBytesToWrite
	,const DWORD sleep_milliseconds	// =3  3...7 about log at once
	,const DWORD time_out_milli_seconds	//=1000  =1sec
  ) {
/*
LPCVOID lpBuffer = static_cast<LPCVOID>(app_str.c_str());
const DWORD nNumberOfBytesToWrite = static_cast<const DWORD>(app_str.length());
*/
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
		/* 保存するファイル開く、なければ新規作成 */
		handle = ::CreateFile(
		file_path		// LPCTSTR lpFileName
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
	if (0 == ::CloseHandle(handle)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
  }
 }
}
#else //--------------------------------------------------------------
#include <stdexcept>	// std::domain_error
#include <fstream>	// std::ofstream
#include "igs_resource_msg_from_err.h"
#include "igs_resource_afbinary.h"
namespace igs {
 namespace resource {
  void append_atomic_file_binary(
	const char *file_path
	,const std::string&str
  ) {
	/* Linux(rhel4)では、
		UNIXにおける操作の不可分性により、
		"std::ofstream"で、別processで２箇所から(追加)保存は、
		問題ない。すべて正常に保存される。
	*/
	std::ofstream ofs;
	ofs.open(file_path, std::ios::out|std::ios::app);
	if (!ofs) {
		std::string err; err += file_path; err += ":can not open";
		throw std::domain_error(igs_resource_msg_from_er( err ));
	}
	ofs << str;
	if (!ofs) {
		ofs.close();
		std::string err; err += file_path; err += ":can not write";
		throw std::domain_error(igs_resource_msg_from_er( err ));
	}
	/* ファイルはこのmethodを抜けると自動的に閉じるが意識的に閉る */
	ofs.close();
  }
 }
}
#endif //-------------------------------------------------------------
#if defined DEBUG_IGS_RESOURCE_AFBINARY
# include <iostream>
# include <vector>
# include "igs_resource_msg_from_err.cxx"
#if defined _WIN32 //-------------------------------------------------
int main( int argc, char *argv[] )
{
	if (6 != argc) {
		std::cout
		<< "Usage : " << argv[0]
		<< " file wstr sleep timeout loop\n"
		<< "wstr    : save string\n"
		<< "sleep   :3    milli seconds of waiting already opend\n"
		<< "timeout :1000 milli seconds of timeout already opend\n"
		<< "loop    :500 loop count\n"
		;
		return 1;
	}
 try {
	std::string str(argv[2]); str += '\n';
	/*
		binary保存なので、改行コードは'\n'そのまま保存される
		Windowsにてテキスト保存する時は、
		UNIX改行コード'\n'
		でなく
		DOS改行コード"\r\n"(CRLF)(0x0d0x0a)"
		を入れること。
	*/
	const LPCVOID lpBuffer = static_cast<LPCVOID>(&str.at(0));
	const DWORD nNumberOfBytesToWrite =
			static_cast<const DWORD>(strlen(argv[2])) + 1;
	DWORD sleep_milliseconds = atoi(argv[3]);
	DWORD time_out_milli_seconds = atoi(argv[4]);
	int loop_count = atoi(argv[5]);
	for (int ii=0; ii<loop_count; ++ii) {
		igs::resource::append_no_sharing_file_binary(
			igs::resource::ts_from_mbs(argv[1]).c_str()
			, lpBuffer
			, nNumberOfBytesToWrite
			, sleep_milliseconds
			, time_out_milli_seconds
		);
	}
 }
 catch (std::exception& e) { std::cerr<<"exception="<<e.what()<<std::endl; }
 catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#else //--------------------------------------------------------------
int main( int argc, char *argv[] )
{
	if (4 != argc) {
		std::cout
		<< "Usage : " << argv[0]
		<< " file wstr loop\n"
		<< "wstr    : save string\n"
		<< "loop    :500 loop count\n"
		;
		return 1;
	}
  try {
	char *file_path = argv[1];
	std::string str(argv[2]); str += '\n';
	int loop_count = atoi(argv[3]);
	for (int ii=0; ii<loop_count; ++ii) {
		igs::resource::append_atomic_file_binary(file_path,str);
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif //-------------------------------------------------------------
#endif  /* !DEBUG_IGS_RESOURCE_AFBINARY */
/*
rem vc2008sp1(32/64bits) :229,246 w! tes_msc_igs_rsc_afbinary.bat
set _DEB=DEBUG_IGS_RESOURCE_AFBINARY
set _SRC=igs_resource_afbinary
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_afbinary
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_afbin_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :247,248 w! tes_gcc_igs_rsc_afbinary.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_AFBINARY -I. igs_resource_afbinary.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_afbinary
*/
