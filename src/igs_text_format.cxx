#include "igs_text_format.h"
#include "igs_path_separation.h"
//--------------------------------------------------------------------
/*
line_from_str("key","This"       ,false,4)	-->key  This
line_from_str("key","This is str",true, 4)	-->key  "This is str"
line_from_str("key","This is str",true, 3)	-->key "This is str"
*/
const std::basic_string<TCHAR> igs::text::line_from_str(
	LPCTSTR key
	,const std::basic_string<TCHAR> str
	,const bool double_quotation_is
	,const int key_wide
) {
	std::basic_ostringstream<TCHAR> line;
	igs::text::separation tsep;
	igs::path::separation psep;
	line << std::setw(key_wide) << std::left << key << tsep.word;
	if (double_quotation_is) { line << psep.encl; }
	line << str;
	if (double_quotation_is) { line << psep.encl; }
	return line.str();
}
/*
line_from_bool("key",true ,"ON","OFF",4)	-->key  ON
line_from_bool("key",false,"ON","OFF",3)	-->key OFF
*/
const std::basic_string<TCHAR> igs::text::line_from_bool(
	LPCTSTR key
	,const bool val
	,LPCTSTR true_key
	,LPCTSTR false_key
	,const int key_wide
) {
	std::basic_ostringstream<TCHAR> line;
	igs::text::separation tsep;
	line << std::setw(key_wide) << std::left << key << tsep.word
		<< (val?true_key:false_key);
	return line.str();
}
namespace {
 /*
	"C:\\Program Files\\Adobe\\Adobe After Effects CS4\\Support Files"
	--> ξü��double quote��Ȥ�space��double quote�˴����� -->
	 C:\\Program"Files\\Adobe\\Adobe"After"Effects"CS4\\Support"Files

	"" ... --> ��Ƭ����ʸ���ϻĤ���¸�ߤ����� --> "" ...

	... "" ... --> ����ζ���ʸ����Ĥ���¸�ߤ����� --> ... "" ...
 */
 void space_to_double_quote_(std::basic_string<TCHAR>&str) {
	bool sw = false;
	for (unsigned ii=0; ii<str.size();) {
		/* ��Ƭ��<""...>�϶���ʸ���ʤΤǤ��Τޤ� */
		if (
		   0 == ii		&& TEXT('\"') == str.at(ii)
		&& (ii+1) < str.size()	&& TEXT('\"') == str.at(ii+1)
		) {
			ii += 2; continue;
		} else

		/* <... ""...>�϶���ʸ���ʤΤǤ��Τޤޡ�<...\""...>�ϰ㤦 */
		if (
		0 < ii			&& TEXT('\\') != str.at(ii-1)
					&& TEXT('\"') == str.at(ii)
		&& (ii+1) < str.size()	&& TEXT('\"') == str.at(ii+1)
		) {
			ii += 2; continue;
		}

		/* "...\"..."��\"��ʸ���ΰ�����ǧ������
			����ʳ���"�Ϻ������
			""��
				�� -> ��
				�� -> ��
			��Ƚ��򤹤�
		*/
		if (	TEXT('\"') == str.at(ii)
		&&	(	0 == ii
				|| TEXT('\\') != str.at(ii-1)
			)
		) {
			sw = (sw?false:true);/* "..."����true������false */
			str.replace(ii,1,TEXT("")); /* ��ʸ����� */
			continue; /* ii�����ä������ְݻ� */
		}

		/* "..."�����Ǥ�spaceʸ����double quote�˴����� */
		if (sw) {
			if (TEXT(' ') == str.at(ii)) {
				str.at(ii) = TEXT('\"');
			}
		}
		++ii; /* ii�ϼ��ΰ��֤� */
	}
 }
 /*
	 C:\\Program\Files\\Adobe\\Adobe\After\Effects\CS4\\Support\Files
	--> double quote��space���᤹ -->
	 C:\\Program Files\\Adobe\\Adobe After Effects CS4\\Support Files
 */
 void double_quote_to_space_(std::basic_string<TCHAR>&str) {
	if (TEXT("\"\"") == str) {
		str = TEXT("");
		return;
	}
	for (unsigned ii=0; ii<str.size(); ++ii) {
		/* "...\"..."��\"��ʸ���ΰ�����ǧ������ */
		/* double quote��spaceʸ���˴����� */
		if (	TEXT('\"') == str.at(ii)
		&&	(	0==ii
				|| TEXT('\\') != str.at(ii-1)
			)
		) {
			str.at(ii) = TEXT(' ');
		}
	}
 }
}
/*
"This is sample"	--> "This" "is" "sample"
"This "is sample""	--> "This" "is sample"
"This "is \"sample\"""	--> "This" "is "sample""
"This """		--> "This" ""
*/
void igs::text::line_to_words(
	const std::basic_string<TCHAR>&line
	,std::vector< std::basic_string<TCHAR> >&words
) {
	std::basic_string<TCHAR> str(line);
	space_to_double_quote_(str);
				/* "After Effect" "" --> After"Effect "" */
	std::basic_istringstream<TCHAR> ist(str);
	for (unsigned ii=0;;++ii) {
		std::basic_string<TCHAR> item;
		ist >> item;
		if (item.empty()) {break;}
		double_quote_to_space_(item);
				/*	After"Effect --> After Effect
					""           --> �������¸�ߤ�����
				*/
		words.push_back(item);
	}
}
/* ʸ������(words)��spaceʸ���Ƕ��ڤ�ʸ����ˤ����֤� */
const std::basic_string<TCHAR> igs::text::line_from_words(
	const std::vector< std::basic_string<TCHAR> >&words
) {
	std::basic_string<TCHAR> hosts;
	for (unsigned ii = 0; ii < words.size(); ++ii) {
		if (0 < ii) { hosts += ' '; }
		hosts += words.at(ii);
	}
	return hosts;
}
/*
ʸ����(str)��˴ޤޤ�븡��ʸ����(from)�򡢤��٤ơ��Ѵ�ʸ����(to)�˴�����
exsample
	ʸ���Ԥ�"\"��"\\"�ˡ�"/"��"\/"���Ѵ�������ˡ...
	igs::text::search_and_replace( str ,"\\" ,"\\\\" );
	igs::text::search_and_replace( str ,"/"  ,"\\/"  );
	"\\" --> "\\\\"��"/" --> "\\/"�����Ǥʤ���Ф���
	�դ���"/" --> "\\/" --> "\\\\/"�Ȥʤ����ܥ������
*/
void igs::text::search_and_replace(
	std::basic_string<TCHAR>&str
	,const std::basic_string<TCHAR>&from
	,const std::basic_string<TCHAR>&to
) {
	std::basic_string<TCHAR>::size_type pos = 0;
	while (std::basic_string<TCHAR>::npos != (pos=str.find(from, pos))){
		str.replace(pos, from.length( ), to);
		pos += to.length();
	}
}
/* ��,ʬ,�äγ��ͤ�ʸ����((��)01:02:03)�ˤ����֤� */
const std::basic_string<TCHAR> igs::text::str_from_hms(
	const int hour
	,const int minute
	,const int second
) {
	/* ��:ʬ:�� ʸ�������� */
	igs::text::separation tsep;
	std::basic_ostringstream<TCHAR> ost;
	ost
		<< std::setw(2)
		<< std::setfill(TEXT('0'))
		<< hour
		<< tsep.time
		<< std::setw(2)
		<< std::setfill(TEXT('0'))
		<< minute
		<< tsep.time
		<< std::setw(2)
		<< std::setfill(TEXT('0'))
		<< second
		;
	return ost.str();
}
/* ǯ,��,���γ��ͤ��̡��ˡ�ʸ����((��)2012,07,01)�ˤ����֤� */
const std::basic_string<TCHAR> igs::text::s04_from_year(const int year) {
	std::basic_ostringstream<TCHAR> ost;
	ost << std::setw(4) << std::setfill(TEXT('0')) << year;
	return ost.str();
}
const std::basic_string<TCHAR> igs::text::s02_from_month(const int month) {
	std::basic_ostringstream<TCHAR> ost;
	ost << std::setw(2) << std::setfill(TEXT('0')) << month;
	return ost.str();
}
const std::basic_string<TCHAR> igs::text::s02_from_day(const int day) {
	std::basic_ostringstream<TCHAR> ost;
	ost << std::setw(2) << std::setfill(TEXT('0')) << day;
	return ost.str();
}
/* ǯ,��,���γ��ͤ�ʸ����((��)2012-07-01)�ˤ����֤� */
const std::basic_string<TCHAR> igs::text::str_from_ymd(
	const int year
	,const int month
	,const int day
) {
	igs::text::separation tsep;
	std::basic_string<TCHAR> str;
	str += igs::text::s04_from_year(year);
	str += tsep.date;
	str += igs::text::s02_from_month(month);
	str += tsep.date;
	str += igs::text::s02_from_day(day);
	return str;
}
/* ǯ,��,��,��,ʬ,�äγ��ͤ�ʸ����((��)2012-07-01 01:02:03)�ˤ����֤� */
const std::basic_string<TCHAR> igs::text::str_from_ymdhms(
	 const int year ,const int month  ,const int day
	,const int hour ,const int minute ,const int second
) {
	igs::text::separation tsep;
	std::basic_string<TCHAR> str;
	str += igs::text::str_from_ymd( year ,month  ,day );
	str += tsep.word;
	str += igs::text::str_from_hms( hour ,minute ,second );
	return str;
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_TEXT_FORMAT
# include <iostream>
# include "igs_resource_time.h"
# include "igs_text_separation.cxx"
# include "igs_path_separation.cxx"
# include "igs_resource_time.cxx"
# include "igs_resource_msg_from_err.cxx"
int main(void) { 
 try {
	std::basic_string<TCHAR> str(TEXT("N:\\path\\desu.txt"));
	int intval = 123;
	unsigned unsval = 456789;
#if defined _WIN32
	DWORDLONG dwl = 123;
	DWORD dwd = 456789;
#endif
	double dbl = 1.23456789012345678901234567890;
	bool boltru = true;
	bool boofal = false;
#if defined UNICODE
	std::wcout
#else
	std::cout
#endif
		<< TEXT("--- line_from_str() test ---\n")
		<< TEXT('<')
		<< igs::text::line_from_str(
			TEXT("str_key"),str,false)
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_str(
			TEXT("str_quart_key"),str,true)
		<< TEXT(">\n")
		<< TEXT("--- line_from_bool() test ---\n")
		<< TEXT('<')
		<< igs::text::line_from_bool(
			TEXT("bool_word_true_key"),true
			,TEXT("ON"),TEXT("OFF"))
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_bool(
			TEXT("bool_word_false_key"),false
			,TEXT("ON"),TEXT("OFF"))
		<< TEXT(">\n")
		<< TEXT("--- line_from_param() test ---\n")
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("int_key"),intval)
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("unsigned_key"),unsval)
		<< TEXT(">\n")
#if defined _WIN32
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("dwordlong_key"),dwl)
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("dword_key"),dwd)
		<< TEXT(">\n")
#endif
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("double_key"),dbl)
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("char_str_key"),str)
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("bool_true_key"),boltru)
		<< TEXT(">\n")
		<< TEXT('<')
		<< igs::text::line_from_param(
			TEXT("bool_false_key"),boofal)
		<< TEXT(">\n")
		;
	/*--- line to words ---*/
	std::basic_ostringstream<TCHAR> ost;

	const std::basic_string<TCHAR> line(TEXT(
	"host1 host2 \"after effect\" \"after effect \\\"CS 6\\\"\" \"\""
// -->   host1 host2  "after effect"   "after effect \"CS 6\""      ""
	));
	std::vector< std::basic_string<TCHAR> > words;
	igs::text::line_to_words(line,words);
	ost
		<< TEXT("--- line_to_words(line,words) test ---\n")
		<< line << std::endl;
	for (unsigned ii=0;ii<words.size();++ii) {
		ost	<< ii
			<< TEXT("\t<") << words.at(ii) << TEXT(">\n");
	}

	const std::basic_string<TCHAR> line2(TEXT(
	"test_dirpath        \"//host1/users/public/\""
	));
	std::vector< std::basic_string<TCHAR> > words2;
	igs::text::line_to_words(line2,words2);
	ost
		<< TEXT("--- line_to_words(line2,words2) test ---\n")
		<< line2 << std::endl;
	for (unsigned ii=0;ii<words2.size();++ii) {
		ost	<< ii
			<< TEXT("\t<") << words2.at(ii) << TEXT(">\n");
	}
	ost	<< TEXT("--- line_from_words(words) test ---\n")
		<< igs::text::line_from_words( words )
		<< std::endl
		<< TEXT("--- line_from_words(words2) test ---\n")
		<< igs::text::line_from_words( words2 )
		<< std::endl
		;
#if defined UNICODE
	std::wcout
#else
	std::cout
#endif
		<< ost.str();
	/*--- text search and replace ---*/
	std::basic_string<TCHAR> src(TEXT("This is this,That is this"));
	std::basic_string<TCHAR> from(TEXT("this")),to(TEXT("that"));
	std::basic_string<TCHAR> dst(src);
	igs::text::search_and_replace( dst ,from ,to );
#if defined UNICODE
	std::wcout
#else
	std::cout
#endif
		<< TEXT("--- search_and_replace() test ---\n")
		<< TEXT("src =") << src << std::endl
		<< TEXT("from=") << from << std::endl
		<< TEXT("to  =") << to << std::endl
		<< TEXT("dst =") << dst << std::endl
		;
	/* ���߻��� */
	igs::resource::time::parameters time_pa;
	igs::resource::time::now_to_param(time_pa);
	/* ��:ʬ:�� ʸ�������� */
#if defined UNICODE
	std::wcout
#else
	std::cout
#endif
		<< TEXT("--- str_from_hms() test ---\n\"")
		<< igs::text::str_from_hms(
			time_pa.hour ,time_pa.minute ,time_pa.second )
		<< TEXT("\"\n--- s04_from_year() test ---\n")
		<< igs::text::s04_from_year(time_pa.year)
		<< TEXT("\n--- s02_from_month() test ---\n")
		<< igs::text::s02_from_month(time_pa.month)
		<< TEXT("\n--- s02_from_day() test ---\n")
		<< igs::text::s02_from_day(time_pa.month_day)
		<< TEXT("\n--- str_from_ymd() test ---\n\"")
		<< igs::text::str_from_ymd(
			time_pa.year ,time_pa.month ,time_pa.month_day )
		<< TEXT("\n--- str_from_ymdhms() test ---\n\"")
		<< igs::text::str_from_ymdhms(
			time_pa.year ,time_pa.month ,time_pa.month_day
			,time_pa.hour ,time_pa.minute ,time_pa.second )
		<< TEXT("\"\n")
		;
 }
 catch(std::exception&e) {
	std::cout << "exception<" << e.what() << ">" << std::endl;
 }
 catch(...) {
	std::cout << "other exception" << std::endl;
 }

	return 0;
}
#endif  /* !DEBUG_IGS_TEXT_FORMAT */
/*
rem vc2010(32/64bits) :421,436 w! tes_msc16_igs_text_format.bat
set _DEB=DEBUG_IGS_TEXT_FORMAT
set _SRC=igs_text_format
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_text_format_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_text_format_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :437,454 w! tes_msc15_igs_text_format.bat
set _DEB=DEBUG_IGS_TEXT_FORMAT
set _SRC=igs_text_format
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_text_format
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_text_format_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :455,457 w! tes_gcc_igs_text_format.csh
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_FORMAT -I. igs_text_format.cxx -o tes_gcc_${HOSTTYPE}_igs_text_format_mb
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_FORMAT -DUNICODE -I. igs_text_format.cxx -o tes_gcc_${HOSTTYPE}_igs_text_format_wc
*/
