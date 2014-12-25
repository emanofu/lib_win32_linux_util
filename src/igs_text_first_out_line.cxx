#include "igs_text_first_out_line.h"
/*	���ϥǡ����κǽ��Ԥˤ����ɬ��!!!
	Return Code is...
		Windows		"\r\n"	= CRLF	=0x0d0x0a
		Linux		"\n"	= LF	=0x0a
	���Ԥ�Ϣ³���Ƥ���(����)���ϼ��������
*/
bool igs::text::first_out_line(
	std::basic_string<TCHAR>&lifoline
	,std::basic_string<TCHAR>&fo_line
) {
	/* lifoline������Ԥ򸡺� */
	std::basic_string<TCHAR>::size_type lsz = lifoline.find_first_of(
		TEXT("\r\n")
	);

	/* ���Ԥʤ�-->ʸ���Ǥ��뤳�Ȥ���� */
	if (std::basic_string<TCHAR>::npos == lsz) { return false; }

	/* ���Խ���Ƭ1��ʬ���� */
	if (0 < lsz) {
		fo_line = lifoline.substr(0,lsz);
	} else {fo_line = TEXT(""); }	/* ���� */

	/* ���Դޤ�Ƭ1��ʬ������� */
	/* ���Ԥ�Ϣ³���Ƥ������Ϥ����Ǽ�������� */
	std::basic_string<TCHAR>::size_type esz =
			lifoline.find_first_not_of(TEXT("\r\n"),lsz);
	lifoline.erase(0,esz);

	/* ���Ԥ��ä��Τ�true���֤�(�ǽ��Ԥ�returncode�ʤ����ڤ�Ф��ʤ�) */
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
	/*--- �ե����볫�� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_ifstream<TCHAR> ifs( file_path.c_str() );
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::ifstream ifs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ifs) {
		std::basic_string<TCHAR> str(TEXT("\""));
		str += file_path;
		str += TEXT("\" can not open");
		throw std::domain_error(igs_resource_msg_from_er(str));
	}
	/*--- 1�ԤŤ��ɹ� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_string<TCHAR> line;
	while (std::getline(ifs,line)) {
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::string line_tmp;
	while (std::getline(ifs,line_tmp)) {
		std::basic_string<TCHAR> line(
			igs::resource::ts_from_mbs(line_tmp)
		);
#endif
		/*--- �ɤ߹��ߥ��顼�����å� ---*/
		if (ifs.bad()) {
			std::basic_string<TCHAR> str(TEXT("\""));
			str += file_path;
			str += TEXT("\" can not read");
			throw std::domain_error(
				igs_resource_msg_from_er(str));
		}
		/*--- ʸ���Ԥ����� ---*/
		/* unix(0x0a)��windows(0x0d0x0a)�β��ԥ�����������н�
		(0x0d)���Ԥν����˻ĤäƤ��ޤ��ΤǺ������
		�ʤ���windows��unix�β��ԥ����ɤ�����ʤ��ɤ� */
		if (0<line.size() && TEXT('\r')==line.at(line.size()-1)) {
			line.erase( line.size()-1 );
		}
		/*--- 1���ɲ� ---*/
		filestr += line;
		filestr += TEXT('\n');
	}
	/*--- �ե������Ĥ��� ---*/
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

	/*--- text�ե����������ɤࡣ���ԥ����ɤ�1ʸ���Ǥ��뤳��! ---*/
	/* ���Ԥ�line���ڤ��äƤ���Τ�file�Ǹ�Ԥޤǲ��Ԥ�ɬ��
	�̾�Υ�å������Ϻǽ��Ԥޤǲ��ԤϤ��� */
	std::basic_string<TCHAR> filestr;
	textfile_to_string_( igs::resource::ts_from_mbs(argv[1]) ,filestr );

	/*--- �ѥ�᡼�� ---*/
	std::basic_string<TCHAR>&lifoline(filestr);// Last In/First Out line
	std::basic_string<TCHAR> fo_line;

	/*--- 1ʸ�����Ľ������� ---*/
	/* ���Ԥ�line���ڤ��� */
	while (igs::text::first_out_line(lifoline, fo_line)) {
		std::basic_ostringstream<TCHAR> ost;
			ost << TEXT("msg  \"") << fo_line << TEXT("\"\n");
#if _WIN32 || !UNICODE /* �̾� */
		std::TCOUT << ost.str();
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
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
