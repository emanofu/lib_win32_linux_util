#ifndef igs_text_format_h
#define igs_text_format_h

#include "igs_os_type.h"

#include <string>	// std::basic_string<TCHAR>
#include <vector>	// std::vector
#include <sstream>	// std::basic_ostringstream<TCHAR>
#include <iomanip>	// std::setw(), std::left
#include "igs_text_separation.h"

namespace igs {
 namespace text {
/*
line_from_str("key","This"       ,false,4)	-->key  This
line_from_str("key","This is str",true ,4)	-->key  "This is str"
line_from_str("key","This is str",true ,3)	-->key "This is str"
*/
    const std::basic_string<TCHAR> line_from_str(
	LPCTSTR key
	,const std::basic_string<TCHAR> str
	,const bool double_quotation_is = false
	,const int key_wide=15
    );

/*
line_from_bool("key",true ,"ON","OFF",4)	-->key  ON
line_from_bool("key",false,"ON","OFF",3)	-->key OFF
*/
    const std::basic_string<TCHAR> line_from_bool(
	LPCTSTR key
	,const bool val
	,LPCTSTR true_key
	,LPCTSTR false_key
	,const int key_wide=15
    );

/*
line_from_param("key",3    ,3)	-->key 3
line_from_param("key",1,23 ,4)	-->key  1.23
line_from_param("key","str",5)	-->key   str
line_from_param("key",true ,6)	-->key    true
*/
    template <class type> const std::basic_string<TCHAR> line_from_param(
	LPCTSTR key
	,const type val
	,const int key_wide=15
    ) {
	std::basic_ostringstream<TCHAR> line;
	igs::text::separation tsep;
	line
		<< std::setw(key_wide) << std::left
		<< key
		<< tsep.word << std::boolalpha << std::setprecision(15)
		<< val;
	return line.str();
    }

/*
"This is sample"	--> "This" "is" "sample"
"This "is sample""	--> "This" "is sample"
"This "is \"sample\"""	--> "This" "is "sample""
*/
    void line_to_words(
	const std::basic_string<TCHAR>&line
	,std::vector< std::basic_string<TCHAR> >&words
    );
    const std::basic_string<TCHAR> line_from_words(
	const std::vector< std::basic_string<TCHAR> >&words
    );

/*
ʸ����(str)��˴ޤޤ�븡��ʸ����(from)�򡢤��٤ơ��Ѵ�ʸ����(to)�˴�����
exsample
	ʸ���Ԥ�"\"��"\\"�ˡ�"/"��"\/"���Ѵ�������ˡ...
	igs::text::search_and_replace( str ,"\\" ,"\\\\" );
	igs::text::search_and_replace( str ,"/"  ,"\\/"  );
	"\\" --> "\\\\"��"/" --> "\\/"�����Ǥʤ���Ф���
	�դ���"/" --> "\\/" --> "\\\\/"�Ȥʤ����ܥ������
*/
    void search_and_replace(
	std::basic_string<TCHAR>&str
	,const std::basic_string<TCHAR>&from
	,const std::basic_string<TCHAR>&to
    );

  /* ��,ʬ,�äγ��ͤ�ʸ����((��)01:02:03)�ˤ����֤� */
  const std::basic_string<TCHAR> str_from_hms(
	const int hour
	,const int minute
	,const int second
  );

  /* ǯ,��,���γ��ͤ��̡��ˡ�ʸ����((��)2012,07,01)�ˤ����֤� */
  const std::basic_string<TCHAR> s04_from_year(const int year);
  const std::basic_string<TCHAR> s02_from_month(const int month);
  const std::basic_string<TCHAR> s02_from_day(const int day);

  /* ǯ,��,���γ��ͤ�ʸ����((��)2012-07-01)�ˤ����֤� */
  const std::basic_string<TCHAR> str_from_ymd(
	const int year
	,const int month
	,const int day
  );

  /* ǯ,��,��,��,ʬ,�äγ��ͤ�ʸ����((��)2012-07-01 01:02:03)�ˤ����֤� */
  const std::basic_string<TCHAR> str_from_ymdhms(
	 const int year ,const int month  ,const int day
	,const int hour ,const int minute ,const int second
  );
 }
}

#endif /* !igs_text_format_h */
