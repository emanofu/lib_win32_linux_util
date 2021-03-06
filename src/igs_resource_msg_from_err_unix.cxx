#include <cerrno>
#include <vector>
#include <stdexcept> // std::domain_error(-)
#include "igs_resource_msg_from_err.h"

/*------ localeを日本に設定し日本語を扱うことを指示(必須)
使う文字コードは"locale -a"で調べる */
void igs::resource::locale_to_jp(void) {
	setlocale(LC_CTYPE, "ja_JP.utf8");
}
#if 0 //------
/*
	リサーチ中
	日本語環境を環境変数から取ってくる場合のルーチン
	deamonの場合これでいいのか???
	さらに調査が必要
	2013-02-18
 */
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
void igs::resource::locale_to_jp(void) {
	/*
		Software Design 1993年3月号
		"SPECIAL ISSUE  どうする?UNIXの日本語環境"
		稚内北星短期大学  丸山 不二夫
		Page14  リスト4  より

		X11R5での日本語処理 - ロケールの設定
		標準的な処理手順
		全てのX(lib)プログラムの先頭で行う
	*/

	/* 次の場合、環境変数 LANG から、地域名を得ます
		引数locale が "" の場合、
		ロケールの各部分の設定には環境変数が参照される。
		その詳細は実装依存である。
		Linux Programmer’s Manual  July 4, 1999より
	*/
	if ( ::setlocale( LC_ALL ,"" ) == NULL ) {
		throw std::domain_error( "Can not set locale." );
	}

	/* Xlibが、現在の地域をサポートしているかチェックします */
	if ( !::XSupportsLocale() ) {
		std::string msg(X is not support locale ");
		msg += setlocale( LC_ALL ,NULL );
		msg += ".\n";
		throw std::domain_error( msg.c_str() );
	}

	/* 次の場合、環境変数 XMODIFIERS から修飾子が得られます
	この修飾子は。入力メソッド (IM) の指定に使われます */
	if ( ::XSetLocaleModifiers("") == NULL ) {
		throw std::domain_error( "Can not set locale modifiers." );
	}
}
/*
#g++ -L/usr/X11R6/lib/ -lXmu -lXext -lX11 -lgthread -lglib -lm tes82.cxx
g++ tes82.cxx -L/usr/X11R6/lib/ -lX11
*/
#endif//------
/*------ マルチバイト文字列 --> ワイド文字文字列 ------*/
void igs::resource::mbs_to_wcs(
	const std::string&mbs ,std::wstring&wcs
) {
	size_t length = 0;
 {
	const char *src_ptr = mbs.c_str();
	mbstate_t ss; ::memset(&ss ,0 ,sizeof(ss));
	length = ::mbsrtowcs(NULL ,&src_ptr ,0 ,&ss);
	if (length == (size_t)(-1)) { /* 不正なマルチバイト列に遭遇した */
		throw std::domain_error(
			"mbstowcs(-) got bad multi byte character,when size"
		);
	}
	if (length <= 0) { return; } /* 文字がないなら何もしない */
	++length;
 }
	//std::vector<wchar_t> dst(length);
	wcs.resize(length);
 {
	const char *src_ptr = mbs.c_str();
	mbstate_t ss; ::memset(&ss ,0 ,sizeof(ss));
	//length = ::mbsrtowcs(&dst.at(0) ,&src_ptr ,length ,&ss);
	length = ::mbsrtowcs(
		const_cast<wchar_t *>(wcs.c_str()) ,&src_ptr ,length ,&ss);
	if (length == (size_t)(-1)) { /* 不正なマルチバイト列に遭遇した */
		throw std::domain_error(
			"mbstowcs(-) got bad multi byte character,when conv"
		);
	}
	if (length <= 0) {
		throw std::domain_error(
			"mbstowcs(-) got zero or under equal -2 "
		);
	}
 }
	//wcs = std::wstring(dst.begin() ,dst.end()-1);/* 終端以外を */
	wcs.erase(wcs.end()-1); /* 終端文字を消す */
}
/*------ ワイド文字文字列 --> マルチバイト文字列 ------*/
void igs::resource::wcs_to_mbs(
	const std::wstring&wcs ,std::string&mbs
) {
	size_t length = 0;
 {
	const wchar_t *src_ptr = wcs.c_str();
	mbstate_t ss; ::memset(&ss ,0 ,sizeof(ss));
	length = ::wcsrtombs(NULL ,&src_ptr ,0 ,&ss);
	if (length <= 0) { return; } /* 文字がないなら何もしない */
	++length;
 }
	//std::vector<char> dst(length);
	mbs.resize(length);
 {
	const wchar_t *src_ptr = wcs.c_str();
	mbstate_t ss; ::memset(&ss ,0 ,sizeof(ss));
	//length = ::wcsrtombs(&dst.at(0) ,&src_ptr ,length ,&ss);
	length = ::wcsrtombs(
		const_cast<char *>(mbs.c_str()) ,&src_ptr ,length ,&ss);
	if (length <= 0) {
		throw std::domain_error(
			"wcstombs(-) got bad wide character"
		);
	}
 }
	//mbs = std::string(dst.begin() ,dst.end()-1);/* 終端以外を */
	mbs.erase(mbs.end()-1); /* 終端文字を消す */
}
/*------ UNICODE宣言ならマルチバイト文字列をワイド文字文字列に変換 ------*/
const std::basic_string<TCHAR> igs::resource::ts_from_mbs(
	const std::string&mbs
) {
#if defined UNICODE
	std::wstring wcs;
	igs::resource::mbs_to_wcs( mbs ,wcs );
	return wcs;
#else
	/* MBCSの場合のsize()は文字数ではなくchar(byte)数,2bytes文字は2 */
	return mbs;
#endif
}
/*------ UNICODE宣言ならワイド文字文字列をマルチバイト文字列に変換 ------*/
const std::string igs::resource::mbs_from_ts(
	const std::basic_string<TCHAR>&ts
) {
#if defined UNICODE
	std::string mbs;
	igs::resource::wcs_to_mbs( ts ,mbs );
	return mbs;
#else
	/* MBCSの場合のsize()は文字数ではなくchar(byte)数,2bytes文字は2 */
	return ts;
#endif
}
/*------ cp932を含む文字列をutf-8に変換(マルチバイト文字列) ------*/
namespace {
 const std::string iconv_to_from_(
	const std::string&text
	,const char *tocode
	,const char *fromcode
 ) {
	iconv_t icd = ::iconv_open(tocode, fromcode); // "iconv --list"
	if (reinterpret_cast<iconv_t>(-1) == icd) {
		throw std::domain_error(
		 igs_resource_msg_from_err(TEXT("iconv_open(-)"),errno)
		);
	}

	std::vector<char> dst(text.size()*4);
	char *inbuf = const_cast<char *>(text.c_str());
	char *outbuf = &dst.at(0);
	size_t inbytesleft = text.size();
	size_t outbytesleft = dst.size();
	size_t ret = ::iconv(
		icd, &inbuf, &inbytesleft, &outbuf, &outbytesleft
	);
	*outbuf = '\0';

	/*
	retに処理した数が入るはずだが、rhel5ではゼロが帰るので、
	処理数を別途計算する
	*/
	ret = dst.size() - outbytesleft;
	if (ret <= 0) {
	//if (static_cast<size_t>(-1) == ret) {
		::iconv_close(icd);

		throw std::domain_error(
		 igs_resource_msg_from_err(TEXT("iconv(-)"),errno)
		);
	}

	if (-1 == ::iconv_close(icd)) {
		throw std::domain_error(
		 igs_resource_msg_from_err(TEXT("iconv_close(-)"),errno)
		);
	}

	std::string mbs(std::string( dst.begin(),dst.begin() +ret ));
	return mbs;
 }
}
const std::string igs::resource::utf8_from_cp932_mb(const std::string&text){
	return iconv_to_from_( text ,"UTF-8", "CP932" ); // "iconv --list"
}
const std::string igs::resource::cp932_from_utf8_mb(const std::string&text){
	return iconv_to_from_( text ,"CP932" ,"UTF-8" ); // "iconv --list"
}
/*------ エラーメッセージ表示の元関数、直接呼び出すことはしない ------*/
#include <cerrno> // errno
#include <cstring> // strerror_r()
#include <sstream> // std::istringstream
#include "igs_resource_msg_from_err.h"

const std::string igs::resource::msg_from_err_(
	const std::basic_string<TCHAR>&tit
	,const int erno
	,const std::string&file
	,const std::string&line
	,const std::string&pretty_function
	,const std::string&comp_type
	,const std::string&gnuc
	,const std::string&gnuc_minor
	,const std::string&gnuc_patchlevel
	,const std::string&gnuc_rh_release
	,const std::string&date
	,const std::string&time
) {
	std::string errmsg;
	errmsg += '\"';

	/* フルパスで入ってきた場合ファイル名だけにする */
	std::string::size_type index = file.find_last_of("/\\");
	if (std::basic_string<TCHAR>::npos != index) {
	 errmsg += file.substr(index+1);
	} else {
	 errmsg += file;
	}

	errmsg += ':'; errmsg += line;
	errmsg += ':'; errmsg += comp_type;
	errmsg += ':'; errmsg += gnuc;
	errmsg += '.'; errmsg += gnuc_minor;
	errmsg += '.'; errmsg += gnuc_patchlevel;
	errmsg += '-'; errmsg += gnuc_rh_release;
	{
	 std::istringstream ist(date);
	 std::string month,day,year;
	 ist >> month; ist >> day; ist >> year;
	 errmsg += ':'; errmsg += year;
	 errmsg += ':'; errmsg += month;
	 errmsg += ':'; errmsg += day;
	}
	errmsg += ':'; errmsg += time;
	errmsg += '\"';
	errmsg += ' ';
	errmsg += '\"';
	errmsg += pretty_function;
	errmsg += '\"';
	errmsg += ' ';
	errmsg += '\"';
	if (0 < tit.size()) {
		errmsg += igs::resource::mbs_from_ts(tit);
	}
      if (0 != erno) {
	errmsg += ':';
#if defined __HP_aCC
	/* 
	HP-UX(v11.23)では、strerror_r()をサポートしない。
	注意::strerror()はThread SafeではなくMulti Threadでは正常動作しない
	*/
	errmsg += ::strerror(erno);
#elseif (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
	/*
	POSIX.1.2002で規定されたXSI準拠のバージョンのstrerror_r()
	*/
	char buff[4096];
	const int ret = ::strerror_r(erno,buff,sizeof(buff));
	if (0 == ret) {
		errmsg += buff;
	} else if (-1 == ret) {
	 swtich (errno) {
	 case EINVAL: errmsg +=
		"strerror_r() gets Error : The value of errnum is not a "
		"valid error number.";
		/* errnum の値が有効なエラー番号ではない */
		break;
	 case ERANGE: errmsg +=
		"strerror_r() gets Error : Insufficient storage was "
		"supplied via strerrbuf and buflen  to contain the "
		"generated message string.";
 		/* エラーコードを説明する文字列のために、
			充分な領域が確保できな かった */
		break;
	 deatult: errmsg +=
		"strerror_r() gets Error and Returns bad errno";
		break;
	 }
	} else {
		errmsg +=
		"strerror_r() returns bad value";
	}
#else
	/* rhel4はここに来る、、、
	GNU仕様のバージョンのstrerror_r()。非標準の拡張
	これはThread Safeか??????
	*/
	char buff[4096];
	const char *ret = ::strerror_r(erno,buff,sizeof(buff));
	errmsg += ret;
#endif
      }
	errmsg += '\"';
	return errmsg;
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_RESOURCE_MSG_FROM_ERR
#include <iostream>	// std::cout
#include <fstream>	// std::basic_ifstream
#include <sys/types.h>	// open()
#include <sys/stat.h>	// open()
#include <fcntl.h>	// open(), _O_RDONLY
#include <iconv.h>	// iconv_open(-) ,iconv(-) ,iconv_close(-)
#include "igs_resource_msg_from_err_utf8.cxx"

int main(int argc, char *argv[])
{
	igs::resource::locale_to_jp();

  try {
	if (2==argc && !strcmp(argv[1],"type")) {
		std::TCOUT << TEXT("typeid_name \"") << typeid(TCHAR).name()
				<< TEXT("\"\n");
		if (typeid(TCHAR) == typeid(char)) {
			std::TCOUT << TEXT("MBCS(Multi Byte Character Set)")
				<< std::endl;
		} else {
			std::TCOUT << TEXT("UNICODE16")
				<< std::endl;
		}
		return 0;
	} else if (2==argc && !strcmp(argv[1],"m2w")) {
		std::wstring wcs;
		igs::resource::mbs_to_wcs(m2w_mbs ,wcs);
		std::string mbs;
		igs::resource::wcs_to_mbs(wcs,mbs);
		std::cout  << "igs::resource::mbs_to_wcs(-)\n";
		std::cout  <<  "m2w_mbs=\"" << m2w_mbs <<  "\"\n";
		std::wcout << L"wcs    =\"" << wcs     << L"\"\n";
		std::cout  <<  "mbs    =\"" << mbs    <<  "\"\n";
	} else if (2==argc && !strcmp(argv[1],"w2m")) {
		std::string mbs;
		igs::resource::wcs_to_mbs(w2m_wcs ,mbs);
		std::cout  <<  "igs::resource::wcs_to_mbs(-)\n";
		std::wcout << L"w2m_wcs=\"" << w2m_wcs << L"\"\n";
		std::cout  <<  "mbs    =\"" << mbs     <<  "\"\n";
	} else if (2==argc && !strcmp(argv[1],"w4m")) {
		std::basic_string<TCHAR> ts(
			igs::resource::ts_from_mbs(w4m_mbs)
		);
		std::cout  << "igs::resource::ts_from_mbs(-)\n";
		std::cout  <<      "w4m_mbs=\""  << w4m_mbs << "\"\n";
		std::TCOUT << TEXT("ts     =\"") << ts
							<< TEXT("\"\n");
			;
	} else if (2==argc && !strcmp(argv[1],"m4w")) {
		std::string mbs(
			igs::resource::mbs_from_ts(m4w_ts)
		);
		std::cout << "igs::resource::mbs_from_ts(-)\n";
		std::TCOUT << TEXT("m4w_ts=\"") << m4w_ts
							<< TEXT("\"\n");
		std::cout  <<      "mbs   =\"" << mbs << "\"\n";
	} else if (3==argc && !strcmp(argv[1],"cp932utf8")) {
		/* cp932 --> utf-8変換  ,euc-jp --> utf-8はだめ */
		//std::basic_ifstream<TCHAR> ifs(igs::resource::ts_from_mbs( argv[2]).c_str() );
		std::basic_ifstream<TCHAR> ifs( argv[2] );
		std::basic_string<TCHAR> line;
		while (std::getline(ifs,line)) {
			/*
				読み込んだlineに改行コードはない
				ただしunixでdosの改行コードを読んだときは
				0x0dが行末に残る
			*/
			std::cout
			<< igs::resource::utf8_from_cp932_mb(
				igs::resource::mbs_from_ts(line)
			)
			<< std::endl;
		}
		ifs.close();
	} else if (3==argc && !strcmp(argv[1],"utf8cp932")) {
		/* utf-8 --> cp932 変換  */
#if 0
		/* utf-8をwcで読んで、
		igs::resource::mbs_from_ts(-)する所で((たぶん)だめ
		*/
		std::basic_ifstream<TCHAR> ifs(igs::resource::ts_from_mbs(
			argv[2]).c_str() );
		std::basic_string<TCHAR> line;
		while (std::getline(ifs,line)) {
			/*
				読み込んだlineに改行コードはない
				ただしunixでdosの改行コードを読んだときは
				0x0dが行末に残る
			*/
			std::cout
			<< igs::resource::cp932_from_utf8_mb(
				igs::resource::mbs_from_ts(line)
			)
			<< std::endl;
		}
		ifs.close();
#endif
		std::ifstream ifs( argv[2] );
		std::string line;
		while (std::getline(ifs,line)) {
			/*
				読み込んだlineに改行コードはない
				ただしunixでdosの改行コードを読んだときは
				0x0dが行末に残る
			*/
			std::cout
			<< igs::resource::cp932_from_utf8_mb( line )
			<< std::endl;
		}
		ifs.close();
	} else if (3==argc && !strcmp(argv[1],"err")) {
		const std::basic_string<TCHAR> path(
			igs::resource::ts_from_mbs(argv[2])
		);
		const int fd = ::open( argv[2], O_RDONLY);
		if (fd < 0) {
			throw std::domain_error(
			 igs_resource_msg_from_err(path,errno)
			);
		}
		::close(fd);
	} else if (2==argc && !strcmp(argv[1],"er")) {
		throw std::domain_error(
		  igs_resource_msg_from_er( er_ts )
		);
	} else if (2==argc && !strcmp(argv[1],"localenum")) {
		igs::resource::locale_to_jp();

		/* 数値書式ロケール設定の確認テスト */
		std::ostringstream ost;
		ost << 1000000;
		std::cout
			<< "locale numeric category test "
			<< ost.str()
			<< std::endl;
	} else {
		std::cout << "Usage : " << argv[0]
					<< " type/m2w/w2m/w4m/m4w/er\n";
		std::cout << "Usage : " << argv[0] << " err file\n";
		std::cout << "Usage : " << argv[0] << " cp932utf8 file\n";
		std::cout << "Usage : " << argv[0] << " utf8cp932 file\n";
		std::cout << "Usage : " << argv[0] << " localenum\n";
		return 1;
	}
  }
  catch (std::exception&e){
	std::cerr << "msg_from_err=\"" << e.what() << "\"\n";
  }
  catch (...) {
	std::cerr << "other exception" << std::endl;
  }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_MSG_FROM_ERR */
/*
# gcc(32/64bits) :476,478 w! tes_gcc_igs_rsc_msg_f_err.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_MSG_FROM_ERR -I. igs_resource_msg_from_err.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_msg_f_err_mb
g++ -Wall -O2 -g -DUNICODE -DDEBUG_IGS_RESOURCE_MSG_FROM_ERR -I. igs_resource_msg_from_err.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_msg_f_err_wc
# gcc(32/64bits) :479,497 w! tes_gcc_igs_rsc_msg_f_err_t.csh
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb type
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc type
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb m2w
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc m2w
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb w2m
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc w2m
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb w4m
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc w4m
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb m4w
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc m4w
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb er
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc er
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb err not_exist_file
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc err not_exist_file
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb cp932utf8 sample.txt > tmpgcc_utf8_mb.txt
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc cp932utf8 sample.txt > tmpgcc_utf8_wc.txt
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_mb utf8cp932 tmpgcc_utf8_mb.txt > tmpgcc_cp932_mb.txt
./tes_gcc_x86_64-linux_igs_rsc_msg_f_err_wc utf8cp932 tmpgcc_utf8_wc.txt > tmpgcc_cp932_wc.txt
*/
