#include "igs_text_first_out_line.h"
/*	入力データの最終行にも改行必須!!!
	Return Code is...
		Windows		"\r\n"	= CRLF	=0x0d0x0a
		Linux		"\n"	= LF	=0x0a
	改行が連続していた(空行)場合は取り除かれる
*/
bool igs::text::first_out_line(
	std::basic_string<TCHAR>&lifoline
	,std::basic_string<TCHAR>&fo_line
) {
	/* lifolineから改行を検索 */
	std::basic_string<TCHAR>::size_type lsz = lifoline.find_first_of(
		TEXT("\r\n")
	);

	/* 改行なし-->文末であることを期待 */
	if (std::basic_string<TCHAR>::npos == lsz) { return false; }

	/* 改行除く頭1行分得る */
	if (0 < lsz) {
		fo_line = lifoline.substr(0,lsz);
	} else {fo_line = TEXT(""); }	/* 空行 */

	/* 改行含め頭1行分を取り除く */
	/* 改行が連続していた場合はここで取り除かれる */
	std::basic_string<TCHAR>::size_type esz =
			lifoline.find_first_not_of(TEXT("\r\n"),lsz);
	lifoline.erase(0,esz);

	/* 改行あったのでtrueを返す(最終行もreturncodeないと切り出せない) */
	return true;
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_TEXT_FIRST_OUT_LINE
#include <iostream>	// std::cout
#include <fstream>	// std::ifstream
#include "igs_resource_msg_from_err.cxx"
namespace {
 void textfile_to_string_(
	const std::basic_string<TCHAR>&file_path
	,std::basic_string<TCHAR>&filestr
 ) {
	/*--- ファイル開く ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_ifstream<TCHAR> ifs( file_path.c_str() );
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::ifstream ifs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ifs) {
		std::basic_string<TCHAR> str(TEXT("\""));
		str += file_path;
		str += TEXT("\" can not open");
		throw std::domain_error(igs_resource_msg_from_er(str));
	}
	/*--- 1行づつ読込 ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_string<TCHAR> line;
	while (std::getline(ifs,line)) {
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::string line_tmp;
	while (std::getline(ifs,line_tmp)) {
		std::basic_string<TCHAR> line(
			igs::resource::ts_from_mbs(line_tmp)
		);
#endif
		/*--- 読み込みエラーチェック ---*/
		if (ifs.bad()) {
			std::basic_string<TCHAR> str(TEXT("\""));
			str += file_path;
			str += TEXT("\" can not read");
			throw std::domain_error(
				igs_resource_msg_from_er(str));
		}
		/*--- 文字行の整形 ---*/
		/* unix(0x0a)でwindows(0x0d0x0a)の改行コード問題の対処
		(0x0d)が行の終わりに残ってしまうので削除する
		なお、windowsでunixの改行コードは問題なく読む */
		if (0<line.size() && TEXT('\r')==line.at(line.size()-1)) {
			line.erase( line.size()-1 );
		}
		/*--- 1行追加 ---*/
		filestr += line;
		filestr += TEXT('\n');
	}
	/*--- ファイル閉じる ---*/
	ifs.close();
 }
}
int main(int argc, char *argv[]) {
	if (2 != argc) {
		std::cout
			<< "Usage : " << argv[0] << " file\n"
			<< "file:\n"
			<< "File Must be Japanese Code by\n"
			<< "\t'locale' on unix\n"
			<< "\t'chcp'   on windows7sp1\n"
			;
		return 1;
	}
  try {
	igs::resource::locale_to_jp();

	/*--- textファイル全て読む。改行コードは1文字であること! ---*/
	/* 改行でlineを切り取っているのでfile最後行まで改行が必須
	通常のメッセージは最終行まで改行はある */
	std::basic_string<TCHAR> filestr;
	textfile_to_string_( igs::resource::ts_from_mbs(argv[1]) ,filestr );

	/*--- パラメータ ---*/
	std::basic_string<TCHAR>&lifoline(filestr);// Last In/First Out line
	std::basic_string<TCHAR> fo_line;

	/*--- 1文字ずつ処理する ---*/
	/* 改行でlineを切り取る */
	while (igs::text::first_out_line(lifoline, fo_line)) {
		std::basic_ostringstream<TCHAR> ost;
			ost << TEXT("msg  \"") << fo_line << TEXT("\"\n");
#if _WIN32 || !UNICODE /* 通常 */
		std::TCOUT << ost.str();
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
		std::cout << igs::resource::mbs_from_ts( ost.str());
#endif
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_TEXT_FIRST_OUT_LINE */
/*
rem vc2008sp1(32/64bits) :132,149 w! tes_msc_igs_text_first_o.bat
set _DEB=DEBUG_IGS_TEXT_FIRST_OUT_LINE
set _SRC=igs_text_first_out_line
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_text_first_o_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_text_first_o_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :150,152 w! tes_gcc_igs_text_first_o.csh
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_FIRST_OUT_LINE -I. igs_text_first_out_line.cxx -o tes_gcc_${HOSTTYPE}_igs_text_first_o_mb
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_FIRST_OUT_LINE -DUNICODE -I. igs_text_first_out_line.cxx -o tes_gcc_${HOSTTYPE}_igs_text_first_o_wc
*/
