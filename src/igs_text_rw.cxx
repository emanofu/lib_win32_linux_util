#include <vector> // std::vector
#include <fstream> // std::basic_ofstream,std::basic_ifstream
#include <sstream> // std::basic_istringstream,std::ostringstream ost
#include <stdexcept> // std::domain_error(-)
#include "igs_resource_msg_from_err.h"
#include "igs_text_rw.h"

/*
	!ifs ,bad() ,good() ,fail()の詳細
	2013-3-7

	"「プログラミング言語C++」第三版 Bjarne Stroustrup著"より
		bad()	ストリームが壊れている(致命的)
			すべて失われている
		fail()	次の演算は失敗する
			同時にbad()でなければストリームは壊れておらず、
			失われた文字はない
		good()	次の演算はおそらく成功する
			直前の入力演算は成功
		!good()	次の演算はnull演算となり失敗する。


	rhel5 "/usr/include/c++/4.1.2/bits/basic_ios.h"より
		!ifs	return this->fail();

	streamのループ方法
	2013-3-8

	ファイル読み込み
	std::ifstream ifs("tmp.txt");
	while (std::getline(ifs,line)) {
		if (ifs.bad()) { // error
		}
	}

	文字列の分割(注意:double quate("50"-->50)は外せない)
	std::istringstream ifs("q w e r t y \"50\"");
	while (!ifs.eof()) {
		std::string str; ifs >> str;
		if (str.empty()) { break; }	<-- この行不要か?
	}
*/
namespace {
 const std::string er_(
	const std::basic_string<TCHAR>&file_path
	,const std::basic_string<TCHAR>&add_msg
 ) {
	std::basic_string<TCHAR> str(TEXT("\""));
	str += file_path;
	str += TEXT("\" can not ");
	str += add_msg;
	return igs_resource_msg_from_er(str);
 }
}
//--------------------------------------------------------------------
void igs::text::read_lines(
	LPCTSTR file_path
	,std::vector< std::basic_string<TCHAR> >&lines
) {
	/*--- ファイル開く ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_ifstream<TCHAR> ifs( file_path );
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::ifstream ifs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ifs) {
		throw std::domain_error(er_(file_path,TEXT("open_r_s")));
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
			throw std::domain_error(er_(file_path,TEXT("r_s")));
		}
		//if ( ifs.fail()) { continue; } /* 次の演算は失敗する */
		//if (!ifs.good()) { continue; } /* 次の演算はnull演算 */

		if (line.size() <= 0) { continue; }	/* 空行 */
		if (TEXT('#')==line.at(0)) { continue;}	/* コメント行 */
		{
			std::basic_istringstream<TCHAR> ist(line);
			std::basic_string<TCHAR> str;
			ist >> str;
			if (str.size() <= 0) { continue; } /* 空白行 */
		}
		/*--- 文字行の整形 ---*/
		/* unix(0x0a)でwindows(0x0d0x0a)の改行コード問題の対処
		(0x0d)が行の終わりに残ってしまうので削除する
		なお、windowsでunixの改行コードは問題なく読む */
		if (0<line.size() && 0x0d == line.at(line.size()-1)) {
			line.erase( line.size()-1 );
		}
		/*--- 1行追加 ---*/
		lines.push_back(line);
	}
	/*--- ファイル閉じる ---*/
	ifs.close();
}
//--------------------------------------------------------------------
void igs::text::read_line(
	LPCTSTR file_path
	,std::basic_string<TCHAR>&line
) {
	/*--- ファイル開く ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_ifstream<TCHAR> ifs( file_path );
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::ifstream ifs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ifs) {
		throw std::domain_error(er_(file_path,TEXT("open_r")));
	}
	/*--- 読み込み ---*/
#if _WIN32 || !UNICODE /* 通常 */
	if (!std::getline(ifs,line)) {
		throw std::domain_error(er_(file_path,TEXT("r")));
	}
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	{
	 std::string line_tmp;
	 if (!std::getline(ifs,line_tmp)) {
		throw std::domain_error(er_(file_path,TEXT("r")));
	 }
	 line = igs::resource::ts_from_mbs(line_tmp);
	}
#endif
	if (ifs.bad()) {
		throw std::domain_error(er_(file_path,TEXT("r2")));
	}
	/*--- 文字行の整形 ---*/
	/* unix(0x0a)でwindows(0x0d0x0a)の改行コード問題の対処
	(0x0d)が行の終わりに残ってしまうので削除する
	なお、windowsでunixの改行コードは問題なく読む */
	if (0<line.size() && 0x0d == line.at(line.size()-1)) {
		line.erase( line.size()-1 );
	}
	/*--- ファイル閉じる ---*/
	ifs.close();
}
//--------------------------------------------------------------------
void igs::text::append_line( /* 改行ありで追加保存 */
	const std::basic_string<TCHAR>&line_with_no_return_code
	,LPCTSTR file_path
) {
	/*--- ファイル開く ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_ofstream<TCHAR> ofs( file_path
		,std::ios::out | std::ios::app
	);
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::ofstream ofs( igs::resource::mbs_from_ts(file_path).c_str()
		,std::ios::out | std::ios::app
	);
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("open_a")));
	}
	/*--- 書き出し ---*/
#if _WIN32 || !UNICODE /* 通常 */
	ofs << line_with_no_return_code << std::endl;
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	ofs	<< igs::resource::mbs_from_ts(line_with_no_return_code)
		<< std::endl;
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("a")));
	}
	/*--- ファイル閉じる ---*/
	ofs.close();
}
//--------------------------------------------------------------------
void igs::text::write_line( /* 改行なしで保存 */
	const std::basic_string<TCHAR>&line
	,LPCTSTR file_path
) {
	/*--- ファイル開く ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_ofstream<TCHAR> ofs( file_path );
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::ofstream ofs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("open_w")));
	}
	/*--- 書き出し ---*/
#if _WIN32 || !UNICODE /* 通常 */
	ofs << line;
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	ofs << igs::resource::mbs_from_ts(line);
#endif
	if (ofs.bad()) {
		throw std::domain_error(er_(file_path,TEXT("w")));
	}
	/*--- ファイル閉じる ---*/
	ofs.close();
}
//--------------------------------------------------------------------
void igs::text::write_lines(
	const std::vector< std::basic_string<TCHAR> >&lines
	,LPCTSTR file_path
) {
	/*--- ファイル開く ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_ofstream<TCHAR> ofs( file_path );
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
	std::ofstream ofs( igs::resource::mbs_from_ts(file_path).c_str());
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("open_w_s")));
	}
	/*--- 書き出し ---*/
	for (unsigned ii=0; ii< lines.size(); ++ii) {
#if _WIN32 || !UNICODE /* 通常 */
		ofs << lines.at(ii) << std::endl;
#else /* unixでUNICODEの時(ファイル名も読込も)MBCS必須 */
		ofs << igs::resource::mbs_from_ts(lines.at(ii)) <<std::endl;
#endif
		if (ofs.bad()) {
			throw std::domain_error(er_(file_path,TEXT("w_s")));
		}
	}
	/*--- ファイル閉じる ---*/
	ofs.close();
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_TEXT_RW
# include <iostream> // std::cout
# include <iomanip> // std::setw(-)
# include "igs_resource_msg_from_err.cxx"
namespace {
 void pr_help_(const char *tit) {
	std::cout
		<< "Usage : " << tit << " act file.txt [...]\n"
		<< "act : w(write) or a(append) or r(read) or r1\n"
		<< "file.txt : filepath\n"
		<< "[...] : writing words\n"
		<< "[exsamples]\n"
		<< "w file.txt line        : write only 1 line\n"
		<< "w file.txt line1 line2 : write 1 or more lines\n"
		<< "a file.txt line        : append only 1 line\n"
		<< "r file.txt             : read 1 or more lines\n"
		<< "r1 file.txt            : read only 1 line\n"
		;
 }
}
int main(int argc, char *argv[]) { 
	/* 日本語文字部分処理のためロケールを日本に設定し、
	数値カテゴリをC localeのままにする */
	igs::resource::locale_to_jp();

  try {
	if (argc < 3) {
		pr_help_(argv[0]);
		return 1;
	}
	const std::basic_string<TCHAR> fpath(
		igs::resource::ts_from_mbs(argv[2])
	);
	/*	注意!!!
	"埋め込み"の日本語文字は、
	linux(rhel5)ではutf-8
	windows(vc2008sp1)ではcp932、
	にしないと正常に動作しない
	*/
	if (argc >= 4 && !strcmp("w",argv[1])) {
		if (argc == 4) { /* 一語を一行として */
			std::cout << "igs::text::write_line(-)\n";
			igs::text::write_line( 
				igs::resource::ts_from_mbs(argv[3])
				,fpath.c_str()
			);
		} else
		if (argc > 4) {
			std::cout << "igs::text::write_lines\n";
			std::vector< std::basic_string<TCHAR> > lines;
			for (int ii=3;ii<argc;++ii) {
				lines.push_back(
					igs::resource::ts_from_mbs(argv[ii])
				);
			}
			igs::text::write_lines( lines ,fpath.c_str() );
		}
	} else
	if (argc == 4 && !strcmp("a",argv[1])) {
		std::cout << "igs::text::append_line\n";
		igs::text::append_line(
			igs::resource::ts_from_mbs(argv[3])
			,fpath.c_str()
		);
	} else
	if (argc == 3 && !strcmp("r",argv[1])) {
		std::cout << "igs::text::read_lines\n";
		std::vector< std::basic_string<TCHAR> > lines;
		igs::text::read_lines( fpath.c_str() ,lines );
		for (unsigned ii=0;ii<lines.size(); ++ii) {
			std::cout
				<< std::setw(3) << ii
				<< "=\""
				<< igs::resource::mbs_from_ts(lines.at(ii))
				<< "\"\n"
				;
		}
	} else
	if (argc == 3 && !strcmp("r1",argv[1])) {
		std::cout << "igs::text::read_line\n";
		std::basic_string<TCHAR> line;
		igs::text::read_line( fpath.c_str() ,line );
		std::cout
			<< "line=\""
			<< igs::resource::mbs_from_ts( line )
			<< "\"\n"
			;
	} else {
		std::cout << "Error : bad argument\n";
		pr_help_(argv[0]);
		return 5;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_TEXT_RW */
/*
rem vc2008sp1(32/64bits) :337,354 w! tes_msc_igs_text_rw.bat
set _DEB=DEBUG_IGS_TEXT_RW
set _SRC=igs_text_rw
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_text_rw_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_text_rw_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :355,357 w! tes_gcc_igs_text_rw.csh
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_RW -I. igs_text_rw.cxx -o tes_gcc_${HOSTTYPE}_igs_text_rw_mb
g++ -Wall -O2 -g -DUNICODE -DDEBUG_IGS_TEXT_RW -I. igs_text_rw.cxx -o tes_gcc_${HOSTTYPE}_igs_text_rw_wc
*/
