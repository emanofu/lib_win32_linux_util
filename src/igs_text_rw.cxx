#include <vector> // std::vector
#include <fstream> // std::basic_ofstream,std::basic_ifstream
#include <sstream> // std::basic_istringstream,std::ostringstream ost
#include <stdexcept> // std::domain_error(-)
#include "igs_resource_msg_from_err.h"
#include "igs_text_rw.h"

/*
	!ifs ,bad() ,good() ,fail()�ξܺ�
	2013-3-7

	"�֥ץ���ߥ󥰸���C++���軰�� Bjarne Stroustrup��"���
		bad()	���ȥ꡼�ब����Ƥ���(��̿Ū)
			���٤Ƽ����Ƥ���
		fail()	���α黻�ϼ��Ԥ���
			Ʊ����bad()�Ǥʤ���Х��ȥ꡼��ϲ���Ƥ��餺��
			����줿ʸ���Ϥʤ�
		good()	���α黻�Ϥ����餯��������
			ľ�������ϱ黻������
		!good()	���α黻��null�黻�Ȥʤ꼺�Ԥ��롣


	rhel5 "/usr/include/c++/4.1.2/bits/basic_ios.h"���
		!ifs	return this->fail();

	stream�Υ롼����ˡ
	2013-3-8

	�ե������ɤ߹���
	std::ifstream ifs("tmp.txt");
	while (std::getline(ifs,line)) {
		if (ifs.bad()) { // error
		}
	}

	ʸ�����ʬ��(���:double quate("50"-->50)�ϳ����ʤ�)
	std::istringstream ifs("q w e r t y \"50\"");
	while (!ifs.eof()) {
		std::string str; ifs >> str;
		if (str.empty()) { break; }	<-- ���ι����פ�?
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
	/*--- �ե����볫�� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_ifstream<TCHAR> ifs( file_path );
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::ifstream ifs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ifs) {
		throw std::domain_error(er_(file_path,TEXT("open_r_s")));
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
			throw std::domain_error(er_(file_path,TEXT("r_s")));
		}
		//if ( ifs.fail()) { continue; } /* ���α黻�ϼ��Ԥ��� */
		//if (!ifs.good()) { continue; } /* ���α黻��null�黻 */

		if (line.size() <= 0) { continue; }	/* ���� */
		if (TEXT('#')==line.at(0)) { continue;}	/* �����ȹ� */
		{
			std::basic_istringstream<TCHAR> ist(line);
			std::basic_string<TCHAR> str;
			ist >> str;
			if (str.size() <= 0) { continue; } /* ����� */
		}
		/*--- ʸ���Ԥ����� ---*/
		/* unix(0x0a)��windows(0x0d0x0a)�β��ԥ�����������н�
		(0x0d)���Ԥν����˻ĤäƤ��ޤ��ΤǺ������
		�ʤ���windows��unix�β��ԥ����ɤ�����ʤ��ɤ� */
		if (0<line.size() && 0x0d == line.at(line.size()-1)) {
			line.erase( line.size()-1 );
		}
		/*--- 1���ɲ� ---*/
		lines.push_back(line);
	}
	/*--- �ե������Ĥ��� ---*/
	ifs.close();
}
//--------------------------------------------------------------------
void igs::text::read_line(
	LPCTSTR file_path
	,std::basic_string<TCHAR>&line
) {
	/*--- �ե����볫�� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_ifstream<TCHAR> ifs( file_path );
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::ifstream ifs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ifs) {
		throw std::domain_error(er_(file_path,TEXT("open_r")));
	}
	/*--- �ɤ߹��� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	if (!std::getline(ifs,line)) {
		throw std::domain_error(er_(file_path,TEXT("r")));
	}
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
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
	/*--- ʸ���Ԥ����� ---*/
	/* unix(0x0a)��windows(0x0d0x0a)�β��ԥ�����������н�
	(0x0d)���Ԥν����˻ĤäƤ��ޤ��ΤǺ������
	�ʤ���windows��unix�β��ԥ����ɤ�����ʤ��ɤ� */
	if (0<line.size() && 0x0d == line.at(line.size()-1)) {
		line.erase( line.size()-1 );
	}
	/*--- �ե������Ĥ��� ---*/
	ifs.close();
}
//--------------------------------------------------------------------
void igs::text::append_line( /* ���Ԥ�����ɲ���¸ */
	const std::basic_string<TCHAR>&line_with_no_return_code
	,LPCTSTR file_path
) {
	/*--- �ե����볫�� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_ofstream<TCHAR> ofs( file_path
		,std::ios::out | std::ios::app
	);
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::ofstream ofs( igs::resource::mbs_from_ts(file_path).c_str()
		,std::ios::out | std::ios::app
	);
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("open_a")));
	}
	/*--- �񤭽Ф� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	ofs << line_with_no_return_code << std::endl;
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	ofs	<< igs::resource::mbs_from_ts(line_with_no_return_code)
		<< std::endl;
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("a")));
	}
	/*--- �ե������Ĥ��� ---*/
	ofs.close();
}
//--------------------------------------------------------------------
void igs::text::write_line( /* ���Ԥʤ�����¸ */
	const std::basic_string<TCHAR>&line
	,LPCTSTR file_path
) {
	/*--- �ե����볫�� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_ofstream<TCHAR> ofs( file_path );
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::ofstream ofs( igs::resource::mbs_from_ts(file_path).c_str() );
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("open_w")));
	}
	/*--- �񤭽Ф� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	ofs << line;
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	ofs << igs::resource::mbs_from_ts(line);
#endif
	if (ofs.bad()) {
		throw std::domain_error(er_(file_path,TEXT("w")));
	}
	/*--- �ե������Ĥ��� ---*/
	ofs.close();
}
//--------------------------------------------------------------------
void igs::text::write_lines(
	const std::vector< std::basic_string<TCHAR> >&lines
	,LPCTSTR file_path
) {
	/*--- �ե����볫�� ---*/
#if _WIN32 || !UNICODE /* �̾� */
	std::basic_ofstream<TCHAR> ofs( file_path );
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
	std::ofstream ofs( igs::resource::mbs_from_ts(file_path).c_str());
#endif
	if (!ofs) {
		throw std::domain_error(er_(file_path,TEXT("open_w_s")));
	}
	/*--- �񤭽Ф� ---*/
	for (unsigned ii=0; ii< lines.size(); ++ii) {
#if _WIN32 || !UNICODE /* �̾� */
		ofs << lines.at(ii) << std::endl;
#else /* unix��UNICODE�λ�(�ե�����̾���ɹ���)MBCSɬ�� */
		ofs << igs::resource::mbs_from_ts(lines.at(ii)) <<std::endl;
#endif
		if (ofs.bad()) {
			throw std::domain_error(er_(file_path,TEXT("w_s")));
		}
	}
	/*--- �ե������Ĥ��� ---*/
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
	/* ���ܸ�ʸ����ʬ�����Τ������������ܤ����ꤷ��
	���ͥ��ƥ����C locale�Τޤޤˤ��� */
	igs::resource::locale_to_jp();

  try {
	if (argc < 3) {
		pr_help_(argv[0]);
		return 1;
	}
	const std::basic_string<TCHAR> fpath(
		igs::resource::ts_from_mbs(argv[2])
	);
	/*	���!!!
	"������"�����ܸ�ʸ���ϡ�
	linux(rhel5)�Ǥ�utf-8
	windows(vc2008sp1)�Ǥ�cp932��
	�ˤ��ʤ��������ư��ʤ�
	*/
	if (argc >= 4 && !strcmp("w",argv[1])) {
		if (argc == 4) { /* �����ԤȤ��� */
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
