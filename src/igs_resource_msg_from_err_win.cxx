// #include <vector>
#include "igs_resource_msg_from_err.h"

/*------ localeを日本に設定し日本語を扱うことを指示
(しないと日本語文字部分のみ処理しない)
ただし数値カテゴリはC localeのままに
(しないと3桁ごとにカンマが付く(1000-->1,000)) */
void igs::resource::locale_to_jp(void) {
	std::locale::global(
	 std::locale(std::locale(),"japanese",std::locale::ctype)
	);
}
/*------ マルチバイト文字列 --> ワイド文字文字列 ------*/
void igs::resource::mbs_to_wcs(
	const std::string&mbs ,std::wstring&wcs ,const UINT code_page
) {
	/* 第４引数で -1 指定により終端文字を含む大きさを返す */
	/*	int MultiByteToWideChar(
 			UINT CodePage
			,DWORD dwFlags
			,LPCSTR lpMultiByteStr
			,int cbMultiByte
			,LPWSTR lpWideCharStr
			,int cchWideChar        
		);
	*/
	int length = ::MultiByteToWideChar(
		code_page,0,mbs.c_str(),-1,0,0
	);
	if (length <= 1) {return;} /* 終端以外の文字がないなら何もしない */

/***	std::vector<wchar_t> buf(length);
	length = ::MultiByteToWideChar(
		code_page ,0 ,mbs.c_str() ,-1
		,&buf.at(0) ,static_cast<int>(buf.size())
	);***/
	wcs.resize(length);
	length = ::MultiByteToWideChar(
		code_page ,0 ,mbs.c_str() ,-1
		,const_cast<LPWSTR>(wcs.data())
		,static_cast<int>(wcs.size())
	);
	if (0 == length) {
		switch (::GetLastError()) {
		case ERROR_INSUFFICIENT_BUFFER:
			throw std::domain_error(
			"MultiByteToWideChar():insufficient buffer"
			);
		case ERROR_INVALID_FLAGS:
			throw std::domain_error(
			"MultiByteToWideChar():invalid flags"
			);
		case ERROR_INVALID_PARAMETER:
			throw std::domain_error(
			"MultiByteToWideChar():invalid parameter"
			);
		case ERROR_NO_UNICODE_TRANSLATION:
			throw std::domain_error(
			"MultiByteToWideChar():no unicode translation"
			);
		}
	}
	//wcs = std::wstring(buf.begin() ,buf.end()-1); /* 終端以外を */
	wcs.erase(wcs.end()-1); /* 終端文字を消す。end()は終端より先位置 */
}
/*------ ワイド文字文字列 --> マルチバイト文字列 ------*/
void igs::resource::wcs_to_mbs(
	const std::wstring&wcs ,std::string&mbs ,const UINT code_page
) {
	/* 第４引数で -1 指定により終端文字を含む大きさを返す */
	int length = ::WideCharToMultiByte(
		code_page ,0 ,wcs.c_str() ,-1 ,0,0,0,0
	);
	if (length <= 1) {return;} /* 終端以外の文字がないなら何もしない */

/***	std::vector<char> buf(length);
	length = ::WideCharToMultiByte(
		code_page ,0 ,wcs.c_str() ,-1
		,&buf.at(0) ,static_cast<int>(buf.size()) ,0 ,NULL
	);***/
	mbs.resize(length);
	length = ::WideCharToMultiByte(
		code_page ,0 ,wcs.c_str() ,-1
		,const_cast<LPSTR>(mbs.data())
		,static_cast<int>(mbs.size()) ,0 ,NULL
	);
	if (0 == length) {
		switch (::GetLastError()) {
		case ERROR_INSUFFICIENT_BUFFER:
			throw std::domain_error(
				"WideCharToMultiByte():insufficient buffer"
			);
		case ERROR_INVALID_FLAGS:
			throw std::domain_error(
				"WideCharToMultiByte():invalid flags"
			);
		case ERROR_INVALID_PARAMETER:
			throw std::domain_error(
				"WideCharToMultiByte():invalid parameter"
			);
		}
	}
	//mbs = std::string(buf.begin() ,buf.end()-1); /* 終端以外を */
	mbs.erase(mbs.end()-1); /* 終端文字を消す。end()は終端より先位置 */
}
/*------ UNICODE宣言ならマルチバイト文字列をワイド文字文字列に変換 ------*/
const std::basic_string<TCHAR> igs::resource::ts_from_mbs(
	const std::string&mbs
	,const UINT code_page
) {
#if defined UNICODE
	std::wstring wcs;
	igs::resource::mbs_to_wcs( mbs ,wcs ,code_page );
	return wcs;
#else
	code_page;
	/* MBCSの場合のsize()は文字数ではなくchar(byte)数,2bytes文字は2 */
	return mbs;
#endif
}
/*------ UNICODE宣言ならワイド文字文字列をマルチバイト文字列に変換 ------*/
const std::string igs::resource::mbs_from_ts(
	const std::basic_string<TCHAR>&ts
	,const UINT code_page
) {
#if defined UNICODE
	std::string mbs;
	igs::resource::wcs_to_mbs( ts ,mbs ,code_page );
	return mbs;
#else
	code_page;
	/* MBCSの場合のsize()は文字数ではなくchar(byte)数,2bytes文字は2 */
	return ts;
#endif
}
/*------ cp932を含む文字列をutf-8に変換(マルチバイト文字列) ------*/
const std::string igs::resource::utf8_from_cp932_mb(const std::string&text){
	std::wstring wcs;
	igs::resource::mbs_to_wcs( text ,wcs );
	std::string mbs;
	igs::resource::wcs_to_mbs( wcs ,mbs ,CP_UTF8 );
	return mbs;
}
/*------ utf-8を含む文字列をcp932に変換(マルチバイト文字列) ------*/
const std::string igs::resource::cp932_from_utf8_mb(const std::string&text){
	std::wstring wcs;
	igs::resource::mbs_to_wcs( text ,wcs ,CP_UTF8 );
	std::string mbs;
	igs::resource::wcs_to_mbs( wcs ,mbs ,932 );
	return mbs;
}
/*------ エラーメッセージ表示の元関数、直接呼び出すことはしない ------*/
#include <sstream>
const std::string igs::resource::msg_from_err_(
	const std::basic_string<TCHAR>&tit
	,const DWORD error_message_id
	,const std::basic_string<TCHAR>&file
	,const std::basic_string<TCHAR>&line
	,const std::basic_string<TCHAR>&funcsig
	,const std::basic_string<TCHAR>&comp_type
	,const std::basic_string<TCHAR>&msc_full_ver
	,const std::basic_string<TCHAR>&date
	,const std::basic_string<TCHAR>&time
) {
/*
汎用データ型  ワイド文字(UNICODE)(※1)	マルチバイト文字(_MBCS)(※2)
TCHAR	      wchar_t			char 
LPTSTR	      wchar_t *			char * 
LPCTSTR	      const wchar_t *		const char * 
※1  1文字を16ビットのワイド文字として表すUnicode を使う方法
	すべての文字が 16 ビットに固定されます。
	マルチバイト文字に比べ、メモリ効率は低下しますが処理速度は向上します
※2  1文字を複数のバイトで表すマルチバイト文字
	MBCS(Multibyte Character Set) と呼ばれる文字集合を使う方法
	可変長だが、事実上、サポートされているのは 2 バイト文字までなので、
	マルチバイト文字の 1 文字は 1 バイトまたは 2 バイトとなります。
	Windows 2000 以降、Windows は内部で Unicode を使用しているため、
	マルチバイト文字を使用すると内部で文字列の変換が発生するため
	オーバーヘッドが発生します。
	UNICODEも_MBCSも未定義のときはこちらになる。
*/
	std::basic_string<TCHAR> errmsg;
	errmsg += TEXT('\"');

	/* makefile-vc2008mdAMD64等でコンパイルすると
	フルパスで入ってくるのでファイル名だけにする
	*/
	std::basic_string<TCHAR>::size_type index =
				file.find_last_of(TEXT("/\\"));
	if (std::basic_string<TCHAR>::npos != index) {
	 errmsg += file.substr(index+1);
	} else {
	 errmsg += file;
	}

	errmsg += TEXT(':'); errmsg += line;
	errmsg += TEXT(':'); errmsg += comp_type;
	errmsg += TEXT(":"); errmsg += msc_full_ver;
	{
	 std::basic_istringstream<TCHAR> ist(date);
	 std::basic_string<TCHAR> month,day,year;
	 ist >> month; ist >> day; ist >> year;
	 errmsg += TEXT(':'); errmsg += year;
	 errmsg += TEXT(':'); errmsg += month;
	 errmsg += TEXT(':'); errmsg += day;
	}
	errmsg += TEXT(':'); errmsg += time;
	errmsg += TEXT('\"');
	errmsg += TEXT(' ');
	errmsg += TEXT('\"');
	errmsg += funcsig;
	errmsg += TEXT('\"');
	errmsg += TEXT(' ');
	errmsg += TEXT('\"'); if (0 < tit.size()) { errmsg += tit; }
      if (NO_ERROR != error_message_id) {
	errmsg += TEXT(':');
	  LPTSTR lpMsgBuf = 0;
	  if (0 < ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS
		,NULL
		,error_message_id
		,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT)/* 既定言語 */
		,reinterpret_cast<LPTSTR>(&lpMsgBuf)
		,0
		,NULL
	  )) {	/* --- 成功 --- */
		errmsg += lpMsgBuf;
		::LocalFree(lpMsgBuf);
		std::string::size_type index =
			errmsg.find_first_of(TEXT("\r\n"));
		if (std::string::npos != index) {
			errmsg.erase(index);
		}
	  }
	  else { /* エラー */
		errmsg += TEXT("FormatMessage() can not get (error)message"
		);
	  }
      }
	errmsg += TEXT('\"');

	/* MBCSで返す */
	return igs::resource::mbs_from_ts( errmsg );
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_RESOURCE_MSG_FROM_ERR
#include <iostream> // std::cout
#include <fstream>  // std::basic_ifstream
#include <stdexcept> // std::domain_error()

/*
埋め込む2byte文字コードは、MS-Cでutf-8なら行末に"?0"が必要...
MS-C --> cp932 ,g++ --> utf-8としてそれぞれに分けないと駄目!!!
コメント内は
MS-C --> euc-jp/utf-8 (with Warning) ,cp932
*/
#include "igs_resource_msg_from_err_cp932.cxx"

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
		const void *p_lib = ::LoadLibrary(path.c_str());
		if (NULL == p_lib) {
			throw std::domain_error(
			 igs_resource_msg_from_err(path,::GetLastError())
			);
		}
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
rem vc2010(32/64bits) :407,422 w! tes_msc16_igs_rsc_msg_f_err.bat
set _DEB=DEBUG_IGS_RESOURCE_MSG_FROM_ERR
set _SRC=igs_resource_msg_from_err
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_msg_f_err_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_msg_f_err_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2010(32/64bits) :423,442 w! tes_msc16_igs_rsc_msg_f_err_t.bat
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe type
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe type
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe m2w
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe m2w
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe w2m
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe w2m
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe w4m
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe w4m
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe m4w
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe m4w
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe er
 (tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe er  2>&1) > tmpmsc16_er.txt
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe er
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe err not_exist_file
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe err not_exist_file
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe cp932utf8 sample.txt > tmpmsc16_utf8_mb.txt
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe cp932utf8 sample.txt > tmpmsc16_utf8_wc.txt
.\tes_msc16_AMD64_igs_rsc_msg_f_err_mb.exe utf8cp932 tmpmsc16_utf8_mb.txt > tmpmsc16_cp932_mb.txt
.\tes_msc16_AMD64_igs_rsc_msg_f_err_wc.exe utf8cp932 tmpmsc16_utf8_wc.txt > tmpmsc16_cp932_wc.txt
*/
/*
rem vc2008sp1(32/64bits) :448,465 w! tes_msc15_igs_rsc_msg_f_err.bat
set _DEB=DEBUG_IGS_RESOURCE_MSG_FROM_ERR
set _SRC=igs_resource_msg_from_err
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_msg_f_err_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_msg_f_err_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :466,485 w! tes_msc15_igs_rsc_msg_f_err_t.bat
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe type
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe type
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe m2w
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe m2w
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe w2m
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe w2m
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe w4m
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe w4m
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe m4w
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe m4w
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe er
 (tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe er  2>&1) > tmpmsc15_er.txt
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe er
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe err not_exist_file
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe err not_exist_file
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe cp932utf8 sample.txt > tmpmsc15_utf8_mb.txt
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe cp932utf8 sample.txt > tmpmsc15_utf8_wc.txt
.\tes_msc15_AMD64_igs_rsc_msg_f_err_mb.exe utf8cp932 tmpmsc15_utf8_mb.txt > tmpmsc15_cp932_mb.txt
.\tes_msc15_AMD64_igs_rsc_msg_f_err_wc.exe utf8cp932 tmpmsc15_utf8_wc.txt > tmpmsc15_cp932_wc.txt
*/
