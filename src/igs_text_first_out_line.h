#ifndef igs_text_first_out_line_h
#define igs_text_first_out_line_h

#include <string>
#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace text {
 /*
	入力データの最終行にも改行必須!!!
	Return Code is...
		Windows		"\r\n"	= CRLF
		Linux		"\n"	= LF
	改行が連続していた(空行)場合は取り除かれる
 */
  IGS_RESOURCE_IRF_EXPORT bool first_out_line(
	std::basic_string<TCHAR>&lifoline
	,std::basic_string<TCHAR>&fo_line
  );
 }
}

#endif /* !igs_text_first_out_line_h */
