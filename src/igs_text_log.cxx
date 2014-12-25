#include <sstream> // std::basic_ostringstream<TCHAR>
#include "igs_resource_append_text_by_atom.h"
#include "igs_resource_exist.h"
#include "igs_resource_msg_from_err.h" // igs::resource::mbs_from_ts(-)
#include "igs_resource_time.h"
#include "igs_path_separation.h"
#include "igs_text_format.h"
#include "igs_text_separation.h"
#include "igs_text_log.h"

igs::text::log::parameters::parameters()
	:err_key(TEXT("err"))
	,out_key(TEXT("out"))
{}

void igs::text::log::append_to_file(
	const std::basic_string<TCHAR>&message_with_no_return /* 改行含ず */
	,const std::basic_string<TCHAR>&file_path
	,const bool&dbgview_sw	/* debug表示指示 */
	,const bool&log_sw	/* log記録実行指示 */
) {
	if (dbgview_sw) {
#if defined _WIN32 /* for win32 debug */
		::OutputDebugString(message_with_no_return.c_str());
#else
		; /* linuxでは今は使わないがdebug用にreserveしとく */
#endif
	}
	if (log_sw) {
/*
日本語文字コードについて、
システム(OS)にネイティブな文字コードのまま保存し、
ファイルの先頭に、
	filecoding cp932
というようにキーワード
	"filecoding"
と文字コード名
	Windows7     --> "cp932"
	rhel5,darwin --> "utf-8"
を示す。
*/
		igs::text::separation tsep;
		std::basic_string<TCHAR> msg2(message_with_no_return);
		msg2 += tsep.line;
		igs::resource::append_text_by_atom(
			file_path ,igs::resource::mbs_from_ts(msg2)
		);
	}
}

/* file_pathの中で必要なdirectoryはパスを得る関数の中で生成する */
// append_hms_key_msg_ret
void igs::text::log::append_hms_key_msg_ret(
	 const std::basic_string<TCHAR>&category_word
	,const std::basic_string<TCHAR>&message_with_no_return/* 改行含ず */
	,const std::basic_string<TCHAR>&file_path
	,const bool&dbgview_sw	/* debug表示指示 */
	,const bool&log_sw	/* log記録実行指示 */
	,const bool&ymdhms_sw	/* hmsでなくymdhmsを各行に記録指示 */
) {
	if (!dbgview_sw && !log_sw) { return; }

	std::basic_string<TCHAR> str;
	igs::text::separation tsep;
	if (false == igs::resource::exist::file(file_path.c_str())) {
		/* ファイルの頭に以下の2行を記述する
			# This file is "file_path"
			#time category_word message...
		*/
		igs::path::separation psep;
		str = TEXT("# This file is");
		str += tsep.word;
		str += psep.encl; str += file_path; str += psep.encl;
		str += tsep.line;
		str += TEXT("#time category message..." );
		str += tsep.line;
#if defined _WIN32
/* 現在のエンコードのタイプを知る方法が分からない!!! ので決め打ち */
		str += TEXT("filecoding cp932");/* Windowsプログラムの場合 */
#else
		str += TEXT("filecoding utf-8");/* linuxプログラム等々 */
#endif
		str += tsep.line;
	}
	/* あとこれらの行が続く、、、
				10:11:12 err message...
				13:14:15 msg message...
	*/
	igs::resource::time::parameters time_pa;
	igs::resource::time::now_to_param(time_pa);
	if (ymdhms_sw) {
		str += igs::text::str_from_ymdhms(
			time_pa.year
			,time_pa.month
			,time_pa.month_day
			,time_pa.hour
			,time_pa.minute
			,time_pa.second
		);				/* 13:14:15	*/
	} else {
		str += igs::text::str_from_hms(
			time_pa.hour
			,time_pa.minute
			,time_pa.second
		);				/* 13:14:15	*/
	}
	str += tsep.word;		/* " "		*/
	str += category_word;		/* "err"or"out"or"errt"or...	*/
	str += tsep.word;		/* " "		*/
	str += message_with_no_return;	/* "etc..."等	*/

	igs::text::log::append_to_file(str,file_path,dbgview_sw,log_sw);
}

/* ------------------------------------------------------ */
#if defined DEBUG_IGS_TEXT_LOG
# include <iostream> // std::cout
# include "igs_resource_exist.cxx"
# include "igs_resource_msg_from_err.cxx"
# include "igs_resource_append_text_by_atom.cxx"
# include "igs_resource_time.cxx"
# include "igs_path_separation.cxx"
# include "igs_text_separation.cxx"
# include "igs_text_format.cxx"
int main(int argc, char *argv[])
{
  try {
	/*------ 表示テスト ------*/
	igs::text::log::parameters tlog;
	std::TCOUT
	<< TEXT("err_key=\"") << tlog.err_key << TEXT("\"\n")
	<< TEXT("out_key=\"") << tlog.out_key << TEXT("\"\n")
	;
	/*------ 表示テスト ------*/
	std::basic_string<TCHAR> str;
	igs::text::separation tsep;
	for (int ii=0;ii<argc;++ii) {
		if (0 < ii) { str += tsep.word; }
		str += igs::resource::ts_from_mbs(argv[ii]);
	}
	std::TCOUT
	<< TEXT("append_hms_key_msg_ret() \"")
	<< str
	<< TEXT("\"\n")
	;
	igs::text::log::parameters log_pa;
	/*------ 通常"保存用テスト ------*/
	igs::text::log::append_hms_key_msg_ret(
		log_pa.out_key,str,TEXT("tmp_log.txt")
	);
	/*------ critical error時"C:/temp/..."保存用テスト ------*/
	igs::text::log::append_hms_key_msg_ret(
		log_pa.err_key,str,TEXT("tmp_log.txt"),true,true,true
	);
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...)               {std::cerr<<"other exception"<<std::endl;}
	return 0;
}
#endif  /* !DEBUG_IGS_TEXT_LOG */
/*
rem vc2010(32/64bits) :205,220 w! tes_msc16_igs_text_log.bat
set _DEB=DEBUG_IGS_TEXT_LOG
set _SRC=igs_text_log
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_text_log_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_text_log_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :208,225 w! tes_msc15_igs_text_log.bat
set _DEB=DEBUG_IGS_TEXT_LOG
set _SRC=igs_text_log
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_text_log_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_text_log_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :239,241 w! tes_gcc_igs_text_log.csh
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_LOG -I. igs_text_log.cxx -o tes_gcc_${HOSTTYPE}_igs_text_log_mb
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_LOG -DUNICODE -I. igs_text_log.cxx -o tes_gcc_${HOSTTYPE}_igs_text_log_wc
*/
