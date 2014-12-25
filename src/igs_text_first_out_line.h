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
	���ϥǡ����κǽ��Ԥˤ����ɬ��!!!
	Return Code is...
		Windows		"\r\n"	= CRLF
		Linux		"\n"	= LF
	���Ԥ�Ϣ³���Ƥ���(����)���ϼ��������
 */
  IGS_RESOURCE_IRF_EXPORT bool first_out_line(
	std::basic_string<TCHAR>&lifoline
	,std::basic_string<TCHAR>&fo_line
  );
 }
}

#endif /* !igs_text_first_out_line_h */
