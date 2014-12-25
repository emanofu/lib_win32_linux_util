#include <vector>
#include <string>
#include <fstream> // std::ifstream
#include <sstream>
#include <stdexcept> // std::domain_error(-)
#include "igs_resource_msg_from_err.h"
#include "igs_text_format.h"
#include "igs_text_scntoc.h"

const std::basic_string<TCHAR> cut_number_(
	const std::basic_string<TCHAR>&str
) {
	std::basic_string<TCHAR>::size_type start = str.find(TEXT(">"));
	++start;
	std::basic_string<TCHAR>::size_type end = str.find(TEXT("<"),start);

	return str.substr(start,end-start);
}
void igs::text::scntoc_start_end(
	const std::basic_string<TCHAR>&file_path
	,int&start_frame
	,int&end_frame
) {
	/*--- ".scntoc"ファイル開く ---*/
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
	/*--- パラメーター初期値設定 ---*/
	enum level {
		level0_none
		,level1_xsi	/* <xsi_file type="SceneTOC" xsi_version="8.0.249.0" syntax_version="2.0"> */
		,level2_passes	/* <Passes>			*/
		,level3_pass	/* <Pass name="Default_Pass">	*/
		,level4_start	/* <FrameStart>1</FrameStart>	*/
		,level4_end	/* <FrameEnd>96</FrameEnd>	*/
	};
	level lev(level0_none);
	/*--- xmlの各行読込と処理 ---*/
#if _WIN32 || !UNICODE /* 通常 */
	std::basic_string<TCHAR> line;
	while (std::getline(ifs,line)) {
#else /* unixでUNICODEの時MBCS必須 */
	std::string line_tmp;
	while (std::getline(ifs,line_tmp)) {
		std::basic_string<TCHAR> line(
			igs::resource::ts_from_mbs(line_tmp)
		);
#endif
		/*--- エラー処理 ---*/
		if (ifs.bad()) {
			std::basic_string<TCHAR> str(TEXT("\""));
			str += file_path;
			str += TEXT("\" can not read");
			throw std::domain_error(
				igs_resource_msg_from_er(str));
		}
		//if (ifs.fail()) { continue; } /* 致命的ではないエラー */
		if (line.size() <= 0) { continue; } /* 空行 */
		if (TEXT('#') == line.at(0)) { continue; } /* コメント行 */
		{
			std::basic_istringstream<TCHAR> ist(line);
			std::basic_string<TCHAR> str;
			ist >> str;
			if (str.size() <= 0) { continue; } /* 空白行 */
		}
		/*--- 行の整形 ---*/
		/*
		unix(0x0a)でwindows(0x0d0x0a)の改行コードを処理するとき
		(0x0d)が行の終わりに残ってしまうので削除する
		なお、windowsでunixの改行コードは問題なく読む
		*/
		if (0<line.size() && 0x0d == line.at(line.size()-1)) {
			line.erase( line.size()-1 );
		}
		/*--- 一行を言葉に分割 ---*/
		std::vector< std::basic_string<TCHAR> > words;
		igs::text::line_to_words( line ,words );
		if (words.size() <= 0) { continue; } /* 空白行 */

		/*--- Nest Level ---*/
		if ((TEXT("<xsi_file")==words.at(0)) && level0_none==lev) {
			lev = level1_xsi;
		} else
		if ((TEXT("<Passes>")==words.at(0))&& level1_xsi==lev) {
			lev = level2_passes;
		} else
		if ((TEXT("<Pass")==words.at(0)) && level2_passes==lev) {
			lev = level3_pass;
		} else
		if (0 == words.at(0).find(TEXT("<FrameStart>")) &&
		level3_pass==lev) {
			std::basic_istringstream<TCHAR> ist(
				cut_number_(words.at(0))
			);
			ist >> start_frame;
		} else
		if (0 == words.at(0).find(TEXT("<FrameEnd>")) &&
		level3_pass==lev) {
			std::basic_istringstream<TCHAR> ist(
				cut_number_(words.at(0))
			);
			ist >> end_frame;
		}
	}
	ifs.close();
}
#if defined DEBUG_IGS_TEXT_SCNTOC
# include <iostream> // std::cout
# include <iomanip> // std::setw(-)
# include "igs_resource_msg_from_err.cxx"
# include "igs_path_separation.cxx"
# include "igs_text_format.cxx"
# include "igs_text_separation.cxx"
int main(int argc, char *argv[]) { 
	igs::resource::locale_to_jp();

  try {
	if (2 != argc) {
		std::cout
			<< "Usage : " << argv[0] << " file.scntoc\n"
			<< "Exsample : " << argv[0] << " "
			<< "Scene.scntoc" << "\n"
			;
		return 1;
	}
	const std::basic_string<TCHAR> fpath(
		igs::resource::ts_from_mbs(argv[1])
	);

	int start_frame = -1;
	int end_frame = -1;
	igs::text::scntoc_start_end(
		fpath ,start_frame ,end_frame
	);
	std::cout
		<< "start=" << start_frame << std::endl
		<< "  end=" << end_frame   << std::endl
		;
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_TEXT_SCNTOC */
/*
rem vc2008sp1(32/64bits) :150,167 w! tes_msc_igs_text_scntoc.bat
set _DEB=DEBUG_IGS_TEXT_SCNTOC
set _SRC=igs_text_scntoc
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_text_scntoc_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_text_scntoc_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :168,170 w! tes_gcc_igs_text_scntoc.csh
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_SCNTOC -I. igs_text_scntoc.cxx -o tes_gcc_${HOSTTYPE}_igs_text_scntoc_mb
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_SCNTOC -DUNICODE -I. igs_text_scntoc.cxx -o tes_gcc_${HOSTTYPE}_igs_text_scntoc_wc
*/
