#include "igs_text_separation.h"

/* 文字行の分離文字の定義(固定) */
igs::text::separation::separation()
	:date(TEXT("-"))
	,time(TEXT(":"))
	,word(TEXT(" "))
#if defined _WIN32
	,line(TEXT("\r\n"))	/* Windows binary保存用の改行 */
#else
	,line(TEXT("\n"))	/* Linux binary保存用の改行 */
#endif
{}

//------------------------------------------------------------------
#if defined DEBUG_IGS_TEXT_SEPARATION
# include <fstream>
# include <sstream>
# include <iostream>
# if defined _WIN32
#  if defined UNICODE && !defined _UNICODE
#   define _UNICODE // _tcslen()
#  endif
#  include <tchar.h> // _tcslen()
# else   /* !_WIN32 */
#  if defined UNICODE
#   include <wchar.h>
#   define _tcslen wcslen
#  else   /* !UNICODE */
#   include <cstring>
#   define _tcslen strlen
#  endif  /* !UNICODE */
# endif /*  !_WIN32 */
int main(void) { 
	igs::text::separation tsep;
	std::basic_ostringstream<TCHAR> ost;
	ost
	 << TEXT("igs::text::separation\n")

			<< TEXT(" date ") << TEXT("len=")
		<< ::_tcslen(tsep.date) << TEXT(" code=")
	 << static_cast<int>(tsep.date[0]) << TEXT(" str=\"")
			  << tsep.date << TEXT("\"\n")

			<< TEXT(" time ") << TEXT("len=")
		<< ::_tcslen(tsep.time) << TEXT(" code=")
	 << static_cast<int>(tsep.time[0]) << TEXT(" str=\"")
			  << tsep.time << TEXT("\"\n")

			<< TEXT(" word ") << TEXT("len=")
		<< ::_tcslen(tsep.word) << TEXT(" code=")
	 << static_cast<int>(tsep.word[0]) << TEXT(" str=\"")
			  << tsep.word << TEXT("\"\n")

			<< TEXT(" line ") << TEXT("len=")
		<< ::_tcslen(tsep.line) << TEXT(" code=")
	 << static_cast<int>(tsep.line[0]) << TEXT(" str=\"")
			  << tsep.line << TEXT("\"\n")
	;
	std::TCOUT << ost.str();

	/* Save binary!!! as binary data */
	std::basic_ofstream<TCHAR> fst("tmp.txt",std::ios::binary);
	fst << ost.str();
	fst.close();

	return 0;
}
#endif  /* !DEBUG_IGS_TEXT_SEPARATION */
/*
rem vc2010(32/64bits) :60,76 w! tes_msc16_igs_text_separat.bat
set _DEB=DEBUG_IGS_TEXT_SEPARATION
set _SRC=igs_text_separation
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_text_sepa_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_text_sepa_wc
rem Need _UNICODE for ::_tcslen()
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D_UNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :58,76 w! tes_msc15_igs_text_separat.bat
set _DEB=DEBUG_IGS_TEXT_SEPARATION
set _SRC=igs_text_separation
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_text_sepa_mb
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_text_sepa_wc
rem Need _UNICODE for ::_tcslen()
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D_UNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :96,98 w! tes_gcc_igs_text_separat.csh
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_SEPARATION -I. igs_text_separation.cxx -o tes_gcc_${HOSTTYPE}_igs_text_sepa_mb
g++ -Wall -O2 -g -DDEBUG_IGS_TEXT_SEPARATION -DUNICODE -I. igs_text_separation.cxx -o tes_gcc_${HOSTTYPE}_igs_text_sepa_wc
*/
